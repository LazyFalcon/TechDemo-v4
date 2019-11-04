#include "core.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

glm::vec4 colorHex(uint32_t hexVal) {
    return glm::vec4(((hexVal >> 24) & 0xFF) / 255.f, ((hexVal >> 16) & 0xFF) / 255.f, ((hexVal >> 8) & 0xFF) / 255.f,
                     (hexVal & 0xFF) / 255.f);
}

int clamp(int val, int a, int b) {
    return std::max(std::min(val, b), a);
}

void setColumn(glm::mat4& mat, int num, glm::vec4 vec) {
    mat[num][0] = vec[0];
    mat[num][1] = vec[1];
    mat[num][2] = vec[2];
    mat[num][3] = vec[3];
}
#ifdef USE_BULLET

std::string to_string(const btVector3& v) {
    return string_formatted(v[0]) + " " + string_formatted(v[1]) + " " + string_formatted(v[2]);
}
// std::string to_string(const btTransform &t){

// static float v[16]={1};

// t.getOpenGLMatrix(v);
//     return glm::to_string(convert(t) );
// }

#endif
glm::vec4 cross(glm::vec4 const& x, glm::vec4 const& y) {
    return glm::vec4(glm::cross(glm::vec3(x.xyz()), glm::vec3(y.xyz())), 0);
}
std::string string_formatted(double f) {
    // std::ostringstream out;
    // out.precision(2);
    // out << std::setprecision(1) << f;
    // out<<f;
    char buffer[10];
    sprintf(buffer, "%.2f", f);

    return std::string(buffer);
}
std::string toString(float f) {
    // std::ostringstream out;
    // out.precision(2);
    // out << std::setprecision(1) << f;
    // out<<f;
    char buffer[10];
    sprintf(buffer, "%.2f", f);

    return std::string(buffer);
}
std::string to_string(const glm::vec4& v) {
    return string_formatted(v[0]) + " " + string_formatted(v[1]) + " " + string_formatted(v[2]) + " "
           + string_formatted(v[3]);
}
std::string to_string(const glm::vec3& v) {
    return string_formatted(v[0]) + " " + string_formatted(v[1]) + " " + string_formatted(v[2]);
}

glm::vec4 plane(glm::vec3 normal, glm::vec4 point) {
    return glm::vec4(normal, -glm::dot(normal, point.xyz()));
}
glm::vec4 plane(glm::vec4 normal, glm::vec4 point) {
    return glm::vec4(glm::normalize(glm::vec3(normal.xyz())), -glm::dot(glm::normalize(normal), point));
}
glm::vec4 plane(glm::vec4 v1, glm::vec4 v2, glm::vec4 v3) {
    glm::vec3 normal = glm::normalize(glm::triangleNormal(v1.xyz(), v2.xyz(), v3.xyz()));
    return glm::vec4(normal, -glm::dot(normal, v3.xyz()));
}

float sign(float x) {
    if(x < 0.f)
        return -1.f;
    else if(x > 0.f)
        return 1.f;
    return 0.f;
}
float period(float val, float a, float b) {
    if(val >= b)
        val -= b - a;
    else if(val < a)
        val += b - a;
    return val;
}
double period(double val) {
    if(val >= dpi)
        return val - dpi2;
    else if(val < -dpi)
        return val + dpi2;
    return val;
}

double sign(double x) {
    if(x < 0.0)
        return -1.0;
    else if(x > 0.0)
        return 1.0;
    return 0.0;
}
double period(double val, double a, double b) {
    if(val >= b)
        val -= b - a;
    else if(val < a)
        val += b - a;
    return val;
}

float circleDistance(float target, float value) {
    float dist = target - value;
    if(dist > pi)
        return pi2 - dist;
    if(dist < -pi)
        return dist - pi2;
    return dist;
}
double circleDistance(double target, double value) {
    double dist = target - value;
    if(dist > dpi)
        return dist - dpi2;
    if(dist < -dpi)
        return dpi2 - dist;
    return dist;
}

double clamp(double x, double minVal, double maxVal) {
    return std::min(std::max(x, minVal), maxVal);
}

float PDreg::operator()(float toReach, float current, float dt) {
    speed = (prev - current) / dt;
    prev = current;

    return (toReach - current) * (dt * kr) - speed * (td * dt);
}

std::list<Statement> statements;
void statement(std::string text, float lifeTime) {
    statements.emplace_back(Statement {text, lifeTime});
}

const std::string getName(const std::string& name) {
    fs::path p(name);
    return p.stem().string();
}
const std::string getExt(const std::string& name) {
    fs::path p(name);
    return p.extension().string();
}
const std::string cutExt(const std::string& name) {
    fs::path p(name);
    return p.parent_path().generic_string() + getName(name);
}
const std::string getPath(const std::string& name) {
    fs::path p(name);
    return p.generic_string();
}

bool isFile(const std::string& file) {
    return fs::exists(file);
}
// TODO: przerobic do bardziej strawnego formatu
bool findFile(const std::string& from, const std::string& name, const std::string& params, std::string& ref) {
    bool recursive = std::string::npos != params.find("-r"s);
    bool cutExt = std::string::npos == params.find("-ext"s);
    const std::string exactName = cutExt ? getName(name) : name;
    try {
        fs::path p(from);
        if(recursive) {
            auto dir_it = fs::recursive_directory_iterator(p);
            for(dir_it; dir_it != fs::recursive_directory_iterator(); dir_it++) {
                if(fs::is_directory(dir_it->status()))
                    continue;
                if(cutExt and exactName == (*dir_it).path().stem().string()) {
                    ref = (*dir_it).path().generic_string();
                    return true;
                }
                else if(exactName == (*dir_it).path().filename().string()) {
                    ref = (*dir_it).path().generic_string();
                    return true;
                }
            }
        }
        else {
            auto dir_it = fs::directory_iterator(p);
            for(dir_it; dir_it != fs::directory_iterator(); dir_it++) {
                if(exactName == (*dir_it).path().stem().string()) {
                    ref = (*dir_it).path().generic_string();
                    return true;
                }
            }
        }
        return false;
    }
    catch(const fs::filesystem_error& ex) {
        console.error("fs::ex: ", ex.what());
        std::cin.ignore();
    }
    return false;
}

std::vector<std::string> filter(const std::vector<std::string>& what, const std::string& toMatch) {
    std::vector<std::string> output;
    std::copy_if(what.begin(), what.end(), std::back_inserter(output),
                 [&](const std::string& str) { return str.find(toMatch) != std::string::npos; });

    return output;
}

std::vector<std::string> listDirectory(const std::string& dir) {
    std::vector<std::string> out;
    // console.log("[ sDFs ] ", name, p.parent_path().string());
    try {
        fs::path p(dir);
        auto dir_it = fs::directory_iterator(p);
        for(dir_it; dir_it != fs::directory_iterator(); dir_it++) {
            out.push_back((*dir_it).path().filename().string());
        }
        return out;
    }
    catch(const fs::filesystem_error& ex) {
        console.error("fs:: ex: ", ex.what());
        std::cin.ignore();
    }
    return out;
}

bool writeBinary(const std::string& path, i8* data, u32 size) {
    std::ofstream file;
    file.open(path, std::ios::binary | std::ios::out | std::ios::trunc);

    if(file.is_open()) {
        file.write(data, size);
    }
    else {
        console.error(__FUNCTION__, "Unable to open file:", path);
        return false;
    }
    file.close();

    return true;
}
bool readBinary(const std::string& path, i8* data, u32& size) {
    std::ifstream file;
    size = 0;
    file.open(path, std::ios::binary | std::ios::in);
    if(file.good()) {
        file.seekg(0, std::ios::end);
        size = file.tellg();
        file.seekg(0, std::ios::beg);

        data = new i8[size];
        file.read(data, size);
    }
    else {
        console.error(__FUNCTION__, "Unable to open file:", path);
        return false;
    }
    file.close();
    return true;
}

bool writeBinary(const std::string& path, std::function<void(std::ofstream&)>& fun) {
    std::ofstream file;
    file.open(path, std::ios::binary | std::ios::out | std::ios::trunc);

    if(file.is_open()) {
        fun(file);
    }
    else {
        console.error(__FUNCTION__, "Unable to open file:", path);
        return false;
    }
    file.close();

    return true;
}
bool readBinary(const std::string& path, std::function<void(i8*, u32)>& fun) {
    std::ifstream file;
    i32 size = 0;
    file.open(path, std::ios::binary | std::ios::in);
    if(file.good()) {
        file.seekg(0, std::ios::end);
        size = file.tellg();
        file.seekg(0, std::ios::beg);

        i8* data = new i8[size];
        file.read(data, size);

        fun(data, size);
    }
    else {
        console.error(__FUNCTION__, "Unable to open file:", path);
        return false;
    }
    file.close();
    return true;
}

std::vector<std::string> splitString(const std::string& text, char token) {
    std::vector<std::string> out;
    std::istringstream f(text);
    std::string s;
    while(getline(f, s, token)) { out.push_back(s); }
    return out;
}
