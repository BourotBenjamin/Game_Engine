#include "RigidBody.h"


RigidBody::RigidBody(Transform& t) :transform(t)
{
}


RigidBody::~RigidBody()
{
}

void RigidBody::update()
{
	this->transform.move(this->velocity);
}
