#pragma once
#include <glm.hpp>
#include "../../Mat4x4.h"
#include <gtc/quaternion.hpp>

class Transform
{
public:
	Transform();
	~Transform();
	void move(const glm::vec4& offset);
	Mat4x4 getMatrix();
private:
	glm::vec4 position;
	glm::vec4 scale;
	glm::vec4 rotation;
};

