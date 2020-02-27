
# Mandelbuld-renderer (name subject to change)
A realtime fractal renderer written using opengl. Gui is done with nanogui.<br/>
It supports many fractals, including the mandelbrot, mandelbulb, mandelbox, buddhabrot, buddhagram and variations of these.<br/>
Will either use approximated lighting models or path tracing with supports for a reflective ocean and clouds.<br/>
There are plenty of sliders to change different aspects of the fractal. Each one has its own appropriate options.<br/>
A 4096 x 2160 with 1000 as max iteration count:<br/>
![Alt text](/Images/buddhabrot.png?raw=true)
A mandelbulb rendered in realtime with a 1070 ti<br/>
![Alt text](/Images/sample.png?raw=true "Rendered in realmtime with a 1070 ti")
# **Controls**<br/>
## **General**<br/>
**Alt+R/F**: Change between 2D and 3D fractals. <br/>
**Alt+E/D**: Change the current fractal file. Fractals will often be very different between files, but ones in the same will be similiar.<br/>
**Alt+W/S**: Change the "specification" in the file. The fractal will be different rather different.<br/>
**Alt+Q/A**: Change the operations for the main loop. This will have the least difference, but the change will still be easily noticable.<br/>

**Ctrl+Z**: Save an image of the current screen (some parameters will be different, making the image look better at the cost of rendering speed).<br/>
**Ctrl+X**: Break if a debugger is attached.<br/>
**Ctrl+Q/A, W/A, E/D etc**: change some aspects of the fractal, like the coloring method, initial parameter changes and adding extra operations to the main loop.<br/>

**X**: Pause time (this affects things like the sun and some 2d fractals).<br/>


## **Movement**<br/>
**Space/shift**: Up/down.<br/>
**WASD**: movement.<br/>

**Scrolling the mousewheel or Q/E**: Zoom into or out of the fractal.<br/>

## **Dependencies**: GLFW3, glew, glm, zlib, libpng, nanogui<br/>
## **[Resources](https://github.com/Matistjati/Mandelbuld-renderer/blob/master/resources.md)** <br/>




