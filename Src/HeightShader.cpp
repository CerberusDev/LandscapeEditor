// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date:
// --------------------------------------------------------------------

#include "HeightShader.h"
#include "LandscapeEditor.h"

// --------------------------------------------------------------------
HeightShader::HeightShader()
{
}

// --------------------------------------------------------------------
bool HeightShader::InitializeUniforms()
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
        LOG("Cannot find uLandscapeSizeX uniform variable");
        return false;
    }

    uLandscapeVertexOffset = glGetUniformLocation(ShaderProgram, "LandscapeVertexOffset");
    if (uLandscapeSizeX == 0xFFFFFFFF)
    {
        LOG("Cannot find LandscapeVertexOffset uniform variable");
        return false;
    }

    uDiffuseStrength = glGetUniformLocation(ShaderProgram, "DiffuseStrength");
    if (uDiffuseStrength == 0xFFFFFFFF)
    {
        LOG("Cannot find DiffuseStrength uniform variable");
        return false;
    }

    uAmbientStrength = glGetUniformLocation(ShaderProgram, "AmbientStrength");
    if (uAmbientStrength == 0xFFFFFFFF)
    {
        LOG("Cannot find AmbientStrength uniform variable");
        return false;
    }

    uMaxHeight = glGetUniformLocation(ShaderProgram, "MaxHeight");
    if (uMaxHeight == 0xFFFFFFFF)
    {
        LOG("Cannot find MaxHeight uniform variable");
        return false;
    }

    return true;
}