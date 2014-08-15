// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date:
// --------------------------------------------------------------------
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Shader.h"

using namespace glm;

/** Shader for drawing black polygon outlines */
class ClipmapWireframeShader : public Shader
{
public:
    /// Standard constructors
    ClipmapWireframeShader();

    /// Uniform setters
	void SetUniform(std::string UniformName, int Value);
	void SetUniform(std::string UniformName, mat4 Value);
	void SetUniform(std::string UniformName, vec2 Value);
	void SetUniform(std::string UniformName, float Value);
	void SetUniform(std::string UniformName, vec3 Value);

protected:
    /// Initialize uniform variables, return false when failure, true on success
    virtual bool InitializeUniforms();
};