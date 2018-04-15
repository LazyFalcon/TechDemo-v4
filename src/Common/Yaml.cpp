#include "Logging.hpp"
#include "Yaml.hpp"
#include <iostream>
#include <fstream>
#include <regex>
#include <boost/algorithm/string/trim.hpp>

struct Stringify : public boost::static_visitor<std::string>
{

    std::string operator()(const std::string &s) const {
        return s;
    }

    std::string operator()(const u32 &u) const {
        std::stringstream stream;
        stream<<std::hex<<u;
        return stream.str();
    }

    std::string operator()(const double &d) const {
        std::stringstream stream;
        stream<<d;
        return stream.str();
    }

    std::string operator()(const glm::vec4 &v) const {
        std::stringstream stream;
        stream<< "[ "<<v.x<<" "<<v.y<<" "<<v.z<<" "<<v.w<<" ]"s;
        return stream.str();
    }

    std::string operator()(const floatVec &v) const {
        std::stringstream stream;
        stream<<"{ ";
        for(const auto &it : v){
            stream<<it<<" "s;
        }
        stream<<" }";
        return stream.str();
    }

    std::string operator()(const bool &b) const {
        return b ? "yes"s : "no"s;
    }

    std::string operator()(const std::function<void(void)> &f) const {
        return "std::function<void(void)>";
    }
};
struct StringifyWithInfo : public boost::static_visitor<std::string>
{

    std::string operator()(const std::string &s) const {
        return "string: "s + s;
    }

    std::string operator()(const u32 &u) const {
        std::stringstream stream;
        stream<<std::hex<<u;
        return "color: "s + stream.str();
    }

    std::string operator()(const double &d) const {
        std::stringstream stream;
        stream<<d;
        return "double: "s + stream.str();
    }

    std::string operator()(const glm::vec4 &v) const {
        std::stringstream stream;
        stream<< "[ "<<v.x<<" "<<v.y<<" "<<v.z<<" "<<v.w<<" ]"s;
        return "glm: "s + stream.str();
    }

    std::string operator()(const floatVec &v) const {
        std::stringstream stream;
        stream<<"{ ";
        for(const auto &it : v){
            stream<<it<<" "s;
        }
        stream<<" }";
        return "vector "s + stream.str();
    }

    std::string operator()(const bool &b) const {
        return "bool: " + (b ? "yes"s : "no"s);
    }

    std::string operator()(const std::function<void(void)> &f) const {
        return "std::function<void(void)>";
    }
};

struct Line
{
    int depth;
    std::string key;
    std::string value;
    bool isArrayElement {false}; // TODO: rename
    Line(const std::string& line){
        countSpaces(line);
        splitLine(line);
        // if(key.empty()) log("depth:", depth, "value:", value);
        // else log("depth:", depth, "key:", key, "value:", value);
    }


    void countSpaces(const std::string &s){
        depth = 0;
        for(auto& it : s){
            if(it == '-'){
                ++depth;
                isArrayElement = true;
            }
            else if(it == ' ') ++depth;
            else return;
        }
    }
    void splitLine(const std::string &s){
        auto res = s.find(':');
        if(res == std::string::npos){ // then weve got only value
            value = s.substr(depth);
            boost::trim(value);
        }
        else {
            key = s.substr(depth, res - depth);
            if(s.size() > res+1) value = s.substr(s.find_first_not_of(' ', res+1));
            boost::trim(key);
            boost::trim(value);
        }
    }

};

class YamlLoader
{
private:
    Yaml& root;
    std::fstream file;
    std::vector<Line> lines;
    int currentLine {};
public:
    YamlLoader(const std::string &filename, Yaml& yaml): root(yaml){
        file.open(filename, std::fstream::in);
    }
    ~YamlLoader(){
        file.close();
    }
    bool isCommentOrEmpty(const std::string&) const;
    void run();
    void fill(Yaml& node, int nodeDepth=0, bool ignoreArrayElement=false);
};

std::string Yaml::string() const {
    return boost::apply_visitor(Stringify(), m_value);
}

std::string Yaml::debugString() const {
    return boost::apply_visitor(StringifyWithInfo(), m_value);
}

Variants Yaml::decode(std::string s){
    std::regex rFloat(R"(-?[0-9]+\.?[0-9]*)");
    std::regex rColor3("([0-9a-fA-F]{6})");
    std::regex rColor4("([0-9a-fA-F]{8})");
    std::smatch result;


    if(s.front() == '[' && s.back() == ']'){ // then match vector
        std::regex_iterator<std::string::iterator> rit ( s.begin(), s.end(), rFloat );
        std::regex_iterator<std::string::iterator> rend;
        glm::vec4 out(0);
        u32 i=0;
        while (rit!=rend && i<4){
            out[i++] = stof(rit->str());
            ++rit;
        }
        return out;
    }
    if(s.front() == '{' && s.back() == '}'){ // then match vector
        std::regex_iterator<std::string::iterator> rit ( s.begin(), s.end(), rFloat );
        std::regex_iterator<std::string::iterator> rend;
        floatVec out;
        while (rit!=rend){
            out.push_back(stof(rit->str()));
            ++rit;
        }
        return out;
    }

    if(std::regex_match(s, rFloat)){
        return std::stod(s);
    }
    if(std::regex_search(s, result, rColor4)){
        return (u32)std::stoul(result[1], nullptr, 16);
    }
    if(std::regex_search(s, result, rColor3)){
        return (u32)std::stoul(std::string(result[1])+"ff"s, nullptr, 16);
    }

    if(s == "yes" || s == "True" || s == "true") return true;
    if(s == "no" || s == "False" || s == "false") return false;

    return s;
}

void Yaml::load(const std::string& filename){
    YamlLoader loader(filename, *this);
    loader.run();
}
void Yaml::save(const std::string& filename) const {
    std::fstream file(filename, std::fstream::out);
    for(auto& it : container){
        it.printToStream(file, "  ", "", isArray);
    }
    file.close();
}

void Yaml::print() const {
    for(auto& it : container){
        it.printToStream(std::cout, "  ", "", isArray);
    }
}

void Yaml::printToStream(std::ostream& output, std::string indent, std::string indentation, bool isPartOfArray) const {
    if(isPartOfArray){
        auto withMinus = indent;
        withMinus[withMinus.size()-2] = '-';
        output << indentation << withMinus;
        if(string().size()) output << string() << std::endl;

        bool printWithoutIndent = string().empty();
        for(auto& it : container){
            it.printToStream(output, indent, printWithoutIndent ? "" : (indentation + indent), it.isArray);
            printWithoutIndent = false;
        }
    }
    else {
        output << indentation << m_key << ": " << string() << " " << std::endl;
        for(auto& it : container){
            it.printToStream(output, indent, indentation + (isArray ? "" : indent), isArray);
        }
    }
}

bool YamlLoader::isCommentOrEmpty(const std::string& s) const {
    for(auto &it : s)
        if(it == '#') return true;
        else if(it != ' ') return false;
    return true;
}

void YamlLoader::run(){
    if(not file.is_open()){
        error("No such file");
        return;
    }

    std::string buff;
    while(!file.eof()){
        getline(file, buff);
        if(not isCommentOrEmpty(buff)){
            lines.emplace_back(buff);
        }
    }

    fill(root);
}

void YamlLoader::fill(Yaml& node, int nodeDepth, bool ignoreArrayElement){
    Yaml* childNode = nullptr;
    bool thenGetBackOnArray = false;
    while(currentLine < lines.size()){
        auto& line = lines[currentLine];
        // log("\tline:", line.key, ":", line.value, nodeDepth, line.isArrayElement and thenGetBackOnArray ? "t" : "n");

        if(line.depth < nodeDepth or (thenGetBackOnArray and line.isArrayElement)){ // end of indented block
            currentLine--;
            // log("\tpop()");
            return;
        }
        else if(line.depth > nodeDepth){ // start of new indendet block
            // log("\tpush()");
            fill(*childNode, line.depth);
        }
        else { // inside current block
            if(line.key.size() and (ignoreArrayElement or not line.isArrayElement)/* and line.value.size() */){ // key: value
                childNode = &(node.push(line.key, line.value));
                if(ignoreArrayElement){
                    /*
                        it's for case:
                        - uno: uno
                          due: due
                        - tres
                    */
                    thenGetBackOnArray = true;
                    ignoreArrayElement = false;
                }
            }
            else if(line.key.empty() and line.isArrayElement){ // - value
                node.isArray = true;
                childNode = &(node.push(line.value));
            }
            else if(line.key.size() and line.isArrayElement){ // - key: value - unnamed node
                node.isArray = true;
                childNode = &(node.push("")); // create node without value, and index as key
                fill(*childNode, line.depth, true);
            }
        }

        currentLine++;
    }

}
