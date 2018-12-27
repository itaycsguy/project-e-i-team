#pragma once
#include "MeshModel.h"
#include <memory>
#include <glm/glm.hpp>



class ParallelLight : public MeshModel
{
private:
	glm::vec3 from;
	glm::vec3 to;
public:
	float Ld, Ls;
	ParallelLight(glm::vec3 to);
	~ParallelLight();
	glm::vec3 GetToVector();
	glm::vec3 GetfromVector();
	glm::vec3 GetLocationAfterTrans();
	glm::vec3 GetDirectionAfterTrans();
};
