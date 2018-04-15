#pragma once
#include "Includes.hpp"
#include "Logging.hpp"
#include <iosfwd>
#include <boost/variant.hpp>
#include <LinearMath/btVector3.h>

/**
 *  http://stackoverflow.com/questions/24628099/group-class-template-specializations grupowanie typów, specjalizacje
 *  dla grup
 *  SFINE & type_triats  :http://eli.thegreenplace.net/2014/sfinae-and-enable_if/
 *
 *  https://rmf.io/cxx11/almost-static-if/ :best?
 */

struct Yaml;

typedef std::vector<float> floatVec;
typedef std::vector<glm::vec4> vec4Vec;
typedef boost::variant<
        std::string,             // 0
        u32,                     // bitfields? colors etc
        double,                  // 1
        glm::vec4,               // 2
        floatVec,                // 3, czy jest jakiś sens na te typy?
        bool,                    //
        std::function<void(void)>// 5
    > Variants;
namespace {
    // template <typename... Condition>
    // using EnableIf = typename std::enable_if<all<Condition...>::value, detail::enabler>::type;

    template<class T>
    struct is_from_true_types: std::false_type {};

    template<>
    struct is_from_true_types<std::string>: std::true_type {};
    template<>
    struct is_from_true_types<double>: std::true_type {};
    template<>
    struct is_from_true_types<glm::vec4>: std::true_type {};
    template<>
    struct is_from_true_types<floatVec>: std::true_type {};
    // template<>
    // struct is_from_true_types<bool>: std::true_type {};
    template<>
    struct is_from_true_types<std::function<void(void)>>: std::true_type {};

}

class Yaml {
private:
    std::string m_key;
    Variants m_value;
    std::vector<Yaml> container;
    void printToStream(std::ostream& output, std::string indent="  ", std::string indentation="", bool isPartOfArray=false) const;
public:
    bool isArray {false};
    Yaml() = default;
    Yaml(const std::string &key, const Variants &val) : m_key(key), m_value(val) {}
    Yaml(const Yaml&) = default;
    Yaml(Yaml&&) = default;
    ~Yaml() { }

    Yaml(const std::string& filepath){
        m_key = "root";
        load(filepath);
    }

    Yaml& push(const Yaml &node){
        container.push_back(node);
        return container.back();
    }
    Yaml& push(const std::string &key, const std::string  &val){
        container.emplace_back(key, decode(val));
        return container.back();
    }
    Yaml& push(const std::string &val){
        container.emplace_back(std::to_string(container.size()), decode(val));
        return container.back();
    }

    Yaml& operator [] (const std::string &s){
        for(auto &it : container)
            if(it.m_key == s) return it;

        return push(s, "");
    }
    Yaml& operator [] (u32 i){ // what if i is far bigger than the size?
        if(i >= container.size())
            push("");
        return container[i];
    }
    const Yaml& operator [] (const std::string &s) const {
        for(const auto &it : container)
            if(it.m_key == s) return it;
        // error(s, "in", m_key, "doesn't exist");
        return *this;
    }
    const Yaml& operator [] (u32 i) const {
        return container.at(i);
    }

    bool has(const std::string &s) const {
        for(const auto &it : container)
            if(it.m_key == s) return true;

        return false;
    }

    const std::string& key() const {
        return m_key;
    }

    auto begin(){
        return container.begin();
    }
    auto begin() const {
        return container.begin();
    }
    auto end(){
        return container.end();
    }
    auto end() const {
        return container.end();
    }
    auto size() const {
        return container.size();
    }

    Variants decode(std::string s);
    void load(const std::string& filepath);
    void save(const std::string& filepath) const;
    void print() const;

    template<typename T, typename = std::enable_if_t<is_from_true_types<T>::value>>
    void operator = (const T &val){
        m_value = val;
    }
    template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value && !std::is_same<T, double>::value>>
    void operator = (T val){
        m_value = (double)val;
    }
    void operator = (const std::string &val){
        m_value = val;
    }
    void operator = (const Yaml &&node){
        m_value = node.m_value;
        container = node.container;
        isArray = node.isArray;
    }
    void operator = (const Yaml &node){
        m_value = node.m_value;
        container = node.container;
        isArray = node.isArray;
    }
    void operator = (glm::vec2 v){
        m_value = glm::vec4(v, 0, 0);
    }

    template<
        typename T,
        typename = std::enable_if_t<is_from_true_types<T>::value>>
    T& as(){
        return boost::get<T>(m_value);
    }
    template<
        typename T,
        typename = std::enable_if_t<std::is_arithmetic<T>::value && !std::is_same<T, double>::value>>
    T as() const {
        return boost::get<double>(m_value);
    }

    std::string string() const ;
    std::string debugString() const ;


    // operator glm::vec2() const {
    //     return vec2();
    // }
    operator glm::vec3() const {
        return vec3();
    }
    operator glm::vec4() const {
        return vec4();
    }
    // operator glm::quat() const {
    //     return quat();
    // }
    // operator btVector3() const {
    //     return btVec();
    // }
    operator float() const {
        return number();
    }
    // operator double() const {
    //     return number();
    // }
    // operator bool() const {
    //     return boolean();
    // }

    glm::vec4 vec4() const try {
        return boost::get<glm::vec4>(m_value);
    } catch (...){
        error(m_key, "not exists");
        return glm::vec4();
    }
    glm::vec4 vec31() const try {
        return glm::vec4(boost::get<glm::vec4>(m_value).xyz(), 1);
    } catch (...){
        error(m_key, "not exists");
        return glm::vec4();
    }
    glm::vec4 vec30() const try {
        return glm::vec4(boost::get<glm::vec4>(m_value).xyz(), 0);
    } catch (...){
        error(m_key, "not exists");
        return glm::vec4();
    }
    glm::vec3 vec3() const try {
        return boost::get<glm::vec4>(m_value).xyz();
    } catch (...){
        error(m_key, "not exists");
        return glm::vec3();
    }
    glm::vec2 vec2() const try {
        return boost::get<glm::vec4>(m_value).xy();
    } catch (...){
        error(m_key, "not exists");
        return glm::vec2();
    }
    glm::quat quat() const try {
        auto val = boost::get<glm::vec4>(m_value);
        return glm::quat(val.x, val.y, val.z, val.w);
    } catch (...){
        error(m_key, "not exists");
        return glm::quat();
    }
    btVector3  btVec() const try {
        auto val = boost::get<glm::vec4>(m_value);
        return btVector3(val.x, val.y, val.z);
    } catch (...){
        error(m_key, "not exists");
        return btVector3();
    }
    u32 color() const try {
        return boost::get<u32>(m_value);
    } catch (...){
        error(m_key, "not exists");
        return 0;
    }

    double number() const try {
        return boost::get<double>(m_value);
    } catch (...){
        error(m_key, "not exists");
        return -1;
    }
    void execute() try {
        boost::get<std::function<void(void)>>(m_value)();
    } catch (...){
        error(m_key, "not exists");
    }
    bool boolean() const try {
        return boost::get<bool>(m_value);
    } catch (...){
        error(m_key, "not exists");
        return false;
    }
};
