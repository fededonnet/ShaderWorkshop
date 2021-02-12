# ShaderWorkshop

This is a simple OpenGL program uses to draw a geometry using shaders.

# Requirements:
	CMAKE - https://cmake.org/download/
	GIT - https://git-scm.com/downloads

# Building (Windows)

  - Get/Clone
  - [git clone https://github.com/fededonnet/ShaderWorkshop]
  - Create folder called [build] in the root.
  - Run console in the [build] folder (shift + right click -> "Open Powershell windows here") 
  - Generate a new visual studio proyect (x86) using CMAKE:
    - [cmake ../ -A Win32] (This generates the VS solution)
    - [cmake --build .] (This compiles the solution)
    - [& .\Debug\ShaderWorkshopMain.exe]
- Once in the VS solution, take the ShaderWorkshop project and setup as starting proyect 


- NOTE: Setup the "ShaderWorkshop" project as the starting project!!!


# VAO & VBO:

https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
https://www.khronos.org/opengl/wiki/GLAPI/glVertexAttribPointer

OpenGL 3D Game Tutorial 2: VAOs and VBOs: https://youtu.be/WMiggUPst-Q (mirar hasta que se ponga a codear)

# glDrawArrays:
https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml

# glBindTexture:
https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindTexture.xhtml

Videos:
1 - https://drive.google.com/file/d/1lK8PKbAIoqy4KvSps5HvVONIY4G1ahv5/view?usp=sharing
2 - https://drive.google.com/file/d/12w_75Fm5CDqAhZKM0XnP6lP_jjeEVOxJ/view?usp=sharing
3 - https://drive.google.com/file/d/1ONsSzWfP6k9M1ke1MzVk2_bw96ZZALc_/view?usp=sharing

Assimp:
	https://github.com/assimp/assimp