/*
* Lorenz Attractor
* by Kirill Novik
*
* Drawing Lorenz Attractor in OpenGL
*
* Key Bindings:
* a 		Toggle Axes
* arrows	Change View Angle
* 0			Reset View Angle
* esc		Exit
* q			Reset R, S and B
* B/b		+/- parameter B
* S/s		+/- parameter S
* R/r		+/- parameter R
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

int th = 0; 		// Azimuth of View Angle
int ph = 0; 		// Elevation of View Angle
int axes = 1;		// Display Axes
double s = -2;
double r = 35;
double b = 2.667;
double dt = 0.001; // limit
double points[100000][3]; // Array with Coordinates
double maxX; double maxY; double maxZ;
double minX; double minY; double minZ;
double tick = -50;
int direction = 0; // Direction of Rotation

						  // Cosine and Sine in Degrees
#define Cos(x) (cos((x)*3.1415927/180)) 
#define Sin(x) (sin((x)*3.1415927/180))

						  // Routine for Raster Text Output
#define LEN 8192 // Maximum Length of Text String
void Print(const char* format, ...)
{
	char buf[LEN];
	char* ch = buf;
	va_list args;
	va_start(args, format);
	vsnprintf(buf, LEN, format, args);
	va_end(args);
	while (*ch)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ch++);
}

void calculateMaxMin() {
	maxX = 0; maxY = 0; maxZ = 0;
	minX = 99999; minY = 99999; minZ = 99999;
	int i = 0;
	for (i = 0; i < 99999; i++) {
		double x = points[i][0];
		double y = points[i][1];
		double z = points[i][2];
		if (x < minX) minX = x;
		if (x > maxX) maxX = x;
		if (y < minY) minY = y;
		if (y > maxY) maxY = y;
		if (z < minZ) minZ = z;
		if (z > maxZ) maxZ = z;
	}
}

void lorenzEquation(double s, double r, double b) {
	// initialize

	points[0][0] = 0.01;
	points[0][1] = 0.01;
	points[0][2] = 0.01;
	int i = 0;
	for ( i = 0; i<100000; i++) {
		//printf("x: %f, y: %f, z: %f\n", points[i][0], points[i][1], points[i][2]);
		double x = points[i][0];
		double y = points[i][1];
		double z = points[i][2];
		points[i + 1][0] = x + (s*(y - x))*dt;
		points[i + 1][1] = y + (x*(r-z) - y)*dt;
		points[i + 1][2] = z + (x*y - b*z)*dt;
	}
	calculateMaxMin();
}

void normalizeColor(double x, double y, double z, double tick) { // color is space is same as the xyz space
	double newX = (x - minX) / (maxX - minX);
	double newY = (y - minY) / (maxY - minY);
	double newZ = (z - minZ) / (maxZ - minZ);
	double animatedX = (double)(((newX * 100) + tick)) / 100;
	double animatedY = (double)(((newY * 100) + tick)) / 100;
	double animatedZ = (double)(((newZ * 100) + tick)) / 100;
	glColor3d(animatedX, animatedY, animatedZ);
}

void displayLorenzAttractor() {
	glBegin(GL_LINE_STRIP);
	glColor3f(1, 1, 1);
	int i;
	for ( i = 0; i < 100000; i++) {
		normalizeColor(points[i][0], points[i][1], points[i][2], tick);
		glVertex3d(points[i][0], points[i][1], points[i][2]);
	}
	glEnd();

}

// Display Routine
void display()
{
	const double len = 20; // Length of axes
							// Erase the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable Z-buffering in OpenGL
	glEnable(GL_DEPTH_TEST);
	// Undo Previous Transformations
	glLoadIdentity();
	// Set View Angle
	glRotatef(ph, 1, 0, 0);
	glRotatef(th, 0, 1, 0);
	glColor3f(1, 1, 1);
	//Draw Lorenz Attractor
	displayLorenzAttractor();
	//Draw Axes
	if (axes)
	{
		glBegin(GL_LINES);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(len, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, len, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, len);
		glEnd();
		// Label Axes
		glRasterPos3d(len, 0.0, 0.0);
		Print("X");
		glRasterPos3d(0.0, len, 0.0);
		Print("Y");
		glRasterPos3d(0.0, 0.0, len);
		Print("Z");
	}
	//  Print the text string
	Print("Angle=%d,%d", th, ph);
	//  Render the scene
	glFlush();
	//  Make the rendered scene visible
	glutSwapBuffers();
}

void reshape(int width, int height)
{
	const double dim = 50;
	//  Ratio of the width to the height of the window
	double w2h = (height>0) ? (double)width / height : 1;
	//  Set the viewport to the entire window
	glViewport(0, 0, width, height);
	//  Tell OpenGL we want to manipulate the projection matrix
	glMatrixMode(GL_PROJECTION);
	//  Undo previous transformations
	glLoadIdentity();
	//  Orthogonal projection
	glOrtho(-w2h*dim, +w2h*dim, -dim, +dim, -dim, +dim);
	//  Switch to manipulating the model matrix
	glMatrixMode(GL_MODELVIEW);
	//  Undo previous transformations
	glLoadIdentity();
}

void recalculate() { // recalculate Lorenz Equation
	lorenzEquation(s, r, b);
	glutPostRedisplay();
	printf("S: %f, R: %f, B: %f\n", s, r, b);
}

/*
* GLUT class this routine for keys other than arrows
*/
void key(unsigned char ch, int x, int y) {
	switch (ch) {
		case 27:
			exit(0);
			
		case 'S':
			s += 1;
			recalculate();
			break;

		case 's':
			s -= 1;
			recalculate();
			break;

		case 'B':
			b += 1;
			recalculate();
			break;

		case'b':
			b -= 1;
			recalculate();
			break;

		case 'R':
			r += 1;
			recalculate();
			break;

		case 'r':
			r -= 1;
			recalculate();
			break;

		case 'a':
			axes = (axes + 1) % 2;
			break;

		case '0':
			th = 0;
			break;

		case 'q':
			lorenzEquation(10, 28, 2.667);
			s = 10;
			r = 28;
			b = 2.667;
			recalculate();
			break;
	}
}

/*
*  GLUT calls this routine when an arrow key is pressed
*/
void special(int key, int x, int y)
{
	//  Right arrow key - increase angle by 5 degrees
	if (key == GLUT_KEY_RIGHT)
		th += 5;
	//  Left arrow key - decrease angle by 5 degrees
	else if (key == GLUT_KEY_LEFT)
		th -= 5;
	//  Up arrow key - increase elevation by 5 degrees
	else if (key == GLUT_KEY_UP)
		ph += 5;
	//  Down arrow key - decrease elevation by 5 degrees
	else if (key == GLUT_KEY_DOWN)
		ph -= 5;
	//  Keep angles to +/-360 degrees
	th %= 360;
	ph %= 360;
	//  Tell GLUT it is necessary to redisplay the scene
	glutPostRedisplay();
}

/*
*  GLUT calls this toutine when there is nothing else to do
*/
void idle()
{
	//double t = glutGet(GLUT_ELAPSED_TIME)/;
	if (tick > 50) direction = 1;
	if (tick < -50) direction = 0;
	if (direction == 0) tick += 0.5;
	if (direction == 1) tick -= 0.5;
	th += 1;
	glutPostRedisplay();
}

// Telling GLUT What to Do
int main(int argc, char* argv[])
{
	lorenzEquation(s, r, b);

	// Inititalize GLUT and Process User Parameters
	glutInit(&argc, argv);

	// Request Double Buffered, True Color Window with Z buffering at 600x600
	glutInitWindowSize(600, 600);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	// Create the Window
	glutCreateWindow("Lorenz Attractor");
	// Tell GLUT to call "idle" when there is nothing else to do
	glutIdleFunc(idle);
	// Tlle GLUT to call "display" when the scene should be drawn
	glutDisplayFunc(display);
	// Tell GLUT to call "reshape" when the window is resized
	glutReshapeFunc(reshape);
	// Tell GLUT to call "special" when an arrow key is pressed
	glutSpecialFunc(special);
	// Tell GLUT to call "key" when a key is pressed
	glutKeyboardFunc(key);
	// Pass controll to GLUT so it can interact with the user
	glutMainLoop();
	return 0;
}
