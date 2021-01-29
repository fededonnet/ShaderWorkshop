# ShaderWorkshop

1 - Get/Clone https://github.com/fededonnet/ShaderWorkshop
2 - Unzip the file file
3 - Create folder called [build] in the root of the descompressed folder
4 - Run console in the [build] folder (shift + right click -> "Open Powershell windows here") 
5 - Generate a new visual studio proyect (x86) using CMAKE:
	a - [cmake ../ -A Win32] (This generates the VS solution)
	b - [cmake --build .] (This compiles the solution)
	c - [& .\Debug\ShaderWorkshopMain.exe] (Runs the executable)
6 - Once in the VS solution, take the ShaderWorkshop project and setup as starting proyect

VAO & VBO:
https://youtu.be/WMiggUPst-Q - OpenGL 3D Game Tutorial 2: VAOs and VBOs