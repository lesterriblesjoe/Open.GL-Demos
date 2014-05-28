#include "c3_exercise_3.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include <cstdio>
#include <cmath>
#include <ctime>
#include <array>

#include <SOIL.h>

GLchar *c3_exercise_3::vertexSource()
{
	return
		"#version 150\n"
		"in vec2 position;"
		"in vec3 color;"
		"in vec2 texcoord;"
		"out vec3 Color;"
		"out vec2 Texcoord;"
		"void main() {"
		"	Color = color;"
		"	Texcoord = texcoord;"
		"	gl_Position = vec4(position, 0.0, 1.0);"
		"}";
}

GLchar *c3_exercise_3::fragmentSource()
{
	return
		"#version 150\n"
		"in vec3 Color;"
		"in vec2 Texcoord;"
		"out vec4 outColor;"
		"uniform sampler2D tex;"
		"uniform float time;"
		"void main() {"
		"	if (Texcoord.y < 0.5)"
		"		outColor = texture(tex, Texcoord);"
		"	else"
		"	{"
		"		float timeOffset = (time * 2) + 60.0;"
		"		float Texcoordx = Texcoord.x + sin(Texcoord.y * timeOffset) / 30;"
		"		outColor = texture(tex, vec2(Texcoordx, 1.0 - Texcoord.y)) * vec4(0.7, 0.7, 1.0, 1.0);"
		//"		outColor = texture(tex, vec2(Texcoord.x + sin(Texcoord.y * 60.0 + time * 2.0) / 30.0, 1.0 - Texcoord.y)) * vec4(0.7, 0.7, 1.0, 1.0);"
		"	}"
		"}";
}

GLuint c3_exercise_3::compileShader(const GLchar *shaderStr, GLenum eShaderType)
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

void c3_exercise_3::setTextureParameters()
{
	// Clamp texture to edge
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Linearly interpolate texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

GLuint c3_exercise_3::loadTexture()
{
	GLuint tex;
	glGenTextures(1, &tex);

	int width, height;
	unsigned char *image;

	std::string kittenTexPath = getBasePath() + "Content\\sample.png";

	image = SOIL_load_image(kittenTexPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	printf("%s\n", SOIL_last_result());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	setTextureParameters();

	return tex;
}

GLuint c3_exercise_3::generateVBO() // VBO (Vertex Buffer Object)
{
	GLuint vbo;
	glGenBuffers(1, &vbo); // Generate 1 buffer

	float vertices[] = {
		//  Position[x,y] Color[r,g,b]          Texcoords[s,t]
		-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-Left
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-Right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-Right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // Bottom-Left
	};

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	return vbo;
}

GLuint c3_exercise_3::generateEBO()
{
	GLuint ebo;
	glGenBuffers(1, &ebo);

	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	return ebo;
}

void c3_exercise_3::run()
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
	GLuint ebo = generateEBO();

	GLuint vertexShader = compileShader(vertexSource(), GL_VERTEX_SHADER);
	GLuint fragmentShader = compileShader(fragmentSource(), GL_FRAGMENT_SHADER);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	//glBindFragDataLocation(shaderProgram, 0, "outColor"); // Not necessary since there is only one output
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	size_t floatSize = sizeof(float);
	size_t stride = 7;

	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, stride * floatSize, 0);

	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, stride * floatSize, (GLvoid *)(2 * floatSize));

	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, stride * floatSize, (GLvoid *)(5 * floatSize));

	GLuint tex = loadTexture();

	GLint uniTime = glGetUniformLocation(shaderProgram, "time");

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

		GLfloat time = (GLfloat)clock() / (GLfloat)CLOCKS_PER_SEC;

		glUniform1f(uniTime, time);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);
	}

	glDeleteTextures(1, &tex);

	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
}