// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date:
// --------------------------------------------------------------------
#pragma once

#include <GL/glew.h>

/** Base class for all shaders wrappers */
class Shader
{
protected:
    /// Shader ID
    GLuint ShaderProgram;

public:
    /// Standard constructors and destructors
    Shader();
    virtual ~Shader();

    /// Create, link, validates shader, return false when failure, true on success
    bool Initialize(char* VertexShaderPath, char* FragmentShaderPath);

    /// Call when you want to start using this shader
    void Use();

    /// Comparison operators
    friend bool operator == (const Shader& left, const Shader& right) {return left.ShaderProgram == right.ShaderProgram;};
    friend bool operator != (const Shader& left, const Shader& right) {return left.ShaderProgram != right.ShaderProgram;};

protected:
    /// Initialize uniform variables, return false when failure, true on success
    virtual bool InitializeUniforms() = 0;
};