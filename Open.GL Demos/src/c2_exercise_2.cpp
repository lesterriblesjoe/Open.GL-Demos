#include "c2_exercise_2.h"


GLchar *c2_exercise_2::fragmentSource()
{
	return
		"#version 150\n"
		"in vec3 Color;"
		"out vec4 outColor;"
		"void main() {"
		"	outColor = vec4(1.0 - Color, 1.0);"
		"}";
}

void c2_exercise_2::run()
{
	super::run();
}