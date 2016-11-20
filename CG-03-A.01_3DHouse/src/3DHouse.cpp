///////////////////////////////////////////////////////////////////////////////////////////////////
// Assignment: CG-03-A.01_3DHouse (Ver 2.1)                                                      //
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
GLint PROGRAM_ID = 0;
GLint MV_MAT4_LOCATION = 0;
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

int MENU_ENTRY = 0;
int MENU_VALUE = 0;
string MENU_NAME;

GLenum POLYGON_MODE = GL_FRONT;
GLenum POLYGON_DRAW = GL_FILL;

const float WIDTH = 5.0f;		  // ground = width * 2
const float HEIGHT = 10.0f;		// height of roof

GLuint VAO[2];


void glutDisplayCB(void)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	// clear window background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get trackball transformation matrix
	glm::mat4 model(1.0f);
	glm::mat4 view = TrackBall::getTransformation();
	model = model * view;

	// set model view transformation matrix
	glUniformMatrix4fv(MV_MAT4_LOCATION, 1, GL_FALSE, glm::value_ptr(model));

	// definition of ground face indices (for GL_TRIANGLES --> 6)
	static GLushort ground_faces[] =
	{
    // ground
		0,2,3, 
		0,1,2,
    // fence bottom
    4,5,8,
    5,9,8,
    // fence right
    5,6,9,
    6,10,9,
    // fence top
    6,7,10,
    7,11,10,
    // fence left
    7,4,11,
    4,8,11
	};

  static GLushort house_faces[] =
  {
    // house base
    0, 2, 3,
    1, 2, 0,
    // house top
    4, 6, 7,
    5, 6, 4,
    // house walls
    0, 1, 4,
    1, 5, 4,
    1, 2, 5,
    2, 6, 5,
    7, 3, 4,
    3, 0, 4,
    6, 2, 3,
    7, 6, 3,
    // house roof
    8, 9, 12,
    9, 10, 12,
    10, 11, 12,
    11, 8, 12
  };

  bool reenable = false; // local varaibel to re-enable culling 
  if (glIsEnabled(GL_CULL_FACE)) {
    glDisable(GL_CULL_FACE);
    reenable = true;
  }

  // bind VAO for ground and fence
  glBindVertexArray(VAO[0]);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(-1, 1);
	// dereferencing the vertices and draw the geometry using glDrawElements
	glDrawElements(GL_TRIANGLES, sizeof(ground_faces)/sizeof(ground_faces[0]), GL_UNSIGNED_SHORT, &ground_faces[0]);
  glDisable(GL_POLYGON_OFFSET_FILL);



  if (reenable) {
    glEnable(GL_CULL_FACE);
    reenable = false;
  }

  // bind VAO for house
  glBindVertexArray(VAO[1]);
  //glPolygonOffset(1, 1);
  // dereferencing the vertices and draw the geometry using glDrawElements
  glDrawElements(GL_TRIANGLES, sizeof(house_faces)/sizeof(house_faces[0]), GL_UNSIGNED_SHORT, &house_faces[0]);

	glutSwapBuffers();
	UtilGLSL::checkOpenGLErrorCode();
}



void initModel(float width, float height)
///////////////////////////////////////////////////////////////////////////////////////////////////
{
  float house_width = width / 2;
  float house_top = 2 * height / 3;
  float fence_heigth = height / 4;

	// setup Vertex Array Objects
	glGenVertexArrays(1, &VAO[0]); // VAO for ground and fence
  glGenVertexArrays(1, &VAO[1]); // VAO for house

	// setup Vertex Buffer Objects
	GLuint vbo[2];
	glGenBuffers(1, &vbo[0]); // VBO for ground and fence
  glGenBuffers(1, &vbo[1]); // VBO for house

  // define ground vertices
  GLfloat ground_vertices[] =
  {
    // ground vertices
    -width, -width, 0.0f, 1.0f, // v0
    width, -width, 0.0f, 1.0f,  // v1
    width,  width, 0.0f, 1.0f,  // v2
    -width,  width, 0.0f, 1.0f, // v3
    // fence bottom vertices
    -width, -width, 0.0f, 1.0f, // v4
    width, -width, 0.0f, 1.0f,  // v5
    width, width, 0.0f, 1.0f,   // v6
    -width, width, 0.0f, 1.0f,  // v7
    // fence top vertices
    -width, -width, fence_heigth, 1.0f, // v8
    width, -width, fence_heigth, 1.0f,  // v9
    width, width, fence_heigth, 1.0f,   // v10
    -width, width, fence_heigth, 1.0f,  // v11
    // fence mid vertices
    0, -width, fence_heigth/2, 1.0f,// v12
    width, 0, fence_heigth/2, 1.0f, // v13
    0, width, fence_heigth/2, 1.0f, // v14
    -width, 0, fence_heigth/2, 1.0f // v15
  };

  GLfloat house_vertices[] =
  {
    // house base vertices
    -house_width, -house_width, 0.0f, 1.0f, // v0
    house_width, -house_width, 0.0f, 1.0f,  // v1
    house_width, house_width, 0.0f, 1.0f,   // v2
    -house_width, house_width, 0.0f, 1.0f,  // v3
    // house top vertices
    -house_width, -house_width, house_top, 1.0f, // v4
    house_width, -house_width,  house_top, 1.0f, // v5
    house_width, house_width,  house_top, 1.0f,  // v6
    -house_width, house_width,  house_top, 1.0f, // v7
    // roof bottom vertices
    -house_width, -house_width,  house_top, 1.0f, // v8
    house_width, -house_width,  house_top, 1.0f,  // v9
    house_width, house_width,  house_top, 1.0f,   // v10
    -house_width, house_width,  house_top, 1.0f,  // v11
    // roof top vertex
    0.0f, 0.0f, height, 1.0f // v12
  };

  GLfloat ground_rgb[] =
  {
    0.0f, // red
    0.8f, // green
    0.0f  // blue
  };

  GLfloat fence_rgb[] =
  {
    0.4f, // red
    0.4f, // green
    0.0f  // blue
  };

  // definition of the colors, each vertex has his own color definition (RGB)
  GLfloat ground_colors[] =
  {
    // ground colors
    ground_rgb[0], ground_rgb[1], ground_rgb[2],
    ground_rgb[0], ground_rgb[1], ground_rgb[2],
    ground_rgb[0], ground_rgb[1], ground_rgb[2],
    ground_rgb[0], ground_rgb[1], ground_rgb[2],
    // fence colors
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2],
    fence_rgb[0], fence_rgb[1], fence_rgb[2]
  };

  GLfloat house_rgb[] =
  {
    0.4f, // red
    0.0f, // green
    1.0f  // blue
  };

  GLfloat roof_rgb[] =
  {
    0.8f, // red
    0.6f, // green
    0.0f  // blue
  };

  GLfloat house_colors[] =
  {
    house_rgb[0], house_rgb[1], house_rgb[2], // house base
    house_rgb[0], house_rgb[1], house_rgb[2],
    house_rgb[0], house_rgb[1], house_rgb[2],
    house_rgb[0], house_rgb[1], house_rgb[2],
    house_rgb[0], house_rgb[1], house_rgb[2], // house top
    house_rgb[0], house_rgb[1], house_rgb[2],
    house_rgb[0], house_rgb[1], house_rgb[2],
    house_rgb[0], house_rgb[1], house_rgb[2],
    roof_rgb[0], roof_rgb[1], roof_rgb[2],    // roof bottom
    roof_rgb[0], roof_rgb[1], roof_rgb[2],
    roof_rgb[0], roof_rgb[1], roof_rgb[2],
    roof_rgb[0], roof_rgb[1], roof_rgb[2],
    0.0f, 0.0f, 0.0f                          // roof top
  };


  ////////// ground and fence ///////////////////////

  // bind VAO and VBO for ground and fence
  glBindVertexArray(VAO[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

  // add ground and fence data
	glBufferData(GL_ARRAY_BUFFER, sizeof(ground_vertices)+sizeof(ground_colors), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ground_vertices), ground_vertices); // vertex data
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(ground_vertices), sizeof(ground_colors), ground_colors); // color data

  // get vertex position attribute location and setup vertex attribute pointer
  // (requires that the shader program has been compiled already!)
  GLuint vecPosition = glGetAttribLocation(PROGRAM_ID, "vecPosition");
  glVertexAttribPointer(vecPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(vecPosition);

  // get vertex color attribute location and setup vertex attribute pointer
  // (requires that the shader program has been compiled already!)
  GLuint vecColor = glGetAttribLocation(PROGRAM_ID, "vecColor");
  glVertexAttribPointer(vecColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(ground_vertices)));
  glEnableVertexAttribArray(vecColor);


  ////////// house ///////////////////////

  // bind VAO and VBO for house
  glBindVertexArray(VAO[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

  // add house data
  glBufferData(GL_ARRAY_BUFFER, sizeof(house_vertices) + sizeof(house_colors), NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(house_vertices), house_vertices); // vertex data
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(house_vertices), sizeof(house_colors), house_colors); // color data

  // get vertex position attribute location and setup vertex attribute pointer
  // (requires that the shader program has been compiled already!)
  vecPosition = glGetAttribLocation(PROGRAM_ID, "vecPosition");
  glVertexAttribPointer(vecPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(vecPosition);

  // get vertex color attribute location and setup vertex attribute pointer
  // (requires that the shader program has been compiled already!)
  vecColor = glGetAttribLocation(PROGRAM_ID, "vecColor");
  glVertexAttribPointer(vecColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(house_vertices)));
  glEnableVertexAttribArray(vecColor);
	
}



void initRendering()
///////////////////////////////////////////////////////////////////////////////////////////////////
{
	// set background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
	MENU_NAME.assign(name);
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
			initMenuChange(2, "Disable Culling", 4);
      glEnable(GL_CULL_FACE);

			break;
		}
		case 4:
		{
			initMenuChange(2, "Enable Culling", 3);
      glDisable(GL_CULL_FACE);
			break;
		}
		case 5:
		{
			initMenuChange(3, "Disable Depth Buffer", 6);
			glutSetWindowTitle("3D House (Depth Buffering Enabled)");
      glEnable(GL_DEPTH_TEST);
			break;
		}
		case 6:
		{
			initMenuChange(3, "Enable Depth Buffer", 5);
			glutSetWindowTitle("3D House (Depth Buffering Disabled)");
      glDisable(GL_DEPTH_TEST);

			break;
		}
		default:
		{
			// reset settings
			glPolygonMode(POLYGON_MODE, GL_FILL);
			POLYGON_DRAW = GL_FILL;
			glutSetWindowTitle("3D House");
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

	/* create sub-menu */
	sub_menu = glutCreateMenu(glutMenuCB2);
	glutAddMenuEntry("GL_FRONT", GL_FRONT);
	glutAddMenuEntry("GL_BACK", GL_BACK);
	glutAddMenuEntry("GL_FRONT_AND_BACK", GL_FRONT_AND_BACK);


	// create menu
	glutCreateMenu(glutMenuCB1);
	glutAddMenuEntry("Enable Wireframe", 1);
	glutAddMenuEntry("Enable Culling", 3);
	glutAddMenuEntry("Enable Depth Buffer", 5);
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
			MENU_NAME.append(1, 0); // guarantee NUL termination
			MENU_NAME.resize(1024);
			glutChangeToMenuEntry(MENU_ENTRY, &MENU_NAME[0], MENU_VALUE);
			MENU_NAME.resize(strlen(&MENU_NAME[0]));
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | FL_OPENGL3);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 640);
	glutCreateWindow("3D House");

	// register extension wrapper library (GLEW)
	glewExperimental = GL_TRUE;
	// necessary to force GLEW to use a modern OpenGL method for function availability check

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
		argv[1] = "../../glsl/house.vert";
		argv[2] = "../../glsl/house.frag";
		PROGRAM_ID = UtilGLSL::initShaderProgram(argc, argv);
	}

	// init application 
	initRendering();
	initModel(WIDTH, HEIGHT);
 	initMenu();

	glutMainLoop();
	return 0;  // only for compatibility purposes
}
