// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date:
// --------------------------------------------------------------------
#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <GL/glew.h>

using namespace glm;

/** Base class for all shaders wrappers */
class Shader
{
protected:
    /// Shader ID
    GLuint ShaderProgram;

	/// Uniform variables names and IDs
	std::unordered_map<std::string, GLuint> Uniforms;

	/// Name of that shader (also filename)
	std::string ShaderName;
public:
    /// Standard constructors and destructors
    Shader();
    ~Shader();

    /// Create, link, validates shader, return false when failure, true on success
    bool Initialize(std::string argShadarName);

    /// Call when you want to start using this shader
    void Use();

    /// Comparison operators
    friend bool operator == (const Shader& left, const Shader& right) {return left.ShaderProgram == right.ShaderProgram;};
    friend bool operator != (const Shader& left, const Shader& right) {return left.ShaderProgram != right.ShaderProgram;};

protected:
	/// Uniform setters
	void SetUniform(std::string UniformName, int Value);
	void SetUniform(std::string UniformName, float Value);
	void SetUniform(std::string UniformName, vec2 Value);
	void SetUniform(std::string UniformName, vec3 Value);
	void SetUniform(std::string UniformName, mat4 Value);

    /// Initialize uniform variables, return false when failure, true on success
    bool InitializeUniforms();
};