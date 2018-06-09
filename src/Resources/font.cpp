#include "core.hpp"
#include "font.hpp"
#include "Assets.hpp"
#include "Utils.hpp"
#include "font.hpp"
#include "Logging.hpp"
#include <regex>
#include <fstream>
#include <iterator>


// Loading utils
glm::vec2 splitVec2(std::string toSplit){
    glm::vec2 out;
    std::istringstream buf(toSplit);
    std::istream_iterator<std::string> beg(buf), end;
    int i = 0;
    for (; beg != end; beg++){
        out[i++] = (float)std::stoi(*beg);
    }
    return out;
}
glm::vec4 splitVec4(std::string toSplit){
    glm::vec4 out;
    std::istringstream buf(toSplit);
    std::istream_iterator<std::string> beg(buf), end;
    int i = 0;
    for (; beg != end; beg++){
        out[i++] = (float)std::stoi(*beg);
    }
    return out;
}

std::optional<int> rgxIntSearch(const std::string &word, const std::string &rgx){
    std::smatch match;
    std::regex regex(rgx);
    if(std::regex_search(word, match, regex))
        return std::stoi(match[1]);

    return {};

}
std::string regexStringSearch(const std::string &word, const std::string &rgx){
    std::smatch match;
    std::regex regex(rgx);
    std::regex_search(word, match, regex);
    return match[1];
}
std::vector<float> rgxVecFloatSearch(const std::string &line){
    std::vector<float> out;
    std::smatch match;
    std::regex regex(R"(([\-]?[0-9]+))");
    std::string word = line;
    while (std::regex_search(word, match, regex)){
        out.push_back(std::stof(match[1]));
        word = match.suffix().str();
    }
    return out;
}
std::vector<int> rgxVecIntSearch(const std::string &line){
    std::vector<int> out;
    std::smatch match;
    std::regex regex(R"(([\-]?[0-9]+))");
    std::string word = line;
    while (std::regex_search(word, match, regex)){\
        out.push_back(std::stoi(match[1]));
        word = match.suffix().str();
    }
    return out;
}

void Font::loadCharacter(const std::string &line, float imageWidth, float imageHeight, int pageOffset){
    auto &&vec = rgxVecFloatSearch(line);
    char16_t id = vec[0];
    float x = vec[1]; // left position of glyph
    float y = vec[2]; // top position of glyph
    float pxWidth = vec[3];
    float pxHeight = vec[4];
    float xoffset = vec[5];
    float yoffset = vec[6];
    float xadvance = vec[7];
    float page = vec[8];
    symbols[id].uv = glm::vec3(x / imageWidth, (imageHeight - y - pxHeight) / imageHeight, page + pageOffset);
    symbols[id].uvSize = glm::vec2(pxWidth / imageWidth, pxHeight / imageHeight);
    symbols[id].pxSize = glm::vec2(pxWidth, pxHeight);
    symbols[id].pxOffset = glm::vec2(xoffset, -yoffset  - (pxHeight - base));
    symbols[id].pxAdvance = xadvance;
}
void Font::loadKerning(const std::string &word){
    auto &&vec = rgxVecIntSearch(word);
    kerning[int(vec[0]<<16) & vec[1]] = vec[2];
}
void Font::load(const std::string &name, std::vector<std::string> &imagesToLoad){
    std::string path = resPath + "fonts/";
    std::ifstream file;
    file.open(path + name, std::ios::in);
    if(not file.good()){
        error("Can't open file", path + name);
    }
    log("Font:", name);
    std::string info, common, count, page;
    std::vector<std::string> lines;
    getline(file, info);
    getline(file, common);

    lineHeight = *rgxIntSearch(common, R"(lineHeight=([0-9]+))");
    base = *rgxIntSearch(common, R"(base=([0-9]+))");
    int pages = *rgxIntSearch(common, R"(pages=([0-9]+))");
    int pageOffset = imagesToLoad.size();
    for(auto i=0; i<pages; i++){
        getline(file, page);
        imagesToLoad.push_back(regexStringSearch(page, R"(file=\"(.*)\")"));
    }
    getline(file, count);

    float sizeU = *rgxIntSearch(common, R"(scaleW=([\-]?[0-9]+))");
    float sizeV = *rgxIntSearch(common, R"(scaleH=([\-]?[0-9]+))");
    int charCount = *rgxIntSearch(count, R"(chars count=([\-]?[0-9]+))");

    lines.resize(charCount);
    for(int i = 0; i < charCount; i++){
        getline(file, lines[i]);
    }
    for(auto &it : lines){
        loadCharacter(it, sizeU, sizeV, pageOffset);
    }
    std::string kernings;
    getline(file, kernings);
    auto kerningCount = rgxIntSearch(kernings, R"(kernings count=([\-]?[0-9]+))");
    if(kerningCount){
        lines.resize(*kerningCount);

        for (int i = 0; i < *kerningCount; i++)
            getline(file, lines[i]);
        for(auto &it : lines)
            loadKerning(it);
    }
    file.close();

    dotLen = symbols['.'].pxAdvance;
}

float Font::calculateTextLength(const std::string &text){
    // TODO: be aware of eol, maybe split text on lines and process each alone?
    float res(0);
    for(int i=text.size()-1; i>=0; i--) res += symbols[text[i]].pxAdvance;
    return res;
}

float Font::calculateTextLength(const std::u16string &text){
    // TODO: be aware of eol, maybe split text on lines and process each alone?
    float res(0);
    for(int i=text.size()-1; i>=0; i--) res += symbols[text[i]].pxAdvance;
    return res;
}

// int convertFontFilenameToId(const std::string &filename){
//     std::string name = getName(filename);
//     auto tokens = splitString(name, '_');
//     if(tokens.size() != 2){
//         error("there should be two parts in", name);
//         return 0;
//     }
//     int id = font::idMap[tokens[0]];
//     int size = std::stoi(tokens[1]);

//     return id + size;
// }
