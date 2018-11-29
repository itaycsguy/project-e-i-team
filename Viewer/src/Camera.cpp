#define _USE_MATH_DEFINES

#include "Trans.h"
#include "Camera.h"
#include "Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Camera::Camera(std::shared_ptr<MeshModel> model,const glm::vec4& eye, const glm::vec4& at, const glm::vec4& up) :
	viewTransformation(glm::mat4x4(1)),
	projectionTransformation(glm::mat4x4(1)),
	transType(0),
	ffovy(FFOVY_BASIC_UNIT), fnear(MIN_FNEAR), ffar(MIN_FFAR),
	worldfRotatex(0.0f), worldfRotatey(0.0f), worldfRotatez(0.0f),
	MeshModel(model)
{
	SetCameraLookAt(eye, at, up);
	origin_eye = glm::vec3(eye.x, eye.y, eye.z);
}

Camera::~Camera(){}

void Camera::SetCameraLookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up)
{
	glm::vec3 z = glm::normalize(eye - at);
	glm::vec3 x = glm::normalize(glm::cross(up,z));
	glm::vec3 y = glm::normalize(glm::cross(z,x));
	glm::mat4 mat(1);
	mat[0][0] = x.x; mat[0][1] = y.x; mat[0][2] = z.x; mat[3][0] = glm::dot(x, eye);
	mat[1][0] = x.y; mat[1][1] = y.y; mat[1][2] = z.y; mat[3][1] = glm::dot(y, eye);
	mat[2][0] = x.z; mat[2][1] = y.z; mat[2][2] = z.z; mat[3][2] = glm::dot(z, eye);

	viewTransformation = mat;
	SetWorldTransformation(glm::inverse(mat));
}


//Elias emplementation:
//aspectRatio = width / height
void Camera::SetOrthographicProjection(
	const float fovy, /* free degree */
	const float aspectRatio,
	const float snear,
	const float sfar,
	const float frameBufferWidth, 
	const float frameBufferHeight)
{
	/*
	*	This projection is about to project the 3D Model to some hyperplane as 2D - zoom in is steps along z-axis
	*/
	float top = tan(fovy / 2) * snear;
	float botton = -1 * top;
	float right = aspectRatio * top;
	float left = -right;

	float S_x = 2.0f / (right - left);
	float S_y = 2.0f / (top - botton);
	float S_z = 2.0f / (snear - sfar);
	float x = -1.0f * ((right + left) / (right - left));
	float y = -1.0f * ((top + botton) / (top - botton));
	float z = -1.0f * ((sfar + snear) / (sfar - snear));

	glm::vec4 v1 = glm::vec4(S_x, 0.0f, 0.0f, 0.0f);
	glm::vec4 v2 = glm::vec4(0 , S_y, 0.0f, 0.0f);
	glm::vec4 v3 = glm::vec4(0, 0, S_z, 0.0f);
	glm::vec4 v4 = glm::vec4(x, y, z, 1.0f);

	projectionTransformation = glm::mat4(v1 ,v2 ,v3 ,v4);
}

// Itay's Implementation
void Camera::SetPerspectiveProjection(
	const float fovy,
	const float aspectRatio,
	const float pnear,
	const float pfar,
	const float frameBufferWidth,
	const float frameBufferHeight)
{
	//float wleft = origin_eye[0] - (frameBufferWidth/2);
	//float wright = origin_eye[0] + (frameBufferWidth/2);
	//float wbottom = origin_eye[1] - (frameBufferHeight/2);
	//float wtop = origin_eye[1] + (frameBufferHeight/2);
	//float pneardef = 1;
	//float pfardef = -1;
	/*
	*	This projection is up to the gap between far hyperplane to near hyperplace which is parallel to y hyperplace
	*	=> cannot remain space to normals to be shown using very small gap [|near - far| < some epsilon]
	*/
	
	float top = tan(fovy / 2) * pnear;
	float botton = -1 * top;
	float right = aspectRatio * top;
	float left = -right;
	
	//float pneardef = 1,pfardef = -1;
	//float wright = 1.0f, wleft = -1.0f, wtop = -1.0f, wbottom = 1.0f;

	glm::vec4 v1 = glm::vec4((2.0f * pnear) / (right - left), 0.0f, 0.0f, 0.0f);


	glm::vec4 v2 = glm::vec4(0.0f, (2.0f * pnear) / (top - botton), 0.0f, 0.0f);



	glm::vec4 v3 = glm::vec4((right + left) / (right - left), (top + botton) / (top - botton), -(pfar + pnear) / (pfar - pnear), -1.0f);


	glm::vec4 v4 = glm::vec4(0.0f, 0.0f, -(2*pnear*pfar) / (pfar - pnear), 0.0f);
	
	
	glm::mat4x4 P = glm::mat4x4(v1,v2,v3,v4);

	projectionTransformation = P;
}