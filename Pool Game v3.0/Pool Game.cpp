// Pool Game.cpp: Defines the entry point for the console app, while it's called Pool.cpp its a Curling Game.
#include "stdafx.h"
#include "stdafx.h"
#include "simulation.h"
#include <glut.h>
#include <math.h>
#include <iostream>
#include <thread>

// Aim Line variables:
float gAimAngle = 0.0;
float gAimPower = 0.25;
bool gAimControl[4] = { false,false,false,false };
float gAimAngleSpeed = 2.0f; //radians per second
float gAimPowerSpeed = 0.25f;
float gAimPowerMax = 0.75;
float gAimPowerMin = 0.1;
float gPlayerStoneFactor = 8.0;
bool gDoAim = true;

// Camera variables:
vec3 gCamPos(0.0, 10.0, 2.1);
vec3 gCamLookAt(0.0, 0.0, 0.0);
bool gCamRotate = true;
float gCamRotSpeed = 0.2;
float gCamMoveSpeed = 0.5;
bool gCamL = false;
bool gCamR = false;
bool gCamU = false;
bool gCamD = false;
bool gCamZin = false;
bool gCamZout = false;

std::vector<curlingSheet> sheet;
player* locPlayer;
team _team;

// Rendering Options:
#define DRAW_SOLID	(1)

/**
	VOID DOCAMERA:
	This function rotates the camera around its center point.
	It takes an int value [ms], which represents milliseconds.
	The first line of code creates a variable named up stores three vars [0.0, 1.0, and 0.0]
	respectively for x-, y- and z-axis coordinates. (represents the direction towards the top of the screen).

	If gCamRotate is true, then vec3 camDir = (gCamLookAt - gCamPos).Normalized()
	if gCamL is true will be carried out, and camDir will have a value between -1 and 1, depending on whether you are looking at your character's left or right side;
	This indicates that the distance from your character's center point will be represented by a positive number when looking leftward from their position,
	while the distance from their center point will be represented by a negative number when looking rightward; As long as they stay within those bounds,
	these values can range from -1 to 1, but since our program only requires one type of value to be returned by this function, we'll use integers instead.

	 SIMPLE EXPLANATION:
	 The code is used to rotate the camera.
	 The first part of the code rotates the camera around its centre point, which is gCamPos.
	 The second part of the code rotates the camera around its right-hand side, which is gCamR.
	 The third part of the code rotates the camera around its left-hand side, which is gCamL.
**/

void DoCamera(int ms) {
	static const vec3 up(0.0, 1.0, 0.0);
	if (gCamRotate) {
		if (gCamL) {
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localL = up.Cross(camDir);
			vec3 inc = (localL * ((gCamRotSpeed * ms) / 1000.0));
			gCamLookAt = gCamPos + camDir + inc;
		}

		if (gCamR) {
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = up.Cross(camDir);
			vec3 inc = (localR * ((gCamRotSpeed * ms) / 1000.0));
			gCamLookAt = gCamPos + camDir - inc;
		}

		if (gCamU) {
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp * ((gCamMoveSpeed * ms) / 1000.0));
			gCamLookAt = gCamPos + camDir + inc;
		}

		if (gCamD) {
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp * ((gCamMoveSpeed * ms) / 1000.0));
			gCamLookAt = gCamPos + camDir - inc;
		}
	}
	else {
		if (gCamL) {
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localL = up.Cross(camDir);
			vec3 inc = (localL * ((gCamMoveSpeed * ms) / 1000.0));
			gCamPos += inc;
			gCamLookAt += inc;
		}

		if (gCamR) {
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 inc = (localR * ((gCamMoveSpeed * ms) / 1000.0));
			gCamPos += inc;
			gCamLookAt += inc;
		}

		if (gCamU) {
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localUp = localR.Cross(camDir);
			vec3 inc = (localUp * ((gCamMoveSpeed * ms) / 1000.0));
			gCamPos += inc;
			gCamLookAt += inc;
		}

		if (gCamD) {
			vec3 camDir = (gCamLookAt - gCamPos).Normalised();
			vec3 localR = camDir.Cross(up);
			vec3 localDown = camDir.Cross(localR);
			vec3 inc = (localDown * ((gCamMoveSpeed * ms) / 1000.0));
			gCamPos += inc;
			gCamLookAt += inc;
		}
	}

	if (gCamZin) {
		vec3 camDir = (gCamLookAt - gCamPos).Normalised();
		vec3 inc = (camDir * ((gCamMoveSpeed * ms) / 1000.0));
		gCamPos += inc;
		gCamLookAt += inc;
	}

	if (gCamZout) {
		vec3 camDir = (gCamLookAt - gCamPos).Normalised();
		vec3 inc = (camDir * ((gCamMoveSpeed * ms) / 1000.0));
		gCamPos -= inc;
		gCamLookAt -= inc;
	}
}

void CamSetLoc(vec3 _position, vec3 _lookat) {
	gCamPos = _position;
	gCamLookAt = _lookat;
}

void DrawCircle(float cx, float cy, float r, int numSegs) {
	glBegin(GL_LINE_LOOP);
	for (int seg = 0; seg < numSegs; seg++) {
		// Get the current angle
		float theta = 2.0f * 3.1415926f * float(seg) / float(numSegs);

		float x = r * cosf(theta); //Calc the X-component
		float y = r * sinf(theta); //Calc the Y-component

		glVertex3f(x + cx, 0.0, y + cy); // Outpt vertex
	}
	glEnd();
}

int RenderTable(size_t tab) {
	for (int i = 0; i < sheet[tab].stoneCnt; i++) {
		glDisable(GL_LIGHTING);
		glColor3f(sheet[tab].stones[i].stoneTeam.colour(0), sheet[tab].stones[i].stoneTeam.colour(1), sheet[tab].stones[i].stoneTeam.colour(2));
		glPushMatrix();
		glTranslatef(sheet[tab].stones[i].stonePos(0), (STONE_RADIUS / 2.0), sheet[tab].stones[i].stonePos(1));
		glScalef(1.0, 0.3, 1.0);
#if DRAW_SOLID
		glutSolidSphere(sheet[tab].stones[i].radius, 32, 32);
#else
		glutWireSphere(sheet[tab].stones[i].radius, 12, 12);
#endif
		glPopMatrix();
		glColor3f(0.0, 0.0, 1.0);
	}
	glColor3f(1.0, 1.0, 1.0);

	// Draw the table
	for (int i = 0; i < NUM_EDGES; i++) {
		glBegin(GL_LINE_LOOP);
		glVertex3f(sheet[tab].edges[i].vertices[0](0), 0.0, sheet[tab].edges[i].vertices[0](1));
		glVertex3f(sheet[tab].edges[i].vertices[0](0), 0.1, sheet[tab].edges[i].vertices[0](1));
		glVertex3f(sheet[tab].edges[i].vertices[1](0), 0.1, sheet[tab].edges[i].vertices[1](1));
		glVertex3f(sheet[tab].edges[i].vertices[1](0), 0.0, sheet[tab].edges[i].vertices[1](1));
		glEnd();
	}

	for (int i = 0; i < TABLE_FEATURES; i++) {
		if (lines* x = dynamic_cast<lines*>(sheet[tab].tableFeatures[i])) {
			glBegin(GL_LINE_LOOP);
			glVertex3f(x -> vertices[0](0), 0.0, x -> vertices[0](1));
			glVertex3f(x -> vertices[1](0), 0.0, x -> vertices[1](1));
			glEnd();
		}
		else if (rings* x = dynamic_cast<rings*>(sheet[tab].tableFeatures[i])) {
			DrawCircle(x->targetCenter(0), x->targetCenter(1), x->targetRad, 30);
		}
	}

	for (int i = 0; i < sheet[tab].parts.num; i++) {
		glColor3f(1.0, 0.0, 0.0);
		glPushMatrix();
		glTranslatef(sheet[tab].parts.particles[i]->partPos(0), sheet[tab].parts.particles[i]->partPos(1), sheet[tab].parts.particles[i]->partPos(2));
#if DRAW_SOLID
		glutSolidSphere(0.002f, 32, 32);
#else
		glutWireSphere(0.002f, 12, 12);
#endif
		glPopMatrix();
	}
	return(0);
}

/**
	RENDERSCENE(VOID)
	Drawing of the stones:
		The code starts by declaring a variable called stonesOnTarget an int used to keep track of how many times the player has successfully hit the target with a stone.
	The code then loops through all of the curling stones on the sheet and checks if they have a NULL value for their tTarget field, which means they are not currently targeting anything.
	If true, then it sets glColor3f(0.0, 0.0, 1.0) and glPushMatrix() before translating them into position relative to where they were originally placed in relation to each other (STONE_RADIUS / 2).
	It then draws a solid sphere using glutSolidSphere() or wire spheres using glutWireSphere(). - MAY BE CHANGED
	After drawing these objects, it pops back out of matrix mode and increments stonesOnTarget by one because now there's one more stone on target than when it started looping through all of the
	curling stones on the sheet again later in its code block after checking if any new ones had been added since last time around.
	The code iterates through all the stones on the sheet and checks if their target is NULL.
	If it is, then it draws a red sphere around the stone with its radius of 32 units.
	If not, then it increments the value of stonesOnTarget by one and draws a blue sphere with its radius of 12 units.

	Drawing of the Players Stone:
	 Used to draw a solid sphere.
	 Using the glTranslatef() function to move the sphere from its current position to another point on the screen.
	 First checks if there are more than one stone left in the target and if so, it will draw them as a group.
	 If not, then it will draw each stone individually.

	 Drawing of the curlingsheet:
	 Then it draws a line for each edge of the curling sheet, which is done in two loops.
		 The first loop draws lines from the start vertex to the end vertex and then back again; this creates an aiming line that will be used later on when calculating where to place stones.
		 The second loop draws lines from one stone's position to another stone's position, creating a path between them that can be seen as they curl around each other.
	 Drawing the curling sheet with glBegin(GL_TRIANGLES).
	 Useing glVertex3f() functions to draw vertices at points 0 and 1 of every triangle in order (0,1), 2 and 3 (2,3), 4 and 5 (4,5), 6 and 7 (6,7) etc., until all triangles have been drawn.
	 Next it uses glColor3f() functions to set colors for every point on every triangle: red for point 0; green for point 1; blue for point 2 etc.
		These colors are used later on when drawing stones so that they can be distinguished from one another while still being part of their respective paths.
	 After this step has completed successfully,
	 The code is a fragment of code that draws the curling sheet.
	 The code begins by drawing the background and then, it draws all the stones on the sheet.
	 The stonePos() function is used to find where each stone is located in relation to the center of the screen.

**/

void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Sets camera
	glLoadIdentity();
	gluLookAt(gCamPos(0), gCamPos(1), gCamPos(2), gCamLookAt(0), gCamLookAt(1), gCamLookAt(2), 0.0f, 1.0f, 0.0f);

	// Draws the stone - THIS NEEDS CHANGING OF THE SHAPE AS THESE ARE SPHERICAL 
	//for (int i = 0; i < NUM_STONES; i++) {
	//	glColor3f(0.0, 0.0, 1.0);
	//	glPushMatrix();
	//	glTranslatef(gCurlingSheet.stones[i].stonePos(0), (STONE_RADIUS / 2.0), gCurlingSheet.stones[i].stonePos(1));
	//	glutSolidSphere(gCurlingSheet.stones[i].radius, 12, 12);
	//	glPopMatrix();
	//}

	glColor3f(1.0,1.0,1.0);

	for (size_t tab = 0; tab < sheet.size(); tab++) {
		RenderTable(tab);
	}

	////Draw the curling Sheet
	//glColor3f(0.0f, 0.5f, 0.5f);
	//for (int i = 0; i < NUM_EDGES; i++) {
	//	glBegin(GL_LINE_LOOP);
	//	glVertex3f(gCurlingSheet.edges[i].vertices[0](0), 0.0, gCurlingSheet.edges[i].vertices[0](1));
	//	glVertex3f(gCurlingSheet.edges[i].vertices[0](0), 0.1, gCurlingSheet.edges[i].vertices[0](1));
	//	glVertex3f(gCurlingSheet.edges[i].vertices[1](0), 0.1, gCurlingSheet.edges[i].vertices[1](1));
	//	glVertex3f(gCurlingSheet.edges[i].vertices[1](0), 0.0, gCurlingSheet.edges[i].vertices[1](1));
	//	glEnd();
	//}

	////Draw the Target.
	//for (int i = 0; i < NUM_RINGS; i++) {
	//	glPushMatrix();
	//	if (i % 2 == 0) { glColor3f(1.0, 1.0, 1.0); } // White Rings 
	//	else { glColor3f(1.0, 0.0, 0.0); } // Red Rings 
	//	GLUquadric* quadric;
	//	quadric = gluNewQuadric();
	//	glTranslatef(gCurlingSheet.rings[i].targetCenter(0), (-0.001 * i), gCurlingSheet.rings[i].targetCenter(1));
	//	glRotatef(9 * 10, 1.0f, 0.0f, 0.0f);
	//	gluDisk(quadric, 0, TARGET_SPACING * i, 15, 15);
	//	gluDeleteQuadric(quadric);
	//	glPopMatrix();
	//}

	// Draw the Aiming Line
	/**
	if (gDoAim) {
		glBegin(GL_LINES);
		float cuex = sin(gAimAngle) * gAimPower;
		float cuez = cos(gAimAngle) * gAimPower;
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(0), (STONE_RADIUS / 2.0f), gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(1));
		glVertex3f((gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(0) + cuex), (STONE_RADIUS / 2.0f), (gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(1) + cuez));
		glEnd();
	}
	glFlush();
	glutSwapBuffers(); **/
	
	// Drawing the Aiming Line:
	if (locPlayer->doAim) {
		glBegin(GL_LINES);
		float cuex = sin(gAimAngle) * gAimPower;
		float cuez = cos(gAimAngle) * gAimPower;
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(sheet[0].stones[sheet[0].stoneCnt - 1].stonePos(0), (STONE_RADIUS / 2.0f), sheet[0].stones[sheet[0].stoneCnt - 1].stonePos(1));
		glVertex3f((sheet[0].stones[sheet[0].stoneCnt - 1].stonePos(0) + cuex), (STONE_RADIUS / 2.0f), (sheet[0].stones[sheet[0].stoneCnt - 1].stonePos(1) + cuez));
		glColor3f(1.0, 1.0, 1.0);
		glEnd();
	}
	glFlush();
	glutSwapBuffers();
}

// NOT CHANGED ANY OF THE BELOW AS ITS FROM THE BASE CODE PROVIDED IN WORKSHOP WEEK 4:
void SpecKeyboardFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
	{
		gAimControl[0] = true;
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		gAimControl[1] = true;
		break;
	}
	case GLUT_KEY_UP:
	{
		gAimControl[2] = true;
		break;
	}
	case GLUT_KEY_DOWN:
	{
		gAimControl[3] = true;
		break;
	}
	}
}

void SpecKeyboardUpFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
	{
		gAimControl[0] = false;
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		gAimControl[1] = false;
		break;
	}
	case GLUT_KEY_UP:
	{
		gAimControl[2] = false;
		break;
	}
	case GLUT_KEY_DOWN:
	{
		gAimControl[3] = false;
		break;
	}
	}
}

void KeyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
	case(13):
	{
		if (locPlayer->doAim){
			vec2 imp((-sin(gAimAngle) * gAimPower * gPlayerStoneFactor),(-cos(gAimAngle) * gAimPower * gPlayerStoneFactor));
			sheet[0].stones[sheet[0].stoneCnt - 1].ApplyImpulse(imp);
			locPlayer->doAim = false;
		}
		break;
	}
	case(27):
	{
		for (int i = 0; i < sheet[0].stoneCnt; i++) {
			sheet[0].stones[i].Reset();
		}
		break;
	}
	case(32):
	{
		gCamRotate = false;
		break;
	}
	case('z'):
	{
		gCamL = true;
		break;
	}
	case('c'):
	{
		gCamR = true;
		break;
	}
	case('s'):
	{
		gCamU = true;
		break;
	}
	case('x'):
	{
		gCamD = true;
		break;
	}
	case('f'):
	{
		gCamZin = true;
		break;
	}
	case('v'):
	{
		gCamZout = true;
		break;
	}
	}
}

void KeyboardUpFunc(unsigned char key, int x, int y) {
	switch (key) {
	case(32):
	{
		gCamRotate = true;
		break;
	}
	case('z'):
	{
		gCamL = false;
		break;
	}
	case('c'):
	{
		gCamR = false;
		break;
	}
	case('s'):
	{
		gCamU = false;
		break;
	}
	case('x'):
	{
		gCamD = false;
		break;
	}
	case('f'):
	{
		gCamZin = false;
		break;
	}
	case('v'):
	{
		gCamZout = false;
		break;
	}
	}
}

void ChangeSize(int w, int h) {
	// Prevent a divide by zero, when window is too short (you cant make a window of zero width).
	if (h == 0) h = 1;
	float ratio = 1.0 * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 0.2, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//gluLookAt(0.0,0.7,2.1, 0.0,0.0,0.0, 0.0f,1.0f,0.0f);
	gluLookAt(gCamPos(0), gCamPos(1), gCamPos(2), gCamLookAt(0), gCamLookAt(1), gCamLookAt(2), 0.0f, 1.0f, 0.0f);
}

void InitLights(void) {
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat light_ambient[] = { 2.0, 2.0, 2.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_DEPTH_TEST);
}

void UpdateScene(int ms) {
	int testVar = sheet.size();
	for (size_t tab = 0; tab < sheet.size(); tab++) {
		if (sheet[tab].AnyStonesMoving() == false) {
			if (sheet[tab].doAim == false) {
				sheet[tab].CheckStones();
				sheet[tab].AddStone();
			}
			sheet[tab].doAim = true;
			CamSetLoc(vec3(0.0, 10, 2.1), vec3(0.0, 0.0, -3.0));
		}
		else {
			sheet[tab].doAim = false;
			CamSetLoc(vec3(0.0, 5, -15 * SCALE_FACTOR), vec3(0.0, 0.0, -7));
		}
		sheet[tab].Update(ms);
	}

	if (locPlayer->doAim) {
		if (gAimControl[0]) gAimAngle -= ((gAimAngleSpeed * ms) / 1000);
		if (gAimControl[1]) gAimAngle += ((gAimAngleSpeed * ms) / 1000);
		if (gAimAngle < 0.0) gAimAngle += TWO_PI;
		if (gAimAngle > TWO_PI) gAimAngle -= TWO_PI;

		if (gAimControl[2]) gAimPower += ((gAimPowerSpeed * ms) / 1000);
		if (gAimControl[3]) gAimPower -= ((gAimPowerSpeed * ms) / 1000);
		if (gAimPower> gAimPowerMax) gAimPower = gAimPowerMax;
		if (gAimPower < gAimPowerMin) gAimPower = gAimPowerMin;
	}
	DoCamera(ms);
	glutTimerFunc(SIM_UPDATE_MS, UpdateScene, SIM_UPDATE_MS);
	glutPostRedisplay();
}

int _tmain(int argc, _TCHAR* argv[]) {
	//gCurlingSheet.SetUpEdges();
	//gCurlingSheet.SetUpRings();
	//gCurlingSheet.stones[0].SetPlayerStone();

	locPlayer = new player();
	locPlayer->doAim = true;
	player* playerTwo = new player();


	team teamOne = team();
	teamOne.name = "Team One";
	teamOne.AddPlayer(locPlayer);
	team teamTwo = team();
	teamTwo.name = "Team Two";
	teamTwo.AddPlayer(playerTwo);

	for (int i = 0; i < TABLE_NUM; i++) {
		sheet.push_back(curlingSheet(i));
		sheet[i].AddPlayer(teamOne, 0);
		sheet[i].AddPlayer(teamTwo, 0);
		sheet[i].SetUpOrder();
		sheet[i].AddStone();
	}

	glutInit(&argc, ((char**)argv));
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1000, 700);
	//glutFullScreen();
	glutCreateWindow("MSc Advanced Programming Assigment: Stone Curling Game (COO19695119)");
#if DRAW_SOLID
	InitLights();
#endif
	glutDisplayFunc(RenderScene);
	glutTimerFunc(SIM_UPDATE_MS, UpdateScene, SIM_UPDATE_MS);
	glutReshapeFunc(ChangeSize);
	glutIdleFunc(RenderScene);

	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(KeyboardFunc);
	glutKeyboardUpFunc(KeyboardUpFunc);
	glutSpecialFunc(SpecKeyboardFunc);
	glutSpecialUpFunc(SpecKeyboardUpFunc);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
}