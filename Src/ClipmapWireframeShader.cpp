// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date:
// --------------------------------------------------------------------

#include "ClipmapWireframeShader.h"
#include "LandscapeEditor.h"

// --------------------------------------------------------------------
ClipmapWireframeShader::ClipmapWireframeShader()
{
}

// --------------------------------------------------------------------
bool ClipmapWireframeShader::InitializeUniforms()
{
	Uniforms.insert(std::make_pair<std::string, GLuint>(BrushTextureSampler, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(TBOSampler, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(gWorld, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(BrushPosition, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(BrushScale, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(ClipmapSizeX, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(ClipmapSizeY, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(LandscapeVertexOffset, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(WireframeColor, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(BrushColor, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(TestOffsetX, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(TestOffsetY, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(ClipmapScale, 0));
	Uniforms.insert(std::make_pair<std::string, GLuint>(ClipmapPartOffset, 0));

	for (auto it = Uniforms.begin(); it != Uniforms.end(); ++it)
	{
		it->second = glGetUniformLocation(ShaderProgram, it->first.c_str());
		if (it->second == 0xFFFFFFFF)
		{
			ERR("Cannot find " << it->first << " uniform variable");
			return false;
		}
	}

    return true;
}

// --------------------------------------------------------------------
void ClipmapWireframeShader::SetUniform(std::string UniformName, int Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniform1i(it->second, Value);
	else
		WARN("Trying to set " << UniformName << " uniform, but it don't exist");
}

// --------------------------------------------------------------------
void ClipmapWireframeShader::SetUniform(std::string UniformName, mat4 Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &Value[0][0]);
	else
		WARN("Trying to set " << UniformName << " uniform, but it don't exist");
}

// --------------------------------------------------------------------
void ClipmapWireframeShader::SetUniform(std::string UniformName, vec2 Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniform2fv(it->second, 1, &Value[0]);
	else
		WARN("Trying to set " << UniformName << " uniform, but it don't exist");
}

// --------------------------------------------------------------------
void ClipmapWireframeShader::SetUniform(std::string UniformName, float Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniform1f(it->second, Value);
	else
		WARN("Trying to set " << UniformName << " uniform, but it don't exist");
}

// --------------------------------------------------------------------
void ClipmapWireframeShader::SetUniform(std::string UniformName, vec3 Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniform3fv(it->second, 1, &Value[0]);
	else
		WARN("Trying to set " << UniformName << " uniform, but it don't exist");
}

// --------------------------------------------------------------------
const std::string ClipmapWireframeShader::BrushTextureSampler = "BrushTextureSampler";
const std::string ClipmapWireframeShader::TBOSampler = "TBOSampler";
const std::string ClipmapWireframeShader::gWorld = "gWorld";
const std::string ClipmapWireframeShader::BrushPosition = "BrushPosition";
const std::string ClipmapWireframeShader::BrushScale = "BrushScale";
const std::string ClipmapWireframeShader::ClipmapSizeX = "ClipmapSizeX";
const std::string ClipmapWireframeShader::ClipmapSizeY = "ClipmapSizeY";
const std::string ClipmapWireframeShader::LandscapeVertexOffset = "LandscapeVertexOffset";
const std::string ClipmapWireframeShader::WireframeColor = "WireframeColor";
const std::string ClipmapWireframeShader::BrushColor = "BrushColor";
const std::string ClipmapWireframeShader::TestOffsetX = "TestOffsetX";
const std::string ClipmapWireframeShader::TestOffsetY = "TestOffsetY";
const std::string ClipmapWireframeShader::ClipmapScale = "ClipmapScale";
const std::string ClipmapWireframeShader::ClipmapPartOffset = "ClipmapPartOffset";