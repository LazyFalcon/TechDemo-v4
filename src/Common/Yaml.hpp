#pragma once
#include <LinearMath/btVector3.h>
#include <boost/optional.hpp>
#include <boost/variant.hpp>
#include <iosfwd>
#include "Logger.hpp"

/**
 *  http://stackoverflow.com/questions/24628099/group-class-template-specializations grupowanie typów, specjalizacje
 *  dla grup
 *  SFINE&  type_triats  :http://eli.thegreenplace.net/2014/sfinae-and-enable_if/
 *
 *  https://rmf.io/cxx11/almost-static-if/ :best?
 */

struct Yaml;

typedef boost::variant<std::string, // 0
                       u32,         // bitfields? colors etc
                       float,       // 1
                       glm::vec4,   // TODO: maybe small vector instead? it will better handle vec2, etc cases?
                       // floatVec,                // 3, czy jest jakiś sens na te typy?
                       // stringVec,                // 3, czy jest jakiś sens na te typy?
                       bool,                     //
                       std::function<void(void)> // ? why store callbacks? were there reason for this?
                       >
    Variants;
namespace
{
// template <typename... Condition>
// using EnableIf = typename std::enable_if<all<Condition...>::value, detail::enabler>::type;

template<class T>
struct is_from_true_types : std::false_type
{};

template<>
struct is_from_true_types<std::string> : std::true_type
{};
template<>
struct is_from_true_types<float> : std::true_type
{};
template<>
struct is_from_true_types<glm::vec4> : std::true_type
{};
// template<>
// struct is_from_true_types<floatVec>: std::true_type {};
// template<>
// struct is_from_true_types<bool>: std::true_type {};
template<>
struct is_from_true_types<std::function<void(void)>> : std::true_type
{};

}

class Yaml
{
private:
    std::vector<Yaml> container;
    std::string m_key;
    Variants m_value;
    Yaml* m_parent {nullptr};
    void printToStream(std::ostream& output, std::string indent = "  ", std::string indentation = "",
                       bool isPartOfArray = false) const;
    Variants decode(std::string s);

public:
    bool m_isArray {false};
    Yaml() = default;
    Yaml(const std::string& key, const std::string& val) : m_key(key), m_value(decode(val)) {}
    // Yaml(const std::string& key, const Variants& val) : m_key(key), m_value(val) {}
    Yaml(const Yaml&) = default;
    Yaml(Yaml&&) = default;
    ~Yaml() {}

    Yaml(const std::string& filepath) {
        m_key = "root";
        load(filepath);
    }
    bool isDict() const {
        return container.size() and not m_isArray;
    }
    bool isArray() const {
        return container.size() and m_isArray;
    }
    const std::vector<Yaml>& getContainer() const {
        return container;
    }

    Yaml& push(const Yaml& node);
    Yaml& push(const std::string& key, const std::string& val);
    Yaml& push(const std::string& val);

    std::string getParents() const {
        Yaml* parent = m_parent;
        std::string out = m_key;
        while(parent != nullptr) {
            out = parent->m_key + "/" + out;
            parent = parent->m_parent;
        }
        return out;
    }

    Yaml& operator[](const std::string& s) {
        for(auto& it : container)
            if(it.m_key == s)
                return it;
        // console.error(s, "doesn't exists in", getParents());
        return push(s, "");
    }
    Yaml& operator[](u32 i) { // what if i is far bigger than the size?
        if(i >= container.size())
            push("");
        return container[i];
    }
    const Yaml& get(const std::string& s) const {
        for(const auto& it : container)
            if(it.m_key == s) {
                // console.log(s, "path:", getParents());
                return it;
            }

        console.error(s, "doesn't exists in", m_key);
        return *this;
    }
    const Yaml& operator[](const std::string& s) const {
        return get(s);
    }
    const Yaml& operator[](u32 i) const {
        return container.at(i);
    }

    bool has(const std::string& s) const {
        for(const auto& it : container)
            if(it.m_key == s)
                return true;

        return false;
    }

    template<typename UnaryPredicate>
    boost::optional<const Yaml&> find(UnaryPredicate predicate) const {
        for(auto& it : container) {
            if(predicate(it))
                return it;
        }
        return boost::none;
    }

    template<typename UnaryPredicate>
    void for_each(UnaryPredicate predicate) const {
        for(auto& it : container) { predicate(it); }
    }

    template<typename UnaryPredicate>
    void for_each(const std::string& key, UnaryPredicate predicate) const {
        if(not has(key))
            return;
        get(key).for_each(predicate);
    }

    bool operator==(const std::string& compareTo) const {
        return compareTo == string();
    }

    bool operator!=(const std::string& compareTo) const {
        return compareTo != string();
    }

    const std::string& key() const {
        return m_key;
    }

    auto begin() {
        return container.begin();
    }
    auto begin() const {
        return container.cbegin();
    }
    auto end() {
        return container.end();
    }
    auto end() const {
        return container.cend();
    }
    auto size() const {
        return container.size();
    }
    bool empty() const {
        return container.empty();
    }

    void load(const std::string& filepath);
    void save(const std::string& filepath) const;
    void print(bool isPartOfArray = false) const;
    void printOnlyThis(bool isPartOfArray = false) const;

    void operator=(bool val) {
        m_value = val;
    }
    template<typename T, typename = std::enable_if_t<is_from_true_types<T>::value>>
    void operator=(const T& val) {
        m_value = val;
    }
    template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value && !std::is_same<T, float>::value>>
    void operator=(T val) {
        m_value = (float)val;
    }
    void operator=(const std::string& val) {
        m_value = val;
    }
    void operator=(const Yaml&& node) {
        m_value = node.m_value;
        m_parent = node.m_parent;
        container = node.container;
        m_isArray = node.m_isArray;
    }
    void operator=(const Yaml& node) {
        m_value = node.m_value;
        m_parent = node.m_parent;
        container = node.container;
        m_isArray = node.m_isArray;
    }
    void operator=(glm::vec2 v) {
        m_value = glm::vec4(v, 0, 0);
    }

    template<typename T, typename = std::enable_if_t<is_from_true_types<T>::value>>
    T& as() {
        return boost::get<T>(m_value);
    }
    template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value && !std::is_same<T, float>::value>>
    T as() const {
        return boost::get<float>(m_value);
    }

    std::string string() const;
    std::string debugString() const;

    std::vector<std::string> strings() const;
    std::vector<float> numbers() const;

    glm::vec4 vec4() const try { return boost::get<glm::vec4>(m_value); }
    catch(...) {
        console.error("not exists as", m_key);
        return glm::vec4();
    }
    glm::vec4 vec31() const try { return glm::vec4(boost::get<glm::vec4>(m_value).xyz(), 1); }
    catch(...) {
        console.error("not exists as", m_key);
        return glm::vec4();
    }
    glm::vec4 vec30() const try { return glm::vec4(boost::get<glm::vec4>(m_value).xyz(), 0); }
    catch(...) {
        console.error("not exists as", m_key);
        return glm::vec4();
    }
    glm::vec3 vec3() const try { return boost::get<glm::vec4>(m_value).xyz(); }
    catch(...) {
        console.error("not exists as", m_key);
        return glm::vec3();
    }
    glm::vec2 vec2() const try { return boost::get<glm::vec4>(m_value).xy(); }
    catch(...) {
        console.error("not exists as", m_key);
        return glm::vec2();
    }
    glm::quat quat() const try {
        auto val = boost::get<glm::vec4>(m_value);
        return glm::quat(val.x, val.y, val.z, val.w);
    }
    catch(...) {
        console.error("not exists as", m_key);
        return glm::quat();
    }
    btVector3 btVec() const try {
        auto val = boost::get<glm::vec4>(m_value);
        return btVector3(val.x, val.y, val.z);
    }
    catch(...) {
        console.error("not exists as", m_key);
        return btVector3();
    }
    u32 color() const try { return boost::get<u32>(m_value); }
    catch(...) {
        console.error("not exists as", m_key);
        return 0;
    }

    float number() const try { return boost::get<float>(m_value); }
    catch(...) {
        console.error("not exists as", m_key);
        return -1;
    }
    void execute() try { boost::get<std::function<void(void)>>(m_value)(); }
    catch(...) {
        console.error("not exists as", m_key);
    }
    bool boolean() const try { return boost::get<bool>(m_value); }
    catch(...) {
        console.error("not exists as", m_key);
        return false;
    }
};
