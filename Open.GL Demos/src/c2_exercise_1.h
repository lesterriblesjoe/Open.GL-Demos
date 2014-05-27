#pragma once
#include "c2_triangle.h"

class c2_exercise_1 : c2_triangle
{
private:
	typedef c2_triangle super;
	GLchar *vertexSource();
public:
	void run();
};

