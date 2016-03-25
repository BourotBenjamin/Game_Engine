#pragma once
#include <vec4.hpp>
#include "Transform.h"
#include "RigidBody.h"
#include "../../World.h"

class GameObject;

class Collider
{
public:
	Collider(Transform& transform, RigidBody& rigidBody, World& world);
	virtual void update() = 0;
	static void Collider::updateAll(Collider* begin, int size);
	~Collider();
protected:
	glm::vec4 offset;
	Transform& transform;
	RigidBody& rigidBody;
	World& world;
};

