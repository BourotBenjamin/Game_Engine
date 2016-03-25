#include "Collider.h"


Collider::Collider(Transform* transform, RigidBody* rigidBody, World* world) : transform(transform), rigidBody(rigidBody), world(world)
{
}


Collider::~Collider()
{
}

void Collider::updateAll(void* begin, int size)
{
	Collider* ptr = (Collider*)begin;
	for (int i = 0; i < size; i++)
	{
		ptr->update();
		ptr++;
	}
}
