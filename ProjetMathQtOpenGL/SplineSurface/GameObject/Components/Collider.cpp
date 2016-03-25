#include "Collider.h"


Collider::Collider(Transform& transform, RigidBody& rigidBody, World& world) : transform(transform), rigidBody(rigidBody), world(world)
{
}


Collider::~Collider()
{
}

void Collider::updateAll(Collider* begin, int size)
{
	for (int i = 0; i < size; i++)
	{
		begin->update();
		begin++;
	}
}
