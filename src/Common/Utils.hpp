#pragma once
#include "PMK.hpp"
#include <fstream>

glm::vec4 colorHex(uint32_t hexVal);
// glm::vec4 colorHex(uint64_t hexVal);
int clamp(int val, int a, int b);

void setColumn(glm::mat4 &mat, int num, glm::vec4 vec);

#ifdef USE_BULLET
inline glm::vec3 convert(const btVector3& v){
    return glm::vec3(v[0], v[1], v[2]);
}
inline glm::vec4 convert(const btVector3& v, float z){
    return glm::vec4(v[0], v[1], v[2], z);
}
inline glm::quat convert(const btQuaternion& q){
    return glm::quat(q.x(), q.y(), q.z(), q.w());
}
inline glm::mat4 convert(const btTransform &trans){
    btScalar matrix[16];

    trans.getOpenGLMatrix(matrix);

    return glm::mat4(
        matrix[0], matrix[1], matrix[2], matrix[3],
        matrix[4], matrix[5], matrix[6], matrix[7],
        matrix[8], matrix[9], matrix[10], matrix[11],
        matrix[12], matrix[13], matrix[14], 1);
}
inline btVector3 convert(const glm::vec4& v){
    return btVector3(v[0], v[1], v[2]);
}
inline btVector3 convert(const glm::vec3& v){
    return btVector3(v[0], v[1], v[2]);
}
inline btQuaternion convert(const glm::quat& q){
    return btQuaternion(q.x, q.y, q.z, q.w);
}
inline btTransform convert(const glm::mat4& mat){
    return btTransform(btMatrix3x3(mat[0][0], mat[1][0], mat[2][0],
                                   mat[0][1], mat[1][1], mat[2][1],
                                   mat[0][2], mat[1][2], mat[2][2]), convert(mat[3]));
}
inline btTransform convert(const glm::quat& quat, const glm::vec4& vec){
    return btTransform(btQuaternion(quat.z, quat.y, quat.z, quat.w), convert(vec));
}

std::string to_string(const btVector3 &v);
std::string to_string(const btTransform &t);
#endif

glm::vec4 cross(glm::vec4 const & x, glm::vec4 const & y);
std::string string_formatted(double f);
std::string toString(float f);
std::string to_string(const glm::vec4 &v);
std::string to_string(const glm::vec3 &v);

glm::vec4 plane(glm::vec3 normal, glm::vec4 point);
glm::vec4 plane(glm::vec4 normal, glm::vec4 point);
glm::vec4 plane(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3);

float period(float val, float a, float b);
double period(double val, double a, double b);
double period(double val);
float sign(float x);
double sign(double x);
float circleDistance(float target, float value);
double circleDistance(double target, double value);

double clamp(double x, double minVal, double maxVal);

template<typename T>
void cyclicIncr(T &value, u32 range){
    if(++value >= range) value = 0;
}
template<typename T>
void cyclicDecr(T &value, u32 range){
    if(value <= 0) value = range-1;
    else value--;
}

template <typename T>
bool saveCache(std::string fileName, const std::vector<T> &data){
    std::ofstream file;
    file.open(dataPath + "cache/"+fileName+".cache", std::ios::binary | std::ios::out | std::ios::trunc);

    // if(file.is_open()){
        file.write((char*)data.data(), data.size()*sizeof(T));
    // }
    // else console.log(__FUNCTION__, "Unable to cache file:", fileName);
    file.close();
    return true;
}
template <typename T>
bool loadCache(std::string fileName, std::vector<T> &data){
    std::ifstream file;
    int size=0;
    // try {
        file.open(dataPath + "cache/"+fileName+".cache", std::ios::binary | std::ios::in);
        // if(file.is_open()){
        if(file.good()){
            file.seekg(0, std::ios::end);
            size = file.tellg()/sizeof(T);
            file.seekg(0, std::ios::beg);

            data.resize(size);
            file.read((char*)data.data(), size*sizeof(T));
            file.close();
        }
        else {
            file.close();
            return false;
        }
    return true;
}

bool writeBinary(const std::string &path, i8 *data, u32 size);
bool readBinary(const std::string &path, i8 *data, u32 size);

bool writeBinary(const std::string &path, std::function<void(std::ofstream&)> &fun);
bool readBinary(const std::string &path, std::function<void(i8*, u32)> &fun);

// void listFiles(std::set<std::string>&files, std::string pathName, std::string fileType);
const std::string getName(const std::string &name);
const std::string getExt(const std::string &name);
const std::string cutExt(const std::string &name);
const std::string getPath(const std::string &name);
bool isFile(const std::string &file);
bool findFile(const std::string &from, const std::string &name, const std::string &params, std::string &ref);
std::vector<std::string> filter(const std::vector<std::string>& what, const std::string& toMatch);
std::vector<std::string> listDirectory(const std::string &dir);

class PDreg {
public:
    float kr;
    float td;
    float speed;
    float prev;

    float operator () (float toReach, float current, float dt);
};

struct Statement {
    std::string statement;
    float lifeTime;
};

void statement(std::string text, float lifeTime = 5.f);

std::vector<std::string> splitString(const std::string &text, char token);
