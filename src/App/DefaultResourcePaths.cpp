#include "core.hpp"
#include "DefaultResourcePaths.hpp"
#include "Logger.hpp"

std::string dataPath;
std::string resPath;
std::string rootPath;
std::string screenshotsPath;
std::string shaderPath;

void resolvePaths(std::string fullPath){
    fs::path p(fullPath);
    auto gameRootDir = p.parent_path().parent_path().generic_string();
    // for(auto& i : gameRootDir) if(i == '\\') i='/';
    console.log("Game directory:", gameRootDir);

    dataPath = gameRootDir + "/data/";
    resPath = gameRootDir + "/res/";
    rootPath = gameRootDir + "/";
    screenshotsPath = gameRootDir + "/screenshots/";
    shaderPath = gameRootDir + "/shaders/";
}
