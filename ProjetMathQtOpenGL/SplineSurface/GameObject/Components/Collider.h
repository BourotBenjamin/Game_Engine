#pragma once
#include <vec4.hpp>
#include "Transform.h"
#include "RigidBody.h"

class GameObject{};

class Collider
{
public:
	Collider(Transform& transform, RigidBody& rigidBody);
	virtual void update() = 0;
	~Collider();
protected:
	glm::vec4 offset;
	Transform& transform;
	RigidBody& rigidBody;
};

