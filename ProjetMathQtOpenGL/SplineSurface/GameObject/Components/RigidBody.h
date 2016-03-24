#pragma once
#include <vec4.hpp>
#include "Transform.h"
class RigidBody
{
public:
	RigidBody(Transform& transform);
	void update();
	static void updateAll(RigidBody* begin, RigidBody* end);
	void collide(const glm::vec4& direction);
	~RigidBody();
private:
	glm::vec4 velocity;
	Transform& transform;
};

