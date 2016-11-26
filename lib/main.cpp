#include <GL/freeglut.h>
#include <stdio.h>
#include <string>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#define HEIGHT 1024  // hight size
#define WIDTH 800    // width size




float ex = 0, ey = 0, ez = 20, cx = 0, cy = 0, cz = -1; 

double rotate_y=0; 
double rotate_x=0;


void objloader(const char *filename)
{
   Assimp::Importer importer;
   const aiScene *scene =  importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Fast);
   aiMesh *mesh = scene->mMeshes[0];
   float *vertexArray;
   float *normalArray;
   float *uvArray;

   int numVerts;

   numVerts = mesh->mNumFaces*3;

   vertexArray = new float[mesh->mNumFaces*3*3];
   normalArray = new float[mesh->mNumFaces*3*3];
   uvArray = new float[mesh->mNumFaces*3*2];

   for(unsigned int i=0;i<mesh->mNumFaces;i++)
   {
      const aiFace& face = mesh->mFaces[i];

      for(int j=0;j<3;j++)
      {
         aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[j]];
         memcpy(uvArray,&uv,sizeof(float)*2);
         uvArray+=2;

         aiVector3D normal = mesh->mNormals[face.mIndices[j]];
         memcpy(normalArray,&normal,sizeof(float)*3);
         normalArray+=3;

         aiVector3D pos = mesh->mVertices[face.mIndices[j]];
         memcpy(vertexArray,&pos,sizeof(float)*3);
         vertexArray+=3;
      }
   }

   uvArray-=mesh->mNumFaces*3*2;
   normalArray-=mesh->mNumFaces*3*3;
   vertexArray-=mesh->mNumFaces*3*3;

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
   glVertexPointer(3,GL_FLOAT,0,vertexArray);
   glNormalPointer(GL_FLOAT,0,normalArray);
    
   glClientActiveTexture(GL_TEXTURE0_ARB);
   glTexCoordPointer(2,GL_FLOAT,0,uvArray);
    
   glDrawArrays(GL_TRIANGLES,0,numVerts);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}


void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;

   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);

   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

GLuint loadTextures(const char *filename)
{
   GLuint texture;
   int height = 256;
   int width = 256;
   unsigned char *data;
   FILE *file;

   file = fopen(filename, "rb");

   if(file == NULL)
   {
      printf("Error opening a file.\n");
      return 0;
   }

   data = (unsigned char *)malloc( height * width * 3 );
   fread(data, height * width * 3, 1, file);
   fclose(file);

   for(int i = 0; i < width * height; i++)
   {
      int index = i*3;
      unsigned char B,R;
      B = data[index];
      R = data[index+2];

      data[index] = R;
      data[index+2] = B;

   }

   glGenTextures(1, &texture);
   glBindTexture( GL_TEXTURE_2D, texture);

   glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );


   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
   glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
   gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
   free( data );

   return texture;
}

void initGL() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClearDepth(1.0f);                   // Set background depth to farthest
   glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
   glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

void displayScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // clearing the frame buffer and depth buffer
   glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
   glLoadIdentity();
   gluLookAt(ex, ey, ez, 0, 0, 0, 0, 1, 0);
   glTranslatef(0.0f,0.0f,-5.0);                      // Move Into The Screen 5 Units

   

   glRotatef( rotate_x, 1.0, 0.0, 0.0 );
   glRotatef( rotate_y, 0.0, 1.0, 0.0 );
   glEnable(GL_TEXTURE_2D);
   GLuint texture;
   texture = loadTextures("Textures/marble.bmp");
   glBindTexture(GL_TEXTURE_2D, texture);

   glBegin(GL_QUADS);
      // Front Face
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
      // Back Face
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
      // Top Face
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
      // Bottom Face
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
      // Right face
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
      // Left Face
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
   glEnd();

   glTranslatef(2.0f,1.0f,-3.0);                      // Move Into The Screen 5 Units
   texture = loadTextures("Textures/pattern.bmp");
   glBindTexture(GL_TEXTURE_2D, texture);

   glBegin(GL_QUADS);
      // Front Face
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
      // Back Face
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
      // Top Face
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
      // Bottom Face
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
      // Right face
      glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
      glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
      // Left Face
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
   glEnd();

   objloader("character_walkingupstairs/character1.0001.obj");

   glFlush();
   glutSwapBuffers();

}


void specialKeys( int key, int x, int y ) {

  //  Right arrow - increase rotation by 5 degree
	if (key == GLUT_KEY_RIGHT)
		rotate_y += 5;

  //  Left arrow - decrease rotation by 5 degree
	else if (key == GLUT_KEY_LEFT)
		rotate_y -= 5;

	else if (key == GLUT_KEY_UP)
		rotate_x += 5;

	else if (key == GLUT_KEY_DOWN)
		rotate_x -= 5;

  //  Request display update
	glutPostRedisplay();

}

int main(int argc, char** argv)
{    
   glutInit(&argc, argv);		                                   // Initializes GLUT (The OpenGL Utility Toolkit)
   glutInitWindowSize(HEIGHT, WIDTH);   				              // ********************
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);    // Enables double buffer mode
   //glutInitWindowPosition(100, 100);    			                 //	Setting the window hight,width and title
   glutCreateWindow("Drawing Cube"); 				                 // ********************

   glEnable(GL_DEPTH_TEST);
   glutDisplayFunc(displayScene);
   glutSpecialFunc(specialKeys);
   glutReshapeFunc(reshape);       // Register callback handler for window re-size event
   initGL();
   glutMainLoop();  

   return 0;
}




