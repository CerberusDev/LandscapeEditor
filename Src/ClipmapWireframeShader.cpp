// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date:
// --------------------------------------------------------------------

#include "ClipmapWireframeShader.h"
#include "LandscapeEditor.h"



// --------------------------------------------------------------------
bool ClipmapWireframeShader::InitializeUniforms()
{
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
