#include "core.hpp"
#include "gl_core_4_5.hpp"
#include "Logging.hpp"
#include "GPUResources.hpp"
#include "ShaderCompiler.hpp"
#include "Texture.hpp"
#include "Utils.hpp"

#include <regex>

void UBO::bind(u32 shader, u32 index, u32 bindingIndex){
    gl::UniformBlockBinding(shader, index, bindingIndex);
    gl::BindBufferRange(gl::UNIFORM_BUFFER, bindingIndex, ID, 0, sizeof(float) * maxSize);
}
void UBO::bind(u32 shader, const std::string &name, u32 bindingIndex){
    u32 index = gl::GetUniformBlockIndex(shader, name.c_str());
    gl::UniformBlockBinding(shader, index, bindingIndex);
    gl::BindBufferRange(gl::UNIFORM_BUFFER, bindingIndex, ID, 0, sizeof(float) * maxSize);
}
void UBO::unbind(){
    gl::BindBuffer(gl::UNIFORM_BUFFER, 0);
}

// ---------------------- UBO ----------------------
void UBO::setup(u32 size){
    maxSize = size;
    gl::GenBuffers(1, &ID);
    gl::BindBuffer(gl::UNIFORM_BUFFER, ID);
    gl::BufferData(gl::UNIFORM_BUFFER, sizeof(float)*maxSize, nullptr, gl::DYNAMIC_DRAW);
    gl::BindBuffer(gl::UNIFORM_BUFFER, 0);
}
void UBO::update(float *data, u32 size){
    gl::BindBuffer(gl::UNIFORM_BUFFER, ID);
    gl::BufferSubData(gl::UNIFORM_BUFFER, 0, sizeof(float)*size, data);
    gl::BindBuffer(gl::UNIFORM_BUFFER, 0);
}

// ---------------------- VBO ----------------------
VBO& VBO::bind(){
    gl::BindBuffer(gl::ARRAY_BUFFER, ID);
    return *this;
}
VBO& VBO::setup(size_t dataSize, bool dynamic){
    maxSize = dataSize;
    drawMode = gl::STATIC_DRAW;
    if(dynamic) drawMode = gl::DYNAMIC_DRAW;

    gl::GenBuffers(1, &ID);
    gl::BindBuffer(gl::ARRAY_BUFFER, ID);
    gl::BufferData(gl::ARRAY_BUFFER, dataSize, nullptr, drawMode);

    return *this;
}
VBO& VBO::setup(void* data, size_t dataSize, bool dynamic){
    drawMode = gl::STATIC_DRAW;
    if(dynamic) drawMode = gl::DYNAMIC_DRAW;

    maxSize = dataSize;

    gl::GenBuffers(1, &ID);
    gl::BindBuffer(gl::ARRAY_BUFFER, ID);
    gl::BufferData(gl::ARRAY_BUFFER, dataSize, data, drawMode);

    return *this;
}
VBO& VBO::update(void *data, size_t dataSize){
    gl::BindBuffer(gl::ARRAY_BUFFER, ID);
    gl::BufferData(gl::ARRAY_BUFFER, maxSize, nullptr, drawMode); /// orphan and create new buffer
    gl::BufferData(gl::ARRAY_BUFFER, dataSize, data, drawMode); /// fill new buffer

    return *this;
}
void VBO::clear(){
    gl::DeleteBuffers(1, &ID);
}

VBO& VBO::attrib(){
    numBuffer++;
    gl::EnableVertexAttribArray(numBuffer);
    return *this;
}
VBO& VBO::attrib(u32 n){
    numBuffer = n;
    gl::EnableVertexAttribArray(numBuffer);
    return *this;
}
VBO& VBO::pointer(int e, u32 dataType_, u32 stride, void *pointer_){
    dataType = dataType_;
    gl::VertexAttribPointer(numBuffer, e, dataType, false, stride, pointer_);

    return *this;
}
/*
For glVertexAttribPointer, if normalized​ is set to GL_TRUE​, it indicates that values stored in an integer format are to be mapped to the range [-1,1] (for signed values) or [0,1] (for unsigned values) when they are accessed and converted to floating point. Otherwise, values will be converted to floats directly without normalization.
*/
VBO& VBO::pointer_float(int elements, u32 stride, void *pointer_){
    dataType = gl::FLOAT;
    gl::VertexAttribPointer(numBuffer, elements, dataType, false, stride, pointer_);

    return *this;
}
/*
For glVertexAttribIPointer, only the integer types GL_BYTE​, GL_UNSIGNED_BYTE​, GL_SHORT​, GL_UNSIGNED_SHORT​, GL_INT​, GL_UNSIGNED_INT​ are accepted. Values are always left as integer values.
*/
VBO& VBO::pointer_integer(int elements, u32 stride, void *pointer_){
    dataType = gl::UNSIGNED_INT;
    gl::VertexAttribIPointer(numBuffer, elements, dataType, stride, pointer_);

    return *this;
}
VBO& VBO::pointer_color(u32 stride, void *pointer_){
    int elements = 4; /// TODO: gl::BGRA żeby nie trzbea było robić swizzla
    dataType = gl::UNSIGNED_BYTE;
    gl::VertexAttribPointer(numBuffer, elements, dataType, true, stride, pointer_);

    return *this;
}
VBO& VBO::divisor(u32 d){
    gl::VertexAttribDivisor(numBuffer, d);

    return *this;
}
void VBO::operator ()(){
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
}

// ---------------------- IBO ----------------------
void IBO::setup(std::vector<u32> &indices){
    gl::GenBuffers(1, &ID);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ID);
    gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, sizeof(u32)*indices.size(), indices.data(), gl::STATIC_DRAW);
}
void IBO::clear(){
    gl::DeleteBuffers(1, &ID);
}
void IBO::bind(){
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ID);
}
// ---------------------- VAO ----------------------
VAO& VAO::setup(){
    gl::GenVertexArrays(1, &ID);
    gl::BindVertexArray(ID);
    return *this;
}
void VAO::clear(){
    if(not ID) return;
    for(u32 i=0; i<vboCount; i++)
        vbo[i].clear();
    ibo.clear();
    gl::DeleteVertexArrays(1, &ID);
}
void VAO::operator()(){
    gl::BindVertexArray(0);
    gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, 0);
    gl::BindBuffer(gl::ARRAY_BUFFER, 0);
}
void VAO::bind(){
    gl::BindVertexArray(ID);
}
void VAO::unbind(){
    gl::BindVertexArray(0);
}
VBO& VAO::addBuffer(void *data, size_t size){
    vbo[vboCount].setup(data, size, false);
    vboCount++;
    return vbo[vboCount-1];
}
VAO& VAO::addBuffer(std::vector<float> &buffer, u32 numOfElements){
    vbo[vboCount].setup(buffer).attrib(vboCount).pointer_float(numOfElements).divisor(0);
    vboCount++;
    return *this;
}
VAO& VAO::addBuffer(std::vector<glm::vec2> &buffer){
    vbo[vboCount].setup(buffer).attrib(vboCount).pointer_float(2).divisor(0);
    vboCount++;
    return *this;
}
VAO& VAO::addBuffer(std::vector<glm::vec3> &buffer){
    vbo[vboCount].setup(buffer).attrib(vboCount).pointer_float(3).divisor(0);
    vboCount++;
    return *this;
}
VAO& VAO::addBuffer(std::vector<glm::vec4> &buffer){
    vbo[vboCount].setup(buffer).attrib(vboCount).pointer_float(4).divisor(0);
    vboCount++;
    return *this;
}
VAO& VAO::addBuffer(std::vector<u32> &buffer){
    ibo.setup(buffer);
    triangleCount = buffer.size()/3;
    info("VAO "+std::to_string(ID), "triangles:", triangleCount);
    return *this;
}

// ---------------------- Shader ----------------------
Shader::~Shader(){
    if(binded){
        gl::BindTexture(gl::TEXTURE_2D, 0);
        // gl::BindVertexArray(0);
        gl::UseProgram(0);
    }
}
Shader& Shader::bind(){
    gl::UseProgram(ID);
    binded = true;
    return *this;
}
Shader& Shader::texture(const GLint uniformId, u32 texture, u32 id){
    gl::Uniform1i(uniformId, id);
    gl::ActiveTexture(gl::TEXTURE0 + id);
    gl::BindTexture(gl::TEXTURE_2D, texture);
    return *this;
}
Shader& Shader::texture(const std::string &name, u32 texture, u32 id){
    gl::Uniform1i(gl::GetUniformLocation(ID, name.c_str()), id);
    gl::ActiveTexture(gl::TEXTURE0 + id);
    gl::BindTexture(gl::TEXTURE_2D, texture);
    return *this;
}
Shader& Shader::texture(const std::string &name, Texture &texture, u32 id){
    gl::Uniform1i(gl::GetUniformLocation(ID, name.c_str()), id);
    gl::ActiveTexture(gl::TEXTURE0 + id);
    gl::BindTexture(texture.type, texture.ID);
    return *this;
}
Shader& Shader::atlas(const std::string &name, u32 texture, u32 id){
    gl::Uniform1i(gl::GetUniformLocation(ID, name.c_str()), id);
    gl::ActiveTexture(gl::TEXTURE0 + id);
    gl::BindTexture(gl::TEXTURE_2D_ARRAY, texture);
    return *this;
}
Shader& Shader::cubeMap(const std::string &name, u32 texture, u32 id){
    gl::Uniform1i(gl::GetUniformLocation(ID, name.c_str()), id);
    gl::ActiveTexture(gl::TEXTURE0 + id);
    gl::BindTexture(gl::TEXTURE_CUBE_MAP, texture);
    return *this;
}
i32 Shader::location(const std::string &name){
    return gl::GetUniformLocation(ID, name.c_str());
}
i32 Shader::uboLocation(const std::string &name){
    return gl::GetUniformBlockIndex(ID, name.c_str());
}

Shader& Shader::ubo(const char *name, u32 uboId, u32 bindingIndex, u32 maxSize){
    u32 index = gl::GetUniformBlockIndex(ID, name);
    gl::UniformBlockBinding(ID, index, bindingIndex);
    gl::BindBufferRange(gl::UNIFORM_BUFFER, bindingIndex, uboId, 0, sizeof(float) * maxSize);
    return *this;
}

Shader& Shader::uniform(const GLint name, const std::vector<glm::mat4> &m){
    gl::UniformMatrix4fv(name, m.size(), false, (float*)m.data());
    return *this;
}
Shader& Shader::uniform(const GLint name, const std::vector<float> &m){
    gl::Uniform1fv(name, m.size(),  m.data());
    return *this;
}
Shader& Shader::uniform(const GLint name, const glm::mat4 &m){
    gl::UniformMatrix4fv(name, 1, false, glm::value_ptr(m));
    return *this;
}
Shader& Shader::uniform(const GLint name, const glm::vec4 &v){
    gl::Uniform4fv(name, 1, glm::value_ptr(v));
    return *this;
}
Shader& Shader::uniform(const GLint name, const glm::vec3 &v){
    gl::Uniform3fv(name, 1, glm::value_ptr(v));
    return *this;
}
Shader& Shader::uniform(const GLint name, const glm::vec2 &v){
    gl::Uniform2fv(name, 1, glm::value_ptr(v));
    return *this;
}
Shader& Shader::uniform(const GLint name, float v){
    gl::Uniform1f(name, v);
    return *this;
}
Shader& Shader::uniform(const GLint name, int v){
    gl::Uniform1i(name, v);
    return *this;
}
Shader& Shader::uniform(const GLint name, uint32_t v){
    gl::Uniform1ui(name, v);
    return *this;
}
Shader& Shader::uniform(const GLint name, uint16_t v){
    gl::Uniform1ui(name, v);
    return *this;
}

Shader& Shader::uniform(const char *name, const std::vector<glm::mat4> &m){
    gl::UniformMatrix4fv(gl::GetUniformLocation(ID,name), m.size(), false, (float*)m.data());
    return *this;
}
Shader& Shader::uniform(const char *name, const std::vector<float> &m){
    gl::Uniform1fv(gl::GetUniformLocation(ID,name), m.size(), m.data());
    return *this;
}
Shader& Shader::uniform(const char *name, const glm::mat4 &m){
    gl::UniformMatrix4fv(gl::GetUniformLocation(ID,name), 1, false, glm::value_ptr(m));
    return *this;
}
Shader& Shader::uniform(const char *name, const glm::vec4 &v){
    gl::Uniform4fv(gl::GetUniformLocation(ID, name), 1, glm::value_ptr(v));
    return *this;
}
Shader& Shader::uniform(const char *name, const glm::vec3 &v){
    gl::Uniform3fv(gl::GetUniformLocation(ID, name), 1, glm::value_ptr(v));
    return *this;
}
Shader& Shader::uniform(const char *name, const glm::vec2 &v){
    gl::Uniform2fv(gl::GetUniformLocation(ID, name), 1, glm::value_ptr(v));
    return *this;
}
Shader& Shader::uniform(const char *name, float v){
    gl::Uniform1f(gl::GetUniformLocation(ID, name), v);
    return *this;
}
Shader& Shader::uniform(const char *name, int v){
    gl::Uniform1i(gl::GetUniformLocation(ID, name), v);
    return *this;
}
Shader& Shader::uniform(const char *name, uint32_t v){
    gl::Uniform1i(gl::GetUniformLocation(ID, name), v);
    return *this;
}

void Shader::loadImports(const std::string path){
    importDefinitions(path);
}
std::map<std::string, Shader> Shader::loadFromFile(const std::string pathTo, const std::string filename){
    std::map<std::string, Shader> out;
    while(true){
        try {
            out = compileShaders(pathTo, filename);
        }
        catch(const std::runtime_error& except){
            log("Shader compile error, file:", filename, "because of");
            log(except.what());
            std::cin.ignore();
            continue;
        }
        break;
    }

    return out;
}

void Shader::reload(Shader& newProgram){
    if(ID){
        gl::DeleteProgram(ID);
    }
    ID = newProgram.ID;
}

void Mesh::render(){
    if(mode == 0)
        gl::DrawElements(gl::TRIANGLES, count, gl::UNSIGNED_INT, offset());
    if(mode == gl::TRIANGLE_STRIP)
        gl::DrawArrays(gl::TRIANGLE_STRIP, begin, count);

}

State& State::blendFunc(u32 src, u32 dst){
    if(not m_blend) return *this;
    m_blendSRC = src;
    m_blendDST = dst;
    gl::BlendFunc(m_blendSRC, m_blendDST);
    return *this;
}
State& State::depthTest(bool b){
    if(b == m_depthTest) return *this;
    m_depthTest = b;
    if(b) gl::Enable(gl::DEPTH_TEST);
    else gl::Disable(gl::DEPTH_TEST);
    return *this;
}
State& State::depthMask(bool b){
    if(b == m_depthMask) return *this;
    m_depthMask = b;
    if(m_depthMask) gl::DepthMask(gl::FALSE_);
    else gl::DepthMask(gl::TRUE_);
    return *this;
}
State& State::culling(bool b, bool ccw){
    if(b == m_cullFace and ccw == m_frontFace) return *this;
    m_cullFace = b;
    m_frontFace = ccw;
    if(m_cullFace){
        gl::Enable(gl::CULL_FACE);
        m_frontFace ? gl::FrontFace(gl::CCW) : gl::FrontFace(gl::CCW);
    }
    else gl::Disable(gl::CULL_FACE);
    return *this;
}
State& State::depthFunc(u32 func){
    if(func == m_depthFunc or not m_depthTest) return *this;
    m_depthFunc = func;
    gl::DepthFunc(m_depthFunc);
    return *this;
}

void checkErrors(){
    int err = gl::GetError();
    if(err != gl::NO_ERROR_){
        error("fail:", err);
    }
}
