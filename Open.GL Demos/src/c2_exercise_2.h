#pragma once
#include "c2_color_triangle.h"

class c2_exercise_2 : c2_color_triangle
{
private:
	typedef c2_color_triangle super;
	GLchar *fragmentSource();
public:
	void run();
};

