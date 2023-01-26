// Pool Game.cpp : Defines the entry point for the console application.
#include "stdafx.h"
#include "stdafx.h"
#include "simulation.h"
#include <glut.h>
#include <math.h>
#include <iostream>
#include <thread>

// Aim variables
float gAimAngle = 0.0;
float gAimPower = 0.25;
bool gAimControl[4] = { false,false,false,false };
float gAimAngleSpeed = 1.0f; //radians per second
float gAimPowerSpeed = 0.15f;
float gAimPowerMax = 0.75;
float gAimPowerMin = 0.1;
float gAimBallFactor = 8.0;
bool gDoAim = true;

//camera variables
vec3 gCamPos(0.0, 10.7, 2.1);
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

//rendering options
#define DRAW_SOLID	(1)
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

void DrawCircle(float cx, float cy, float r, int num_segments) {
	glBegin(GL_LINE_LOOP);
	for (int seg = 0; seg < num_segments; seg++) {
		// Get the current angle
		float theta = 2.0f * 3.1415926f * float(seg) / float(num_segments);
		float x = r * cosf(theta);// Calc the x component
		float y = r * sinf(theta);// Calc the y component
		glVertex3f(x + cx, 0.0, y + cy);//output vertex
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
		glutWireSphere(sheet[tab].balls[i].radius, 12, 12);
#endif
		glPopMatrix();
		glColor3f(0.0, 0.0, 1.0);
	}
	glColor3f(1.0, 1.0, 1.0);

	//draw the table
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
			glVertex3f(x->vertices[0](0), 0.0, x->vertices[0](1));
			glVertex3f(x->vertices[1](0), 0.0, x->vertices[1](1));
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

void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set camera
	glLoadIdentity();
	gluLookAt(gCamPos(0), gCamPos(1), gCamPos(2), gCamLookAt(0), gCamLookAt(1), gCamLookAt(2), 0.0f, 1.0f, 0.0f);

	//draw the ball
	glColor3f(1.0, 1.0, 1.0);

	for (size_t tab = 0; tab < sheet.size(); tab++) {
		RenderTable(tab);
	}

	//draw the cue
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
		if (locPlayer->doAim) {
			vec2 imp((-sin(gAimAngle) * gAimPower * gAimBallFactor),
				(-cos(gAimAngle) * gAimPower * gAimBallFactor));
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
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
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
	int TestVar = sheet.size();
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
		if (gAimPower > gAimPowerMax) gAimPower = gAimPowerMax;
		if (gAimPower < gAimPowerMin) gAimPower = gAimPowerMin;
	}
	DoCamera(ms);
	glutTimerFunc(SIM_UPDATE_MS, UpdateScene, SIM_UPDATE_MS);
	glutPostRedisplay();
}

int _tmain(int argc, _TCHAR* argv[]) {
	locPlayer = new player();
	locPlayer->doAim = true;
	player* playerTwo = new player();

	team teamOne = team();
	teamOne.name = "Team One: ";
	teamOne.AddPlayer(locPlayer);
	team teamTwo = team();
	teamTwo.name = "Team Two ";
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
	glutCreateWindow("MSc Comp Sci AP Assignment; COO19695119:");
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