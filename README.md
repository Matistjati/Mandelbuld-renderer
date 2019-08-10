
# Mandelbuld-renderer (name subject to change)
A fractal renderer written using opengl.<br/>
An example image rendered in realtime with a 1070 ti<br/>
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

**X**: Pause time (this affects things like the sun and some 2d fractals)<br/>
**Z**: flip the fractal across the horizontal axis.<br/>
**WASD**: movement<br/>
**C/V**: Increase/decrease the power used for the set.<br/>
## **3D**<br/>
**Space/shift**: Up/down<br/>
**R/F**: Increase/decrease the a generic parameter used differently depending on the fractal.<br/>
**Q/E**: Tilt the world. Navigation is subpar when doing this.<br/>
**Scrolling the mousewheel**: Zoom into the fractal. If you want to go for "deep" zooms, increase maxIterations and decrease zoomDetailRatio (a small number greater than 0).<br/>
## **2D**<br/>
**Q/E**: Zoom in/out<br/>

## **Dependencies**: GLFW3, glew, glm, zlib, libpng<br/>
## **Resources**: <br/>
[A video to set up a basic raymarching engine (where i started)](https://www.youtube.com/watch?v=yxNnRSefK94&list=LLk3DQC5zS5U7Icg-YoE8Rsw&index=6&t=0s)<br/>

[An implementation of the mandelbulb](https://www.shadertoy.com/view/ltfSWn)<br/>

[General on 3D distance estimators and shading](http://blog.hvidtfeldts.net/index.php/2011/06/distance-estimated-3d-fractals-part-i/)<br/>

[An algorith for polygonizing a scalar field (what we use for 3D fractals). This can be used for 3D printing](http://paulbourke.net/geometry/polygonise/)<br/>

[In-depth techniques for coloring the mandelbrot set](https://www.math.univ-toulouse.fr/~cheritat/wiki-draw/index.php/Mandelbrot_set)<br/>

[An overview of the buddhabrot and ways to optimize it](https://benedikt-bitterli.me/buddhabrot/)<br/>

[An algorithm for zooming into the buddhabrot (this is nontrivial unlike the case with the mandelbrot set)](http://www.steckles.com/buddha/)<br/>
