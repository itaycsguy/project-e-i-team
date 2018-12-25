#pragma once
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <memory>
#include "Face.h"
#include <stdio.h>
#include <time.h>
#include "Trans.h"

using namespace std;

#define MESH_MODEL_TYPE 0
#define CAMERA_MODEL_TYPE 1

#define POS_DOUBLE_PI 2.2f*M_PI

#define MAX_NORMAL_LENGTH 100.0f
#define MIN_NORMAL_LENGTH 50.0f

#define MAX_SCALE_FACTOR 100.0f
#define MIN_SCALE_FACTOR 1.0f
#define SCALE_OBJ_FACTOR 25.0f
#define CAMERA_BASIC_SIZE 100.0f // it is as big as we can achieve for this moment

#define MAX_TRANSLATION_LENGTH 100.0f
#define MIN_TRANSLATION_LENGTH -100.0f

// smooth moving:
#define XTRANS_FACTOR 20.0f
#define YTRANS_FACTOR 20.0f
#define BLACK_COLOR_LINE glm::vec4(0.0f, 0.0f, 0.0f,1.0f)

#define AMBIENT				0 
#define DIFFUSE				1
#define SPECULAR			2
#define PHONG_ILLUMINATION	3
#define PARALLEL_LIGHT      4
#define POINT_LIGHT      5


static glm::vec4 VERTEX_NORMAL_COLOR = glm::vec4(1.0f, 0.0f, 0.0f, 1.00f); // (r,g,b)
static glm::vec4 FACE_NORMAL_COLOR = glm::vec4(0.0f, 0.0f, 1.0f, 1.00f); // (r,g,b)
static glm::vec4 BOUNDING_BOX_COLOR = glm::vec4(0.0625f, 0.433f, 0.050f, 1.00f); // (r,g,b)

/*
getRandColor() success depend on srand(time(NULL)) in the top on main function
*/
static glm::vec3* getRandColor() {
	int r = rand() % 256;
	int g = rand() % 256;
	int b = rand() % 256;
	glm::vec3* color = &glm::vec3(r, g, b);
	//std::cout << "(R=" << r << ",G=" << g << ",B=" << b << ")" << std::endl;
	return color;
}

/*
 * MeshModel class.
 * This class represents a mesh model (with faces and normals informations).
 * You can use Utils::LoadMeshModel to create instances of this class from .obj files.
 */
class MeshModel
{
private:
	std::vector<Face> faces;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4x4 worldTransform;
	std::string modelName;
public:
	glm::vec3 BoundMin,BoundMax,BoundMiddle,color,BoundingBoxColor;
	bool showFaceNormals,showVertexNormals,showBoundingBox;
	glm::vec4 fNcolor,vNcolor;
	float fScale, fRotatex, fRotatey, fRotatez, wfScale, wfRotatex, wfRotatey, wfRotatez;
	float fTranslatex, fTranslatey, fTranslatez;
	float fNlength, vNlength;
	glm::vec3 ambientColor, diffuseColor, specularColor;
	float Ka, Kd, Ks, alpha;			// reflected ray of the mesh model it-self
	int lightType;

	MeshModel(){}
	MeshModel(const std::vector<Face>& faces, const std::vector<glm::vec3>& vertices,const std::vector<glm::vec3>& normals, glm::vec3 BoundMin, glm::vec3 BoundMax, glm::vec3 BoundMiddle,const std::string& modelName = "");
	MeshModel(std::shared_ptr<MeshModel> model, float defsize = CAMERA_BASIC_SIZE,bool showFNormals = false, bool showVNormals = false);
	virtual ~MeshModel();
	void SetWorldTransformation(const glm::mat4x4& worldTransform);
	const glm::mat4x4& GetWorldTransformation() const;
	void SetColor(const glm::vec4& color);
	std::vector<glm::vec3> MeshModel::GetNormals();
	std::vector<Face> MeshModel::GetFaces();
	std::vector<glm::vec3> MeshModel::GetVertices();
	std::string MeshModel::GetModelName();
	void MeshModel::UpdateworldTransform(glm::mat4x4 T);
	void MeshModel::UpdateLeftworldTransform(glm::mat4x4 T);
	void MeshModel::resetModel(float fScaleDef = SCALE_OBJ_FACTOR,
		bool showFNormals = true,
		bool showVNormals = true,
		bool isPointLight = false,
		glm::vec4 vcolorDef = VERTEX_NORMAL_COLOR,
		glm::vec4 fcolorDef = FACE_NORMAL_COLOR,
		glm::vec3* modelColor = getRandColor(),
		float vertexNlength = MAX_NORMAL_LENGTH,
		float faceNlength = MAX_NORMAL_LENGTH);
	glm::vec3 MeshModel::GetVerticeByIndex(int index);
	void MeshModel::SetFaceNormalLength(float length);
	void MeshModel::SetFaceNormalColor(glm::vec4 color);
	float MeshModel::GetFaceNormalLength();
	glm::vec4 MeshModel::GetFaceNormalColor();
	bool MeshModel::GetFaceNormalView();
	void MeshModel::SetFaceNormalView(bool NeedShowNormals);
	void MeshModel::SetVertexNormalView(bool NeedShowNormals);
	bool MeshModel::GetVertexNormalView();
	void MeshModel::SetVertexNormalLength(float length);
	void MeshModel::SetVertexNormalColor(glm::vec4 color);
	float MeshModel::GetVertexNormalLength();
	glm::vec4 MeshModel::GetVertexNormalColor();
	glm::vec3 GetModelLocationAfterTrans();
};
