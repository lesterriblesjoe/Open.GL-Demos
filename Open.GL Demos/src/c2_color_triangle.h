#pragma once
#include "Tutorial.h"

#define GLEW_STATIC
#include <GL/glew.h>

class c2_color_triangle : Tutorial
{
private:
	virtual GLchar *vertexSource();
	virtual GLchar *fragmentSource();
	GLuint compileShader(const GLchar *shaderStr, GLenum eShaderType);
	GLuint generateVBO();
public:
	void run();
};

