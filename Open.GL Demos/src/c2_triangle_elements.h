#pragma once
#include "Tutorial.h"

#define GLEW_STATIC
#include <GL/glew.h>

class c2_triangle_elements : Tutorial
{
private:
	virtual GLchar *vertexSource();
	virtual GLchar *fragmentSource();
	GLuint compileShader(const GLchar *shaderStr, GLenum eShaderType);
	GLuint generateVBO();
	GLuint generateEBO();
public:
	void run();
};

