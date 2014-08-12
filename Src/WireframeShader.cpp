// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date:
// --------------------------------------------------------------------

#include "WireframeShader.h"
#include "LandscapeEditor.h"

// --------------------------------------------------------------------
WireframeShader::WireframeShader()
{
}

// --------------------------------------------------------------------
bool WireframeShader::InitializeUniforms()
{
    uBrushTexture = glGetUniformLocation(ShaderProgram, "BrushTextureSampler");
    if (uBrushTexture == 0xFFFFFFFF)
    {
        LOG("Cannot find BrushTextureSampler uniform variable");
        return false;
    }

    uTBO = glGetUniformLocation(ShaderProgram, "TBOSampler");
    if (uTBO == 0xFFFFFFFF)
    {
        LOG("Cannot find TBOSampler uniform variable");
        return false;
    }

       uMVP = glGetUniformLocation(ShaderProgram, "gWorld");
    if (uMVP == 0xFFFFFFFF)
    {
        LOG("Cannot find gWorld uniform variable");
        return false;
    }

    uBrushPosition = glGetUniformLocation(ShaderProgram, "BrushPosition");
    if (uBrushPosition == 0xFFFFFFFF)
    {
        LOG("Cannot find BrushPosition uniform variable");
        return false;
    }

    uBrushScale = glGetUniformLocation(ShaderProgram, "BrushScale");
    if (uBrushScale == 0xFFFFFFFF)
    {
        LOG("Cannot find BrushScale uniform variable");
        return false;
    }

    uLandscapeSizeX = glGetUniformLocation(ShaderProgram, "LandscapeSizeX");
    if (uLandscapeSizeX == 0xFFFFFFFF)
    {
        LOG("Cannot find LandscapeSizeX uniform variable");
        return false;
    }

    uLandscapeVertexOffset = glGetUniformLocation(ShaderProgram, "LandscapeVertexOffset");
    if (uLandscapeSizeX == 0xFFFFFFFF)
    {
        LOG("Cannot find LandscapeVertexOffset uniform variable");
        return false;
    }

    uWireframeColor = glGetUniformLocation(ShaderProgram, "WireframeColor");
    if (uWireframeColor == 0xFFFFFFFF)
    {
        LOG("Cannot find WireframeColor uniform variable");
        return false;
    }

    uBrushColor = glGetUniformLocation(ShaderProgram, "BrushColor");
    if (uBrushColor == 0xFFFFFFFF)
    {
        LOG("Cannot find BrushColor uniform variable");
        return false;
    }

	uTestOffsetX = glGetUniformLocation(ShaderProgram, "TestOffsetX");
    if (uTestOffsetX == 0xFFFFFFFF)
    {
        LOG("Cannot find TestOffsetX uniform variable");
        return false;
    }

	uTestOffsetY = glGetUniformLocation(ShaderProgram, "TestOffsetY");
    if (uTestOffsetY == 0xFFFFFFFF)
    {
        LOG("Cannot find TestOffsetY uniform variable");
        return false;
    }

	uClipmapPartOffset = glGetUniformLocation(ShaderProgram, "ClipmapPartOffset");
    if (uClipmapPartOffset == 0xFFFFFFFF)
    {
        LOG("Cannot find ClipmapPartOffset uniform variable");
        return false;
    }
	
    return true;
}