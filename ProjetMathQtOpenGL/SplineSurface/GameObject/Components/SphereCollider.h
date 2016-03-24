#pragma once
#include "Collider.h"

class SphereCollider : Collider
{
public:
	SphereCollider(Transform& transform, RigidBody& r);
	void update();
	~SphereCollider();
private:
	float radius;
	void SphereCollider::collide(const SphereCollider* collider);
};

