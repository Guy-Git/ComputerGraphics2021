#include "MeshModel.h"

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces_(faces),
	vertices_(vertices),
	normals_(normals),
	model_name_(model_name),
	scaleFactor_(1),
	rotateAngle_(0),
	position_(0)
{

}

MeshModel::~MeshModel()
{

}

const Face& MeshModel::GetFace(int index) const
{
	return faces_[index];
}

int MeshModel::GetFacesCount() const
{
	return faces_.size();
}

const std::string& MeshModel::GetModelName() const
{
	return model_name_;
}

int MeshModel::GetVerticesCount() const
{
	return vertices_.size();
}

const glm::vec3& MeshModel::GetVertex(int index) const
{
	return vertices_.at(index);
}

void MeshModel::SetScaleFactor(float scaleFactor)
{
	MeshModel::scaleFactor_ = scaleFactor;
}

float MeshModel::GetScaleFactor()
{
	return MeshModel::scaleFactor_;
}

void MeshModel::SetRotateAngle(glm::vec3 rotateAngle)
{
	MeshModel::rotateAngle_ = rotateAngle;
}

glm::vec3 MeshModel::GetRotateAngle()
{
	return MeshModel::rotateAngle_;
}

void MeshModel::SetNewPosition(glm::vec3 newPos)
{
	position_ = newPos;
}

glm::vec3 MeshModel::GetPosition()
{
	return position_;
}

