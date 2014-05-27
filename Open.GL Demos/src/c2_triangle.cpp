#include "c2_triangle.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include <cstdio>
#include <cmath>
#include <ctime>

GLchar *c2_triangle::vertexSource()
{
	return
		"#version 150\n"
		"in vec2 position;"
		"void main() {"
		"	gl_Position = vec4(position, 0.0, 1.0);"
		"}";
}

GLchar *c2_triangle::fragmentSource()
{
	return
		"#version 150\n"
		"out vec4 outColor;"
		"void main() {"
		"	outColor = vec4(1.0, 1.0, 1.0, 1.0);"
		"}";
}

GLuint c2_triangle::compileShader(const GLchar *shaderStr, GLenum eShaderType)
{
	GLuint shader = glCreateShader(eShaderType);
	glShaderSource(shader, 1, &shaderStr, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		char buffer[512];
		glGetShaderInfoLog(shader, 512, NULL, buffer);

		printf("%s\n", buffer);
	}
	else
	{
		printf("%s\n", "Shader compiled successfully");
	}

	return shader;
}

GLuint c2_triangle::generateVBO(/*float *vertices[]*/) // VBO (Vertex Buffer Object)
{
	GLuint vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer

	float vertices[] = {
		0.0f, 0.5f,
		0.5f, -0.5f,
		-0.5f, -0.5f
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	return vbo;
}

void c2_triangle::run()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	glewInit();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo = generateVBO();

	GLuint vertexShader = compileShader(vertexSource(), GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentSource(), GL_FRAGMENT_SHADER);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary since there is only one output
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

	SDL_Event windowEvent;
	while (true)
	{
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT)
			{
				break;
			}

			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
			{
				break;
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window);
	}

	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
}