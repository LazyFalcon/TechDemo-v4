#include <stdexcept>

/*
* shader structure

* @shader: name  : full new shader, thanks to this there can be multiple shaders defined in one file
* @vertex:/geometry/tesslation/compute/fragment:  : shader stages
* @import: name  : imports tag
* @export: name  : exports tag

*/
namespace {
    std::map<std::string, std::string> exports;

    using Lines = std::vector<std::string>;
    Lines loadFileAndAddLineNumbers(const std::string& filename){
        Lines lines;
        std::fstream file;

        file.open(filename, std::fstream::in);
        if(not file.is_open()){
            throw std::runtime_error("No such file");
        }

        int pos(0);
        std::string buff;
        while(!file.eof()){
            getline(file, buff);
            lines.push_back(buff);

            if(buff[0] == '@'){
                lines.push_back("#line " + std::to_string(++pos));
            }
            else ++pos;

        }
        return lines;
    }

    std::string combine(Lines& lines, int from=0, int to=99999){
        to = std::min(to, (int)lines.size());
        std::string out;

        for(int i=from; i<to; i++)
            out += lines[i] + "\n";

        return out;
    }

    std::string getNameFrom(const std::string& tag){
        auto namepos = tag.find_first_not_of(": ", tag.find(':'));
        return namepos==std::string::npos ? "" :tag.substr(namepos);
    }

    // TODO: should extract vector of lines, instead imports could not be resolved
    void extractExports(Lines& lines){
        auto extractExport = [&](int exportPosition) -> int {
            int linecount(0);
            for(int i=exportPosition+1; i<lines.size(); i++){
                if(not lines[i].empty() and lines[i][0] == '@' and (lines[i].find("export")!=std::string::npos or lines[i].find("shader")!=std::string::npos or lines[i].find("end")!=std::string::npos) or i==lines.size()-1){

                    auto name = getNameFrom(lines[exportPosition]);
                    if(name == "") throw std::runtime_error("export must have name! : line: " + std::to_string(i));
                    exports[name] = combine(lines, exportPosition+1, i==lines.size()-1 ? i : i-1);
                    return linecount;
                }
                linecount++;
            }
            return 0;
        };

        for(int i=0; i<lines.size(); i++){
            if(not lines[i].empty() and lines[i][0] == '@' and lines[i].find("export")!=std::string::npos){
                i += extractExport(i);
            }
        }
    }

    bool resolveImports(Lines& lines){
        bool anyImportFound(false);
        for(int i=0; i<lines.size(); i++){
            if(not lines[i].empty() and lines[i][0] == '@' and lines[i].find("import")!=std::string::npos){
                auto name = getNameFrom(lines[i]);
                lines[i] = exports.at(name);

                anyImportFound = true;
            }
        }

        return anyImportFound;
    }

    auto splitOnShaders(Lines& lines, const std::string& defaultName){
        std::map<std::string, Lines> out;

        auto upTo = [&](int from)->int{
            int i = from+1;
            for(; i<=lines.size(); i++){
                if(i==lines.size() or (not lines[i].empty() and lines[i][0] == '@' and (lines[i].find("shader")!=std::string::npos or lines[i].find("end")!=std::string::npos))){

                    auto name = getNameFrom(lines[from]);
                    if(name == "") name = defaultName;

                    out[name] = Lines(lines.begin()+from+1, lines.begin()+i);

                    return i;
                }
            }
            return i;
        };

        for(int i=0; i<lines.size(); i++){
            if(not lines[i].empty() and lines[i][0] == '@' and lines[i].find("shader")!=std::string::npos){
                i = upTo(i)-1;
            }
        }

        return out;
    }

    template<typename T>
    struct shaderDefinition
    {
        std::optional<T> vertex;
        std::optional<T> fragment;
        std::optional<T> geometry;
        std::optional<T> tesselation;
        std::optional<T> compute;
    };

    std::string typeToName(int shaderType){
        switch(shaderType){
            case gl::VERTEX_SHADER: return "vertex";
            case gl::GEOMETRY_SHADER: return "geometry";
            case gl::FRAGMENT_SHADER: return "fragment";
            default: return "unknown";
        }
    }


    auto splitShader(Lines& lines){
        shaderDefinition<std::string> out {};

        auto goToNextTag = [&](int from)->int {
            for(int i=from+1; i<lines.size(); i++){
                if(not lines[i].empty() and lines[i][0] == '@') return i-1;
            }
            return lines.size()-1;
        };

        for(int i=0; i<lines.size(); i++){
            if(not lines[i].empty() and lines[i][0] == '@'){
                int to = goToNextTag(i);

                if(lines[i].find("vertex")!=std::string::npos) out.vertex = combine(lines, i+1, to);
                else if(lines[i].find("fragment")!=std::string::npos) out.fragment = combine(lines, i+1, to);
                else if(lines[i].find("geometry")!=std::string::npos) out.geometry = combine(lines, i+1, to);
                else if(lines[i].find("tesselation")!=std::string::npos) out.tesselation = combine(lines, i+1, to);
                else if(lines[i].find("compute")!=std::string::npos) out.compute = combine(lines, i+1, to);

                i = to;
            }
        }

        return out;
    }

    std::string shaderInfoLog(int shader){
        int infoLogLen = 0;
        int charsWritten = 0;

        gl::GetShaderiv(shader, gl::INFO_LOG_LENGTH, &infoLogLen);

        if (infoLogLen > 0){
            std::vector<char> infoLog(infoLogLen);
            gl::GetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog.data());

            return "shader compilation error:\n" + std::string(infoLog.data(), charsWritten);
        }

        return "no info";
    }

    auto compile(std::string& source, int shaderType){
        GLuint out = gl::CreateShader(shaderType);
#ifdef USEOGL33
        source.insert(0, "#version 330\n");
#else
        source.insert(0, "#version 460\n");
#endif

        const char *csource = source.c_str();
        // TODO: it is possible to pass array of strings heere
        gl::ShaderSource(out, 1, &csource, nullptr);

        gl::CompileShader(out);

        GLint compiled;
        gl::GetShaderiv(out, gl::COMPILE_STATUS, &compiled);

        if (!compiled){
            throw std::runtime_error("Compilation error in " + typeToName(shaderType) + " " + shaderInfoLog(out));
        }

        return out;
    }

    Shader compileShader(Lines& lines){
        auto stages = splitShader(lines);

        shaderDefinition<GLuint> shaderPrograms;

        if(stages.vertex) shaderPrograms.vertex = compile(*stages.vertex, gl::VERTEX_SHADER);
        if(stages.geometry) shaderPrograms.geometry = compile(*stages.geometry, gl::GEOMETRY_SHADER);
        if(stages.fragment) shaderPrograms.fragment = compile(*stages.fragment, gl::FRAGMENT_SHADER);

        u32 shaderId = gl::CreateProgram();
        if(shaderPrograms.vertex) gl::AttachShader(shaderId, *shaderPrograms.vertex);
        if(shaderPrograms.geometry) gl::AttachShader(shaderId, *shaderPrograms.geometry);
        if(shaderPrograms.fragment) gl::AttachShader(shaderId, *shaderPrograms.fragment);

        gl::LinkProgram(shaderId);

        if(shaderPrograms.vertex) gl::DeleteShader(*shaderPrograms.vertex);
        if(shaderPrograms.geometry) gl::DeleteShader(*shaderPrograms.geometry);
        if(shaderPrograms.fragment) gl::DeleteShader(*shaderPrograms.fragment);

        if(shaderPrograms.vertex) gl::DetachShader(shaderId, *shaderPrograms.vertex);
        if(shaderPrograms.geometry) gl::DetachShader(shaderId, *shaderPrograms.geometry);
        if(shaderPrograms.fragment) gl::DetachShader(shaderId, *shaderPrograms.fragment);

        return {shaderId};
    }
}

// * if zero is returned, user should retry shader compilation(if shader is fixed ofc)
auto compileShaders(const std::string& filepath, const std::string& filename){
    std::map<std::string, Shader> out;

    auto content = ::loadFileAndAddLineNumbers(filepath);

    ::extractExports(content);
    while(::resolveImports(content)){}


    auto shaders = ::splitOnShaders(content, filename);

    if(shaders.empty()) out[filename] = ::compileShader(content);
    else for(auto & it : shaders) out[it.first] = ::compileShader(it.second);

    return out;
}

void importDefinitions(const std::string& filepath){
    auto content = ::loadFileAndAddLineNumbers(filepath);
    ::extractExports(content);
}
