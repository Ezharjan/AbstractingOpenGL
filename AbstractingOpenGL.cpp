﻿#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include "Renender.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
using namespace std;

//by Alexander

struct ShaderProgramSource
{
	string VertexSource;
	string FragmentSource;
};

static ShaderProgramSource ParseShader(const string filePath) {

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	ifstream stream(filePath);
	ShaderType type = ShaderType::NONE;
	string line;
	stringstream ss[2]; // One for vertex shader while the other is for the fragment shader
	while (getline(stream, line))
	{
		if (line.find("#shader") != string::npos)
		{
			if (line.find("vertex") != string::npos)
			{
				//set vertex mode
				type = ShaderType::VERTEX;
			}
			else {
				//set fragment mode
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n'; // Add line to the stream string
		}
	}
	return { ss[0].str(), ss[1].str() };
}

static int CompilerShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex " : "fragment ") << "shader" << endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static unsigned int CreateShader(const string& vertexShader, const string& fragmentShader) {
	unsigned int programe = glCreateProgram();  // "unsigned int" equals to "GLuint"
	unsigned int vShder = CompilerShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fShder = CompilerShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(programe, vShder);
	glAttachShader(programe, fShder);
	glLinkProgram(programe);
	GLCall(glValidateProgram(programe));

	glDeleteShader(vShder);
	glDeleteShader(fShder);

	return programe;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);// Must bind object below because it's Core and no defaultly bound

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(480, 480, "Shaders in OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); // Set the interval time among each rendering time

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error!!!" << std::endl;  // Will not an Error printing 
	}
	std::cout << "Your current GL version is : " << glGetString(GL_VERSION) << std::endl;

	{ //Surround in case the valid context was terminated before the index buffer destructs itself
		float position[] = {
			-0.5f,-0.5f,//0
			0.5f, -0.5f,//1
			0.5f,  0.5f,//2
		   -0.5f,  0.5f//3
		};

		unsigned int indecies[] = {
			0,1,2,
			0,2,3
		};

		////VAO must be bound in the "Core" profile
		unsigned int vao; // vao ----> vertex array object: which will hold vertex array object ID
		GLCall(glGenVertexArrays(1, &vao));
		GLCall(glBindVertexArray(vao));

		VertexBuffer vb(position, 4 * 2 * sizeof(float));

		GLCall(glEnableVertexAttribArray(0)); //The index must be the same with the code below
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		IndexBuffer ib(indecies, 6);

		ShaderProgramSource source = ParseShader("Basics.shader");

		unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
		glUseProgram(shader);

		int location = glGetUniformLocation(shader, "u_Color");
		ASSERT(location != -1); // Assert the location in case it was ruined or fragiled！*********Very Important*********

		GLCall(glBindVertexArray(0)); // Bind vertex array object below in the while-loop
		GLCall(glUseProgram(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

		float red = 0.0f;
		float increment = 0.05f;

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			GLCall(glUseProgram(shader)); // Reloading the shader to the GPU ----> (1) Bind shader
			GLCall(glUniform4f(location, red, 0.9f, 0.0f, 1.0f)); // Only the Grayscale will change when RGB is equally distributed

			GLCall(glBindVertexArray(vao)); //Rebinding vao
			ib.Bind();

			/*Draw here*/
			//glDrawArrays(GL_TRIANGLES, 0, 6);
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			if (red > 1.0f)
			{
				increment = -0.05f; // Make the increment go back to initial state
			}
			else if (red < 0.0f)
			{
				increment = 0.05f; // Make the increment go back to initial state
			}
			red += increment; // Add the increment in each loop

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
		glDeleteProgram(shader);

	}//Surround in case the valid context was terminated before the index buffer destructs itself
	glfwTerminate();// Destroy valid OpenGL context. !!!!!!!!!!Comedy!!!!!!!!!!!!
	/*Since the glCheckError() is set formally to run in the while-loop, it will always run glCheckError()
	in the IndexBuffer class and will never stop until force stopped ---->the console window did
	not return 0, until I forced-cloaed programe it returned -1.[Amazing!]
	But the 'glfwTerminate()' destroys the valid OpenGL context, meanwhile the while-loop has not stopped
	yet, so the function 'glCheckError()' to check the error is still running.
	'glCheckError' then stop at the while-loop line of 'glClearError()' function because
	of the vanished OpenGL context, it tries to clear all the errors in the loop.
	!!!But this behavior of 'glClearError()' has actually prevented(stopped) the destrustion
	of index buffer from index buffer itself. Remember that the 'IndexBuffer id(indicies, 6)' is
	a stack allocator object ----> the destructor method comes last(in the stack)
	only at the end of main function(very last that runs only at the front of return 0)!
	*/
	return 0;
}

