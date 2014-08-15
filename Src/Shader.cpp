// --------------------------------------------------------------------
// Created by: Maciej Pryc
// Date:
// --------------------------------------------------------------------

#include "Shader.h"
#include "LandscapeEditor.h"


// --------------------------------------------------------------------
Shader::Shader():
ShaderProgram(0xFFFFFFFF)
{
}

// --------------------------------------------------------------------
Shader::~Shader()
{
	if (ShaderProgram != 0xFFFFFFFF)
		glDeleteProgram(ShaderProgram);
}

// --------------------------------------------------------------------
bool Shader::Initialize(std::string argShadarName)
{
	ShaderName = argShadarName;
    LOG("Preparing shader " << ShaderName << "...");

	std::string VertexShaderName = "Src/Shaders/" + ShaderName + ".vs";
	std::string FragmentShaderName = "Src/Shaders/" + ShaderName + ".fs";

	const char* VertexShaderSrc = LandscapeEditor::TextFileRead(VertexShaderName.c_str());
	const char* FragmentShaderSrc = LandscapeEditor::TextFileRead(FragmentShaderName.c_str());
    GLint success = 0;
    GLuint VertexShader, FragmentShader;
    GLchar InfoLog[1024];

    if (VertexShaderSrc == NULL || FragmentShaderSrc == NULL)
    {
        ERR("Can't find source of " << ShaderName << " shader!");
        return false;
    }

	VertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (VertexShader == NULL) 
        return false;

	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (FragmentShader == NULL) 
        return false;

	glShaderSource(VertexShader, 1, &VertexShaderSrc, NULL);
	glShaderSource(FragmentShader, 1, &FragmentShaderSrc, NULL);

	delete VertexShaderSrc;
    delete FragmentShaderSrc;

	glCompileShader(VertexShader);
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(VertexShader, 1024, NULL, InfoLog);
        ERR("Error occured when compiling Vertex Shader for " << ShaderName << ":" << std::endl << InfoLog);
        return false;
    }

	glCompileShader(FragmentShader);
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(FragmentShader, 1024, NULL, InfoLog);
        ERR("Error occured when compiling FragmentShader for " << ShaderName << ":" << std::endl << InfoLog);
        return false;
    }

	ShaderProgram = glCreateProgram();
    if (ShaderProgram == NULL)
        return false;

	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);

    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(InfoLog), NULL, InfoLog);
		ERR("Error during linking shader program for " << ShaderName << ":" << std::endl << InfoLog);
        return false;
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(InfoLog), NULL, InfoLog);
        ERR("Invalid shader program " << ShaderName << ":" << std::endl << InfoLog);
        return false;
    }

    if (InitializeUniforms() == false)
        return false;

    LOG("Shader " << ShaderName << " ready to go!");

    return true;
}

// --------------------------------------------------------------------
void Shader::Use()
{
    glUseProgram(ShaderProgram);
}

// --------------------------------------------------------------------
void Shader::SetUniform(std::string UniformName, int Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniform1i(it->second, Value);
	else
		WARN("Trying to set " << UniformName << " uniform in " << ShaderName << " shader, but it doesn't exist");
}

// --------------------------------------------------------------------
void Shader::SetUniform(std::string UniformName, float Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniform1f(it->second, Value);
	else
		WARN("Trying to set " << UniformName << " uniform in " << ShaderName << " shader, but it doesn't exist");
}

// --------------------------------------------------------------------
void Shader::SetUniform(std::string UniformName, vec2 Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniform2fv(it->second, 1, &Value[0]);
	else
		WARN("Trying to set " << UniformName << " uniform in " << ShaderName << " shader, but it doesn't exist");
}

// --------------------------------------------------------------------
void Shader::SetUniform(std::string UniformName, vec3 Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniform3fv(it->second, 1, &Value[0]);
	else
		WARN("Trying to set " << UniformName << " uniform in " << ShaderName << " shader, but it doesn't exist");
}

// --------------------------------------------------------------------
void Shader::SetUniform(std::string UniformName, mat4 Value)
{
	std::unordered_map<std::string, GLuint>::const_iterator it = Uniforms.find(UniformName);

	if (it != Uniforms.end())
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &Value[0][0]);
	else
		WARN("Trying to set " << UniformName << " uniform in " << ShaderName << " shader, but it doesn't exist");
}

// --------------------------------------------------------------------
bool Shader::InitializeUniforms()
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

