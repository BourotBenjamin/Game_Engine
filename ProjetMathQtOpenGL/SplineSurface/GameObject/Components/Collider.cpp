#include "Collider.h"


Collider::Collider(Transform& transform, RigidBody& rigidBody, World& world) : transform(transform), rigidBody(rigidBody), world(world)
{
}


Collider::~Collider()
{
}
