#define GLFW_INCLUDE_ES2 1
#define GLFW_DLL 1

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include "angle_util/Matrix.h"
#include "angle_util/geometry_utils.h"
#include "bitmap.h"

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define TEXTURE_COUNT 6

GLint GprogramID = -1;
GLuint GtextureID[TEXTURE_COUNT];


GLuint Gframebuffer;
GLuint GtextureBG;
GLuint GdepthTextureBG;
GLint GtexBGWidth = WINDOW_WIDTH / 1;
GLint GtexBGHeight = WINDOW_HEIGHT / 1;

GLFWwindow* window;

Matrix4 gPerspectiveMatrix;
Matrix4 gViewMatrix;

#define RECT_VERTICE_W 6
#define RECT_VERTICE_H 6

//every quad has 6 vertices
const int RECT_VERTEX_COUNT = RECT_VERTICE_W * RECT_VERTICE_H * 6;
//every vertex has 3 components(x, y z)
const int RECT_VERTEX_ARRAY_SIZE = RECT_VERTEX_COUNT * 3;

const int RECT_UV_ARRAY_SIZE = RECT_VERTEX_COUNT * 2;

GLfloat mRectVertices[RECT_VERTEX_ARRAY_SIZE];
GLfloat mRectUV[RECT_UV_ARRAY_SIZE];

void genPlane(void)
{
	const float width = 2.0f;
	const float height = 2.0f;
	const float halfWidth = width * 0.5f;
	const float halfHeight = height * 0.5f;

	const float texMul = 1.0f;

	int currentVert = -1;
	int currentIndex = -1;
	for (int h = 0; h < RECT_VERTICE_H; h++)
	{
		for (int w = 0; w < RECT_VERTICE_W; w++)
		{
			//========= 6 vertices to form one sub-rectangle

			//1st vertex
			int vertex1 = ++currentVert;
			mRectVertices[vertex1 * 3] = -halfWidth + (float)(w) / (float)RECT_VERTICE_W * width;
			mRectVertices[vertex1 * 3 + 1] = -halfHeight + (float)(h) / (float)RECT_VERTICE_H * height;
			mRectVertices[vertex1 * 3 + 2] = 0.0f;
			mRectUV[vertex1 * 2] = (float)(w) / (float)RECT_VERTICE_W * texMul;
			mRectUV[vertex1 * 2 + 1] = (float)(h) / (float)RECT_VERTICE_H* texMul;

			//2nd vertex
			int vertex2 = ++currentVert;
			mRectVertices[vertex2 * 3] = -halfWidth + (float)(w) / (float)RECT_VERTICE_W * width;
			mRectVertices[vertex2 * 3 + 1] = -halfHeight + (float)(h + 1) / (float)RECT_VERTICE_H * height;
			mRectVertices[vertex2 * 3 + 2] = 0.0f;
			mRectUV[vertex2 * 2] = (float)(w) / (float)RECT_VERTICE_W* texMul;
			mRectUV[vertex2 * 2 + 1] = (float)(h + 1) / (float)RECT_VERTICE_H* texMul;


			//3rd vertex
			int vertex3 = ++currentVert;
			mRectVertices[vertex3 * 3] = -halfWidth + (float)(w + 1) / (float)RECT_VERTICE_W * width;
			mRectVertices[vertex3 * 3 + 1] = -halfHeight + (float)(h + 1) / (float)RECT_VERTICE_H * height;
			mRectVertices[vertex3 * 3 + 2] = 0.0f;
			mRectUV[vertex3 * 2] = (float)(w + 1) / (float)RECT_VERTICE_W * texMul;
			mRectUV[vertex3 * 2 + 1] = (float)(h + 1) / (float)RECT_VERTICE_H * texMul;


			//4th vertex
			int vertex4 = ++currentVert;
			mRectVertices[vertex4 * 3] = mRectVertices[vertex3 * 3];
			mRectVertices[vertex4 * 3 + 1] = mRectVertices[vertex3 * 3 + 1];
			mRectVertices[vertex4 * 3 + 2] = mRectVertices[vertex3 * 3 + 2];
			mRectUV[vertex4 * 2] = mRectUV[vertex3 * 2];
			mRectUV[vertex4 * 2 + 1] = mRectUV[vertex3 * 2 + 1];


			//5th vertex
			int vertex5 = ++currentVert;
			mRectVertices[vertex5 * 3] = -halfWidth + (float)(w + 1) / (float)RECT_VERTICE_W * width;
			mRectVertices[vertex5 * 3 + 1] = -halfHeight + (float)(h) / (float)RECT_VERTICE_H * height;
			mRectVertices[vertex5 * 3 + 2] = 0.0f;
			mRectUV[vertex5 * 2] = (float)(w + 1) / (float)RECT_VERTICE_W * texMul;
			mRectUV[vertex5 * 2 + 1] = (float)(h) / (float)RECT_VERTICE_H * texMul;

			//6th vertex
			int vertex6 = ++currentVert;
			mRectVertices[vertex6 * 3] = mRectVertices[vertex1 * 3];
			mRectVertices[vertex6 * 3 + 1] = mRectVertices[vertex1 * 3 + 1];
			mRectVertices[vertex6 * 3 + 2] = mRectVertices[vertex1 * 3 + 2];
			mRectUV[vertex6 * 2] = mRectUV[vertex1 * 2];
			mRectUV[vertex6 * 2 + 1] = mRectUV[vertex1 * 2 + 1];
		}
	}
}



static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

GLuint LoadShader(GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;
   
   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
   	return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );
   
   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled ) 
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
		 char infoLog[4096];
         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         printf ( "Error compiling shader:\n%s\n", infoLog );            
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;
}

GLuint LoadShaderFromFile(GLenum shaderType, std::string path)
{
    GLuint shaderID = 0;
    std::string shaderString;
    std::ifstream sourceFile( path.c_str() );

    if( sourceFile )
    {
        shaderString.assign( ( std::istreambuf_iterator< char >( sourceFile ) ), std::istreambuf_iterator< char >() );
        const GLchar* shaderSource = shaderString.c_str();

		return LoadShader(shaderType, shaderSource);
    }
    else
        printf( "Unable to open file %s\n", path.c_str() );

    return shaderID;
}


void loadTexture(const char* path, GLuint textureID)
{
	CBitmap bitmap(path);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 

	// bilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.GetWidth(), bitmap.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap.GetBits());
}

int Init ( void )
{
   GLuint vertexShader;
   GLuint fragmentShader;
   GLuint programObject;
   GLint linked;

   //load textures
   glGenTextures(TEXTURE_COUNT, GtextureID);
   loadTexture("../media/rocks.bmp", GtextureID[0]);
   loadTexture("../media/glass.bmp", GtextureID[1]);
   loadTexture("../media/win8.bmp", GtextureID[2]);
   loadTexture("../media/fury_nano2.bmp", GtextureID[3]);
   loadTexture("../media/noise.bmp", GtextureID[4]);
   loadTexture("../media/radial.bmp", GtextureID[5]);
   //====

   genPlane();

   //==================== set up frame buffer, render buffer, and create an empty texture for blurring purpose
   // Create a new FBO
   glGenFramebuffers(1, &Gframebuffer);
   
   // Create a new empty texture
   glGenTextures(1, &GtextureBG);
   glBindTexture(GL_TEXTURE_2D, GtextureBG);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GtexBGWidth, GtexBGHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


   glGenTextures(1, &GdepthTextureBG);
   glBindTexture(GL_TEXTURE_2D, GdepthTextureBG);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, GtexBGWidth, GtexBGHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);
   glBindTexture(GL_TEXTURE_2D, 0);


   fragmentShader = LoadShaderFromFile(GL_VERTEX_SHADER, "../vertexShader1.vert" );
   vertexShader = LoadShaderFromFile(GL_FRAGMENT_SHADER, "../fragmentShader1.frag" );

   // Create the program object
   programObject = glCreateProgram ( );
   
   if ( programObject == 0 )
      return 0;

   glAttachShader ( programObject, vertexShader );
   glAttachShader ( programObject, fragmentShader );

   glBindAttribLocation ( programObject, 0, "vPosition" );
   glBindAttribLocation ( programObject, 1, "vColor" );
   glBindAttribLocation ( programObject, 2, "vTexCoord" );

   // Link the program
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked ) 
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
		 char infoLog[1024];
         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         printf ( "Error linking program:\n%s\n", infoLog );            
      }

      glDeleteProgram ( programObject );
      return 0;
   }

   // Store the program object
   GprogramID = programObject;


   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   //initialize matrices
   gPerspectiveMatrix = Matrix4::perspective(60.0f,
											(float)WINDOW_WIDTH/(float)WINDOW_HEIGHT,
                                             0.5f, 400.0f);
  // gOthorMatrix = Matrix4::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.5f, 30.0f);

   gViewMatrix = Matrix4::translate(Vector3(0.0f, 0.0f, -15.0f));


   return 1;
}


void UpdateCamera(void)
{
	static float yaw = 0.0f;
	static float pitch = 0.0f;
	static float distance = 5.5f;

	if(glfwGetKey(window, 'A')) pitch -= 1.0f;
	if(glfwGetKey(window, 'D')) pitch += 1.0f;
	if(glfwGetKey(window, 'W')) yaw -= 1.0f;
	if(glfwGetKey(window, 'S')) yaw += 1.0f;

	if(glfwGetKey(window, 'R'))
	{
		distance -= 0.06f;
		if(distance < 1.0f)
			distance = 1.0f;
	}
	if(glfwGetKey(window, 'F')) distance += 0.06f;

	gViewMatrix =	Matrix4::translate(Vector3(0.0f, 0.0f, -distance)) * 
					Matrix4::rotate(yaw, Vector3(1.0f, 0.0f, 0.0f)) *
					Matrix4::rotate(pitch, Vector3(0.0f, 1.0f, 0.0f));
}

void DrawSquare(GLuint texture)
{
    static GLfloat vVertices[] = {-1.0f,  1.0f, 0.0f,
								-1.0f, -1.0f, 0.0f,
								1.0f, -1.0f,  0.0f,
								1.0f,  -1.0f, 0.0f,
								1.0f, 1.0f, 0.0f,
								-1.0f, 1.0f,  0.0f};
					 

   static GLfloat vColors[] = {1.0f,  0.0f, 0.0f, 1.0f,
								0.0f, 1.0f, 0.0f, 1.0f,
								0.0f, 0.0f,  1.0f, 1.0f,
								0.0f,  0.0f, 1.0f, 1.0f,
								1.0f, 1.0f, 0.0f, 1.0f,
								1.0f, 0.0f,  0.0f, 1.0f};

   static GLfloat vTexCoords[] = {0.0f,  1.0f,
									0.0f, 0.0f,
									1.0f, 0.0f,
									1.0f,  0.0f,
									1.0f, 1.0f,
									0.0f, 1.0f};


   glBindTexture(GL_TEXTURE_2D, texture);


   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, vColors);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, vTexCoords);

   glEnableVertexAttribArray(0);
   glEnableVertexAttribArray(1);
   glEnableVertexAttribArray(2);


   glDrawArrays(GL_TRIANGLES, 0, 6);


   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
   glDisableVertexAttribArray(2);
}

void Draw(void)
{	
	// Use the program object, it's possible that you have multiple shader programs and switch it accordingly
    glUseProgram(GprogramID);

	// Set the sampler2D varying variable to the first texture unit(index 0)
	glUniform1i(glGetUniformLocation(GprogramID, "sampler2d"), 0);

	static float factor1 = 0.0f;
	factor1 += 0.01f;
	glUniform1f(glGetUniformLocation(GprogramID, "ufactor1"), factor1);

	// ======== Pass texture size to shader
    glUniform1f(glGetUniformLocation(GprogramID, "windowW"), (GLfloat)WINDOW_WIDTH);
    glUniform1f(glGetUniformLocation(GprogramID, "windowH"), (GLfloat)WINDOW_HEIGHT);
    // ========


	UpdateCamera();


	//======================= Bind the framebuffer and render BG to texture
	glBindFramebuffer(GL_FRAMEBUFFER, Gframebuffer);
	glBindTexture (GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GtextureBG, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, GdepthTextureBG, 0);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	Matrix4 modelMatrix, mvpMatrix;
	if (status == GL_FRAMEBUFFER_COMPLETE)
	{
		// Set the viewport
		glViewport(0, 0, GtexBGWidth, GtexBGHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		modelMatrix =
					Matrix4::translate(Vector3(0.0f, 0.0f, -3.0f)) *
					Matrix4::scale(Vector3(4.0f, 4.0f, 4.0f)) *
					Matrix4::rotate(0, Vector3(0.0f, 1.0f, 0.0f));
		mvpMatrix = gPerspectiveMatrix * gViewMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(GprogramID, "uMvpMatrix"), 1, GL_FALSE, mvpMatrix.data);
		glUniform1i(glGetUniformLocation(GprogramID, "myFlag1"), 0);
		DrawSquare(GtextureID[3]);

		modelMatrix =
			Matrix4::translate(Vector3(-4.0f, 0.0f, 4.0f)) *
			Matrix4::scale(Vector3(3.0f, 3.0f, 3.0f)) *
			Matrix4::rotate(0, Vector3(0.0f, 1.0f, 0.0f));
		mvpMatrix = gPerspectiveMatrix * gViewMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(GprogramID, "uMvpMatrix"), 1, GL_FALSE, mvpMatrix.data);
		DrawSquare(GtextureID[3]);
	}
	else
	{
		printf("Frame buffer is not ready!\n");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
			printf("1\n");
		else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS)
			printf("2\n");
		else if (status == GL_FRAMEBUFFER_UNSUPPORTED)
			printf("3\n");
	}
	//=================================================
	

	//======================= Set active framebuffer to default system buffer and draw the BG texture(always follow camera)
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUniformMatrix4fv(glGetUniformLocation(GprogramID, "uMvpMatrix"), 1, GL_FALSE, Matrix4::identity().data);
	glUniform1i(glGetUniformLocation(GprogramID, "myFlag1"), 0);
	//glDepthMask(false); 
	DrawSquare(GtextureBG);
//	glDepthMask(true);
	//=============================================


	//======================= Draw the transparent object
	glDepthFunc(GL_ALWAYS);

	modelMatrix =
		Matrix4::translate(Vector3(0.0f, 0.0f, 1.0f)) *
		Matrix4::scale(Vector3(2.0f, 2.0f, 2.0f)) *
		Matrix4::rotate(0, Vector3(0.0f, 1.0f, 0.0f));
	Matrix4 modelViewMatrix = gViewMatrix * modelMatrix;
	// Billboarding
	// 1st column
	/*modelViewMatrix[0][0] = 1.0;
	modelViewMatrix[0][1] = 0.0;
	modelViewMatrix[0][2] = 0.0;
	// 2nd column
	modelViewMatrix[1][0] = 0.0;
	modelViewMatrix[1][1] = 1.0;
	modelViewMatrix[1][2] = 0.0;
	// 3rd column
	modelViewMatrix[2][0] = 0.0;
	modelViewMatrix[2][1] = 0.0;
	modelViewMatrix[2][2] = 1.0;
	*/
	modelViewMatrix.data[0] = 1.0;
	modelViewMatrix.data[1] = 0.0;
	modelViewMatrix.data[2] = 0.0;
	// 2nd column
	modelViewMatrix.data[4] = 0.0;
	modelViewMatrix.data[5] = 1.0;
	modelViewMatrix.data[6] = 0.0;
	// 3rd column
	modelViewMatrix.data[8] = 0.0;
	modelViewMatrix.data[9] = 0.0;
	modelViewMatrix.data[10] = 1.0;
	//=================

	mvpMatrix = gPerspectiveMatrix * modelViewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(GprogramID, "uMvpMatrix"), 1, GL_FALSE, mvpMatrix.data);
	glUniform1i(glGetUniformLocation(GprogramID, "myFlag1"), 1);

	glActiveTexture(GL_TEXTURE0 + 0); // Texture unit 0
	glBindTexture(GL_TEXTURE_2D, GtextureID[1]);
	glActiveTexture(GL_TEXTURE0 + 1); // Texture unit 1
	glBindTexture(GL_TEXTURE_2D, GtextureBG);
	glActiveTexture(GL_TEXTURE0 + 2); 
	glBindTexture(GL_TEXTURE_2D, GdepthTextureBG);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, GtextureID[5]);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, GtextureID[6]);
	glUniform1i(glGetUniformLocation(GprogramID, "sampler2d"), 0);
	glUniform1i(glGetUniformLocation(GprogramID, "bgTexture"), 1);
	glUniform1i(glGetUniformLocation(GprogramID, "bgDepthTexture"), 2);
	glUniform1i(glGetUniformLocation(GprogramID, "noiseTexture"), 3);
	glUniform1i(glGetUniformLocation(GprogramID, "radialTexture"), 4);
	glActiveTexture(GL_TEXTURE0);

	//DrawSquare(GtextureID[1]);


	glBindTexture(GL_TEXTURE_2D, GtextureID[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, mRectVertices);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, mRectUV);

	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	
	glDrawArrays(GL_TRIANGLES, 0, RECT_VERTEX_COUNT);

	//wireframe test
	/* for(int i = 0; i < RECT_VERTEX_COUNT; i+=3)
		glDrawArrays(GL_LINE_LOOP, i, 3);*/

	glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glDepthFunc(GL_LESS);
	//=============================================
}

int main(void)
{
  glfwSetErrorCallback(error_callback);


  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Create and open a window
  window = glfwCreateWindow(WINDOW_WIDTH,
                            WINDOW_HEIGHT,
                            "Hello World",
                            NULL,
                            NULL);

  if(!window)
  {
    glfwTerminate();
    printf("glfwCreateWindow Error\n");
    exit(1);
  }

  glfwMakeContextCurrent(window);

  Init();

  // Repeat
  while(!glfwWindowShouldClose(window))
  {
	  Draw();
	  glfwSwapBuffers(window);
	  glfwPollEvents();

	  if(glfwGetKey(window, GLFW_KEY_ESCAPE))
			break;
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
