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
	static const std::string BrushTextureSampler;
	static const std::string TBOSampler;
	static const std::string gWorld;
	static const std::string BrushPosition;
	static const std::string BrushScale;
	static const std::string ClipmapSizeX;
	static const std::string ClipmapSizeY;
	static const std::string LandscapeVertexOffset;
	static const std::string WireframeColor;
	static const std::string BrushColor;
	static const std::string TestOffsetX;
	static const std::string TestOffsetY;
	static const std::string ClipmapScale;
	static const std::string ClipmapPartOffset;

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