# ShaderWorkshop

1 - Bajen el repo (git: https://github.com/fededonnet/ShaderWorkshop)
2 - Descomprimir file (ShaderWorkshop)
3 - Crear carpeta: build (en el root)
4 - Correr consola en la carpeta build (sobre [build] shift + click derecho: "Open Powershell windows here") 
5 - Generar proyecto en visual studio (x86) usando CMAKE:
	a - [cmake ../ -A Win32] (Genera la solution)
	b - [cmake --build .] (COMPILA)
	c - [& .\Debug\ShaderWorkshopMain.exe] (RUN)