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
    void setVec2(const char* name, float valueX, float valueY) const;
    void setVec3(const char* name, float valueX, float valueY, float valueZ) const;
    void setVec4(const char* name, float valueX, float valueY, float valueZ, float valueW) const;
    void setMat4(const char* name, int count, const GLfloat* value) const;
    void deleteShader() const;
};

#endif