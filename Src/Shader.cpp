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
bool Shader::Initialize(char* VertexShaderPath, char* FragmentShaderPath)
{
    LOG("Preparing shaders " << VertexShaderPath << " and " << FragmentShaderPath << "..");

	const char* VertexShaderSrc = LandscapeEditor::TextFileRead(VertexShaderPath);
	const char* FragmentShaderSrc = LandscapeEditor::TextFileRead(FragmentShaderPath);
    GLint success = 0;
    GLuint VertexShader, FragmentShader;
    GLchar InfoLog[1024];

    if (VertexShaderSrc == NULL || FragmentShaderSrc == NULL)
    {
        ERR("Can't find shader source");
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
        ERR("Error occured when compiling VertexShader " << VertexShaderPath << ":" << std::endl << InfoLog);
        return false;
    }

	glCompileShader(FragmentShader);
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(FragmentShader, 1024, NULL, InfoLog);
        ERR("Error occured when compiling FragmentShader " << FragmentShaderPath << ":" << std::endl << InfoLog);
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
		ERR("Error linking shader program:" << std::endl << InfoLog);
        return false;
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(InfoLog), NULL, InfoLog);
        ERR("Invalid shader program:" << std::endl << InfoLog);
        return false;
    }

    if (InitializeUniforms() == false)
        return false;

    LOG("Shader ready to go!");

    return true;
}

// --------------------------------------------------------------------
void Shader::Use()
{
    glUseProgram(ShaderProgram);
}