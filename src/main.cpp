#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream> // cout

// read shader file
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "AssimpHelper.h"


// http://stackoverflow.com/questions/24088002/stb-image-h-in-visual-studio-unresolved-external-symbol
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

glm::mat4 transform;
glm::mat4 projection;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

///////////////////// STBI HELPERS FUNCTIONS //////////////////////////////////////////////////////////////////////////////
unsigned char* LoadImage(
	const char* absoluteFilePath,
	int& x,
	int& y,
	int& channels_in_file,
	const unsigned int& desired_channels,
	const bool& flip_vertically)
{
	// Basic usage (see HDR discussion below for HDR usage):
	//    int x,y,n;
	//    unsigned char *data = stbi_load(filename, &x, &y, &n, 0);
	//    // ... process data if not NULL ...
	//    // ... x = width, y = height, n = # 8-bit components per pixel ...
	//    // ... replace '0' with '1'..'4' to force that many components per pixel
	//    // ... but 'n' will always be the number that it would have been if you said 0
	//    stbi_image_free(data)

	stbi_set_flip_vertically_on_load(flip_vertically);

	int _x = 0;
	int _y = 0;
	int _channels_in_file = 0;

	unsigned char* buffer = stbi_load(absoluteFilePath, &_x, &_y, &_channels_in_file, desired_channels);
	if (buffer == nullptr)
	{
		std::cout << "failed to load texture: " << absoluteFilePath <<std::endl;
		return nullptr;
	}

	x = _x;
	y = _y;
	channels_in_file = _channels_in_file;

	return buffer;
}

void FreeImage(unsigned char* pixelsData)
{
	stbi_image_free(pixelsData);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////// GENERAL GL SHADERS HELPERS FUNCTIONS //////////////////////////////////////////////////////////////////////////////
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
///////////////////// GENERAL GL TEXTURE HELPERS FUNCTIONS //////////////////////////////////////////////////////////////////////////////
void CreateGLTexture(unsigned int& texture)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	{
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

/// <summary>
/// 
/// </summary>
/// <param name="texture"> The texture </param>
/// <param name="width"> Specifies the width of the texture image. All implementations support texture images that are at least 1024 texels wide. </param>
/// <param name="height"> Specifies the height of the texture image, or the number of layers in a texture array, in the case of the GL_TEXTURE_1D_ARRAY and GL_PROXY_TEXTURE_1D_ARRAY targets. All implementations support 2D texture images that are at least 1024 texels high, and texture arrays that are at least 256 layers deep. </param>
/// <param name="internalformat"> Specifies the number of color components in the texture. Must be one of base internal formats given in Table 1, one of the sized internal formats given in Table 2, or one of the compressed internal formats given in Table 3, below. </param>
/// <param name="format"> Specifies the format of the pixel data. The following symbolic values are accepted: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL. </param>
/// <param name="type"> Specifies the data type of the pixel data. The following symbolic values are accepted: GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV. </param>
/// <param name="pixelsData"></param>
/// <returns></returns>
void SetImageToGLTexture(
	unsigned int texture,
	const int width, 
	const int height,
	GLint internalformat,
	GLenum format,
	GLenum type,
	const void* pixelsData,
	const bool generateMipMaps = false)
{
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml
	if (pixelsData == nullptr)
	{
		return;
	}

	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	{
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, pixelsData);
		if (generateMipMaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
		glfwTerminate();
		return -1;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);

	projection = glm::perspective(glm::radians(75.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 100.0f);
	//projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 100.0f);

	{
		//transform = glm::translate(transform, glm::vec3(1.0f, 0.0f, 0.0f));
	}

	//GLfloat cube_vertex_data[] =
	//{
	//	// Vertex position		// Vertex color			// UV
	//	// front
	//	-0.5f, -0.5f, 0.5f,		1.0, 0.0, 0.0,			0.666667f, 0.333333f,
	//	 0.5f, -0.5f, 0.5f,		0.0, 1.0, 0.0,			0.333333f, 0.333333f,
	//	 0.5f,  0.5f, 0.5f,		0.0, 0.0, 1.0,			0.333333, 0.000000f,
	//	-0.5f,  0.5f, 0.5f,		1.0, 1.0, 1.0,			0.666667, 0.000000f,
	//	// back
	//	-0.5f, -0.5f, -0.5f,	1.0, 0.0, 0.0,			0.333333f, 0.666667f,
	//	 0.5f, -0.5f, -0.5f,	0.0, 1.0, 0.0,			0.000000f, 0.666667f,
	//	 0.5f,  0.5f, -0.5f,	0.0, 0.0, 1.0,			0.000000f, 0.333333f,
	//	-0.5f,  0.5f, -0.5f,	1.0, 1.0, 1.0,			0.333333f, 0.333333f
	//};

	///* init_resources */
	//unsigned int cube_elements[] = 
	//{
	//	// front
	//	0, 1, 2,
	//	2, 3, 0,
	//	1, 5, 6,
	//	6, 2, 1,
	//	// back
	//	7, 6, 5,
	//	5, 4, 7,
	//	// left
	//	4, 0, 3,
	//	3, 7, 4,
	//	// bottom
	//	4, 5, 1,
	//	1, 0, 4,
	//	// top
	//	3, 2, 6,
	//	6, 7, 3
	//};

	struct VertexData
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 uv;
	};

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	if (AssimpHelper::ImportMesh("../res/models/Monkey.fbx", positions, uvs, normals, indices))
	{

	}

	std::vector<VertexData> vertices = std::vector<VertexData>(positions.size());
	for (size_t n = 0; n < positions.size(); ++n)
	{
		vertices[n].position = positions[n];
		vertices[n].uv = uvs[n];
		vertices[n].color = glm::vec3();
	}

	unsigned int IBO;
	{ // Create IBO (INDEX BUFFER OBJECT)
		glGenBuffers(1, &IBO);

		// Bind IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		{
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
		}
		// Unbind IBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Step 2: Creates a VBO (Vertex buffer object)
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	// Step 3: Store the geometry data into the buffer data
	// a: Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	{
		// b: Store geometry data into the buffer data using "glBufferData"
		// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBufferData.xhtml
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData[0]) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
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
			GLsizei bytePerVertex = 8 * sizeof(GLfloat);
			void* offset = (void*)0;

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, bytePerVertex, offset);

			// color attribute
			offset = (void*)(3 * sizeof(float));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, bytePerVertex, offset);

			// uvs attribute
			offset = (void*)(6 * sizeof(float));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, bytePerVertex, offset);
		}
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

	int shaderProgram = -1;
	{ // Create shader
		{
			// Step 0 Read, build and compile the Vertex & Fragment shaders program
			const std::string vertexShaderSource = ReadShader("../res/shaders/shader.vs");
			const std::string fragmentShaderSource = ReadShader("../res/shaders/shader.fs");
			shaderProgram = CreateCompileAndLinkShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());
		}
	}

	int transformUniformLocation = glGetUniformLocation(shaderProgram, "transform");
	int projectionUniformLocation = glGetUniformLocation(shaderProgram, "projection");
	int alphaUniformLocation = glGetUniformLocation(shaderProgram, "alpha");

	transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, -3.0f));
	transform = glm::rotate(transform, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	unsigned int texture = 0;
	{
		// Create and load texture:

		CreateGLTexture(texture);

		// Load the pixels data
		int w;
		int h;
		int channelsCount;
		unsigned char* pixelsData = LoadImage("../res/textures/placeHolder.jpg", w, h, channelsCount, 3, true);
		if (pixelsData != nullptr)
		{
			SetImageToGLTexture(texture, w, h, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, pixelsData);
		}
		FreeImage(pixelsData);
	}

	// Loop
	while (!glfwWindowShouldClose(window))
	{
		// Input
		{
			processInput(window);
		}

		// Update
		{
			//transform = glm::rotate(transform, 0.01f, glm::vec3(1.0f, 1.0f, 1.0f));
			
		}

		float alpha = 0.0f;

		// Render Pass
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			// Draw the VAO using the VBO
			glBindVertexArray(VAO);
			{
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
				{
					glUseProgram(shaderProgram);
					{
						// Update shader uniforms
						{
							glUniformMatrix4fv(transformUniformLocation, 1, GL_FALSE, &transform[0][0]);
							glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, &projection[0][0]);
							glUniform1f(alphaUniformLocation, alpha);
						}

						{ // Bind the textures
							glBindTexture(GL_TEXTURE_2D, texture);
						}
					}

					// Draw call
					glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
				}
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

	{
		// Destroy the Textures
		glDeleteTextures(1, &texture);
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
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		transform = glm::rotate(transform, -0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(-0.01f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(0.01f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(0.0f, 0.01f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		transform = glm::translate(transform, glm::vec3(0.0f, -0.01f, 0.0f));
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);

	projection = glm::perspective(glm::radians(75.0f), (float)width / (float)height, 0.01f, 100.0f);
}