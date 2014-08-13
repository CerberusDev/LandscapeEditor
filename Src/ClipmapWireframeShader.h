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
protected:
    /// Uniforms
    GLuint uMVP, uBrushPosition, uBrushScale, uBrushTexture, uTBO, uClipmapSizeX, uClipmapSizeY, uLandscapeVertexOffset, uWireframeColor, uBrushColor, uTestOffsetY, uTestOffsetX, uClipmapScale, uClipmapMode, uClipmapPartOffset;

public:
    /// Standard constructors
    ClipmapWireframeShader();

    /// Uniform setters
	void SetUniform1i(std::string UniformName, int Value);
	void SetUniformM4fv(std::string UniformName, mat4 Value);
	void SetUniform2fv(std::string UniformName, vec2 Value);
	void SetUniform1f(std::string UniformName, float Value);
	void SetUniform3fv(std::string UniformName, vec3 Value);

protected:
    /// Initialize uniform variables, return false when failure, true on success
    virtual bool InitializeUniforms();
};