#include "Collider.h"


Collider::Collider(Transform& transform, RigidBody& rigidBody) : transform(transform), rigidBody(rigidBody)
{
}


Collider::~Collider()
{
}
