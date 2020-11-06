#include "MeshModel.h"

MeshModel::MeshModel(std::vector<Face> faces, std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, const std::string& model_name) :
	faces_(faces),
	vertices_(vertices),
	normals_(normals),
	model_name_(model_name)
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

