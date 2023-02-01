#include "stdafx.h"
#include <math.h>
#include "simulation.h"
#include "SerialisedThreadedClient.h"
#include "networkInp.h"
#include <glut.h>

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
bool moveStone = true;

// Camera variables:
vec3 gCamPos(0.0, 0.7, 2.25);
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

Client client;
networkInp netInp;

// Rendering Options:
#define DRAW_SOLID	(0)

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


void RenderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Sets camera
	glLoadIdentity();
	gluLookAt(gCamPos(0), gCamPos(1), gCamPos(2), gCamLookAt(0), gCamLookAt(1), gCamLookAt(2), 0.0f, 1.0f, 0.0f);

	// Draws the stone - THIS NEEDS CHANGING OF THE SHAPE AS THESE ARE SPHERICAL 
	for (int i = 0; i < NUM_STONES; i++) {
		if (gCurlingSheet.stones[i].isTeamOne){ glColor3f(0.0, 0.0, 1.0); }
		else { glColor3f(1.0, 0.0, 0.0); }
		
		glPushMatrix();
		glTranslatef(gCurlingSheet.stones[i].stonePos(0), (STONE_RADIUS / 2.0), gCurlingSheet.stones[i].stonePos(1));
		glutSolidSphere(gCurlingSheet.stones[i].radius, 12, 12);
		glPopMatrix();
	}

	//Draw the curling Sheet
	glColor3f(0.0f, 0.5f, 0.5f);
	for (int i = 0; i < NUM_EDGES; i++) {
		glBegin(GL_LINE_LOOP);
		glVertex3f(gCurlingSheet.edges[i].vertices[0](0), 0.0, gCurlingSheet.edges[i].vertices[0](1));
		glVertex3f(gCurlingSheet.edges[i].vertices[0](0), 0.1, gCurlingSheet.edges[i].vertices[0](1));
		glVertex3f(gCurlingSheet.edges[i].vertices[1](0), 0.1, gCurlingSheet.edges[i].vertices[1](1));
		glVertex3f(gCurlingSheet.edges[i].vertices[1](0), 0.0, gCurlingSheet.edges[i].vertices[1](1));
		glEnd();
	}

	//Draw the Target.
	for (int i = 0; i < NUM_RINGS; i++) {
		glPushMatrix();
		if (i % 2 == 0) { glColor3f(1.0, 1.0, 1.0); } // White Rings 
		else { glColor3f(1.0, 0.0, 0.0); } // Red Rings 
		GLUquadric* quadric;
		quadric = gluNewQuadric();
		glTranslatef(gCurlingSheet.rings[i].targetCenter(0), (-0.001 * i), gCurlingSheet.rings[i].targetCenter(1));
		glRotatef(9 * 10, 1.0f, 0.0f, 0.0f);
		gluDisk(quadric, 0, TARGET_SPACING * i, 15, 15);
		gluDeleteQuadric(quadric);
		glPopMatrix();
	}

	// Draw the Aiming Line
	if (gDoAim) {
		glBegin(GL_LINES);
		float cuex = sin(gAimAngle) * gAimPower;
		float cuez = cos(gAimAngle) * gAimPower;
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(0), (STONE_RADIUS / 2.0f), gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(1));
		glVertex3f((gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(0) + cuex), (STONE_RADIUS / 2.0f), (gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(1) + cuez));
		glEnd();
	}

	// Scoreboard ? 
	glPushMatrix();
	glRasterPos3d(1, 0, 0);
	// Strings for the scores
	std::string scoresOutput = "Team One: " + std::to_string(gCurlingSheet.scores[0]) + " | Team Two: " + std::to_string(gCurlingSheet.scores[1]);
	// Outputs the score in Char format.
	for (char c : scoresOutput) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
	}
	glTranslatef(0, 0, 0);
	glPopMatrix();


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
		if (gDoAim) // If enter is pressed the shotcan be played.
		{
			vec2 imp((-sin(gAimAngle) * gAimPower * gPlayerStoneFactor), (-cos(gAimAngle) * gAimPower * gPlayerStoneFactor));
			gCurlingSheet.stones[gCurlingSheet.currntStone].ApplyImpulse(imp);
			gCurlingSheet.stones[gCurlingSheet.currntStone].isPlayerStone = true;
			client.message = "B|" + to_string(imp(0)) + "|" + to_string(imp(1));
			client.sendMsg();
			gCurlingSheet.currntStone += 1;
		}
		break;
	}
	case(27):
	{
		for (int i = 0; i < NUM_STONES; i++) {
			gCurlingSheet.stones[i].Reset();
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
	if (netInp.updateScores) {
		netInp.DisplayScores();
		netInp.updateScores = false;
	}

	// Get Network Inputs
	for (int i = 0; i < netInp.maxSheets; i++) {
		if (netInp.posChange[i]) {
			gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(0) = netInp.stonePos[i];
			gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(1) = 1.0;
			netInp.posChange[i] = false;
		}
		if (netInp.impChange[i]) {
			gCurlingSheet.stones[gCurlingSheet.currntStone].ApplyImpulse(netInp.stoneImps[i]);
			netInp.impChange[i] = false;
			gCurlingSheet.stones[gCurlingSheet.currntStone].isPlayerStone = true;
		}
	}

	if (gCurlingSheet.AnyStonesMoving() == false) {
		gDoAim = true;
		gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(0) = 0;
		gCurlingSheet.stones[gCurlingSheet.currntStone].stonePos(1) = TABLE_Z - 0.25;
	}
	else {
		gDoAim = false;
		gCurlingSheet.scores[0] = 0;
		gCurlingSheet.scores[1] = 0;
		for (int i = 0; i < gCurlingSheet.currntStone; i++) {
			double stonePoints = 0.0;
			stonePoints = sqrt(pow(gCurlingSheet.stones[i].stonePos(0), 2) + pow(gCurlingSheet.stones[i].stonePos(1) - (gCurlingSheet.rings[0].targetCenter(1)), 2)); // Euclideian Dist
			if (stonePoints <= (NUM_RINGS - 1) * TARGET_SPACING) {
				stonePoints /= (NUM_RINGS - 1) * TARGET_SPACING;
				stonePoints *= (NUM_RINGS - 1);
				stonePoints -= (NUM_RINGS - 1);
				stonePoints = 0 - stonePoints;
				stonePoints += 1;
				// Addind points to the players.
				if (gCurlingSheet.stones[i].isTeamOne) {
					gCurlingSheet.scores[0] += stonePoints;
				}
				else {
					gCurlingSheet.scores[1] += stonePoints;
				}
			}
		}
	}

		if (gDoAim) {
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
		gCurlingSheet.Update(ms);
		glutTimerFunc(SIM_UPDATE_MS, UpdateScene, SIM_UPDATE_MS);
		glutPostRedisplay();
	}

int _tmain(int argc, _TCHAR * argv[]) {
	string hostIP;
	string port;
	cout << "Enter an IP address: \n" << endl;
	cin >> hostIP;
	cout << "Enter the port: \n" << endl;
	cin >> port;
	client.start(hostIP.c_str(), port.c_str(), &netInp);
	std::thread thOne(&Client::getMsg, &client); // this makes it so the game and receiving messages from the server aare concurrent  
	thOne.detach();

	for (int i = 0; i < SHEETCOUNT; i++) {
		gCurlingSheet.SetUpEdges();
		gCurlingSheet.SetUpRings();
		gCurlingSheet.stones[0].SetPlayerStone();
	}
	
	while (!netInp.welcomeRecieved) {} // this just holds you until the connection is confirmed
	
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