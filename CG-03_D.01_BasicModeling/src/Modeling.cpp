﻿///////////////////////////////////////////////////////////////////////////////////////////////////
// Demo: CG-03-D.01D_BasicModeling (Ver 2.1)                                                     //
///////////////////////////////////////////////////////////////////////////////////////////////////



// system includes ////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <string>
using namespace std;


// OpenGL helper includes /////////////////////////////////////////////////////////////////////////
#include <GL/glew.h>
#ifdef _MSC_VER
#pragma warning( disable: 4312 ) // ignore visual studio warnings for FLTK 64-bit type casts
#endif
#include <FL/glut.H>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// application helper includes ////////////////////////////////////////////////////////////////////
#include "../../_COMMON/inc/TrackBall.h"
#include "../../_COMMON/inc/UtilGLSL.h"


// application global variables and constants /////////////////////////////////////////////////////
GLenum POLYGON_MODE = GL_FRONT;
GLenum POLYGON_DRAW = GL_FILL;
GLint PROGRAM_ID = 0;
GLint MV_MAT4_LOCATION = 0;
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

int MENU_ENTRY = 0;
int MENU_VALUE = 0;
string MENU_ENTRY_STR[3];

const float WIDTH = 5.0f;		// width of cube
GLuint INDICES_COUNT = 0; 



void glutDisplayCB(void)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	// clear window background
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get trackball transformation matrix
	glm::mat4 model(1.0f);
	glm::mat4 view = TrackBall::getTransformation();
	model = model * view;

	// set model view transformation matrix
	glUniformMatrix4fv(MV_MAT4_LOCATION, 1, GL_FALSE, glm::value_ptr(model));

	/* definition of cube face indices (for GL_TRIANGLES --> 30)
	static GLushort indices[] =
	{
		0, 1, 2,
		3, 0, 2,
		1, 7, 6,
		2, 1, 6,
		4, 5, 6,
		7, 4, 6,
		4, 0, 3,
		5, 4, 3,

		0, 4, 7,
		1, 0, 7
	};
	//*/

	/* definition of two perpendicular planes face indices (for GL_TRIANGLES --> 12)
	static GLushort indices[] =
	{
		0, 4, 6,
		6, 2, 0,

		3, 5, 7,
		7, 1, 3
	};
	//*/

	/* definition of cube face indices (for GL_TRIANGLE_STRIP --> 15)
	static GLushort indices[] =
	{
		3, 0, 2,
		1, 6,
		7, 5,
		4, 3, 0,
		0xFFFF,
		0, 4, 1, 7
	};
	INDICES_COUNT = sizeof(indices) / sizeof(indices[0]);
	//*/


	// dereferencing the vertices and draw the geometry using glDrawArrays
	//glDrawArrays(GL_TRIANGLES, 0, 8);
	//glDrawArrays(GL_QUADS, 0, 8);


	// dereferencing the vertices and draw the geometry using glDrawElements
	//glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_SHORT, &indices[0]);
	//*
	glPrimitiveRestartIndex(0xFFFF);
	glEnable(GL_PRIMITIVE_RESTART);
	//glDrawElements(GL_TRIANGLE_STRIP, INDICES_COUNT, GL_UNSIGNED_SHORT, &indices[0]);
	glDrawElements(GL_TRIANGLE_STRIP, INDICES_COUNT, GL_UNSIGNED_SHORT, 0);
	//*/



	//glFlush();
	glutSwapBuffers();
	UtilGLSL::checkOpenGLErrorCode();
}



void initModel(float witdth)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	// definition of the cube vertices (X, Y, Z)
	GLfloat vertices[] =
	{
		-witdth, -witdth,  witdth, 1.0f,  // v0
		 witdth, -witdth,  witdth, 1.0f,  // v1
		 witdth,  witdth,  witdth, 1.0f,  // v2
		-witdth,  witdth,  witdth, 1.0f,  // v3
		-witdth, -witdth, -witdth, 1.0f,  // v4
		-witdth,  witdth, -witdth, 1.0f,  // v5
		 witdth,  witdth, -witdth, 1.0f,  // v6
		 witdth, -witdth, -witdth, 1.0f   // v7
	};


	// definition of the colors, each vertex has his own color definition (RGB)
	GLfloat colors[] =
	{
		1.0f, 0.2f, 0.2f,
		1.0f, 0.2f, 0.2f,
		1.0f, 0.2f, 1.0f,
		1.0f, 0.2f, 1.0f,
		1.0f, 1.0f, 0.2f,
		1.0f, 1.0f, 0.2f,
		0.2f, 0.2f, 1.0f,
		0.2f, 0.2f, 1.0f
	};

	//* definition of cube face indices (for GL_TRIANGLE_STRIP --> 15)
	static GLushort indices[] =
	{
		3, 0, 2,
		1, 6,
		7, 5,
		4, 3, 0,
		0xFFFF,
		0, 4, 1, 7
	};
	INDICES_COUNT = sizeof(indices) / sizeof(indices[0]);
	//*/



	// setup Vertex Array Object
	GLuint vao;
	glGenVertexArrays(1, &vao);	
	glBindVertexArray(vao);

	// setup Vertex Buffer Object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);

	//* setup local Index Buffer Object
	GLuint ibo;
	glGenBuffers(1, &ibo);

	// setup Indices Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(indices), indices);
	//*/

	// get position vertex attribute location
	// (requires that the shader program has been compiled already!)
	GLuint vecPosition = glGetAttribLocation(PROGRAM_ID, "vecPosition");
	glVertexAttribPointer(vecPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vecPosition);
	
	// get color vertex attribute location
	// (requires that the shader program has been compiled already!)
	GLuint vecColor = glGetAttribLocation(PROGRAM_ID, "vecColor");
	glVertexAttribPointer(vecColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vertices)));
	glEnableVertexAttribArray(vecColor);
}



void initRendering()
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	// set background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(POLYGON_MODE, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	// get and setup orthographic projection matrix
	glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -10.0f, 10.0f);
	GLint location = glGetUniformLocation(PROGRAM_ID, "matProjection");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));

	// get modelview matrix location
	MV_MAT4_LOCATION = glGetUniformLocation(PROGRAM_ID, "matModelView");
}



void initMenuChange(int entry, char* name, int value)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	MENU_ENTRY = entry;
	MENU_VALUE = value;
	if (entry > 0) MENU_ENTRY_STR[entry].assign(name);
}



void glutMenuCB1(int item)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	switch (item)
	{
		case 0:
		{
			exit(0);
			break;
		}
		case 1:
		{
			glPolygonMode(POLYGON_MODE, GL_LINE);
			POLYGON_DRAW = GL_LINE;
			initMenuChange(1, "Disable Wireframe", 2);
			break;
		}
		case 2:
		{
			glPolygonMode(POLYGON_MODE, GL_FILL);
			POLYGON_DRAW = GL_FILL;
			initMenuChange(1, "Enable Wireframe", 1);
			break;
		}
		case 3:
		{
			glEnable(GL_CULL_FACE);
			initMenuChange(2, "Disable Culling", 4);
			break;
		}
		case 4:
		{
			glDisable(GL_CULL_FACE);
			initMenuChange(2, "Enable Culling", 3);
			break;
		}
		case 5:
		{
			glEnable(GL_DEPTH_TEST);
			initMenuChange(3, "Disable Depth Buffer", 6);
			glutSetWindowTitle("Basic Modeling (Depth Buffering Enabled)");
			break;
		}
		case 6:
		{
			glDisable(GL_DEPTH_TEST);
			initMenuChange(3, "Enable Depth Buffer", 5);
			glutSetWindowTitle("Basic Modeling (Depth Buffering Disabled)");
			break;
		}
		default:
		{
			// reset settings
			glPolygonMode(POLYGON_MODE, GL_FILL);
			POLYGON_DRAW = GL_FILL;
			glutSetWindowTitle("Basic Modeling");
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);

			initMenuChange(-1, "Reset Menu", -1);
		}
	}
	glutPostRedisplay();
}



void glutMenuCB2(int item)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
    POLYGON_MODE = item;
	glPolygonMode(POLYGON_MODE, POLYGON_DRAW);
    glutPostRedisplay();
}



void initMenu()
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	int sub_menu;

	// create sub-menu
	sub_menu = glutCreateMenu(glutMenuCB2);
	glutAddMenuEntry("GL_FRONT", GL_FRONT);
	glutAddMenuEntry("GL_BACK", GL_BACK);
	glutAddMenuEntry("GL_FRONT_AND_BACK", GL_FRONT_AND_BACK);

	// work around for FLTK bug in glutChangeToMenuEntry() function
	MENU_ENTRY_STR[0] = ""; // dummy entry since GLUT menus start at index 1
	MENU_ENTRY_STR[1] = "Enable Wireframe";
	MENU_ENTRY_STR[2] = "Enable Culling";
	MENU_ENTRY_STR[3] = "Enable Depth Buffer";

	// create menu
	glutCreateMenu(glutMenuCB1);
	glutAddMenuEntry(const_cast<char*>(MENU_ENTRY_STR[1].c_str()), 1);
	glutAddMenuEntry(const_cast<char*>(MENU_ENTRY_STR[2].c_str()), 3);
	glutAddMenuEntry(const_cast<char*>(MENU_ENTRY_STR[3].c_str()), 5);
	glutAddSubMenu("Set Polygon Face", sub_menu);
	glutAddMenuEntry("Reset", 9);
	glutAddMenuEntry("Exit", 0);

	// attach menu to right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



void glutUpdateMenuCB(int status, int x, int y)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	if (status == GLUT_MENU_NOT_IN_USE)
	{
		// menu not in use, time to make changes
		if (MENU_ENTRY < 0)
		{
			// reset menu to default
			cout << "Resetting Menu..." << endl;
			initMenu();
		}
		else
		{
			glutChangeToMenuEntry(MENU_ENTRY, const_cast<char*>(MENU_ENTRY_STR[MENU_ENTRY].c_str()), MENU_VALUE);
		}
	}
}



void glutKeyboardCB(unsigned char key, int x, int y)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	switch (key)
	{
		case 27:
		{
			exit(0);
			break;
		}
	}
}



int main(int argc, char *argv[])
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_RGBA | GLUT_SINGLE); //not working with FLTK
	//glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | FL_OPENGL3);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | FL_OPENGL3);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 640);
	glutCreateWindow("Basic Modeling");

	// register extension wrapper library (GLEW)
	glewExperimental = GL_TRUE; // needed for Apple
	if(glewInit() != GLEW_OK)
	{
		std::cout << "ERROR: GLEW not initialized: " << glewInit() << endl;
		return -1;
	}

	// show version of OpenGL and GLSL
	UtilGLSL::showOpenGLVersion();
	UtilGLSL::showGLSLVersion();
	UtilGLSL::initOpenGLDebugCallback();

	// check for shader 4.x support
	if (UtilGLSL::checkOpenGLVersion() < 4.0)
	{
		cout << "Program requires OpenGL Version >= 4.x (Supported: ";
		cout << UtilGLSL::checkOpenGLVersion() << ") - exiting!" << endl;
		cout << "Press ENTER to continue... " << endl;
		cin.ignore();
		exit(1);
	}

	// register GLUT/FLTK callbacks
	glutDisplayFunc(glutDisplayCB);
	glutKeyboardFunc(glutKeyboardCB);
	glutMenuStatusFunc(glutUpdateMenuCB);

	glutMouseFunc(TrackBall::glutMouseButtonCB);
	glutMotionFunc(TrackBall::glutMouseMotionCB);
	glutSpecialFunc(TrackBall::glutSpecialFuncCB);

	// check for command line argument supplied shaders
	if (argc > 1)
	{
		PROGRAM_ID = UtilGLSL::initShaderProgram(argc, argv);
	}
	else
	{
		argc = 3;
		argv[0] = "";
		argv[1] = "../../glsl/modeling.vert";
		argv[2] = "../../glsl/modeling.frag";
		PROGRAM_ID = UtilGLSL::initShaderProgram(argc, argv);
	}

	// init application
	initRendering();
	initModel(WIDTH);
	initMenu();

	glutMainLoop();
	return 0;  // only for compatibility purposes
}
