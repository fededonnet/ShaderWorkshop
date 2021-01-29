# ShaderWorkshop

This is a simple OpenGL program wich uses a simple shader and draw a triangle.

# Building

  - Get/Clone https://github.com/fededonnet/ShaderWorkshop
  - Unzip the file file
  - Create folder called [build] in the root of the descompressed folder
  - Run console in the [build] folder (shift + right click -> "Open Powershell windows here") 
  - Generate a new visual studio proyect (x86) using CMAKE:
    - [cmake ../ -A Win32] (This generates the VS solution)
    - [cmake --build .] (This compiles the solution)
    - [& .\Debug\ShaderWorkshopMain.exe] (Runs the executable)
- Once in the VS solution, take the ShaderWorkshop project and setup as starting proyect 

# VAO & VBO:

https://www.khronos.org/opengl/wiki/Vertex_Specification#Vertex_Array_Object
https://youtu.be/WMiggUPst-Q - OpenGL 3D Game Tutorial 2: VAOs and VBOs
https://www.khronos.org/opengl/wiki/GLAPI/glVertexAttribPointer

# glDrawArrays:
https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
