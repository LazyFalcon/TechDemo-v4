#include "core.hpp"
#include "Yaml.hpp"
#include <algorithm>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <fstream>
#include <iostream>
#include <regex>

struct Stringify : public boost::static_visitor<std::string>
{
    std::string operator()(const std::string& s) const {
        return s;
    }

    std::string operator()(const u32& u) const {
        std::stringstream stream;
        stream << std::hex << u;
        return stream.str();
    }

    std::string operator()(const float& d) const {
        std::stringstream stream;
        stream << d;
        return stream.str();
    }

    std::string operator()(const glm::vec4& v) const {
        std::stringstream stream;
        stream << "< " << v.x << " " << v.y << " " << v.z << " " << v.w << " >"s;
        return stream.str();
    }

    std::string operator()(const bool& b) const {
        return b ? "yes"s : "no"s;
    }

    std::string operator()(const std::function<void(void)>& f) const {
        return "std::function<void(void)>";
    }
};
struct StringifyWithInfo : public boost::static_visitor<std::string>
{
    std::string operator()(const std::string& s) const {
        return "string: "s + s;
    }

    std::string operator()(const u32& u) const {
        std::stringstream stream;
        stream << std::hex << u;
        return "color: "s + stream.str();
    }

    std::string operator()(const float& d) const {
        std::stringstream stream;
        stream << d;
        return "float: "s + stream.str();
    }

    std::string operator()(const glm::vec4& v) const {
        std::stringstream stream;
        stream << "< " << v.x << " " << v.y << " " << v.z << " " << v.w << " >"s;
        return "glm: "s + stream.str();
    }

    std::string operator()(const bool& b) const {
        return "bool: " + (b ? "yes"s : "no"s);
    }

    std::string operator()(const std::function<void(void)>& f) const {
        return "std::function<void(void)>";
    }
};

std::vector<std::string> Yaml::strings() const {
    std::vector<std::string> out(container.size());
    std::transform(container.begin(), container.end(), out.begin(), [this](const Yaml& yml) { return yml.string(); });
    return out;
}
std::vector<float> Yaml::numbers() const {
    std::vector<float> out(container.size());
    std::transform(container.begin(), container.end(), out.begin(), [this](const Yaml& yml) { return yml.number(); });
    return out;
}

Yaml& Yaml::push(const Yaml& node) {
    container.push_back(node);
    container.back().m_parent = this;
    return container.back();
}
Yaml& Yaml::push(const std::string& val) {
    return push(std::to_string(container.size()), val);
}
Yaml& Yaml::push(const std::string& key, const std::string& val) {
    auto& yml = container.emplace_back(key, val);
    yml.m_parent = this;
    return yml;
}

std::string Yaml::string() const {
    return boost::apply_visitor(Stringify(), m_value);
}

std::string Yaml::debugString() const {
    return boost::apply_visitor(StringifyWithInfo(), m_value);
}

// ! here! most important function!
Variants Yaml::decode(std::string s) {
    std::regex rFloat(R"(-?[0-9]+[.]?[eE]?-?+?[0-9]*)");
    std::regex rColor3("([0-9a-fA-F]{6})");
    std::regex rColor4("([0-9a-fA-F]{8})");
    std::smatch result;

    // glm::vector
    if(s.front() == '<' and s.back() == '>') { // * mathutilsVector:<Vector (1.0000, 2.0000, 3.0000, 4.0000)>
        std::regex_iterator<std::string::iterator> rit(s.begin(), s.end(), rFloat);
        std::regex_iterator<std::string::iterator> rend;
        glm::vec4 out(0);
        u32 i = 0;
        while(rit != rend and i < 4) {
            out[i++] = stof(rit->str());
            ++rit;
        }
        return out;
    }
    if(s == "[]" or s == "{} ")
        return "";
    // array of simple types
    if(((s.front() == '{' and s.back() == '}') or (s.front() == '[' and s.back() == ']'))) {
        auto bigSubstr = s.substr(1, s.size() - 2);
        size_t posA = 0;
        size_t posB = 0;
        while(posB != std::string::npos) {
            posB = bigSubstr.find(",", posA + 1);
            auto subs = bigSubstr.substr(posA, posB - posA);
            boost::trim(subs);
            boost::replace_all(subs, "'", "");
            push(subs);
            posA = s.find(",", posB + 1);
        }
        return "";
    }

    if(std::regex_match(s, rFloat)) {
        return std::stof(s);
    }
    if(std::regex_search(s, result, rColor4)) {
        return (u32)std::stoul(result[1], nullptr, 16);
    }
    if(std::regex_search(s, result, rColor3)) {
        return (u32)std::stoul(std::string(result[1]) + "ff"s, nullptr, 16);
    }

    if(s == "yes" or s == "True" or s == "true")
        return true;
    if(s == "no" or s == "False" or s == "false")
        return false;

    return s;
}

void Yaml::save(const std::string& filename) const {
    std::fstream file(filename, std::fstream::out);
    for(auto& it : container) { it.printToStream(file, "    ", "", m_isArray); }
    file << "\n";
    file.close();
}

void Yaml::print(bool isPartOfArray) const {
    for(auto& it : container) {
        it.printToStream(std::cout, "    ", "  ", m_isArray);
        // printToStream(std::cout, "  ", "  ", isPartOfArray);
    }
}

void Yaml::printOnlyThis(bool isPartOfArray) const {
    std::cout << (isPartOfArray ? "-" : m_key) << ": " << string() << "\n";
    for(auto& it : container) {
        std::cout << "    " << it.key() << ": ";
        if(not it.container.empty()) {
            std::cout << (it.m_isArray ? "[]" : "{}");
        }
        else
            std::cout << it.string();
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void Yaml::printToStream(std::ostream& output, std::string indent, std::string indentation, bool isArrayItem) const {
    if(isArrayItem) {
        auto withMinus = indent;
        withMinus[withMinus.size() - 2] = '-';
        output << indentation << withMinus;
        if(string().size())
            output << string() << std::endl;

        bool printWithoutIndent = string().empty();
        for(auto& it : container) {
            it.printToStream(output, indent, printWithoutIndent ? "" : (indentation + indent), m_isArray);
            printWithoutIndent = false;
        }
    }
    else {
        output << indentation << m_key << ": " << string() << "" << std::endl;
        for(auto& it : container) {
            it.printToStream(output, indent, indentation + (m_isArray ? "" : indent), m_isArray);
        }
    }
}

struct Line
{
    int depth;
    std::string key;
    std::string value;
    bool isArrayElement {false};
    Line(const std::string& line) {
        countSpaces(line);
        splitLine(line);
    }

    void countSpaces(const std::string& s) {
        depth = 0;
        for(auto& it : s) {
            if(it == '-') {
                //! ++depth; // it's easiet to traverse when array notifier has different depth
                isArrayElement = true;
            }
            else if(it == ' ')
                ++depth;
            else
                return;
        }
    }
    void splitLine(const std::string& s) {
        auto res = s.find(':');
        if(res == std::string::npos) { // then weve got only value
            value = s.substr(depth);
            boost::trim(value);
        }
        else {
            key = s.substr(depth, res - depth);
            if(auto valStart = s.find_first_not_of(' ', res + 1); valStart != std::string::npos)
                value = s.substr(valStart);
            boost::trim(key);
            boost::trim(value);
        }
    }
};

class YamlLoader
{
private:
    Yaml& m_root;
    std::fstream file;
    std::vector<Line> lines;
    int m_currentLine {0};
    std::string m_filename;

public:
    YamlLoader(const std::string& filename, Yaml& yaml) : m_root(yaml) {
        m_filename = filename;
        file.open(m_filename, std::fstream::in);
    }
    ~YamlLoader() {
        file.close();
    }
    bool isCommentOrEmpty(const std::string&) const;
    void run();
    void fill(Yaml& node, int expectedDepth = 0, bool ignoreArrayElement = false);
};

void Yaml::load(const std::string& filename) try {
    console_prefix("YAML");
    YamlLoader loader(filename, *this);
    console.log(filename);
    loader.run();
}
catch(...) {
    console.error("exception caught for:", filename);
}

bool YamlLoader::isCommentOrEmpty(const std::string& s) const {
    for(auto& it : s)
        if(it == '#')
            return true;
        else if(it != ' ')
            return false;
    return true;
}

void YamlLoader::run() {
    if(not file.is_open()) {
        console.error("No such file:", m_filename);
        return;
    }

    std::string buff;
    while(!file.eof()) {
        getline(file, buff);
        if(not isCommentOrEmpty(buff)) {
            lines.emplace_back(buff);
        }
    }

    if(not lines.empty()) {
        auto& line = lines[m_currentLine];
        m_root.m_isArray = line.isArrayElement;
        fill(m_root, m_root.m_isArray ? 1 : 0, line.isArrayElement and line.key.size());
    }
}

// ! and filler here!
void YamlLoader::fill(Yaml& nodeToFill, int expectedDepth, bool ignoreArrayElement) {
    Yaml* childNode = nullptr;

    while(m_currentLine < lines.size()) {
        auto& line = lines[m_currentLine];
        // console.log("\n line {", line.key, line.depth, "}", expectedDepth, ".", line.isArrayElement, ignoreArrayElement);

        if(line.depth < (ignoreArrayElement ? expectedDepth - 1 : expectedDepth)) { //* finish filling node
            return;
        }
        else if(line.depth
                > (ignoreArrayElement ? expectedDepth - 1 : expectedDepth)) { //* new indent block, fill last child
            // console.log("\t fill new node", childNode->key());
            fill(*childNode, line.depth);
        }
        else { // inside current block
            if(line.key.size()
               and (ignoreArrayElement or not line.isArrayElement) /* and line.value.size() */) { //* key: value
                // console.log("\t creating:", line.key, line.value, "for ", nodeToFill.key());
                childNode = &(nodeToFill.push(line.key, line.value));
                if(ignoreArrayElement) {
                    ignoreArrayElement = false;
                }
                m_currentLine++;
            }
            else if(line.key.empty() and line.isArrayElement) { //* - value
                // console.log("\t simple array element for", nodeToFill.key());
                nodeToFill.m_isArray = true;
                childNode = &(nodeToFill.push(line.value));
                m_currentLine++;
            }
            else if(line.key.size() and line.isArrayElement) { //* - key: value - unnamed node
                nodeToFill.m_isArray = true;
                // console.log("\t node array() element for", nodeToFill.key());
                fill((nodeToFill.push("")), line.depth + 1,
                     true); //* array nodes have two depths: pne for '-' and second for value
            }
        }
    }
}
