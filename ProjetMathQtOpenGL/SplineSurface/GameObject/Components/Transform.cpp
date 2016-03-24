#include "Transform.h"


Transform::Transform()
{
}


Transform::~Transform()
{
}


void Transform::move(const glm::vec4& offset)
{
	this->position += offset;
}


Mat4x4 Transform::getMatrix()
{
	return Mat4x4::translate(this->position) * Mat4x4::rotate(this->rotation) * Mat4x4::scale(this->scale);
}