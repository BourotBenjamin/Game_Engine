#pragma once
#include "Collider.h"

class SphereCollider : Collider
{
public:
	SphereCollider(Transform& transform, RigidBody& r, World& world);
	void update();
	~SphereCollider();
private:
	float radius;
	void SphereCollider::collide(const SphereCollider* collider);
};

