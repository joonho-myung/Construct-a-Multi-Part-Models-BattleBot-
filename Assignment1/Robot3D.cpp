/*******************************************************************
		   Hierarchical Multi-Part Wedge Flipper

		   Joonho Myung 500845049
********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "cube.h"
#include "QuadMesh.h"

const int vWidth  = 650;    // Viewport width in pixels
const int vHeight = 500;    // Viewport height in pixels

// Note how everything depends on robot body dimensions so that can scale entire robot proportionately
// just by changing robot body scale
float robotBodyWidth = 6.0;
float robotBodyLength = 2.0;
float robotBodyDepth = 6.0;

// control the size of wheels of the Wedge Flipper
float wheelLength = 1.0;
float wheelWidth = 1.0;
float wheelDepth = 1.0;

//forward backward for x and z axis
float translateForward1 = 0.0;
float translateForward2 = 0.0;
float translateBackward1 = 0.0;
float translateBackward2 = 0.0;

float leftWheelRotate = 5.0;
float rightWheelRotate = 5.0;

// spin angle while object rotating 
float spinAngle = 0.0;

// the wedge going up and down
float wedgeRotate = 0.0;

// counter to check if the wedge reached certain angle
int counter = 0;


float cubeAngle = 0.0;

// Lighting/shading and material properties for robot - upcoming lecture - just copy for now
// Robot RGBA material properties (NOTE: we will learn about this later in the semester)
GLfloat robotWedge_mat_ambient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat robotWedge_mat_specular[] = { 0.45f,0.55f,0.45f,1.0f };
GLfloat robotWedge_mat_diffuse[] = { 0.1f,0.35f,0.1f,1.0f };
GLfloat robotWedge_mat_shininess[] = { 32.0F };


GLfloat robotWheel_mat_ambient[] = { 0.0215f, 0.1745f, 0.0215f, 0.55f };
GLfloat robotWheel_mat_diffuse[] = { 0.5f,0.0f,0.0f,1.0f };
GLfloat robotWheel_mat_specular[] = { 0.7f, 0.6f, 0.6f, 1.0f };
GLfloat robotWheel_mat_shininess[] = { 32.0F };

GLfloat upperBody_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat upperBody_diffuse[] = { 0.01f,0.0f,0.01f,0.01f };
GLfloat upperBody_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat upperBody_shininess[] = { 100.0F };

GLfloat robotBody_mat_ambient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat robotBody_mat_diffuse[] = { 0.4f, 0.4f, 0.4f, 1.0f };
GLfloat robotBody_mat_specular[] = { 0.774597f, 0.774597f, 0.774597f, 1.0f };
GLfloat robotBody_mat_shininess[] = { 76.8F };


// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };


// Mouse button
int currentButton;

// A template cube mesh
CubeMesh *cubeMesh = createCubeMesh();

// A flat open mesh
QuadMesh *groundMesh = NULL;

// Structure defining a bounding box, currently unused
typedef struct BoundingBox {
    VECTOR3D min;
	VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 16;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler1(int param);
void drawRobot();
void drawBody();
void drawWedge();
void drawRightWheel();
void drawLeftWheel();
void drawTopUpperBody();

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("Assignment 1 - Wedge Flipper");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.4F, 0.4F, 0.4F, 0.0F);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	// Other initializatuion
	// Set up ground quad mesh
	VECTOR3D origin = VECTOR3D(-16.0f, 0.0f, 16.0f);
	VECTOR3D dir1v = VECTOR3D(1.0f, 0.0f, 0.0f);
	VECTOR3D dir2v = VECTOR3D(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 32.0);
	groundMesh->InitMesh(meshSize, origin, 32.0, 32.0, dir1v, dir2v);

	VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
	VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
	VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

}


// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	//gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	 gluLookAt(10.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// Draw Robot

	// Apply modelling transformations M to move robot
	// Current transformation matrix is set to IV, where I is identity matrix
	// CTM = IV
	  drawRobot();


	// Example of drawing a mesh (closed cube mesh)
	
	glPushMatrix();
	// spin cube
	glTranslatef(8.0, 0, 3.0);
	glRotatef(cubeAngle, 0.0, 1.0, 0.0);
	glTranslatef(-8.0, 0, -3.0);
	// position and draw cube
	glTranslatef(8.0, 0, 3.0); 
	drawCubeMesh(cubeMesh);
	glPopMatrix();
	


	// Draw ground
	glPushMatrix();
	glTranslatef(0.0, -3.0, 0.0);
	groundMesh->DrawMesh(meshSize);
	glPopMatrix();

	glutSwapBuffers();   // Double buffering, swap buffers
}


void drawRobot()
{
	glPushMatrix();
	 //spin robot on base. 
	//glRotatef(90, 0, 1.0, 0);

	glPushMatrix();
	 glTranslatef(translateForward2 - translateBackward2, 0, translateForward1 - translateBackward1);
     glRotatef(spinAngle, 0, 1.0, 0);	 
	 glTranslatef(-translateForward2 + translateBackward2, 0, -translateForward1 + translateBackward1);


	glPushMatrix();
	 glTranslatef(translateForward2 - translateBackward2, 0.0, translateForward1 - translateBackward1);
	 drawBody();
	 drawWedge();
	 drawRightWheel();
	 drawLeftWheel();
	 drawTopUpperBody();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}


void drawBody()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotBody_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotBody_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotBody_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotBody_mat_shininess);

	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glScalef(robotBodyWidth, robotBodyLength, robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawWedge()
{
	// Set robot material properties per body part. Can have seperate material properties for each part
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotWedge_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotWedge_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotWedge_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotWedge_mat_shininess);

	glRotatef(-90, 0, 1, 0);

	glPushMatrix();
	glTranslatef(0.5 * robotBodyWidth, 0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glRotatef(wedgeRotate, 0, 0, 1.0);
	glTranslatef(-0.5 * robotBodyWidth, -0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glBegin(GL_QUADS);

	
	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, -0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(robotBodyWidth, -0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	
	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, -0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(robotBodyWidth, -0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, -0.5 * robotBodyDepth);

	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(robotBodyWidth, -0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(robotBodyWidth, -0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	

	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, -0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, -0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, 0.5 * robotBodyLength, 0.5 * robotBodyDepth);

	glVertex3f(0.5 * robotBodyWidth, -0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(robotBodyWidth, -0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glVertex3f(robotBodyWidth, -0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, -0.5 * robotBodyLength, -0.5 * robotBodyDepth);
	glVertex3f(0.5 * robotBodyWidth, -0.5 * robotBodyLength, 0.5 * robotBodyDepth);

	glEnd();

	glPopMatrix();
}


void drawRightWheel()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotWheel_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotWheel_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotWheel_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotWheel_mat_shininess);

	GLUquadricObj* Wheel1;
	Wheel1 = gluNewQuadric();
	gluQuadricDrawStyle(Wheel1, GLU_LINE);

	GLUquadricObj* Cover1;
	Cover1 = gluNewQuadric();
	gluQuadricDrawStyle(Cover1, GLU_LINE);

	glPushMatrix();

	glPushMatrix();
	glTranslatef(-0.33 * robotBodyWidth, -0.3, 0.4 * robotBodyDepth);
	glRotatef(rightWheelRotate, 0.0, 0.0, 1.0);
	glScalef(wheelWidth, wheelLength, wheelDepth);
	gluCylinder(Wheel1, 1.0, 1.0, 1.0, 100.0, 2.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.33 * robotBodyWidth, -0.3, 0.567 * robotBodyDepth);
	glRotatef(rightWheelRotate, 0.0, 0.0, 1.0);
	glScalef(wheelWidth, wheelLength, wheelDepth);
	gluDisk(Cover1, 0.0, 1.0, 10.0, 10.0);
	glPopMatrix();

	glPopMatrix();
}


void drawLeftWheel()
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, robotWheel_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, robotWheel_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, robotWheel_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, robotWheel_mat_shininess);

	GLUquadricObj* Wheel2;
	Wheel2 = gluNewQuadric();
	gluQuadricDrawStyle(Wheel2, GLU_LINE);

	GLUquadricObj* Cover2;
	Cover2 = gluNewQuadric();
	gluQuadricDrawStyle(Cover2, GLU_LINE);

	glPushMatrix();
	
	glPushMatrix();
	glTranslatef(-0.33 * robotBodyWidth, -0.3, -0.567 * robotBodyDepth);
	glRotatef(leftWheelRotate, 0.0, 0.0, -1.0);
	glScalef(wheelWidth, wheelLength, wheelDepth);
	gluCylinder(Wheel2, 1.0, 1.0, 1.0, 100.0, 2.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.33 * robotBodyWidth, -0.3, -0.567 * robotBodyDepth);
	glRotatef(leftWheelRotate, 0.0, 0.0, -1.0);
	glRotatef(180, 0.0, 1.0, 0.0);
	glScalef(wheelWidth, wheelLength, wheelDepth);
	gluDisk(Cover2, 0.0, 1.0, 10.0, 10.0);
	glPopMatrix();

	glPopMatrix();
}


void drawTopUpperBody() {
	glMaterialfv(GL_FRONT, GL_AMBIENT, upperBody_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, upperBody_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, upperBody_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, upperBody_shininess);

	glPushMatrix();
	glTranslatef(0, 0.5 * robotBodyLength, 0);
	glScalef(0.5 * robotBodyWidth, 0.5 * robotBodyLength, 0.5 * robotBodyDepth);
	glutSolidCube(1.0);
	glPopMatrix();

}


// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 40.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

bool stop = false;

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 't':

		break;
	case 'S':
		stop = true;
		break;
	case 'f':
		translateForward1 += 0.5 * cosf(-spinAngle * 3.141 / 180);
		translateForward2 -= 0.5 * sinf(-spinAngle * 3.141 / 180);
		leftWheelRotate += 3.0;
		rightWheelRotate -= 3.0;
		break;
	case 'b':
		translateBackward1 += 0.5 * cosf(-spinAngle * 3.141 / 180);
		translateBackward2 -= 0.5 * sinf(-spinAngle * 3.141 / 180);
		leftWheelRotate -= 3.0;
		rightWheelRotate += 3.0;
		break;
	case 'l':
		spinAngle += 2.0;
		rightWheelRotate -= 3.0;
		break;
	case 'r':
		spinAngle -= 2.0;
		leftWheelRotate += 3.0;
		break;
	case ' ':
		stop = false;
		counter = 0;
		glutTimerFunc(10, animationHandler1, 0);
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


void animationHandler1(int param)
{
	if (!stop)
	{

		if (wedgeRotate > 60) {
			counter += 1;
		}
		
		if (counter == 0) {
			wedgeRotate += 3;
		}
		else {
			wedgeRotate -= 3; 
		}
		
		if (wedgeRotate == 0) {
			stop = true;
		}
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler1, 0);
	}
}


// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	// Help key
	if (key == GLUT_KEY_F1)
	{
		printf("Joonho Myung 500845049\n\n");
		printf("How to control the Wedge Flipper\n");
		printf("Forward: Press or Hold f\n");
		printf("Backward: Press or Hold b\n");
		printf("LeftSpin: Press or Hold l\n");
		printf("RightSpin: Press or Hold r\n");
		printf("Weapon(Flipping Action): Press Space-bar");
	}
	// Do transformations with arrow keys
	//else if (...)   // GLUT_KEY_DOWN, GLUT_KEY_UP, GLUT_KEY_RIGHT, GLUT_KEY_LEFT
	//{
	//}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

