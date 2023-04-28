#include "pch.h"
#include "Transform.h"

using namespace NarakaKarEngine;

//ToDo: Wrap in Math class Global variable not good
const float toRadians = static_cast<float>(M_PI) / 180.0f;

void Transform::Translate(glm::vec3&& position)
{
	this->position = std::move(position);
}

void Transform::RotateOnAxis(float&& angleInDegrees, glm::vec3&& axis)
{
	quaternion = glm::rotate(quaternion, angleInDegrees, axis);
	eulerAngles = glm::eulerAngles(quaternion) * toRadians;
}

void Transform::Scale(glm::vec3&& scale)
{
	this->scale = std::move(scale);
}

void Transform::UpdateModelMatrix()
{
	model_matrix = glm::translate(model_matrix, position);
	model_matrix *= glm::toMat4(quaternion);
	model_matrix = glm::scale(model_matrix, scale);
}