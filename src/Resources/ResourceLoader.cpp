#include "core.hpp"
#include "Logging.hpp"
#include "GPUResources.hpp"
#include "Yaml.hpp"
#include "ResourceLoader.hpp"
#include "ModelLoader.hpp"
#include "ImageLoader.hpp"
#include "PerfTimers.hpp"
#include "LightSource.hpp"
#include "DecalsAndMarkers.hpp"
#include "Utils.hpp"
#include "font.hpp"


u32 ResourceLoader::s_id = 0;

ResourceLoader::~ResourceLoader(){
    // TODO: zapewnić sprzątanie zaalokowanych zasobów, usunięcie z GPU i pozostałego pierdolnika

}

namespace {
    template<typename T>
    void insert(std::vector<T> &a, std::vector<T> &b){
        a.insert(a.end(), b.begin(), b.end());
    }
}

void ResourceLoader::loadResources(const Yaml &cfg){
    if(cfg.has("Images")) for(auto &image : cfg["Images"]){
        loadImage(image);
    }
    if(cfg.has("ImageSets")) for(auto &set : cfg["ImageSets"]){
        loadImageSet(set);
    }
    if(isFile(resPath + "materials/")) loadTextureArray(resPath + "materials/", "Materials");

    if(isFile(resPath + "decals/")){
        loadTextureArray(resPath + "/decals/", "Decals");
        // loadTextureArray("Terrain");
        // loadTextureArray("Foliage");
        // loadTextureArray("Vfx256");
        // loadCubeMap("Park");
    }

    if(isFile(resPath + "images/")){
        loadImages(resPath + "/images/");
    }
    if(isFile(resPath + "models/")){
        ModelLoader modelLoader;
        modelLoader.loadTangents = true;
        modelLoader.open(resPath + "/models/CommonModels.dae", std::move(assets::layerSearch(assets::getAlbedoArray("Materials"))));
        auto names = modelLoader.getNames();
        for(auto &name : names){
            auto mesh = modelLoader.beginMesh();
            modelLoader.load(name);
            modelLoader.endMesh(mesh);

            assets::addMesh(mesh, name);
        }
        assets::addVao(modelLoader.build(), "Common");
    }
    loadFonts();
}

void ResourceLoader::loadImages(const std::string& dir){
    try {
        fs::path p(dir);
        auto dir_it = fs::recursive_directory_iterator(p);
        for(dir_it; dir_it != fs::recursive_directory_iterator(); dir_it++){
            if(fs::is_directory(dir_it->status()) ) continue;

            loadImage((*dir_it).path().generic_string());
        }
    }
    catch (const fs::filesystem_error& ex){
        error("fs::filesystem ex: ", ex.what());
    }
}

/**
 *  Reads shaders from ../shaders directory and its subfolders
 */
bool ResourceLoader::loadShaders(){
    log("---shaders");
    try {
        fs::path p(shaderPath);
        auto dir_it = fs::recursive_directory_iterator(p);
        for(dir_it; dir_it != fs::recursive_directory_iterator(); dir_it++){
            if(fs::is_directory(dir_it->status()) ) continue;

            std::string localShaderPath = (*dir_it).path().generic_string();

            auto shaderName = (*dir_it).path().stem().string();
            if((*dir_it).path().extension().string() != ".glsl") continue;
            assets::setShader(shaderName).loadFromFile(localShaderPath, shaderName);
        }
    return true;
    }
    catch (const fs::filesystem_error& ex){
        error("fs::filesystem ex: ", ex.what());
    }
    return false;
}
bool ResourceLoader::reloadShader(const std::string &name){

    std::string pathToShader;
    findFile(shaderPath, name, "-r", pathToShader);

    assets::getShader(name).loadFromFile(pathToShader, name);
    log("[ SHADER ] reload:", name);

    return true;
}

Mesh ResourceLoader::loadMesh(std::string meshName){
    std::string fileName = resPath+"/models/"+meshName+".obj";

    Mesh mesh;
    std::fstream file;
    file.open(fileName, std::ios::in);

    if(!file.is_open()){
        error(fileName, "not exist");
        return {};
    }

    mesh.begin = model_indices.size();
    loadObj(file);
    mesh.end = model_indices.size();
    mesh.count = mesh.end - mesh.begin;

    file.close();
    assets::addMesh(mesh, meshName);

    log("[ MESH ]", meshName);
    return mesh;
}
bool ResourceLoader::loadMesh(const Yaml &cfg){
    std::string name = cfg["Name"].string();
    loadMesh(name);
    return true;
}
// TODO: really this still exists?!
bool ResourceLoader::loadObj(std::fstream &file){
    std::string tmp, mpt;

    char nop;
    while(tmp != "v" && !file.eof())
        file>>tmp;

    std::vector<float> vertex;
    std::vector<float> coord;
    std::vector<float> normal;
    vertex.reserve(800000);
    coord.reserve(400000);
    normal.reserve(800000);
    int u(0), k(0);
    // file>>tmp;
    float x,y,z;
    int a,b,c;
    while(tmp=="v"){
            file>>x>>y>>z;
            vertex[u++]=x;
            vertex[u++]=y;
            vertex[u++]=z;
            file>>tmp;
    }
    u=0;
    while(tmp=="vt"){
        file>>x>>y;
        coord[u++]=x;
        coord[u++]=y;
        file>>tmp;
    }
    u=0;
    while(tmp=="vn"){
        file>>x>>y>>z;
        normal[u++]=x;
        normal[u++]=y;
        normal[u++]=z;
        file>>tmp;
    }
    while(tmp != "f" && !file.eof())
        file>>tmp;
    int i(0);
    if(model_indices.size()>0)
        i = model_indices[model_indices.size()-1]+1;
    // i+=przebieg;

    u=0;//model_vertices.size();
    k=0;//model_coords.size();


    /*
    1______________2
    |            / |
    |         /    |
    |      /       |
    |   /          |
    |/             |
    0______________3
    012 023
    */
    while(tmp == "f" && !file.eof()){
            for(int t=0; t<3; t++){
                file >> a >> nop >> b >> nop >> c;
                a--;b--;c--;
                model_vertices.push_back(vertex[a*3]);
                model_vertices.push_back(vertex[a*3+1]);
                model_vertices.push_back(vertex[a*3+2]);
                model_vertices.push_back(1.f);
                model_coords.push_back(coord[b*2]);
                model_coords.push_back(coord[b*2+1]);
                model_normals.push_back(normal[c*3]);
                model_normals.push_back(normal[c*3+1]);
                model_normals.push_back(normal[c*3+2]);
                model_normals.push_back(0.f);
                u+=3;k+=2;
                model_indices.push_back(i++);
            }
            file>>tmp;
            if(tmp=="f");
            else { // in case thera are 4 vertices(quad)
                model_indices.push_back(i-3);
                model_indices.push_back(i-1);
                file >> nop >> b >> nop >> c;
                a=atoi(tmp.c_str());a--;b--;c--;
                model_vertices.push_back(vertex[a*3]);
                model_vertices.push_back(vertex[a*3+1]);
                model_vertices.push_back(vertex[a*3+2]);
                model_coords.push_back(coord[b*2]);
                model_coords.push_back(coord[b*2+1]);
                model_normals.push_back(normal[c*3]);
                model_normals.push_back(normal[c*3+1]);
                model_normals.push_back(normal[c*3+2]);
                u+=3;k+=2;
                model_indices.push_back(i++);
                file>>tmp;
            }
        }

    return true;
}

bool ResourceLoader::loadFonts(){
    log("---fonts");
    std::vector<std::string> imagesToLoad;
    auto fontsToLoad = filter(listDirectory(resPath + "/fonts/"), ".fnt");
    for(auto &font : fontsToLoad){
        loadFont(font, imagesToLoad);
    }

    std::vector<fs::path> imagesToLoad_fs;
    for(auto &it : imagesToLoad) imagesToLoad_fs.emplace_back(it = resPath + "/fonts/" + it);
    assets::TextureArray out;
    out.id = ImageUtils::loadArrayToGpu(imagesToLoad_fs).id;
    assets::addAlbedoArray(id, out, "Fonts");

    return true;
}
bool ResourceLoader::loadFont(const std::string &fontFileName, std::vector<std::string> &imagesToLoad){
    auto &font = assets::getFont(cutExt(fontFileName));
    font.load(fontFileName, imagesToLoad);
    return true;
}

bool ResourceLoader::loadImage(const Yaml &cfg){
    std::string name = cfg.string();
    return loadImage(resPath+"/textures/"+name).ID;
}
Image ResourceLoader::loadImage(const std::string &filePath){
    fs::path p(filePath);

    std::string exactName = getName(filePath);
    Image image = assets::getImage(exactName);
    if(image.ID != 0){
        return image;
    }
    log("[ IMAGE ] ", exactName);

    auto imageData = ImageUtils::loadImageToGpu(filePath);

    image = Image { imageData.id, imageData.width, imageData.height };
    assets::addImage(id, image, exactName);
    return image;
}
bool ResourceLoader::loadImageSet(const Yaml &cfg){
    std::string name = cfg["Config"].string();
    std::string image = cfg["Image"].string();
    try {
        Yaml descr(resPath + "/misc/"+name+".yml");
        auto modeStr = descr["Mode"].string();
        u32 mode = 10;
        if(modeStr == "fromBottom") mode = 0;
        else if(modeStr == "fromTop") mode = 1;
        else if(modeStr == "irfanView") mode = 2;

        loadImage(image);
        auto tmp = loadImage(image);

        ImageSet set;
        int w = set.w = tmp.width;
        int h = set.h = tmp.height;
        set.ID = tmp.ID;
        int count = descr.size();

        for(auto &it : descr["Root"]){
            std::string n = it["name"].string();
            glm::vec4 r = it["rect"].vec4();
            if(mode == 2)
                r.y += r.w;

            r.y = h - r.y;

            glm::vec4 d = glm::vec4(0,0,r.z, r.w);
            r = glm::vec4(r.x/w, r.y/h, r.z/w, r.w/h);
            set.set.emplace(n, Icon {r, d, 0xffffffff});
        }

        assets::addImageSet(set, name);
    }
    catch(...) {
        error("loading image set:", name);
        return false;
    }
    return true;
}

namespace {
    template<typename T>
    void forEachFileInDirectory(fs::path root, T&&func){
        for(auto it = fs::recursive_directory_iterator(root); it != fs::recursive_directory_iterator(); it++){
        // for(auto& it : fs::recursive_directory_iterator(root)){
            if(not fs::is_regular_file(it->status()) ) continue;

            func(it->path());
        }
    }

    // * if something is not found: error is printed and material is not loaded
    auto extractAlbedoRoughnessMetallic(const std::string& dir){
        std::vector<fs::path> roughness;
        std::vector<fs::path> metallic;
        std::vector<fs::path> albedo;
        try {
            std::vector<fs::path> albedo_unmatched;
            std::vector<fs::path> roughness_unmatched;
            std::vector<fs::path> metallic_unmatched;
            forEachFileInDirectory(fs::path(dir), [&](fs::path p){
                if(p.stem().string().find("metallic") != std::string::npos) metallic_unmatched.push_back(p);
                else if(p.stem().string().find("roughness") != std::string::npos) roughness_unmatched.push_back(p);
                else if(p.stem().string().find("albedo") != std::string::npos or p.stem().string().find("basecolor") != std::string::npos) albedo_unmatched.push_back(p);
            });

            for(auto& base : albedo_unmatched){
                std::string basename = base.stem().string();
                auto pos = basename.find("albedo");
                pos = (pos != std::string::npos) ? pos : basename.find("basecolor");
                basename = basename.substr(0, pos-1);

                auto r = std::find_if(roughness_unmatched.begin(), roughness_unmatched.end(), [&basename](fs::path& p){
                    return p.stem().string().find(basename) != std::string::npos;
                });
                auto m = std::find_if(metallic_unmatched.begin(), metallic_unmatched.end(), [&basename](fs::path& p){
                    return p.stem().string().find(basename) != std::string::npos;
                });
                if(r!=roughness_unmatched.end() and m!=metallic_unmatched.end()){
                    albedo.push_back(base);
                    roughness.push_back(*r);
                    metallic.push_back(*m);
                }
                else {
                    error("Cannot find roughness or metallic material for:", base.string(), basename);
                }
            }
        }
        catch (const fs::filesystem_error& ex){
            error("fs::filesystem ex: ", ex.what());
        }
        return std::tuple(albedo, metallic, roughness);
    }

}

// loads albedo, normal, metalic and roughness maps
assets::TextureArray ResourceLoader::loadTextureArray(const std::string &folder, const std::string &containerName){
    auto [albedo, metallic, roughness] = extractAlbedoRoughnessMetallic(folder);

    auto convertAndGetName = [](std::vector<fs::path> &files, std::vector<std::string> &out){
        for(auto &it : files) out.push_back(it.stem().string());
    };

    {
        assets::TextureArray out;
        out.id = ImageUtils::loadArrayToGpu(albedo).id;
        convertAndGetName(albedo, out.content);
        assets::addAlbedoArray(id, out, containerName);
    }
    {
        assets::TextureArray out;
        out.id = ImageUtils::loadArrayToGpu(metallic).id;
        convertAndGetName(metallic, out.content);
        assets::addMetallicArray(id, out, containerName);
    }
    {
        assets::TextureArray out;
        out.id = ImageUtils::loadArrayToGpu(roughness).id;
        convertAndGetName(roughness, out.content);
        assets::addRoughnessArray(id, out, containerName);
    }

    return assets::getAlbedoArray(folder);
}
assets::TextureArray ResourceLoader::loadCubeMap(const std::string &folder){
    bool found = false;
    auto extract = [&](std::vector<std::string> &files, std::string ext){
        std::vector<std::string> out;
        for(auto &file : files)
            if(file.find(ext) != std::string::npos){
                out.push_back(file);
                found = true;
            }

        return out;
    };
    auto cutExt = [](std::vector<std::string> &files){
        for(auto &it : files) it = getName(it);
    };

    std::vector<std::string> files = listDirectory(resPath + "/atlases/" + folder);
    for(auto &it : files){
        it = resPath + "/atlases/" + folder + "/"s + it;
    }

    assets::TextureArray out;
    out.id = ImageUtils::loadCubemapToGpu(files).id;
    out.content = files;
    cutExt(out.content);
    assets::addCubeMap(id, out, folder);

    return out;
}
