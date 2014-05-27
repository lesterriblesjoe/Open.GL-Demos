#pragma once
#include "Tutorial.h"

#define GLEW_STATIC
#include <GL/glew.h>

#include <array>

class c4_3d : Tutorial
{
private:
	GLchar *vertexSource();
	GLchar *fragmentSource();
	GLuint compileShader(const GLchar *shaderStr, GLenum eShaderType);
	void setTextureParameters();
	void loadTextures(std::array<GLuint, 2> &textures, GLuint shaderProgram);
	GLuint generateVBO();
	GLuint generateEBO();
public:
	void run();
};

