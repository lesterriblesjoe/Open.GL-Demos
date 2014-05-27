#include "c2_exercise_1.h"


GLchar *c2_exercise_1::vertexSource()
{
	return
		"#version 150\n"
		"in vec2 position;"
		"void main() {"
		"	gl_Position = vec4(position.x, -position.y, 0.0, 1.0);"
		"}";
}

void c2_exercise_1::run()
{
	super::run();
}