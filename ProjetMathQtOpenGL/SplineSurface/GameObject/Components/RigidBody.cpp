#include "RigidBody.h"


RigidBody::RigidBody(Transform& t) :transform(t)
{
	this->velocity = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
}


RigidBody::~RigidBody()
{
}

void RigidBody::update()
{
	this->transform.move(this->velocity);
}

void RigidBody::collide(const glm::vec4& direction)
{
	this->velocity -= direction;
}

void RigidBody::updateAll(RigidBody* begin, RigidBody* end)
{
	while (begin != end)
	{
		begin->update();
		begin++;
	}
}
