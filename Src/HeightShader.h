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

/** This shaders implicates colours on fragments dependly on its height */
class HeightShader : public Shader
{
public:
    /// Uniform setters
	void SetBrushTextureSampler(int Value) {SetUniform("BrushTextureSampler", Value);};
	void SetTBOSampler(int Value) {SetUniform("TBOSampler", Value);};
	void SetgWorld(mat4 Value) {SetUniform("gWorld", Value);};
	void SetBrushPosition(vec2 Value) {SetUniform("BrushPosition", Value);};
	void SetBrushScale(float Value) {SetUniform("BrushScale", Value);};
	void SetLandscapeSizeX(float Value) {SetUniform("LandscapeSizeX", Value);};
	void SetLandscapeVertexOffset(float Value) {SetUniform("LandscapeVertexOffset", Value);};
	void SetDiffuseStrength(float Value) {SetUniform("DiffuseStrength", Value);};
	void SetAmbientStrength(float Value) {SetUniform("AmbientStrength", Value);};
	void SetMaxHeight(float Value) {SetUniform("MaxHeight", Value);};

    /// Standard constructor
	HeightShader()
	{
		Uniforms.insert(std::make_pair<std::string, GLuint>("BrushTextureSampler", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("TBOSampler", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("gWorld", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("BrushPosition", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("BrushScale", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("LandscapeSizeX", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("LandscapeVertexOffset", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("DiffuseStrength", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("AmbientStrength", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("MaxHeight", 0));
	}
};