HW2 for CS 4731 Computer Graphics
Jonas McGowan-Martin
11/22/16

All of my code is contained in GLSLExperiment/example1.cpp. All of the .PLY files are in the GLSLExperiment directory as well.

Keybinds are coded to the specifications of the assignment, capitalization included. For example, this means N and P must be pushed to display the next and previous wireframes, not n and p.



Homework 2 Overview

In this project, you will load a mesh stored in the .ply file format, render it as a 3D wireframe model using Vertex Buffer Objects and also add keyboard control that lets us move the .ply file around. A few optional preparation steps are suggested. You will not turn in the code which you generate in your preparatory steps. 
Preparation

Read section 3.6 of your text: Code to render a cube is described in that section.

Some starter code, a working implementation of this cube program, which runs in the zoolab has been created. You can get this starter code here [ Starter Code ] . 
Compile the cube program and make sure it runs okay in the zoolab. As mentioned previously, Course staff will help mostly with questions about getting the starter code working in the zoolab. However, one problem that some students may have problems on their home machines is that OpenGL 3.2 has problems with glGenVertexArrays under Linux. You can find some fixes to this bug on the class [ FAQ Page ] 

Modify your program to read in .ply files and store them in a vertex list data structure. You can get 43 PLY files to work with [ Here ] . Further explanations about the format for PLY files are given below.

Modify your program to render wireframe drawings of your .ply files from your vertex list using Vertex Buffer Objects (VBOs) and glDrawArrays as with the cube. Here's an example wireframe drawing of the cow.ply file: 



Set up a Current Transform Matrix (CTM) as described in section 3.11 of the text. The starter code includes [ Angel.h ] which already includes [ mat.h ] and [ vec.h] You may use the matrix and vector manipulation methods in these header files for your work.

Calculate the normal of each mesh face using the Newell method and then generate face normals 

Pulsing Meshes One simple yet visually interesting way to animate a mesh is to make it "pulse" by translating each face some fixed amount in its normal direction. By linearly interpolating the position of each vertex belonging to a given face between its original position and v+cn (where c is a constant) and then interpolating back in the opposite direction, we can make the mesh bulge outward and then recede in a smooth fashion. This operation should make the meshes look like they are "breathing" back and forth. Note that when the mesh breathes in, the faces of the mesh are in their original positions and when the mesh breathes out, the faces move outwards and temporarily separate from neighboring faces. Make sure the faces move out enough for the bulging effect to be noticeable and make the face movements nice and smooth (Not too fast).

Implement keyboard controls that enable you to perform the keyboard controls described below in the section "Behavior of your submitted program".
Behavior of your submitted program

User hits 'W' (Draw your wireframe) at a suitable initial position from the viewer. 

User hits 'N' (Draw next wireframe) Organize the PLY files in a list going from 1-43. Hitting N should load and draw the next wireframe model to the current one in your list of PLY files. You can hardcode filenames if you want. The PLY files may not all be of the same size. So to properly set up the viewing position using LookAt, you may have to calculate the bounding box of the mesh and then set your view distance to a suitable multiple of the bounding box

User hits 'P' (Draw previous wireframe) Organize the PLY files in a list going from 1-43. Hitting P should load and draw the previous wireframe model to the current one in your list of PLY files. 

User hits 'X' (Translate your wireframe in the +ve X direction) Continously move your wireframe some small units along the +ve X axis and redraw it. Use the idle function to animate this. The ply file should continue to slide along the +ve X axis till the user hits 'X' again. Essentially, the 'X' key acts as a toggle key. If the ply file is stationary and the user hits the 'X' key, the ply file should continue to slide along the +ve X axis until the user hits 'X' again. Camera position remains fixed for this translation and all other translations below. The exact amount to move the ply file before redrawing will affect how much and how much your translation is apparent depends on how far you positioned your wireframe from the viewer. So, it's left to you as a design choice to pick an appropriate distance to translate the wireframe along the +ve X axis each time the user hits 'X'. 

User hits 'x' (Translate your wireframe in the -ve X direction) Use the idle function to continuously move your wireframe some units along the -ve X axis. The number of units to translate your wireframe each time the user hits 'x' is left to you as a design choice. 

User hits 'Y' (Translate your wireframe in the +ve Y direction) Use the idle function to continuously move your wireframe some units along the +ve Y axis. The number of units to translate your wireframe each time the user hits 'Y' is left to you as a design choice. 

User hits 'y' (Translate your wireframe in the -ve y direction) Use the idle function to continuously move your wireframe some units along the -ve Y axis. The number of units to translate your wireframe each time the user hits 'y' is left to you as a design choice. 

User hits 'Z' (Translate your wireframe in the +ve Z direction) Use the idle function to continuously move your wireframe some units along the +ve Z axis. The number of units to translate your wireframe each time the user hits 'Z' is left to you as a design choice. 

User hits 'z' (Translate your wireframe in the -ve Z direction) Use the idle function to continuously move your wireframe some units along the -ve Z axis. The number of units to translate your wireframe each time the user hits 'z' is left to you as a design choice. 

User hits 'R' (Rotate your wireframe in an X-roll about it's CURRENT position) Just like a rotisserie chicken, rotate your wireframe smoothly 360 degrees at a moderate speed (X-roll) about its CURRENT position (not about the center of the scene) This rotation is NOT the same as moving the wireframe in a wide arc. The rotation should be about the X axis and the wireframe should not translate while rotating. Hint: Use double buffering (glutSwapBuffers( )) to make the rotation smooth. You can continously update the new wireframe positions and redisplay the meshes in the glutIdleFunc function. 

User hits Key 'B': Toggle pulsing meshes ON/OFF. When ON, the mesh faces pulse back and forth continuously as described above. When OFF the meshes do not pulse. Hint: Use double buffering (glutSwapBuffers( )) to make the breathing smooth. You can continously update the new vertex positions and redisplay the meshes in the glutIdleFunc function.

User hits Key 'm': Toggle a drawing of each face normal (ON/OFF). When ON, the mesh normals of all mesh faces are drawn. When off, the face normals are not drawn. Each face normal is drawn as a short line starting from the middle of each face and extending outwards. When you draw all normals, it will look like the mesh has pins sticking out of each face

User hits Key 'h': Increment the amount of shearing of the wireframe along the X axis by a small amount. Repeatedly hitting the 'h' key should shear the wireframe by a bit more and more. Note that after you shear the mesh, performing a transform (e.g. rotation, scale or translate) should transform the sheared mesh. 

User hits Key 'H': Decrease the amount of shearing of the wireframe along the X axis by a small amount. Repeatedly hitting the 'H' key should shear the wireframe by a bit less and less. Note that after you shear the mesh, performing a transform (e.g. rotation, scale or translate) should transform the sheared mesh.