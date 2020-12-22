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
	void MeshModel::SetRotateAngle(glm::vec3 rotateAngle);
	glm::vec3 MeshModel::GetRotateAngle();
	void MeshModel::SetNewPosition(glm::vec3 newPos);
	glm::vec3 MeshModel::GetPosition();

	void SetFaceNormalShown(bool isShown);
	bool GetFaceNormalShown();
	void SetVertexNormalShown(bool isShown);
	bool GetVertexNormalShown();

	void SetMinMax(glm::vec4 minMaxVector);
	glm::vec4 GetMinMax();

	const glm::vec3& MeshModel::GetVertexNormal(int index) const;

	void SetBoundingBoxShown(bool isShown);

	bool GetBoundingBoxShown();

	void SetColorOfMesh(glm::vec3 colorOfMesh);
	glm::vec3 GetColorOfMesh();
	void SetAmbient(float ambient);
	float GetAmbient();
	void SetDiffuse(float diffuse);
	float GetDiffuse();
	void SetSpecular(float specular);
	float GetSpecular();
	void SetLightModel(int lightModel);
	float GetLightModel();

	int kindOfModel; // 0 - model, 1 - point, 2 - parallel 
	bool isLightRotating_;

private:
	std::vector<Face> faces_;
	std::vector<glm::vec3> vertices_;
	std::vector<glm::vec3> normals_;
	std::string model_name_;

	float scaleFactor_;
	glm::vec3 rotateAngle_;
	glm::vec3 position_;

	glm::vec3 colorOfMesh_;

	bool isFaceNormalShown_;
	bool isVertexNormalShown_;
	bool isBoundingBoxShown_;

	int lightModel_;
	
	float ambient_;
	float diffuse_;
	float specular_;

	glm::vec4 minMaxXY_; // (max X, min X, max y, min y)
};
