# CS410p_Raycaster
Final project for my CS410p: Full Stack Web Development course.

# Overview
In this project, I set out to implement a raycast system. In brief,
Raycasting takes a top down, 2D world, and renders it as though it were
3D. It was the algorithm used to implement Wolfenstein 3D's graphics.

The features this has includes:
	* Renders textured walls, floors, and ceiling
	* Renders sprites ("things" in the world)
	* Renders skys for outdoorsy areas.
	* Can walk around the world.

# To Run
For the path of absolute least resistance, go to the following website: https://web.cecs.pdx.edu/~wynnliam

The final (for now) version of this can be found in two places:
1. In the folder ./public/index.html. Open this with either Firefox or Internet Explorer.
   Chrome seems to be a mixed bag at the moment, and I've no idea with Edge.
2. Use the command "make all" to make the most recent version, and then see the above
   item. See Compiling for more.

# Compiling
There are two ways to compile: use "make all", or "make all\_out". The former builds the raycaster
for the browser, the latter as an executable.

## make all
To run make all, you need the emscripten compiler installed, as well as the SDL 2 library.
Open up the make file and make sure the location for CMP is set appropriately. Next, make sure
the lines 3 and 68 in main.c are UNCOMMENTED. Then, make sure the while loop from line 60 to
65 is COMMENTED. After that, you should be able to run make all. Then, open the html file
./public/index.html. 

## make all_out
To run make all\_out, you need just SDL 2. However, you will need to change some lines in main.c.
You need to make sure the while loop on lines 60 to 65 is UNCOMMENTED and the lines on 3 and 68 are
COMMENTED out. Then, you should be able to run make all\_out. An executable a.out should now exist.
Open that up and enjoy the raycaster.
