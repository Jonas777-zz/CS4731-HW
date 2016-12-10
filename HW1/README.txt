Homework 1: Polibook, an interactive polyline viewer and editor

by Jonas McGowan-Martin for CS4731

11/8/2016

Notes:
- all polyline files are located in the Polylines folder, and are in the GRS format.
- the vertex and fragment shaders are located in the GLSLExperiment folder, titled vshader1 and fshader1 respectively
- Drawing mode ('e' mode) is working fully, but the 'b' button behavior is different from what the problem requested: instead of 
holding down 'b' and then clicking to start a new line, you have to tap the b key and then the next time you click it will 
make a new line. I could not figure out how to make GLUT poll for buttons that are held down.



Homework 1 Overview

The aim of this project is to get you comfortable with working in 2D using OpenGL 4.x You will be setting viewports, world windows, drawing and tiling polylines, rendering fractals and exploring keyboard and mouse interaction routines. You will create a program Polibook which views polyline files in interesting ways and also allows you to interactively draw your own polylines onto the screen. Here goes: 

Step 1: Read more polyline files in GRS format: 

Use the starter code given to you for homework 0 as a starting point and build this project on that. This ensures that your code runs on the zoolab machines right away. You can get the HW0 starter code [ HERE ] The following polyline files are in the GRS format:

dino.dat
birdhead.dat
dragon.dat
house.dat
knight.dat
rex.dat
scene.dat
usa.dat
vinci.dat

The structure of GRS files is:

a number of comment lines, followed by a line starting with at least one asterisk: '*'.
The "extent" of the figure: (left, top, right, bottom).
The number of polylines in the figure.
The list of polylines: each starts with the number of points in the polyline, followed by the (x, y) pairs for each point.

First, modify the starter code to read in one of the GRS files. The plan is that when you re-compile it, the polyline file is drawn on the screen. There are a few things you should note for your implementation.
Be careful with how you pass parameters to the glViewport( ), Ortho2D( ) commands. Make sure you understand how they work.
The "extent" line of the GRS file will be used to set the world window i.e. passed to Ortho2D( )
If you look at the vertex coordinates of some of the GRS files, they are specified in floating point numbers, so you'll have to use float or double formats as appropriate. For instance, the x and y coordinates of the vertices of dino.dat are integers, while the vertices in the other GRS files are floating point numbers.
Note that a lot of the GRS files start with comments. You can either manually cut those out or read them in and dump them in your program.
The format for dino.dat is a little different in that it doesn't have the window dimensions (or comments) right at the top. Therefore, off the bat, a program which reads other GRS files without problems will have new problems with dino.dat file. You can either throw in a dummy extents window at the top of dino.dat or come up with a solution that works. Hint: a world window of (0,640,0,480) should work!!

Summary of Your program behavior
The figure above shows the layout of a polyline viewer you should produce. At the top of your viewport, 10 thumbnails (9 thumbnails of polylines above with 2 occurrences of dragon.dat) of the polyline files should be lined up. These 10 thumbnails should ALWAYS be visible at the top edge of the viewport whenever your program is running. The larger drawing area below the thumbnails is the main drawing area in which all drawing will be done. On start up, vinci.dat should be drawn in the main drawing area as a default. The default drawing color is red. The thumbnails AND main drawing automatically resize if the viewport is expanded or resized. 

Clicking on any thumbnails lined up at the top should make the corresponding polyline file the current drawing and draw a larger version of the polyline file to fill the main drawing area while maintaining aspect ratio. This is also known as the 'p state'. More details on the program behavior is given below. Your program should also have the following behavior and user (keyboard and mouse) interaction capabilities when you run it: 

Event: A key is pressed:
'p' key: (state p) Response: the program draws all polyline files provided at the top of the viewport in 10 equal-sized thumbnails (9 thumbnails of polylines above + 2 occurrences of dragon.dat) and draws vinci.dat in the "main drawing window". In the state p, if a user clicks on any of the thumbnails at the top, the corresponding polyline file in that thumbnail becomes the current drawing. The "main drawing space" is erased and the polyline in the thumbnail selected is redrawn to fill the entire "main drawing space". The default drawing color should be red

't' key: (state t) Response: A 5x5 tiling of all polyline files is drawn in the "main drawing space" (polyline thumbnails still at top edge). Repeatedly hitting the 't' key should create a different 5x5 tiling where the polyline drawn in any tile is "random".(polyline thumbnails still at the top edge and aspect ratios are maintained within each tile)

'e' key (state e) Response: PoliBook clears the screen and goes into drawing mode (polyline thumbnails still at the top edge). On the first click in the drawing area, a dot is drawn at wherever the user clicked. On subsequent clicks, a line is drawn from the last accepted mouse click position to the current mouse click position. Your program should be set up to accept up to 100 possible mouse clicks in one polyline. You can accept and store user-provided points in an array. If the 'b" key is held down while clicking, the current click point is NOT joined to the previous click point and instead a fresh polyline is started and drawn in addition to whatever previous polyline had been drawn. For instance, holding the "b" key down may be useful for beginning to draw the window of a house after drawing other parts. Part a) of the figure below shows how the drawing or addition of points works. The example shows a house in the process of being drawn; the user has just clicked at the position shown, and a line has been drawn from the previous point to the one designated by the mouse.

'g' key (state g) Response: Clear the screen and draw the gingerbread man described in class to fill the drawing area.

'f' key (state f) Response: Clear the screen and draw the Fern fractal described in class to fill the drawing area.

'c' key (state c) Response: Cycle the drawing color of whatever is drawn on the screen between Red, Green and Blue. For instance, if red is the current drawing color, hitting the 'c' key redraws everything in green. Hitting the 'c' key again redraws everything in Blue. Hitting the 'c' key again redraws everything in red, etc



Note: States (p and t) are polyline viewing states in which you can draw polyline files stored in a .dat file. State e is the polyline drawing state. In this state, you should be able to accept and draw new polylines onto the screen, but your program does NOT have to be able to edit the provided ones (e.g. dino.dat, usa.dat, etc). You also don't have to be able to save or write out the polyline file drawn on the screen to a file. You may choose to do that but no extra credit will be given. Make sure that reshape works for all states (p, t, e, f, and g). i.e. if the user grabs the lower right corner of the window and increases or reduces the screen window size, whatever was drawn in it before (and thumbnails) is redrawn to the largest possible size, without distortion (i.e. always maintain correct aspect ratio for each polyline file when drawn either as thumbnails or in the main drawing window).

Important Notes: No deprecated OpenGL commands, fixed function commands (glBegin, glVertex, etc) or immediate mode drawing commands should be used in your program. All drawing should be done using shaders, Vertex Buffer Objects, and glDrawArrays similar to the code in your textbook (and in the starter code) 
Submitting Your Work

Make sure to double-check that everything works before submitting. Submit all your executable and source files. Put all your work files (Visual Studio solution, OpenGL program, shaders, executable and input files into a folder and zip it. Essentially, after your project is complete, just zip the project directory created by Visual Studio. Submit your zip file using InstructAssist at [ https://ia.wpi.edu/cs4731/] . Do not email me your program or submit it via dropbox. To make your submission smaller, delete the \ipch folder in the visual studio. On recompilation, Visual Studio will regenerate this folder without problems. 

Create documentation for your program and submit it along with the project inside the zip file. Your documentation can be either a pure ASCII text or Microsoft Word file. The documentation does not have to be long. Briefly describe the structure of your program, what each file turned in contains. Explain briefly what each module does and tie in your filenames. Most importantly, give clear instructions on how to compile and run your program. MAKE SURE IT RUNS IN THE ZOOLAB before submission. Name your zip file according to the convention FirstName_lastName_hw1.zip 

General Hints

Here are a more few hints you might find useful:
You need to modify your keyboard( ) function in order to react to keyboard input from the user and your mouse( ) function to react to mouse input.
Section 2.11 of your text explains simple mouse and keyboard interaction using openGL.