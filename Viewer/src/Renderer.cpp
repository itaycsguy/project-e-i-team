#define _USE_MATH_DEFINES

#include "Renderer.h"
#include "InitShader.h"
#include "MeshModel.h"
#include <imgui/imgui.h>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <fenv.h>       /* fegetround, FE_* */
#include <math.h>       /* nearbyint */

using namespace std;

#define INDEX(width,x,y,c) ((x)+(y)*(width))*3+(c)

static float p1 = -50, q1 = 50;

static float p2 = -50, q2 = -100;

Renderer::Renderer(int viewportWidth, int viewportHeight, int viewportX, int viewportY) :
	colorBuffer(nullptr),
	zBuffer(nullptr)
{
	initOpenGLRendering();
	SetViewport(viewportWidth, viewportHeight, viewportX, viewportY);
}

Renderer::~Renderer()
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}
}

void Renderer::putPixel(int i, int j, const glm::vec3& color)
{
	if (i < 0) return; if (i >= viewportWidth) return;
	if (j < 0) return; if (j >= viewportHeight) return;
	colorBuffer[INDEX(viewportWidth, i, j, 0)] = color.x;
	colorBuffer[INDEX(viewportWidth, i, j, 1)] = color.y;
	colorBuffer[INDEX(viewportWidth, i, j, 2)] = color.z;
}

void Renderer::createBuffers(int viewportWidth, int viewportHeight)
{
	if (colorBuffer)
	{
		delete[] colorBuffer;
	}

	colorBuffer = new float[3* viewportWidth * viewportHeight];
	for (int x = 0; x < viewportWidth; x++)
	{
		for (int y = 0; y < viewportHeight; y++)
		{
			putPixel(x, y, glm::vec3(0.0f, 0.0f, 0.0f));
		}
	}
}

void Renderer::ClearColorBuffer(const glm::vec3& color)
{
	for (int i = 0; i < viewportWidth; i++)
	{
		for (int j = 0; j < viewportHeight; j++)
		{
			putPixel(i, j, color);
		}
	}
}

void Renderer::SetViewport(int viewportWidth, int viewportHeight, int viewportX, int viewportY)
{
	this->viewportX = viewportX;
	this->viewportY = viewportY;
	this->viewportWidth = viewportWidth;
	this->viewportHeight = viewportHeight;
	createBuffers(viewportWidth, viewportHeight);
	createOpenGLBuffer();
}


// Elias Function Implementation:
void Renderer::NaiveAlg(float p1, float p2, float q1, float q2, const glm::vec3& color)
{
	float delta_p = p2 - p1;
	float delta_q = q2 - q1;
	float m = delta_q / delta_p;
	float c = q1 - m * p1;
	
	float x,to,y;
	if (p1 < p2) {
		x = p1;
		to = p2;
	}
	else {
		x = p2;
		to = p1;
	}
	for (; x <= to; x++) {
		y = round(m*x + c);
		putPixel(x, 720-y, color);
	}
}

// Elias Function Implementation:
void Renderer::DrawLine(float p1, float p2, float q1, float q2, const glm::vec3& color) {
	float a = (q1 - q2) / (p1 - p2);

	if (a >= 0 && a <= 1) {
		if (p1 < p2) {
			BresenhamAlg(p1, p2, q1, q2, false, false, false, color);
		}
		else {
			BresenhamAlg(p2, p1, q2, q1, false, false, false, color);
		}
	}
	else if (a > 1) {
		if (q1 < q2) {
			BresenhamAlg(q1, q2, p1, p2, true, false, false, color);
		}
		else {
			BresenhamAlg(q2, q1, p2, p1, true, false, false, color);
		}
	}
	else if (a < 0 && a >= -1) {
		if (p1 < p2) {
			BresenhamAlg(p1, p2, q1, q2 + 2*(q1-q2), false, true, false, color);
		}
		else {
			BresenhamAlg(p2, p1, q2, q1 + 2 * (q2 - q1), false, true, false, color);
		}
	}
	else if (a < -1) {
		if (q1 < q2) {
			BresenhamAlg(q1, q2, p1, p2+2*(p1-p2), true, true, false, color);
		}
		else {
			BresenhamAlg(q1, q2 + 2 * (q1 - q2), p1, p2, true, false, true, color);
		}
	}

}

// Elias Function Implementation:
void Renderer::BresenhamAlg(float p1, float p2, float q1, float q2, bool switch_print, bool NegX, bool NegY, const glm::vec3& color) {
	float x, y, e;
	float delta_p = p2 - p1;
	float delta_q = q2 - q1;
	
	x = p1;
	y = q1;
	e = -delta_p;

	while (x <= p2) {
		if (e > 0) {

			if (NegX) {
				y--;
			}
			else {
				y++;
			}
			e = e - 2 * delta_p;
		}
		
		if (switch_print) {
			if (NegY) {
				putPixel((viewportWidth / 2) + y , (viewportHeight / 2) - x + 2*p1, color);
			}
			else {
				putPixel((viewportWidth / 2) + y, (viewportHeight / 2) + x,color);
			}
		}
		else {
			putPixel((viewportWidth / 2) + x, (viewportHeight / 2) + y, color);
		}
		
		x++;
		e = e + 2 * delta_q;
	}
}



void Renderer::Render(const Scene& scene, const ImGuiIO& io)
{
	//#############################################
	//## You should override this implementation ##
	//## Here you should render the scene.       ##
	//#############################################


	/*
	// Draw a chess board in the middle of the screen
	for (int i = 0; i < viewportWidth; i++)
	{
		for (int j = 0; j < viewportHeight; j++)
		{
			int mod_i = i / 50;
			int mod_j = j / 50;

			int odd = (mod_i + mod_j) % 2;
			if (odd)
			{
				putPixel(i, j, glm::vec3(0, 1, 0));
			}
			else
			{
				putPixel(i, j, glm::vec3(1, 0, 0));
			}
		}
	}
	*/


	//Get mouse position
	p2 = io.MousePos.x - (viewportWidth/2);
	q2 = (viewportHeight/2) - io.MousePos.y;


	
	//DrawLine(50, p2, 50, q2, glm::vec3(0, 0, 1));
	//DrawLine(-50, p2, 50, q2, glm::vec3(0, 1, 0));
	//DrawLine(50, p2, -50, q2, glm::vec3(1, 0, 0));
	//DrawLine(-50, p2, -50, q2, glm::vec3(0, 1, 1));

	cout << scene.GetModelCount() << "  :   ";
	cout << scene.GetActiveModelIndex() << endl;
	
<<<<<<< HEAD
	
	
	int modelCount = scene.GetModelCount();
	if (modelCount > 0) {

<<<<<<< HEAD
	if (scene.GetModelCount() > 0) {
		
		std::vector<Face> faces = scene.getModelFaces(0);
		
		
		
		for (auto i = faces.begin(); i != faces.end(); ++i)
		{
			
=======
		for (int m = 0; m < modelCount; m++) {
>>>>>>> 82f5db21e6e6c02afc5d86a5d6562a54390e249c

			std::vector<Face> faces = scene.getModelFaces(m);

			for (auto i = faces.begin(); i != faces.end(); i++)
			{
				int vertex0Index = i->GetVertexIndex(0);
				float x0 = scene.getModelVertices(0, vertex0Index).x;
				float y0 = scene.getModelVertices(0, vertex0Index).y;
				float z0 = scene.getModelVertices(0, vertex0Index).z;
				
				// Stucking HERE!!! with array out of bound exception - cannot catch it with try catch block!
				int vertex1Index = i->GetVertexIndex(1);
				float x1 = scene.getModelVertices(0, vertex1Index).x;
				float y1 = scene.getModelVertices(0, vertex1Index).y;
				float z1 = scene.getModelVertices(0, vertex1Index).z;
			
				int vertex2Index = i->GetVertexIndex(2);
				float x2 = scene.getModelVertices(0, vertex2Index).x;
				float y2 = scene.getModelVertices(0, vertex2Index).y;
				float z2 = scene.getModelVertices(0, vertex2Index).z;
			
				cout << "( " << x0 << " , " << y0 << " , " << z0 << " )  -  ";
				cout << "( " << x0 << " , " << y0 << " , " << z0 << " )  -  ";
				cout << "( " << x0 << " , " << y0 << " , " << z0 << " )" << endl;

				std::shared_ptr<MeshModel> model = scene.GetModel(m);
				glm::mat4x4 scaleMatrix = model->GetScaleWorldTransform();
				model->SetWorldTransformation(scaleMatrix.operator*=(model->GetWorldTransformation()));

				glm::vec3 blackColorVec = glm::vec3(0, 0, 0);
				DrawLine(x0, x1, y0, y1, blackColorVec);
				DrawLine(x0, x2, y0, y2, blackColorVec);
				DrawLine(x1, x2, y1, y2, blackColorVec);
			}
		}
	}
	

=======
>>>>>>> parent of 797ad31... load OBJ file


	//Draw X and Y axis lines
	DrawLine(-(viewportWidth/2), (viewportWidth / 2), 0, 0, glm::vec3(0, 0, 0));
	DrawLine(0, 0, (viewportHeight / 2), -(viewportHeight / 2), glm::vec3(0, 0, 0));
}

//##############################
//##OpenGL stuff. Don't touch.##
//##############################

// Basic tutorial on how opengl works:
// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/
// don't linger here for now, we will have a few tutorials about opengl later.
void Renderer::initOpenGLRendering()
{
	// Creates a unique identifier for an opengl texture.
	glGenTextures(1, &glScreenTex);

	// Same for vertex array object (VAO). VAO is a set of buffers that describe a renderable object.
	glGenVertexArrays(1, &glScreenVtc);

	GLuint buffer;

	// Makes this VAO the current one.
	glBindVertexArray(glScreenVtc);

	// Creates a unique identifier for a buffer.
	glGenBuffers(1, &buffer);

	// (-1, 1)____(1, 1)
	//	     |\  |
	//	     | \ | <--- The exture is drawn over two triangles that stretch over the screen.
	//	     |__\|
	// (-1,-1)    (1,-1)
	const GLfloat vtc[]={
		-1, -1,
		 1, -1,
		-1,  1,
		-1,  1,
		 1, -1,
		 1,  1
	};

	const GLfloat tex[]={
		0,0,
		1,0,
		0,1,
		0,1,
		1,0,
		1,1};

	// Makes this buffer the current one.
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	// This is the opengl way for doing malloc on the gpu. 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtc)+sizeof(tex), NULL, GL_STATIC_DRAW);

	// memcopy vtc to buffer[0,sizeof(vtc)-1]
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vtc), vtc);

	// memcopy tex to buffer[sizeof(vtc),sizeof(vtc)+sizeof(tex)]
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vtc), sizeof(tex), tex);

	// Loads and compiles a sheder.
	GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );

	// Make this program the current one.
	glUseProgram(program);

	// Tells the shader where to look for the vertex position data, and the data dimensions.
	GLint  vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition,2,GL_FLOAT,GL_FALSE,0,0 );

	// Same for texture coordinates data.
	GLint  vTexCoord = glGetAttribLocation( program, "vTexCoord" );
	glEnableVertexAttribArray( vTexCoord );
	glVertexAttribPointer( vTexCoord,2,GL_FLOAT,GL_FALSE,0,(GLvoid *)sizeof(vtc) );

	//glProgramUniform1i( program, glGetUniformLocation(program, "texture"), 0 );

	// Tells the shader to use GL_TEXTURE0 as the texture id.
	glUniform1i(glGetUniformLocation(program, "texture"),0);
}

void Renderer::createOpenGLBuffer()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// malloc for a texture on the gpu.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, viewportWidth, viewportHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glViewport(0, 0, viewportWidth, viewportHeight);
}

void Renderer::SwapBuffers()
{
	// Makes GL_TEXTURE0 the current active texture unit
	glActiveTexture(GL_TEXTURE0);

	// Makes glScreenTex (which was allocated earlier) the current texture.
	glBindTexture(GL_TEXTURE_2D, glScreenTex);

	// memcopy's colorBuffer into the gpu.
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewportWidth, viewportHeight, GL_RGB, GL_FLOAT, colorBuffer);

	// Tells opengl to use mipmapping
	glGenerateMipmap(GL_TEXTURE_2D);

	// Make glScreenVtc current VAO
	glBindVertexArray(glScreenVtc);

	// Finally renders the data.
	glDrawArrays(GL_TRIANGLES, 0, 6);
}