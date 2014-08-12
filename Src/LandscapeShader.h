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
class LandscapeShader : public Shader
{
protected:
    /// Uniforms
    GLuint uMVP, uBrushPosition, uBrushScale, uBrushTexture, uTBO, uLandscapeSizeX, uLandscapeVertexOffset, uWireframeColor, uBrushColor, uTestOffsetY, uTestOffsetX, uClipmapPartOffset, uLandscapeTexture;

public:
    /// Standard constructors
    LandscapeShader();

    /// Uniform setters
    void SetBrushTexture(int TextureUnit) {glUniform1i(uBrushTexture, TextureUnit);};
    void SetTBO(int TBO) {glUniform1i(uTBO, TBO);};
    void SetMVP(mat4 MVP) {glUniformMatrix4fv(uMVP, 1, GL_FALSE, &MVP[0][0]);};
    void SetBrushPosition(vec2 BrushPosition) {glUniform2fv(uBrushPosition, 1, &BrushPosition[0]);};
    void SetBrushScale(float BrushScale) {glUniform1f(uBrushScale, BrushScale);};
    void SetLandscapeSizeX(float LandscapeSizeX) {glUniform1f(uLandscapeSizeX, LandscapeSizeX);};
    void SetLandscapeVertexOffset(float LandscapeVertexOffset) {glUniform1f(uLandscapeVertexOffset, LandscapeVertexOffset);};
    void SetWireframeColor(vec3 WireframeColor) {glUniform3fv(uWireframeColor, 1, &WireframeColor[0]);};
    void SetBrushColor(vec3 BrushColor) {glUniform3fv(uBrushColor, 1, &BrushColor[0]);};
	void SetTestOffsetX(float TestOffsetX) {glUniform1f(uTestOffsetX, TestOffsetX);};
	void SetTestOffsetY(float TestOffsetY) {glUniform1f(uTestOffsetY, TestOffsetY);};
	void SetClipmapPartOffset(vec2 ClipmapPartOffset) {glUniform2fv(uClipmapPartOffset, 1, &ClipmapPartOffset[0]);};
	void SetLandscapeTexture(int TextureUnit) {glUniform1i(uLandscapeTexture, TextureUnit);};

protected:
    /// Initialize uniform variables, return false when failure, true on success
    virtual bool InitializeUniforms();
};