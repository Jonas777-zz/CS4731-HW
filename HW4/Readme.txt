HW4 for CS 4731 Computer Graphics
Jonas McGowan-Martin
12/9/16

All of my code is contained in GLSLExperiment/example1.cpp, and the shaders in the same directory under vshader1 and fshader1. The cow .PLY and all texture bitmap files are in the GLSLExperiment directory as well.

Keybinds are coded to the specifications of the assignment, including capitalization.

A: Toggles shadows on and off.

B: Toggles wall textures on and off.

C: Toggles reflection on and off. Turns off refraction if it is active.

D: Toggles refraction on and off. Turns off reflection if it is active.




Homework 4 Overview

In this project you will add more realism to your hierarchy from project 3 by adding texturing and shadows


Homework 4 Preparation

Understand how to load textures using the libbmpread library: In this project you will load textures in the bmp file format and use them to texture parts of your scene. The following [ Visual Studio solution ] contains a working program that reads in and displays a bmp file usain_bolt.bmp using the libbmpread library. To start off, download the Visual Studio Solution, compile it and run it. I have tested it and it works in the Zoolab. 

To read in the usain_bolt.bmp file, the program uses the [ libbmpread library ] which is a tiny, fast bitmap (.bmp) image file loader. Specifically, libbmpread is implemented as two files bmpread.c and bmpread.h which have been included in the Visual Studio starter code. bmp file loading functions and data structures are then used in the starter program to load the bmp file. Study this example focusing on how the bmp files are being read in. You can also get more documentation on the libbmpread library website [ Here ] . 

Ideally, you should complete this project by extending your hierarchy in project 3. Add a floor plane below your hierarchy and 2 walls behind it. You will implement shadows. The floor and walls will be surfaces on which the shadows can be cast. Your floor plane and two walls should be similar to the one shown below. You do not have to render the scene objects (table, rectangular block, lamp shade and teapot). 


However, if you were unable to get the hierarchy working properly, download the homework 2 starter code as a starting point. You can get this starter code here [ Starter Code ] Instead of the hierarchy, you can render a cow PLY file spinning continously in the center of the room with 2 walls and a floor. i.e. hovering slightly above the floor and away from the two walls. Render the cow mesh as a solid model. Apply per-fragment (Phong) smooth shading. You can get this cow PLY file here [ Cow PLY ] 

Homework 4 Specifics

The specific features you will implement in this homework are:
Texture the floor plane with grass and walls with stone: Texture map some grass onto the floor of your scene to make it look like grass is growing. Use the following grass texture [ grass.bmp ] Make it look like the walls were made of stone by texturing both walls using a stone texture [ stones.bmp] Use the libbmpread library above to read in your bmp texture files. Do not try to stretch these small textures over a large floor (or walls). Map each texture to the bottom left corner of the floor (or walls) undistorted and then use the appropriate parameters to repeat the texture to cover the entire floor. The example program in Appendix A.8 on page 638 of your text gives the complete working code of a program that does texturing. 

Shadows: Section 4.10 of your textbook describes a simple technique to render shadows using projection. Implement this shadow algorithm such that the shadows of the hierarchy/spinning cow are projected onto the floor plane and walls, and updated continuously as the hierarchy/cow spins. 

Environment Mapping: Add in reflective and refractive environment mapping using cubemaps to your scene. The example in section 7.9 (page 393) of your text presents a complete working example of a reflection map. The class slides present how to implement refraction. Use the following cube map with its [ cube map sides in 6 images ] as your environment map. For reference purposes, the complete environment map looks like this:


Summary of Your program behavior

Control your scene using the following keystrokes.
Key A: Toggle shadows ON/OFF. When ON, the shadows show up and when OFF the shadows do not show up. 

Key B Toggle ON/OFF between a scene with grass texture on floor and stone texture on the walls and a plain wall (no texturing). When ON, the floor is textured with grass and the walls are textured with stone. when OFF the floor and walls have no texturing, displaying a gray floor and blue walls. 

Key C: Toggle reflection ON/OFF. When ON, the hierarchy/cow PLY is drawn with reflection. When OFF, the hierarchy/cow PLY objects is drawn with no reflection (rendered as a solid model(s) with per-fragment lighting). Select an appropriate shininess (reflectivity) for the cow model.

Key D: Toggle refraction ON/OFF. When ON, the hierarchy/cow PLY is drawn with refraction. When OFF, the hierarchy/cow PLY file is drawn with no refraction ( rendered as a solid model(s) with per-fragment lighting). Select an appropriate refractive index for the hierarchy/cow model. 

Notes: No OpenGL fixed function commands (glBegin, glVertex, etc) or immediate mode drawing commands should be used in your program. All drawing should be done using shaders, retained mode, Vertex Buffer Objects, and glDrawArrays similar to the code in your textbook (and in your previous projects) 