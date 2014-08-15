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

/** Shader for drawing terrain with light and textures - clipmap compatible version */
class ClipmapLandscapeShader : public Shader
{
public:
    /// Uniform setters
	void SetBrushTextureSampler(int Value) {SetUniform("BrushTextureSampler", Value);};
	void SetTBOSampler(int Value) {SetUniform("TBOSampler", Value);};
	void SetgWorld(mat4 Value) {SetUniform("gWorld", Value);};
	void SetBrushPosition(vec2 Value) {SetUniform("BrushPosition", Value);};
	void SetBrushScale(float Value) {SetUniform("BrushScale", Value);};
	void SetClipmapSizeX(float Value) {SetUniform("ClipmapSizeX", Value);};
	void SetClipmapSizeY(float Value) {SetUniform("ClipmapSizeY", Value);};
	void SetLandscapeVertexOffset(float Value) {SetUniform("LandscapeVertexOffset", Value);};
	void SetWireframeColor(vec3 Value) {SetUniform("WireframeColor", Value);};
	void SetBrushColor(vec3 Value) {SetUniform("BrushColor", Value);};
	void SetTestOffsetX(float Value) {SetUniform("TestOffsetX", Value);};
	void SetTestOffsetY(float Value) {SetUniform("TestOffsetY", Value);};
	void SetClipmapScale(float Value) {SetUniform("ClipmapScale", Value);};
	void SetClipmapPartOffset(vec2 Value) {SetUniform("ClipmapPartOffset", Value);};
	void SetTextureSampler(int Value) {SetUniform("TextureSampler", Value);};

    /// Standard constructor
	ClipmapLandscapeShader()
	{
		Uniforms.insert(std::make_pair<std::string, GLuint>("BrushTextureSampler", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("TBOSampler", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("gWorld", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("BrushPosition", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("BrushScale", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("ClipmapSizeX", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("ClipmapSizeY", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("LandscapeVertexOffset", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("WireframeColor", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("BrushColor", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("TestOffsetX", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("TestOffsetY", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("ClipmapScale", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("ClipmapPartOffset", 0));
		Uniforms.insert(std::make_pair<std::string, GLuint>("TextureSampler", 0));
	}
};