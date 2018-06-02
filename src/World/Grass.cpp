#include <cstddef>
#include "Grass.hpp"
#include "PerfCounter.hpp"

#include "ResourceLoader.hpp"
#include "Assets.hpp"
#include "Yaml.hpp"
#include "Sampler2D.hpp"
#include "PerfTimers.hpp"
// INFO: lod 0 is the lovest level of detail
// level 3 is the most detailed, so 3 is better and more detailed than 2
const float lodDistances[] = {130, 65, 40, 20};

namespace doc {
/*
    http://twvideo01.ubm-us.net/o1/vault/GDC2014/Presentations/Gollent_Marcin_Landscape_Creation_and.pdf
    -> color of grass is sampled from top-down view of terrain
    -> grass is placed randomly based on slope and seed(position)
    -> there should be a coverage map too(like for trees)

    TODO: translate
    Jak to jest zrobione: chunk terenu ma 4 pola trawy, sa to 4 wskaniki na GrassField (4 cwiartki chunka)
    GrassField trzyma zwrotny wskanik na chunka.

    iloś GrassField jest zależna od lod chunka.

    GrassField posiada 4 idiki do tablicy z paczkami trawy i zwrotnym wskanikiem do GrassField /* moze wystpic problem ze wskanikami w liście?
    więc kazdy chunk terenu posiada 4 pola po 4 lod trawy



*/
}
// TODO: move responsibility of planting from GrassField to Grass

std::vector<GrassPatchPositions> GrassField::patchData;
std::vector<GrassField*> GrassField::patchDataOwner;
u32 GrassField::noOfpatchData = 0;
glm::vec4 GrassField::size;

// create new VAO which shares vbos(verts, uvs, normals) with commonVao
// TODO: create vao for scene foliage, grass meshes will vary with scenes
const u32 VERTICES = 0, UVS = 1, NORMALS = 2, POSITIONS = 3, SCALE = 4, FALLOF = 5;
void Grass::initVBO(){
    auto &commonVao = assets::getVao("Common");
    vao.setup();
    vao.vbo[VERTICES] = commonVao.vbo[VERTICES];
    vao.vbo[VERTICES].bind().attrib(VERTICES).pointer_float(4).divisor(0);
    vao.vbo[UVS] = commonVao.vbo[UVS];
    vao.vbo[UVS].bind().attrib(UVS).pointer_float(3).divisor(0);
    vao.vbo[NORMALS] = commonVao.vbo[NORMALS];
    vao.vbo[NORMALS].bind().attrib(NORMALS).pointer_float(4).divisor(0);
    vao.ibo = commonVao.ibo;
    vao.ibo.bind();

    vao.vbo[POSITIONS].setup(sizeof(GrassField)*MAX_FIELD_COUNT/sizeof(float), true)
        .attrib(POSITIONS).pointer_float(4, (sizeof(SingleGrassPatch)), (void*)offsetof(SingleGrassPatch, position)).divisor(1)
        .attrib(SCALE).pointer_float(1, sizeof(SingleGrassPatch), (void*)offsetof(SingleGrassPatch, size)).divisor(1)
        .attrib(FALLOF).pointer_float(1, sizeof(SingleGrassPatch), (void*)offsetof(SingleGrassPatch, fallof)).divisor(1);
    vao();

    GrassField::init();
}
void Grass::loadData(ResourceLoader &loader, const Yaml &cfg){
    texture = loader.loadImage("Grass.png");

    densitySampler = std::make_shared<Sampler2D>(resPath + "textures/GrassDensity.png");
}
void Grass::updateBuffer(){
    PerfCounter::records["fieldCount: "] = GrassField::noOfpatchData;
    vao.vbo[POSITIONS].update(GrassField::patchData.data(), sizeof(SingleGrassPatch)/sizeof(float)*getPatchCount())();
}
void GrassField::init(){
    patchData.reserve(2048);
    patchDataOwner.reserve(2048);
}
u32 planted(0), removed(0);
void GrassField::remove(){
    if(lod == OUT_OF_VIEW) return;
    for(i32 i=0; i<=lod; i++){
    // for(i32 i=0; i<=3; i++){
        if(fieldIds[i] >= 0){
            patchDataOwner[fieldIds[i]] = nullptr;
            removed++;
        }
    }
    fieldIds = {{-1,-1,-1,-1}};
    lod = OUT_OF_VIEW;
}
bool GrassField::updateLod(const glm::vec4 &eye, QuadTree &QT){
    auto getLod = [](const glm::vec4 &p2, const glm::vec4 &p1) -> i32 {
        float distance = glm::distance(p1.xyz(), p2.xyz());
        for(i32 lod=3; lod>=0; lod--){
            if(distance < lodDistances[lod]){
                return lod;
            }
        }
        return LOD_OUT_OF_VIEW;
    };

    i32 newLod = getLod(eye, center);
    if(newLod == lod) return false;

    if(newLod > lod){ // plant grass in new levels
        for(i32 i=lod+1; i<= newLod; i++){
            plantGrass(i, QT);
            planted++;
        }
    }
    else if(newLod < lod){ // remove grass from levels
        for(i32 i=lod; i> newLod; i--){
            patchDataOwner[fieldIds[i]] = nullptr;
            planted--;
        }
    }
    lod = newLod;
    return true;
}
void GrassField::plantGrass(i32 lodLevel, QuadTree &QT){
    patchData.emplace_back();
    fieldIds[lodLevel] = patchData.size()-1;
    patchDataOwner.push_back(this);
    noOfpatchData++;

    std::default_random_engine generator(lodLevel + owner->id);
    std::uniform_real_distribution<float> x(center.x + size.x, center.x - size.x);
    std::uniform_real_distribution<float> y(center.y + size.y, center.y - size.y);
    std::uniform_real_distribution<float> fallofFix(2.f, 10.f);

    auto sample = [&, this](glm::vec4 &position) -> bool {
        auto result = QT.sampleHeight(position, owner);
        position.z = result.position.z - (1 - result.normal.z)*2;
        position.w = 0.f;
        if(result.normal.z < 0.93) position.z = -100000;
        return result.normal.z > 0.9;
    };
    u32 failureCount(0), maxFailures(2);

    for(u32 i=0; i<NO_OF_GRASS_PATCHES_IN_FIELD; i++){
        auto &patch = patchData.back()[i];
        patch.position = glm::vec4(x(generator), y(generator), 0, 0);

        if(not sample(patch.position)){
        // if(not sample(patch.position) && failureCount++ < maxFailures){
            // i--;
            patch.size = 0;
        }
        else if(failureCount >= maxFailures){ // skip patch
            failureCount = 0;
            continue;
        }
        else {
            patch.size = 1;
            // patch.size = 2;
            patch.fallof = lodDistances[lodLevel]-fallofFix(generator);
        }
    }

}
void GrassField::cleanup(){
    u32 trimmed(0), nullptrs(0);
    auto trim = [&trimmed, &nullptrs](){
        for(i32 i=noOfpatchData-1; i>=0; i--){
            if(patchDataOwner[i] == nullptr){
                noOfpatchData--;
                trimmed++;
                nullptrs++;
                // log("trimmed:", i);
            }
            else return;
        }
    };
    auto overwritePatchData = [](i32 from, i32 to){
        patchData[to] = patchData[from];
        std::swap(patchDataOwner[to], patchDataOwner[from]);
        for(auto &p : patchDataOwner[to]->fieldIds)
            if(p == from){
                p = to;
                return;
            }
    };

    trim();
    for(i32 i=0; i<noOfpatchData; i++){
        if(patchDataOwner[i] == nullptr){
            overwritePatchData(noOfpatchData-1, i);
            trim();
        }
    }
    patchData.resize(noOfpatchData);
    patchDataOwner.resize(noOfpatchData);
}
void Grass::update(glm::vec4 eyePos){
    if(QT.lodLevels[0].removedNodes.size() == 0 and QT.lodLevels[0].addedNodes.size() == 0) return;
    CPU_SCOPE_TIMER("Grass::update");
    GrassField::size = glm::vec4(QT.chunkSize.x/4.f);
    bool updateGpu = false;
    removed = 0;
    for(auto &it : QT.lodLevels[0].removedNodes){
        if(it->payload.grassData)
             for(auto &fieldId : it->payload.grassData->fields){
                fieldId->remove();
                grassFields.remove(*fieldId);
                updateGpu = true;
            }
    }

    for(auto &it : QT.lodLevels[0].addedNodes){
        if(not it->payload.grassData) it->payload.grassData = std::make_unique<GrassData>();
        for(i32 fieldId=0; fieldId<4; fieldId++){
            i32 newId = grassFields.size();
            grassFields.emplace_back(fieldId, it);
            updateGpu = true;
        }
    }

    GrassField::cleanup();
    planted = 0;
    for(auto &field : grassFields){
        updateGpu |= field.updateLod(eyePos, QT);
    }

    if(updateGpu){
        GrassField::cleanup();
        updateBuffer();
    }
}
