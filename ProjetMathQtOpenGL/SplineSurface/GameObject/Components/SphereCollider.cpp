#include "SphereCollider.h"
#include "../GameObject.h"


SphereCollider::SphereCollider(Transform& t, RigidBody& r) :Collider(t, r)
{
	radius = 5.0f;
}

void SphereCollider::update()
{
	int pos = transform.getMapPos();
	int offsetX = radius / World::WIDTH_ZONE + 1;
	int offsetY = radius / World::HEIGHT_ZONE + 1;
	for (int i = -offsetX; i < offsetX; i++)
	{
		for (int j = -offsetY; j < offsetY; j++)
		{
			int* zone = (int*)World::getMaps()[pos + (World::ZONES_Y * offsetX) + offsetY];
			int nb = *(zone++);
			if (nb > 0)
			{
				GameObject** begin = (GameObject**)zone;
				for (int k = 0; k < nb; k++)
				{
					// TODO Update ( HOW ? / Heritance )
					this->collide((SphereCollider*) begin[k]->getCollider());
				}

			}

		}
	}
}

void SphereCollider::collide(const SphereCollider* collider)
{
	if (Transform::getDistSquare(transform.getPosition(), collider->transform.getPosition()) < (collider->radius + radius) * (collider->radius + radius))
	{
		rigidBody.collide(collider->transform.getPosition() - transform.getPosition());
	}
}

SphereCollider::~SphereCollider()
{
}
