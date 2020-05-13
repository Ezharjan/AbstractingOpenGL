#include "Renender.h"
#include<iostream>

void GLClearError() {
	while (glGetError() != GL_NO_ERROR) {}; // Just clear the error history while there is no error.
}

bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) // Log error info  while there is the errors.
	{
		std :: cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ":" << line << std :: endl;
		return false;
	}
	return true;
}