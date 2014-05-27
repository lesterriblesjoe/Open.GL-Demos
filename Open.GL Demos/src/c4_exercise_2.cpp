#include "c4_exercise_2.h"

#include <SDL.h>
#include <SDL_opengl.h>

#include <cstdio>
#include <cmath>
#include <ctime>
#include <array>

#include <SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLchar *c4_exercise_2::vertexSource()
{
	return
		"#version 150\n"
		"in vec2 position;"
		"in vec3 color;"
		"in vec2 texcoord;"
		"out vec3 Color;"
		"out vec2 Texcoord;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 proj;"
		"void main() {"
		"	Color = color;"
		"	Texcoord = texcoord;"
		"	gl_Position = proj * view * model * vec4(position, 0.0, 1.0);"
		"}";
}

GLchar *c4_exercise_2::fragmentSource()
{
	return
		"#version 150\n"
		"in vec3 Color;"
		"in vec2 Texcoord;"
		"out vec4 outColor;"
		"uniform sampler2D texKitten;"
		"uniform sampler2D texPuppy;"
		"uniform float time;"
		"void main() {"
		"	float factor = (sin(time) + 1.0) / 2;"
		"	vec4 colKitten = texture(texKitten, Texcoord);"
		"	vec4 colPuppy = texture(texPuppy, Texcoord);"
		"	outColor = mix(colKitten, colPuppy, factor);"
		"}";
}

GLuint c4_exercise_2::compileShader(const GLchar *shaderStr, GLenum eShaderType)
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

void c4_exercise_2::setTextureParameters()
{
	// Clamp texture to edge
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Linearly interpolate texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void c4_exercise_2::loadTextures(std::array<GLuint, 2> &textures, GLuint shaderProgram)
{
	glGenTextures(2, textures.data());

	int width, height;
	unsigned char *image;

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	image = SOIL_load_image("Content\\sample.png", &width, &height, 0, SOIL_LOAD_RGB);
	printf("%s\n", SOIL_last_result());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);
	setTextureParameters();

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	image = SOIL_load_image("Content\\sample2.png", &width, &height, 0, SOIL_LOAD_RGB);
	printf("%s\n", SOIL_last_result());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);
	setTextureParameters();
}

GLuint c4_exercise_2::generateVBO() // VBO (Vertex Buffer Object)
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

GLuint c4_exercise_2::generateEBO()
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

void c4_exercise_2::run()
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

	std::array<GLuint, 2> textures{};
	loadTextures(textures, shaderProgram);

	GLint uniTime = glGetUniformLocation(shaderProgram, "time");

	GLint uniModel = glGetUniformLocation(shaderProgram, "model");

	glm::mat4 view = glm::lookAt(
		glm::vec3(1.2f, 1.2f, 1.2f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	GLint uniView = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 proj = glm::perspective(45.0f, 800.0f / 600.0f, 1.0f, 10.f);
	GLint uniProj = glGetUniformLocation(shaderProgram, "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	GLfloat angle = -45.0f;
	GLfloat speed = 0.0f;

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

			if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_SPACE)
			{
				speed = 180.f;
			}
		}

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat time = (GLfloat)clock() / (GLfloat)CLOCKS_PER_SEC;

		glm::mat4 model;
		model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));

		glUniform1f(uniTime, time);

		angle += speed / (GLfloat)CLOCKS_PER_SEC;
		speed /= 1.0f + (1.5f / (GLfloat)CLOCKS_PER_SEC);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window);
	}

	glDeleteTextures(2, textures.data());

	glDeleteProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteBuffers(1, &vbo);

	glDeleteVertexArrays(1, &vao);

	SDL_GL_DeleteContext(context);
	SDL_Quit();
}