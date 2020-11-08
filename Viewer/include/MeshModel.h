#pragma once
#include <glm/glm.hpp>
#include <string>
#include "Face.h"

class MeshModel
{
public:
	MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name);
	virtual ~MeshModel();
	const Face& GetFace(int index) const;
	int GetFacesCount() const;
	const std::string& GetModelName() const;
	int MeshModel::GetVerticesCount() const;
	const glm::vec3& MeshModel::GetVertex(int index) const;
	float MeshModel::GetScaleFactor();
	void MeshModel::SetScaleFactor(float scaleFactor);
	void MeshModel::SetRotateAngle(float rotateAngle);
	float MeshModel::GetRotateAngle();

	

private:
	std::vector<Face> faces_;
	std::vector<glm::vec3> vertices_;
	std::vector<glm::vec3> normals_;
	std::string model_name_;

	float scaleFactor_;
	float rotateAngle_; //90 
};
