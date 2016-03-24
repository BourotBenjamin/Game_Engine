#pragma once
#include <vec4.hpp>
#include "Transform.h"
class RigidBody
{
public:
	RigidBody(Transform& transform);
	void update();
	~RigidBody();
private:
	glm::vec4 velocity;
	Transform& transform;
};

