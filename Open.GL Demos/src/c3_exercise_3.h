#pragma once
#include "Tutorial.h"

#define GLEW_STATIC
#include <GL/glew.h>

class c3_exercise_3
{
private:
	virtual GLchar *vertexSource();
	virtual GLchar *fragmentSource();
	GLuint compileShader(const GLchar *shaderStr, GLenum eShaderType);
	void setTextureParameters();
	GLuint loadTexture();
	GLuint generateVBO();
	GLuint generateEBO();
public:
	void run();
};

