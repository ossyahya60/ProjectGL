#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVecf(const char* name, uint32_t count, const GLfloat* value) const;
    void setVeci(const char* name, uint32_t count, const GLint* value) const;
    void setMat4(const char* name, int count, const GLfloat* value) const;
    void deleteShader() const;
    ~Shader();
};

#endif