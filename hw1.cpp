/*
 CSCI 420 Computer Graphics, USC
 Assignment 1: Height Fields
 C++ starter code
 
 Student username: greent
 */

#ifdef WIN32
#include <windows.h>

#endif
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include "imageIO.h"
#include "openGLMatrix.h"
#include "basicPipelineProgram.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "glew32d.lib")
#else
#pragma comment(lib, "glew32.lib")
#endif
#endif

#ifdef WIN32
char shaderBasePath[1024] = SHADER_BASE_PATH;
#else
char shaderBasePath[1024] = "../openGLHelper-starterCode";
#endif
using namespace std;


BasicPipelineProgram *y;
y = new BasicPipelineProgram();

// represents one control point along the spline
struct Point
{
    double x;
    double y;
    double z;
};

// spline struct
// contains how many control points the spline has, and an array of control points
struct Spline
{
    int numControlPoints;
    Point * points;
};


int mousePos[2]; // x,y coordinate of the mouse position

int leftMouseButton = 0; // 1 if pressed, 0 if not
int middleMouseButton = 0; // 1 if pressed, 0 if not
int rightMouseButton = 0; // 1 if pressed, 0 if not

typedef enum { ROTATE, TRANSLATE, SCALE } CONTROL_STATE;
CONTROL_STATE controlState = ROTATE;

// state of the world
float landRotate[3] = { 0.0f, 0.0f, 0.0f };
float landTranslate[3] = { 0.0f, 0.0f, 0.0f };
float landScale[3] = { 1.0f, 1.0f, 1.0f };
GLint axis = 2;
GLfloat delta = 2.0;
int windowWidth = 1280;
int windowHeight = 720;
char windowTitle[512] = "CSCI 420 homework I";

ImageIO * heightmapImage;
OpenGLMatrix* matrix;
BasicPipelineProgram *pipelineProgram;
GLuint vao, buffer;
GLfloat * p;

int number_vertices = 0;
int stop = 0;
GLfloat* positions;
GLfloat* colors;
//mode tells us what state the DEM is in
// 1 is for triangles 0 is for wire frame 2 is for point cloud
int mode = 0;



float s = 0.5;
glm::mat4x4 M(-s, 2-s, s-2, s, 2*s, s-3, 3-2*s, -s, -s, 0, s, 0, 0, 1, 0, 0);
glm::mat3x4 C;

// the spline array
Spline * splines;
// total number of splines
int numSplines;

int loadSplines(char * argv)
{
    char * cName = (char *) malloc(128 * sizeof(char));
    FILE * fileList;
    FILE * fileSpline;
    int iType, i = 0, j, iLength;
    
    // load the track file
    fileList = fopen(argv, "r");
    if (fileList == NULL)
    {
        printf ("can't open file\n");
        exit(1);
    }
    
    // stores the number of splines in a global variable
    fscanf(fileList, "%d", &numSplines);
    
    splines = (Spline*) malloc(numSplines * sizeof(Spline));
    
    // reads through the spline files
    for (j = 0; j < numSplines; j++)
    {
        i = 0;
        fscanf(fileList, "%s", cName);
        fileSpline = fopen(cName, "r");
        
        if (fileSpline == NULL)
        {
            printf ("can't open file\n");
            exit(1);
        }
        
        // gets length for spline file
        fscanf(fileSpline, "%d %d", &iLength, &iType);
        
        // allocate memory for all the points
        splines[j].points = (Point *)malloc(iLength * sizeof(Point));
        splines[j].numControlPoints = iLength;
        
        // saves the data to the struct
        while (fscanf(fileSpline, "%lf %lf %lf",
                      &splines[j].points[i].x,
                      &splines[j].points[i].y,
                      &splines[j].points[i].z) != EOF)
        {
            i++;
        }
    }
    
    free(cName);
    
    return 0;
}

int initTexture(const char * imageFilename, GLuint textureHandle)
{
    // read the texture image
    ImageIO img;
    ImageIO::fileFormatType imgFormat;
    ImageIO::errorType err = img.load(imageFilename, &imgFormat);
    
    if (err != ImageIO::OK)
    {
        printf("Loading texture from %s failed.\n", imageFilename);
        return -1;
    }
    
    // check that the number of bytes is a multiple of 4
    if (img.getWidth() * img.getBytesPerPixel() % 4)
    {
        printf("Error (%s): The width*numChannels in the loaded image must be a multiple of 4.\n", imageFilename);
        return -1;
    }
    
    // allocate space for an array of pixels
    int width = img.getWidth();
    int height = img.getHeight();
    unsigned char * pixelsRGBA = new unsigned char[4 * width * height]; // we will use 4 bytes per pixel, i.e., RGBA
    
    // fill the pixelsRGBA array with the image pixels
    memset(pixelsRGBA, 0, 4 * width * height); // set all bytes to 0
    for (int h = 0; h < height; h++)
        for (int w = 0; w < width; w++)
        {
            // assign some default byte values (for the case where img.getBytesPerPixel() < 4)
            pixelsRGBA[4 * (h * width + w) + 0] = 0; // red
            pixelsRGBA[4 * (h * width + w) + 1] = 0; // green
            pixelsRGBA[4 * (h * width + w) + 2] = 0; // blue
            pixelsRGBA[4 * (h * width + w) + 3] = 255; // alpha channel; fully opaque
            
            // set the RGBA channels, based on the loaded image
            int numChannels = img.getBytesPerPixel();
            for (int c = 0; c < numChannels; c++) // only set as many channels as are available in the loaded image; the rest get the default value
                pixelsRGBA[4 * (h * width + w) + c] = img.getPixel(w, h, c);
        }
    
    // bind the texture
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    
    // initialize the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelsRGBA);
    
    // generate the mipmaps for this texture
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // set the texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // query support for anisotropic texture filtering
    GLfloat fLargest;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
    printf("Max available anisotropic samples: %f\n", fLargest);
    // set anisotropic texture filtering
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 0.5f * fLargest);
    
    // query for any errors
    GLenum errCode = glGetError();
    if (errCode != 0)
    {
        printf("Texture initialization error. Error code: %d.\n", errCode);
        return -1;
    }
    
    // de-allocate the pixel array -- it is no longer needed
    delete [] pixelsRGBA;
    
    return 0;
}

// write a screenshot to the specified filename
void saveScreenshot(const char * filename)
{
    unsigned char * screenshotData = new unsigned char[windowWidth * windowHeight * 3];
    glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);
    
    ImageIO screenshotImg(windowWidth, windowHeight, 3, screenshotData);
    
    if (screenshotImg.save(filename, ImageIO::FORMAT_JPEG) == ImageIO::OK)
        cout << "File " << filename << " saved successfully." << endl;
    else cout << "Failed to save file " << filename << '.' << endl;
    
    delete [] screenshotData;
}

void renderDEM(){
    GLint first = 0;
    GLsizei count = number_vertices;
    
    //changes the display mode of the terrain
    //wireframe, solid, and point cloud respectively
    if (mode == 0){
        glDrawArrays(GL_LINES, first, count);
    }else if (mode == 1){
        glDrawArrays(GL_TRIANGLES, first, count);
    }else if (mode==2){
        glDrawArrays(GL_POINTS, first, count);
    }
    
    
}
void displayFunc(void)
{
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    matrix->SetMatrixMode(OpenGLMatrix::ModelView);
    matrix->LoadIdentity();
    
    //this will look straight down on the model
    matrix->LookAt(0,0,2,0,0,0,0,1,0);
    
    //set up Rotate Scale and Translate
    matrix->Translate(landTranslate[0], landTranslate[1], landTranslate[2]);
    matrix->Scale(landScale[0], landScale[1], landScale[2]);
    matrix->Rotate(landRotate[0], 1.0, 0.0, 0.0);
    matrix->Rotate(landRotate[1], 0.0, 1.0, 0.0);
    matrix->Rotate(landRotate[2], 0.0, 0.0, 1.0);
    
    GLuint program = pipelineProgram->GetProgramHandle();
    // PPT4-26
    // get a handle to the modelViewMatrix shader variable
    
    
    GLint h_modelViewMatrix = glGetUniformLocation(program, "modelViewMatrix");
    float m[16]; // column-major
    // here, must fill m… (use our OpenGLMatrix class)
    matrix->GetMatrix(m);
    
    GLboolean isRowMajor = GL_FALSE;
    glUniformMatrix4fv(h_modelViewMatrix, 1, isRowMajor, m);
    // PPT6-38 change to projection mode
    matrix->SetMatrixMode(OpenGLMatrix::Projection);
    
    GLfloat aspect = (GLfloat) windowWidth / (GLfloat) windowHeight;
    matrix->SetMatrixMode(OpenGLMatrix::Projection);
    matrix->LoadIdentity();
    matrix->Ortho(-2.0, 2.0, -2.0/aspect, 2.0/aspect, 0.0, 10.0);
    
    
    // PPT4-27
    // get a handle to the projectionMatrix shader variable
    
    GLint h_projectionMatrix = glGetUniformLocation(program, "projectionMatrix");
    
    float p[16]; // column-major
    matrix->GetMatrix(p);
    
    // upload p to the GPU
    glUniformMatrix4fv(h_projectionMatrix, 1, isRowMajor, p);
    
    pipelineProgram->Bind(); // bind the pipeline program
    glBindVertexArray(vao);
    
    renderDEM();
    glBindVertexArray(0);
    glutSwapBuffers();
}

void idleFunc()
{
    // do some stuff...
    // for example, here, you can save the screenshots to disk (to make the animation)
    // make the screen update
    if (stop != 0) {
        landRotate[axis] += delta;
        if (landRotate[axis] > 360.0) landRotate[axis] -= 360.0;
    }
    glutPostRedisplay();
}

void reshapeFunc(int w, int h)
{   glViewport(0, 0, w, h);
    GLfloat aspect = (GLfloat) w / (GLfloat) h;
    // setup perspective matrix...
    
    
    matrix->SetMatrixMode(OpenGLMatrix::Projection);
    matrix->LoadIdentity();
    matrix->Ortho(-2.0, 2.0, -2.0/aspect, 2.0/aspect, 0.0, 10.0);
    matrix->SetMatrixMode(OpenGLMatrix::ModelView);
}

void mouseMotionDragFunc(int x, int y)
{
    // mouse has moved and one of the mouse buttons is pressed (dragging)
    
    // the change in mouse position since the last invocation of this function
    int mousePosDelta[2] = { x - mousePos[0], y - mousePos[1] };
    
    switch (controlState)
    {
            // translate the landscape
        case TRANSLATE:
            if (leftMouseButton)
            {
                // control x,y translation via the left mouse button
                landTranslate[0] += mousePosDelta[0] * 0.01f;
                landTranslate[1] -= mousePosDelta[1] * 0.01f;
            }
            if (middleMouseButton)
            {
                // control z translation via the middle mouse button
                landTranslate[2] += mousePosDelta[1] * 0.01f;
            }
            break;
            
            // rotate the landscape
        case ROTATE:
            if (leftMouseButton)
            {
                // control x,y rotation via the left mouse button
                landRotate[0] += mousePosDelta[1];
                landRotate[1] += mousePosDelta[0];
            }
            if (middleMouseButton)
            {
                // control z rotation via the middle mouse button
                landRotate[2] += mousePosDelta[1];
            }
            break;
            
            // scale the landscape
        case SCALE:
            if (leftMouseButton)
            {
                // control x,y scaling via the left mouse button
                landScale[0] *= 1.0f + mousePosDelta[0] * 0.01f;
                landScale[1] *= 1.0f - mousePosDelta[1] * 0.01f;
            }
            if (middleMouseButton)
            {
                // control z scaling via the middle mouse button
                landScale[2] *= 1.0f - mousePosDelta[1] * 0.01f;
            }
            break;
    }
    
    // store the new mouse position
    mousePos[0] = x;
    mousePos[1] = y;
}

void mouseMotionFunc(int x, int y)
{
    // mouse has moved
    // store the new mouse position
    mousePos[0] = x;
    mousePos[1] = y;
}

void mouseButtonFunc(int button, int state, int x, int y)
{
    // a mouse button has has been pressed or depressed
    
    // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            leftMouseButton = (state == GLUT_DOWN);
            axis = 0;
            break;
            
        case GLUT_MIDDLE_BUTTON:
            middleMouseButton = (state == GLUT_DOWN);
            axis = 1;
            break;
            
        case GLUT_RIGHT_BUTTON:
            rightMouseButton = (state == GLUT_DOWN);
            axis = 2;
            break;
    }
    
    // keep track of whether CTRL and SHIFT keys are pressed
    switch (glutGetModifiers())
    {
        case GLUT_ACTIVE_CTRL:
            controlState = TRANSLATE;
            break;
            
        case GLUT_ACTIVE_SHIFT:
            controlState = SCALE;
            break;
            
            // if CTRL and SHIFT are not pressed, we are in rotate mode
        default:
            controlState = ROTATE;
            break;
    }
    
    // store the new mouse position
    mousePos[0] = x;
    mousePos[1] = y;
}

void keyboardFunc(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27: // ESC key
            exit(0); // exit the program
            break;
        case ' ':
            axis = 0;
            stop = !stop;
            break;
            
        case '4':
            // take a screenshot
            saveScreenshot("screenshot.jpg");
            break;
        case '1':
            // change to solid mode
            mode = 1;
            break;
        case '2':
            // change to wireframe mode
            mode = 0;
            break;
        case '3':
            // change to point cloud mode
            mode = 2;
            break;
            
    }
}

void initScene(int argc, char *argv[])
{
    // load the splines from the provided filename
    loadSplines(argv[1]);
    
    positions = new GLfloat [numSplines*3];
    colors = new GLfloat [numSplines*4];
    long int ip = 0;
    long int ic = 0;
    
    for (int i =0; i <numSplines; i++){
        positions[ip+1]= splines[0]. points[0].x;
            positions[ip+1]=splines[0]. points[0].y;
            positions[ip+2]=splines[0]. points[0].z;
            colors[ic]= 1.0;
            colors[ic+1]= 1.0;
            colors[ic+2]= 1.0;
            colors[ic+3]= 1.0;
            ip = ip + 3;
            ic = ic + 4;

        }
    }
    y = new BasicPipelineProgram();

    pipelineProgram = new BasicPipelineProgram();
    pipelineProgram -> Init(shaderBasePath);
    GLuint program = pipelineProgram -> GetProgramHandle();
    
    //enable depth testinf
    glEnable(GL_DEPTH_TEST);
    matrix = new OpenGLMatrix();
    
    //init VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    //VBO INIT
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*numSplines*3 + sizeof(float)*numSplines*4, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*numSplines*3, positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*numSplines*3, sizeof(float)*numSplines*4, colors);
    
    
    GLuint loc = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(loc);
    const void * offset = (const void*) 0;
    GLsizei stride = 3*sizeof(GLfloat);
    GLboolean normalized = GL_FALSE;
    
    glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, stride, offset);
    
    GLfloat h[numSplines*3];
    loc = glGetAttribLocation(program, "color");
    glEnableVertexAttribArray(loc);
    offset = (const void*) sizeof(h);
    stride = 4*sizeof(GLfloat);
    
    glVertexAttribPointer(loc, 4, GL_FLOAT, normalized, stride, offset);
    glBindVertexArray(0);
    
}

int main(int argc, char *argv[])
{
    if (argc<2)
    {
        printf ("usage: %s <trackfile>\n", argv[0]);
        exit(0);
    }
    
    cout << "Initializing GLUT..." << endl;
    glutInit(&argc,argv);
    
    cout << "Initializing OpenGL..." << endl;
    
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
#else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
#endif
    
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(windowTitle);
    
    cout << "OpenGL Version: " << glGetString(GL_VERSION) << endl;
    cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    
    printf("Loaded %d spline(s).\n", numSplines);
    for(int i=0; i<numSplines; i++)
        printf("Num control points in spline %d: %d.\n", i, splines[i].numControlPoints);
    
    // tells glut to use a particular display function to redraw
    glutDisplayFunc(displayFunc);
    // perform animation inside idleFunc
    glutIdleFunc(idleFunc);
    // callback for mouse drags
    glutMotionFunc(mouseMotionDragFunc);
    // callback for idle mouse movement
    glutPassiveMotionFunc(mouseMotionFunc);
    // callback for mouse button changes
    glutMouseFunc(mouseButtonFunc);
    // callback for resizing the window
    glutReshapeFunc(reshapeFunc);
    // callback for pressing the keys on the keyboard
    glutKeyboardFunc(keyboardFunc);
    
    // init glew
#ifdef __APPLE__
    // nothing is needed on Apple
#else
    // Windows, Linux
    GLint result = glewInit();
    if (result != GLEW_OK)
    {
        cout << "error: " << glewGetErrorString(result) << endl;
        exit(EXIT_FAILURE);
    }
#endif
    glMatrixMode(GL_MODELVIEW);
    // do initialization
    initScene(argc, argv);
    
    // sink forever into the glut loop
    glutMainLoop();
}
