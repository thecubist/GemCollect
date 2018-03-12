#pragma region includes
#include <math.h>						
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   //used for seeding rand
#include <string.h>
#include <string>
#include <assert.h>
#include <iostream>	//Needed for console output (debugging)
#include <gl/freeglut.h>
#include <iostream>
#include <GL\glut.h>

#ifdef WIN32
#include "gltools.h"
#include <Windows.h>		// Must have for Windows platform builds
#include <gl\gl.h>			// Microsoft OpenGL headers (version 1.1 by themselves)
#include <gl\glu.h>			// OpenGL Utilities
#include "glm.h"
#endif

using std::cout;
using std::endl;
using std::string;
//
//  texture.cpp
//
//
//  Created by Philip Trwoga on 09/11/2012. Modified on 20/1/16 and 20/12/17 to include the many VS2015 changes
//  Copyright (c) 2012 __Philip Trwoga__. All rights reserved.
// DO NOT DISTRIBUTE WITHOUT THE AUTHOR'S PERMISSION  

//note that this needs gltools.h and gltools.cpp to be included in the shared/library directory
#pragma endregion

#pragma region Declarations				
#pragma region Texture Declarations
//can define any number of textures here - we just have 2 images
//these are just integers so you can use them as such

#define TEX_PINGU 0
#define TEX_HORSE 1
#define TEX_FLOOR 2
#define TEX_HUD 3
#define TEX_JUPI 4
#define TEX_STARS 5
#define TEX_STAR_FIELD 6
//below is simply a character array to hold the file names
//note that you may need to replace the below with the full directory root depending on where you put your image files
//if you put them where the exe is then you just need the name as below - THESE IMAGES ARE IN THE DEBUG FOLDER, YOU CAN ADD ANY NEW ONES THERE 
const char *textureFiles[] = { "pingu.tga", "horse.tga","floor.tga", "hud.tga", "jupiterC.tga", "starField.tga", "advStarField.tga" }; //LOADS IN WRONG ORDER 
const GLuint texCount = sizeof(textureFiles) / sizeof(char*); //automatically count number of textures
GLuint textures[texCount];
GLint TEXTURE_OFFSET_PATCH = 2;

#pragma endregion
#pragma region Base Lighting
				//for lighting if you want to experiment with these
GLfloat mKa[4] = { 0.11f,0.06f,0.11f,1.0f }; //ambient
GLfloat mKd[4] = { 0.43f,0.47f,0.54f,1.0f }; //diffuse
GLfloat mKs[4] = { 1.0f,1.0f,1.0f,1.0f }; //specular
GLfloat mKe[4] = { 0.5f,0.5f,0.0f,1.0f }; //emission  
#pragma endregion
#pragma region Keyboard Input Variables
GLboolean upArrow = false;
GLboolean downArrow = false;
GLboolean leftArrow = false;
GLboolean rightArrow = false;
GLboolean wKey = false;
GLboolean aKey = false;
GLboolean sKey = false; 
GLboolean dKey = false;
GLboolean qKey = false;
GLboolean eKey = false;
GLboolean shiftKey = false;
#pragma endregion
#pragma region Cam variables
struct vec3 { GLfloat x; GLfloat y; GLfloat z; };
const GLfloat SPEED = 3.0f;
const GLfloat SPEEDMULT = 2.0f;
vec3 camOffset;
vec3 camRotation;
vec3 playerPos; //will be used for collision detection
vec3 playerRotation; //used for rotation readout only
#pragma endregion
#pragma region Coloured Light Variables
// Useful lighting colour values
GLfloat  whiteLightBright[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat  redLight[] = { 1.0f, 0.0f, 0.0f, 1.0f };
GLfloat  whiteLightLessBright[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat	 lightPos[] = { 100.0f, 100.0f, 50.0f, 1.0f };
#pragma endregion
#pragma region Texture Load Variables
//we need these for the texture loader
//they are to do with the image format and size
GLint iWidth, iHeight, iComponents;
GLenum eFormat;
// this is a pointer to memory where the image bytes will be held 
GLbyte *pBytes0;
#pragma endregion
#pragma region world space variables
#define RENDER_DISTANCE 50000
#pragma endregion
#pragma region Gem variables
struct gem { vec3 pos; GLboolean collected = false; GLboolean wrongGemCollide = false; GLfloat isoScale; };
gem gems[6]; //change the array size to make more gems
long gemRotation = 0;
#define ISO_SCALE 10.0f
GLint collectedGems = 0;
const GLfloat hitboxSize = (ISO_SCALE / 100 * 125);
#pragma endregion
#pragma region Developer Declarations
#define DEV_MODE false //used to enable developer debugging features
GLboolean lsqBracket = false;
GLboolean rsqBracket = false;
GLboolean hash = false;
GLint gridDrawQuantity = 9000;
GLfloat gridSpacing = 100.0f;
GLboolean gridX = true, gridY = false, gridZ = true;
GLboolean texDebugTut = false;
GLboolean windowSizeReadout = false;
//developer functions
void drawGemWaypoints(GLfloat lineWidth);
void printCurrentModelMatrix();
void drawAxis(GLfloat scale);
void resetPosition();
void drawHitbox();
void drawGrid(GLfloat spacing);
#pragma endregion
#pragma region Misc declarations
GLboolean drawHud = true;
GLUquadricObj *skySphere;
GLUquadricObj *jupiter;
GLint score = 0;
GLint windowWidth;
GLint windowHeight;
GLboolean showHud = false;
long planetRotation = 0;
#pragma endregion
#pragma region Function Declarations
int ranInt(int min, int max);
void drawTextureTorus(int image, GLfloat innerRad, GLfloat outerRad, GLint sides, GLint rings);
void drawTextureSphere(GLint image, GLUquadricObj *sphere, GLboolean CWwind, GLint radius, GLint slices, GLint stacks);
void drawIcoso(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ);
void drawGem(int gemId);
void drawTexturedQuad(int image);
void drawOverlay(int image);
void cameraRotation();
void cameraFrontBack();
void cameraLeftRight();
void cameraUpDown();
void processCamera();
void RenderScene(void);
void loadTextures();
void setupGems(GLfloat isoScale);
void Update(int value);
void ChangeSize(int w, int h);
void processsSpecialKeys(int key, int x, int y);
void keyReleased(unsigned char key, int x, int y);
void specialKeyReleased(int key, int x, int y);
void processKeys(unsigned char key, int x, int y);
void init();
void checkCollision(vec3 originVector);
void textGen(GLfloat x, GLfloat y, char *inputString);
void textGen(GLfloat x, GLfloat y, char *inputString, GLfloat red, GLfloat green, GLfloat blue);
void drawString(void *font, char *string);
void draw2D();
void draw3D();
#pragma endregion
#pragma region Screenspace declarations
const GLint screenX = 800;
const GLint screenY = 600;
#pragma endregion
#pragma endregion

#pragma region Developer Funcs
#pragma region Print Modelview Matrix
//print the current model view matrix in the console
void printCurrentModelMatrix()
{
	system("cls"); //clearing the console (CAUSES LAG)
	
	GLfloat matrix[16]; //creating a float array to hold the matrix information
	//storing the matrix information in a matrix array
	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	cout << "________________________________________________ \n";

	for (GLint i = 0; i < 4; i++) //new line loop
	{
		for (GLint j = 0; j < 4; j++) //print matrix line loop
		{
			cout << "[" << matrix[i*j] << "]"; 
		}
		cout << endl;
	}
	cout << "________________________________________________ \n";
}
#pragma endregion
#pragma region Draw Waypoints
/* Draws a line to each gem currently in the world*/
void drawGemWaypoints(GLfloat lineWidth)
{
	glLineWidth(lineWidth);
	glColor3f(1.0f, 0.2f, 0.7f);
	GLfloat offset = 0.0f;
	glFrontFace(GL_CW);
	for (int i = 0; i < (sizeof(gems) / sizeof(gem)); i++)
	{
		glBegin(GL_LINES); 
		glVertex3f(playerPos.x + offset, playerPos.y + offset, playerPos.z + offset); //start at player location
		glVertex3f(gems[i].pos.x, gems[i].pos.y, gems[i].pos.z); //end at gem location
		glEnd();
	}
}
#pragma endregion
#pragma region reset position
//reset the location vectors for the player hitbox without moving the camera to allow for 
//free camera view of hitboxes
void resetPosition()
{
	cout << "currently located at vec3(" << playerPos.x << "," << playerPos.y << "," << playerPos.z << ") \n";
	cout << "moving hitbox to vec3(" << playerPos.x - playerPos.x << "," << playerPos.x - playerPos.y << "," << playerPos.x - playerPos.z << ") \n";
	glRotatef(0,1.0f,1.0f,1.0f);
	glTranslatef(-playerPos.x,-playerPos.y,-playerPos.z);
	playerPos.x = 0;
	playerPos.y = 0;
	playerPos.z = 0;
	
	camRotation.x = 0;
	camRotation.y = 0;
	camRotation.z = 0;
}
#pragma endregion
#pragma region Axis
//draw a 3D axis
void drawAxis(GLfloat scale)
{
	if (DEV_MODE)
	{
		glPushMatrix();
		glScalef(scale, scale, scale);
		gltDrawUnitAxes();
		glPopMatrix();
	}
}
#pragma endregion
#pragma region Draw hitboxes
//draw the hitboxes of all gems and the player
void drawHitbox()
{
	if (DEV_MODE) //run only if devmode is enabled
	{ 
		glLineWidth(1.0f);
		glColor3f(1.0f, 1.0f, 1.0f); 
		for (int i = 0; i < (sizeof(gems) / sizeof(gem)); i++)
		{
			glPushMatrix();
			glTranslatef(gems[i].pos.x, gems[i].pos.y, gems[i].pos.z);
			glutWireSphere(hitboxSize, 10, 10); 
			glPopMatrix();
		}

		//drawing the hitbox of the player (slightly larger than the actual hitbox so I can see it)
		glColor3f(0.0f, 1.0f, 0.0f);

		glPushMatrix();
		glTranslatef(playerPos.x, playerPos.y, playerPos.z);
		glutWireSphere(hitboxSize, 10, 10);
		glPopMatrix();

	}
}
#pragma endregion
#pragma region Draw Grid
//draw a grid along the X,Y and Z axis for spacial debugging
//(all settings are declared as globals)
void drawGrid(GLfloat spacing)
{

	glLineWidth(1.0f);

	glBegin(GL_LINES);
	for (GLfloat i = -gridDrawQuantity; i < gridDrawQuantity; i += spacing)
	{
		if (gridZ)
		{
			//cout << "drawz \n";
			glColor3f(0, 0, 1);
			glVertex3f(i, 0, -gridDrawQuantity);
			glVertex3f(i, 0, gridDrawQuantity);
		}

		if (gridY)
		{
			//cout << "drawy \n";
			glColor3f(0, 1, 0);
			glVertex3f(i, -gridDrawQuantity, 0);
			glVertex3f(i, gridDrawQuantity, 0);
		}

		if (gridX)
		{
			//cout << "drawx \n";
			glColor3f(1, 0, 0);
			glVertex3f(-gridDrawQuantity, 0, i);
			glVertex3f(gridDrawQuantity, 0, i);
		}
	};
	glEnd();
}
#pragma endregion
#pragma endregion

#pragma region Misc
//return a vector as a string
string toString(vec3 vector)
{
	return ("vec3(" + std::to_string(vector.x) + ", " + std::to_string(vector.y) + ", " + std::to_string(vector.z) + ")");
}

//creates a random int between the minimum and maximum values
int ranInt(int min, int max)
{
	return rand() % max + min;
}

/*uses a glTranslatef directly from the passed vector and
translate negatively if negative boolean is true*/
void glTranslatefv(vec3 pos, GLboolean negative)
{
	if (!negative) { glTranslatef(pos.x, pos.y, pos.z); }
	else { glTranslatef(-pos.x, -pos.y, -pos.z); }
}

//prints controls in the console and if devmode is active 
//then dev controls will also be printed
void printControls()
{
	cout << "___________________controls___________________\n";
	cout << "UP - fly up\n";
	cout << "DOWN - fly down\n";
	cout << "LEFT - tilt left(Z axis)\n";
	cout << "RIGHT - tilt right(Z axis)\n";
	cout << "A - strafe left\n";
	cout << "D - strafe right\n";
	cout << "W - move forward\n";
	cout << "S - move backward\n";
	cout << "Z - point to previous crystal\n";
	cout << "X - point to next crystal\n";
	cout << "Q - turn left(Y axis)\n";
	cout << "E - turn right(Y axis)\n";

	if (DEV_MODE)
	{
		cout << "--------DEVELOPER-------\n";
		cout << "[-show waypoints\n";
		cout << "] - reset location\n";
		cout << "# - regenerate gems\n";
	}
}
#pragma endregion

#pragma region Collision Detection
/* check the position of each gem against the passed vector by using
pythagoras in 3D. If the gems have collided with the vector then set 
them to collected and increment the relevant counters*/
void checkCollision(vec3 originVector)
{
	/*
	Theory for collision detection was based off of the following extract from 
	http://www.bbc.co.uk/schools/gcsebitesize/maths/geometry/pythagoras3drev1.shtml

	First use Pythagoras' theorem in triangle ABC to find length AC.
	AC2 = 62 + 22
	AC = sqrt -> 40
	You do not need to find the root as we will need to square it in the following step. Next we use Pythagoras' theorem in triangle ACF to find length AF.
	AF2 = AC2 + CF2
	AF2 = 40 + 32
	AF = sqrt -> 49
	AF = 7
	*/

	GLfloat distance, acs, afs, x, y, z;

	for (int i = 0; i < (sizeof(gems) / sizeof(gem)); i++)
	{
		if (!gems[i].collected) //if the current gem was not collected then check collision
		{
			x = originVector.x - gems[i].pos.x;
			y = originVector.y - gems[i].pos.y;
			z = originVector.z - gems[i].pos.z;

			acs = (x*x) + (z*z); 
			afs = (acs + (y*y));
			distance = sqrt(afs);

			if(distance <= hitboxSize && i == collectedGems) //if the distance between objects is within hitbox distance and it is the correct gem
			{
				gems[i].collected = true;
				collectedGems++;
				score++;

				if(collectedGems >= (sizeof(gems) / sizeof(gem))) //if all gems are collected
				{
					setupGems(ISO_SCALE);
					//outputting basic game info to console
					cout << "______________________________________________\n";
					cout << "All gems collected. Resetting gems \n";
					cout << "Current score is " << score << " points\n";
					cout << "______________________________________________\n";

					//play victory tune
					Beep(500, 10);
					Beep(700, 30);
					Beep(1000, 30);
					Beep(1500, 50);
				}
				else //if there are still gems left
				{
					//play pickup noise
					Beep(500, 20);
					Beep(700, 30);
				}
			}
			else if (distance <= hitboxSize && i != collectedGems) //if you are trying to pick up the wrong gem
			{
				gems[i].wrongGemCollide = true; //if you are colliding with the wrong gem
			}
			else
			{
				gems[i].wrongGemCollide = false; //reset wrongGemCollide if you are not colliding with it
			}
		}
	}
}
#pragma endregion

#pragma region Draw Funcs
#pragma region Torus
//draw a textured torus and scale based on passed parameters
void drawTextureTorus(int image, GLfloat innerRad, GLfloat outerRad, GLint sides, GLint rings)
{
	glEnable(GL_TEXTURE_GEN_S); //allowing the generated texture coords to be used
	glEnable(GL_TEXTURE_GEN_T); //allowing the generated texture coords to be used
	glEnable(GL_TEXTURE_2D); //allowing the use of 2d textures

	glBindTexture(GL_TEXTURE_2D, image + TEXTURE_OFFSET_PATCH); //binding a 2d texture

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //generating texture coords based on a sphere's verts
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //generating texture coords based on a sphere's verts

	glPushMatrix();
	//glScalef(20.0f, 20.0f, 20.0f);

	//PARAMETER PASS THE SCALES
	//glutSolidTorus(5.0f, 20.0f, 20, 20); //making the torus 
	glutSolidTorus(innerRad, outerRad, sides, rings); //making the torus 
	glPopMatrix();

	glDisable(GL_TEXTURE_2D); //disabling the use of 2d textures
	glDisable(GL_TEXTURE_GEN_S); //disabling texture coord generation
	glDisable(GL_TEXTURE_GEN_T); //disabling texture coord generation
}
#pragma endregion
#pragma region Sphere
//draw a textured sphere quadric and scale based on passed parameters. additionally
//the winding boolean can be passed to allow easy creation of interior textured spheres
void drawTextureSphere(GLint image, GLUquadricObj *sphere, GLboolean CWwind, GLint radius, GLint slices, GLint stacks)
{
	if (CWwind) { glFrontFace(GL_CW); }
	else { glFrontFace(GL_CCW); }
	glEnable(GL_TEXTURE_2D); //allowing the use of 2d textures
	glBindTexture(GL_TEXTURE_2D, (image + TEXTURE_OFFSET_PATCH)); //binding a 2d texture

	gluQuadricDrawStyle(sphere, GLU_FILL);
	gluQuadricTexture(sphere, GL_TRUE);
	gluQuadricNormals(sphere, GLU_SMOOTH);

	gluSphere(sphere, radius, slices, stacks); //making the sphere 

	glDisable(GL_TEXTURE_2D);	 //disabling the use of 2d textures
	glFrontFace(GL_CCW);
}
#pragma endregion
#pragma region Icosohedron
//draw an icosohedron and scale it up based on passed values
void drawIcoso(GLfloat scaleX, GLfloat scaleY, GLfloat scaleZ)
{
	glPushMatrix();
	glScalef(scaleX, scaleY, scaleZ);
	glutSolidIcosahedron();
	glPopMatrix();
}
#pragma endregion
#pragma region Quad
/*draw a textured quad and apply a texture to it manually with 
texture coords(only used for debugging textures)*/
void drawTexturedQuad(int image)
{
	//add some lighting normals for each vertex
	//draw the texture on the front
	glEnable(GL_TEXTURE_2D);
	glColor3f(0.8, 0.8, 0.8);
	glEnable(GL_TEXTURE_2D);
	//bind the texture 
	glBindTexture(GL_TEXTURE_2D, textures[image]);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-50.0, 0.0, 100.0);
	glTexCoord3f(1.0, 0.0, 0.0);
	glVertex3f(50.0, 0.0, 100.0);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(50.0, 100.0, 100.0);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-50.0, 100.0, 100.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}
#pragma endregion
#pragma region Overlay
//draw a 2D quad and apply a texture to it
void drawOverlay(int image)
{
	if(drawHud)
	{		
		glMatrixMode(GL_PROJECTION); //making sure the matrix mode is correct

		glEnable(GL_BLEND); //enabling blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //using an alpha channel blend
		
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[image]);
		glFrontFace(GL_CW);
		glColor4f(0.0f, 0.0f, 0.0f, 0.4f); //setting the colour to black with alpha
		
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(0.0 + playerPos.x, 0.0 + playerPos.y, playerPos.z);
		glTexCoord2f(1.0, 0.0);
		glVertex3f(windowWidth + playerPos.x, 0.0 + playerPos.y, playerPos.z);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(windowWidth + playerPos.x, windowHeight + playerPos.y, playerPos.z);
		glTexCoord2f(0.0, 1.0);
		glVertex3f(0 + playerPos.x, windowHeight + playerPos.y, playerPos.z);
		glEnd();

		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);

		glMatrixMode(GL_MODELVIEW); //switching the matrix mode back
	}
}
#pragma endregion
#pragma region gem
/* draw a gem and the ring to go around it. change the gem colour if 
the gem should be picked up and change ring texture if it has been 
collected. additionally this function will move the gems to their
respectful locations and spin the hoops around them*/
void drawGem(int gemId)
{
	glColor3f(0.0f, 0.5f, 1.0f); //setting initial colour incase textures fail
	
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f); //move to 0,0 for hoop spin
	glRotatef(gemRotation * 5, 0.0f, 1.0f, 0.0f); //rotate the hoop
	if (!gems[gemId].collected) 
	{ 
		if (gems[gemId].wrongGemCollide) { drawTextureTorus(TEX_HUD, 1.0f, 30.0f, 20, 20); gemRotation += 3; } //draw the wrong gem hoop and make the hoops go faster
		else { drawTextureTorus(TEX_STARS, 5.0f, 20.0f, 20, 20); } //draw the hoop normally
	} 
	else { drawTextureTorus(TEX_HORSE, 5.0f, 20.0f, 20, 20); } //draw the collected hoop
	glTranslatef(gems[gemId].pos.x, gems[gemId].pos.y, gems[gemId].pos.z); //translate to gem location
	glPopMatrix();


	if (!gems[gemId].collected)  //if the gem was not collected yet
	{ 
		glEnable(GL_BLEND); //enabling blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //allow alpha blend
		glClearColor(0.0, 0.0, 0.0, 0.0);
		if (gemId != collectedGems) 
		{ 
			if (!gems[gemId].wrongGemCollide) { glColor4f(1.0f, 0.0f, 0.0f, 0.5f); } //if the gem should not be collected yet use this colour
		} 
		else { glColor4f(0.0f, 1.0f, 0.0f, 0.75f); } //if the gem should be collected use this colour

		if (!gems[gemId].wrongGemCollide) { drawIcoso(gems[gemId].isoScale, gems[gemId].isoScale, gems[gemId].isoScale); } //draw the gem and scale it relative to its own scale 

		glDisable(GL_BLEND); //disable blending
	}
}
#pragma endregion
#pragma region Print Text
//copied from tutorial
void displayText(GLfloat x, GLfloat y, GLfloat z, GLint r, GLint g, GLint b, const char *string, GLboolean stroke)
{
	int j = strlen(string); //get the length of the string

	glColor3f(r, g, b); //set the colour relative to passed parameters
	glRasterPos3f(x, y, z); //draw the string at the location specified
	for (int i = 0; i < j; i++) //draw each character from passed string
	{
		if (stroke)
		{
			glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, string[i]);
		}
		else
		{
			glutBitmapCharacter(GLUT_BITMAP_9_BY_15, string[i]);
		}
	}
}
#pragma endregion
#pragma endregion

#pragma region Cam Control
//do all rotations
void cameraRotation()
{
	glMatrixMode(GL_PROJECTION); //switching matrix modes to rotate around player
	glRotatef(camRotation.x, 1.0f, 0.0f, 0.0f); 
	glRotatef(camRotation.y, 0.0f, 1.0f, 0.0f);
	glRotatef(camRotation.z, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW); //switching matrix modes for other draw functions
}

//translate along Z
void cameraFrontBack()
{
	glTranslatef(0.0f, 0.0f, camOffset.z);
}

//translate along X
void cameraLeftRight()
{
	glTranslatef(camOffset.x, 0.0f, 0.0f);
}

//translate along Y
void cameraUpDown()
{
	glTranslatef(0.0f, camOffset.y, 0.0f);
}

//run all the translations on the "camera"
void processCamera()
{
	cameraRotation();
	cameraUpDown();
	cameraLeftRight();
	cameraFrontBack();
}
#pragma endregion

#pragma region Draw Stuff
#pragma region Switch display mode
//The following code (setOrtho and setProjection) was supplied within the assignment brief
void setOrtho(GLdouble width, GLdouble height)
{
	// switch to projection mode
	glMatrixMode(GL_PROJECTION);
	// save the previous matrix which contains the
	//settings for the perspective projection
	glPushMatrix();
	// reset matrix
	glLoadIdentity();
	// set a 2D orthographic projection
	gluOrtho2D(0, width, 0, height);
	// invert the y axis, down is positive
	glScalef(1, -1, 1);
	// mover the origin from the bottom left corner
	// to the upper left corner
	glTranslatef(0, -height, 0);
	glMatrixMode(GL_MODELVIEW);
	//ChangeSize(windowWidth, windowHeight);
}
//once you have finished your 2D you can reset your projection with this - it simply pops back the previous projection
void setProjection()
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
#pragma endregion
#pragma region draw2D stuff
//draw anything that should be in 2D (HUD and data readout)
void draw2D()
{
	setOrtho(windowWidth, windowHeight);
	glFrontFace(GL_CW);

	//on screen display readout
	//printing score readout
	string temp = string("score: ") + std::to_string(score);
	displayText(10 + playerPos.x, 20 + playerPos.y, playerPos.z, 1, 1, 1, const_cast<char*>(temp.c_str()), false);

	//printing position readout
	temp = string("Player Position: ") + toString(playerPos);
	displayText(10 + playerPos.x, 35 + playerPos.y, playerPos.z, 1, 1, 1, const_cast<char*>(temp.c_str()), false);
	
	//setting player rotation limit to the modulus of 360 for more accurate degree depiction
	playerRotation.x = fmodf(playerRotation.x, 360);
	playerRotation.y = fmodf(playerRotation.y, 360);
	playerRotation.z = fmodf(playerRotation.z, 360);

	//printing rotation readout
	temp = string("Player Rotation: ") + toString(playerRotation);
	displayText(10 + playerPos.x, 50 + playerPos.y, playerPos.z, 1, 1, 1, const_cast<char*>(temp.c_str()), false);

	drawOverlay(TEX_HUD); //drawing overlay
}
#pragma endregion
#pragma region draw 3D stuff
//draw all 3D stuff
void draw3D()
{
	setProjection();
	glMatrixMode(GL_MODELVIEW);

	processCamera();

	glColor3f(0.5, 0, 0);

	glFrontFace(GL_CCW);

	//drawing each gem by running through all the structures and retrieving the co ordinates
	//and then drawing the gem at its own co ordinates
	for (GLint i = 0; i < (sizeof(gems) / sizeof(gem)); i++)
	{
		glPushMatrix();
		glTranslatef(gems[i].pos.x, gems[i].pos.y, gems[i].pos.z); //moving to gem pos to draw it
		drawGem(i);
		glPopMatrix();
	}
	drawAxis(100.0f);
	drawHitbox();
	//drawGem();

	#pragma region sky sphere
	//drawing and moving the sky sphere
	glColor3f(0, 0, 0.1);
	glPushMatrix();
	if (!DEV_MODE) { glTranslatefv(playerPos, false); }
	glRotatef(90, 1, 0, 0);
	drawTextureSphere(TEX_STAR_FIELD, skySphere, true, 9000, 20, 20);
	glPopMatrix();
	#pragma endregion
	#pragma region jupiter
	//drawing and moving jupiter
	glPushMatrix();
	if (!DEV_MODE) { glTranslatef(-2000 + playerPos.x, 500 + playerPos.y, -5000 + playerPos.z); }
	else { glTranslatef(-2000, 500, -5000); }
	glRotatef(90, 1, 0, 0);
	glRotatef(planetRotation / 3.5, 0, 0, 1);
	drawTextureSphere(TEX_JUPI, jupiter, false, 300, 20, 20);
	glPopMatrix();
	#pragma endregion
	#pragma region Dev function calls
	if (DEV_MODE)
	{
		if (lsqBracket) { drawGemWaypoints(10.0f); }
		if (rsqBracket) { resetPosition(); }
		if (hash) { setupGems(ISO_SCALE); }
		drawGrid(gridSpacing);
		if (texDebugTut)
		{
			glPushMatrix();
			//rotate the quad slightly
			glRotatef(15.0, 0.0, 1.0, 0.0);
			drawTexturedQuad(TEX_STARS);
			glPopMatrix();

			glPushMatrix();
			//rotate and then translate the quad 
			glTranslatef(0.0, -100.0, 0.0);
			//try setting to -185.0 to see the back of the quad
			glRotatef(-15.0, 0.0, 1.0, 0.0);
			drawTexturedQuad(TEX_STARS);
			glPopMatrix();
		}
	}
	#pragma endregion
}
#pragma endregion
#pragma region Main Draw Function
//main draw function (called by glutPostRedisplay)
void RenderScene(void)
{
	// Clear the window with current clearing colour
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	draw3D();
	if (showHud) { draw2D(); } //if the hud is true then draw 2d stuff
}
#pragma endregion
#pragma endregion

#pragma region Load textures
//load all textures so they can be applied to objects
void loadTextures()
{
	GLuint texture;
	// allocate a texture name
	glGenTextures(1, &texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// photoshop is a good converter to targa (TGA)
	//the gltLoadTGA method is found in gltools.cpp and is orignally from the OpenGL SuperBible book
	//there are quite a few ways of loading images
	// Load textures in a for loop
	glGenTextures(texCount, textures);

	//this puts the texture into OpenGL texture memory
	//glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); - not defined so probably need to update GLEW - not needed here so ignore
	for (int i = 0; i < texCount; i++)
	{
		cout << "attempting to load texture " << i << endl;
		// Bind to next texture object
		glBindTexture(GL_TEXTURE_2D, textures[i]);

		// Load texture data, set filter and wrap modes
		//note that gltLoadTGA is in the glm.cpp file and not a built-in openGL function
		pBytes0 = gltLoadTGA(textureFiles[i], &iWidth, &iHeight, &iComponents, &eFormat);

		glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat, GL_UNSIGNED_BYTE, pBytes0);

		//set up texture parameters

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//try these too
		//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		free(pBytes0);
	}

	cout << "finished loading textures \n________________________________________________\n";

	//enable textures
	glEnable(GL_TEXTURE_2D);


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal    
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
	glEnable(GL_CULL_FACE);		// Do not calculate inside
	
	//glCullFace(GL_FRONT_AND_BACK);

	// Enable lighting
	//glEnable(GL_LIGHTING);
	glEnable(GL_POINT_SMOOTH);
	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLightLessBright);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, redLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glEnable(GL_LIGHT0);
	
	// Enable colour tracking
	glEnable(GL_COLOR_MATERIAL);

	// Set Material properties to follow glColor values
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Black blue background
	glClearColor(0.0f, 0.0f, 0.03f, 1.0f);
}
#pragma endregion

#pragma region Setup Gems
//sets up all gems so the game can be played. This is called
//when the game starts and when all gems are collected
void setupGems(GLfloat isoScale)
{
	for (GLint i = 0; i < (sizeof(gems) / sizeof(gem)); i++) //for each gem slot in the array
	{
		gems[i].collected = false; //reset collected so the player can pick gems back up
		collectedGems = 0; //reset collected gems to keep track of when all gems are collected
		gems[i].isoScale = isoScale; //setup the scale of gems

		//randomise the positions
		gems[i].pos.x = ranInt(-500, ranInt(250, 500));
		gems[i].pos.y = ranInt(0, ranInt(250, 500));
		gems[i].pos.z = ranInt(ranInt(-1500, -200), 1);
		cout << "Gem " << i << " positioned \n";
	}
	if (!DEV_MODE) { system("cls"); } //if devmode is not enabled then hide all debug prints
	printControls(); //then print the controls again
}
#pragma endregion

#pragma region Update
//update timer is used for logic updates and calling the draw function
void Update(int value)
{
	#pragma region Cam Controls
	//update the players position
	#pragma region Controls
	/*
	UP - fly up
	DOWN - fly down
	LEFT - tilt left (Z axis)
	RIGHT - tilt right (Z axis)
	A - strafe left
	D - strafe right
	W - move forward
	S - move backward
	Z - point to previous crystal
	X - point to next crystal
	Q - turn left (Y axis)
	E - turn right (Y axis)

		--------DEVELOPER-------
	[ - show waypoints
	] - reset location
	# - regenerate gems
	*/
	#pragma endregion
	#pragma region Cam Movement
	if (upArrow) { camOffset.y = -SPEED; }
	if (downArrow) { camOffset.y = SPEED; }
	if (leftArrow) { camRotation.z = -(SPEED / 3); }
	if (rightArrow) { camRotation.z = (SPEED / 3); }
	if (aKey){ camOffset.x = SPEED; }
	if (dKey) { camOffset.x = -SPEED; }
	if (wKey) { camOffset.z = (SPEED * 2); }
	if (sKey) { camOffset.z = -(SPEED * 2); }
	if (qKey) { camRotation.y = -SPEED; }
	if (eKey) { camRotation.y = SPEED; }
	#pragma endregion
	#pragma region SPEED multipliers
	if (shiftKey)
	{
		camOffset.x *= SPEEDMULT;
		camOffset.y *= SPEEDMULT;
		camOffset.z *= SPEEDMULT;
	}
	#pragma endregion
	#pragma region Update Player Co ordinates
	//if any of the movement keys are being pressed then update the player co-ordinates
	//relevant to the current keys being pressed down
	if (upArrow || downArrow || leftArrow || rightArrow || wKey || aKey || sKey || dKey || qKey || eKey)
	{
		//updating player position
		if (wKey || sKey) { playerPos.z -= camOffset.z; }
		if (aKey || dKey) { playerPos.x -= camOffset.x; }
		if (upArrow || downArrow) { playerPos.y -= camOffset.y; }
		
		//updating player rotation
		if (leftArrow || rightArrow) { playerRotation.z -= camRotation.z; }
		if (qKey || eKey) { playerRotation.y -= camRotation.y; }

	}
	#pragma endregion
	#pragma region Stop moving code
	//reset the current movement offset to 0 if neither of the corrosponding keys 
	//are currently being held down
	if (!wKey && !sKey) { camOffset.z = 0; }
	if (!aKey && !dKey) { camOffset.x = 0; }
	if (!upArrow && !downArrow) { camOffset.y = 0; }

	if (!qKey && !eKey) { camRotation.y = 0; }
	if (!leftArrow && !rightArrow) { camRotation.z = 0; }
	
	#pragma endregion
	#pragma endregion
	#pragma region Rotation Updates
	//update rotation of different objects so they spin
	gemRotation++;
	planetRotation++;
	#pragma endregion
	checkCollision(playerPos); //check the collisions of the gems
	glutSwapBuffers(); 
	glutPostRedisplay(); //call redraw
	glutTimerFunc(25, Update, 1);
}
#pragma endregion

#pragma region Window Resize
//called whenever the window is resized and maintains aspect ratio
void ChangeSize(GLsizei width, GLsizei height)
{
	//GLfloat fAspect;
	windowWidth = width;
	windowHeight = height;
	
	if (height == 0) { height = 1; } // Prevent a divide by zero

	//Set Viewport to window dimensions
	glViewport(0, 0, width, height);

	//Set the perspective coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//field of view of 45 degrees, near and far planes 1.0 up to the RENDER_DISTANCE
	gluPerspective(60.0f, ((GLfloat)width / (GLfloat)height), 1.0, RENDER_DISTANCE);

	//Modelview matrix reset
	glMatrixMode(GL_MODELVIEW);
	
	if (DEV_MODE && windowSizeReadout) { cout << "w = " << width << "| h = " << height << endl; }
}
#pragma endregion

#pragma region Keyboard Input
//read the inputs from the keyboard and update booleans 
#pragma region Special Key Pressed
void processsSpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_LEFT) { leftArrow = true; }
	if (key == GLUT_KEY_RIGHT) { rightArrow = true; }
	if (key == GLUT_KEY_UP) { upArrow = true; }
	if (key == GLUT_KEY_DOWN) { downArrow = true; }
	if (key == GLUT_KEY_SHIFT_L) { shiftKey = true; }
}
#pragma endregion
#pragma region Standard Key Release
void keyReleased(unsigned char key, int x, int y)
{
	key = tolower(key); //converting chars to lower case to prevent case sensitivity

	//resetting any released keys
	if (key == 'w') { wKey = false; }
	if (key == 'a') { aKey = false; }
	if (key == 's') { sKey = false; }
	if (key == 'd') { dKey = false; }
	if (key == 'e') { eKey = false; }
	if (key == 'q') { qKey = false; }
	if (key == 'h') { showHud = !showHud; } //toggle inversion on showHud
	if (key == 27)  { exit(0); } //if the escape key is pressed then exit with code 0
	
	if (DEV_MODE) //if developer mode is enabled then check the dev keys 
	{
		if (key == '[') { lsqBracket = false; }
		if (key == ']') { rsqBracket = false; }
		if (key == '#') { hash = false; }
	}
}
#pragma endregion
#pragma region Special Key Release
void specialKeyReleased(int key, int x, int y)
{
	//reset keys if they are released
	if (GLUT_KEY_UP) { upArrow = false; }
	if (GLUT_KEY_DOWN) { downArrow = false; }
	if (GLUT_KEY_LEFT) { leftArrow = false; }
	if (GLUT_KEY_RIGHT) { rightArrow = false; }
	if (key == GLUT_KEY_SHIFT_L) { shiftKey = false; }
}
#pragma endregion
#pragma region Standard Key Pressed
void processKeys(unsigned char key, int x, int y)
{
	key = tolower(key); //converting to lower case for checks

	if (key == 'w') { wKey = true; }
	if (key == 'a') { aKey = true; }
	if (key == 's') { sKey = true; }
	if (key == 'd') { dKey = true; }
	if (key == 'e') { eKey = true; }
	if (key == 'q') { qKey = true; }

	if (DEV_MODE) //if developer mode is enabled then check the dev keys
	{
		if (key == '[') { lsqBracket = true; }
		if (key == ']') { rsqBracket = true; }
		if (key == '#') { hash = true; }
	}
}
#pragma endregion
#pragma endregion

#pragma region Initialise
//setup most of the scene
void init()
{
	cout << "entered init \n";
	#pragma region Sky Sphere and Jupiter Setup
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	cout << "passed enables and disables \n";
	glFrontFace(GL_CW);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	cout << "set up tex params \n";

	skySphere = gluNewQuadric();
	jupiter = gluNewQuadric();
	cout << "created quadrics \n";

	gluQuadricDrawStyle(skySphere, GLU_FILL);
	gluQuadricTexture(skySphere, GL_TRUE);
	gluQuadricNormals(skySphere, GLU_SMOOTH);
	cout << "set up skySphere quadric params \n";

	gluQuadricDrawStyle(jupiter, GLU_FILL);
	gluQuadricTexture(jupiter, GL_TRUE);
	gluQuadricNormals(jupiter, GLU_SMOOTH);
	cout << "set up jupiter quadric params \n";
	#pragma endregion
	#pragma region setup player coords based on glu
	playerPos.x = 0;
	playerPos.y = 0;
	playerPos.z = 0;
	playerRotation.x = 0;
	playerRotation.y = 0;
	playerRotation.z = 0;
	#pragma endregion
	if (!DEV_MODE) { system("cls"); }
}
#pragma endregion

#pragma region Main Func
//first call location. central functions are set up here
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(screenX, screenY);// a 4:3 ratio
	glutCreateWindow("3D Space Game");
	cout << sizeof(textureFiles) / sizeof(char*) << " textures are preparing to load \n";
	loadTextures(); 
	init();
	srand(time(NULL));
	glutSpecialFunc(processsSpecialKeys);
	glutSpecialUpFunc(specialKeyReleased);
	glutKeyboardFunc(processKeys);
	glutKeyboardUpFunc(keyReleased);
	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	glutTimerFunc(25, Update, 1);
	
	setupGems(ISO_SCALE);
	glutMainLoop();
	return 0;
}
#pragma endregion