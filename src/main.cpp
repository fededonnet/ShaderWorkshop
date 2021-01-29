#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream> // cout

// read shader file
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/vec3.hpp>

#include <vector>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const std::string ReadShader(const char* shaderPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string shaderString;
	std::ifstream shaderFile;
	
	// ensure ifstream objects can throw exceptions:
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		shaderFile.open(shaderPath);
		std::stringstream shaderStream;
		// read file's buffer contents into streams
		shaderStream << shaderFile.rdbuf();
		// close file handlers
		shaderFile.close();
		// convert stream into string
		shaderString = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	return shaderString;
}

int CreateCompileAndLinkShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glCreateShader.xhtml
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}

	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		return -1;
	}

	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return -1;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ShaderWorkshop", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	int shaderProgram = -1;
	{
		// Step 0 Read, build and compile the Vertex & Fragment shaders program
		const std::string vertexShaderSource = ReadShader("../res/shaders/shader.vs");
		const std::string fragmentShaderSource = ReadShader("../res/shaders/shader.fs");
		shaderProgram = CreateCompileAndLinkShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
	}

	// Step 1: Load geometry data
	std::vector<glm::vec3> vertexData =
	{
		glm::vec3(0.5f, -0.5f, 0.0f),	glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-0.5f, -0.5f, 0.0f),	glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f,  0.5f, 0.0f),	glm::vec3(0.0f, 0.0f, 1.0f)
	};

	// Step 2: Creates a VBO (Vertex buffer object)
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// Step 3: Store the geometry data into the buffer data
	// a: Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	{
		// b: Store geometry data into the buffer data using "glBufferData"
		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBufferData.xhtml
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData[0]) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Step 4: Creates a VAO (Vertex array object):
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	// Step 6: Specialize the vertex attributes in the VAO:
	glBindVertexArray(VAO);
	{
		// We need to bind a valid vbo before call glVertexAttribPointer()
		// https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object 
		// See "Vertex Buffer Object" section
		// https://stackoverflow.com/questions/3665671/is-vertexattribpointer-needed-after-each-bindbuffer
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		{
			// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
			// position attribute
			GLsizei stride = 6 * sizeof(float);
			void* offset = (void*)0;

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, offset);

			// color attribute
			offset = (void*)(3 * sizeof(float));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, offset);
			
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	// Loop
	while (!glfwWindowShouldClose(window))
	{
		// Input
		{
			processInput(window);
		}

		// Update
		{
			// TODO
		}

		// Render Pass
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			// Draw the VAO using the VBO
			glBindVertexArray(VAO);
			{
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				{
					glUseProgram(shaderProgram);
					// Update uniforms here!

					glDrawArrays(GL_TRIANGLES, 0, 3);
				}
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
			glBindVertexArray(0);
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	{ // Destroy the VAO
		glDeleteVertexArrays(1, &VAO);
	}

	{ // Destroy the VBO
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}