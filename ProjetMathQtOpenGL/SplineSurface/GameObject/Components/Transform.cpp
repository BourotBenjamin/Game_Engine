#include "Transform.h"
#include "../GameObject.h"

Transform::Transform(World& world) : world(world), position(glm::vec4(0))
{
}
Transform::Transform(World& world, float x, float y, float z) : world(world), position(glm::vec4(x,y,z,0))
{
}


Transform::~Transform()
{(
}


void Transform::move(const glm::vec4& offset)
{
	int oldpos = (world.ZONES_Y * position.x / 100 + position.y / 100);
	this->position += offset;
	int pos = (world.ZONES_Y * position.x / 100 + position.y / 100);
	if (pos != oldpos)
	{
		int* zone = (int*)world.getMaps()[oldpos];
		int nb = *(zone++);
		GameObject** begin = (GameObject**)zone;
		for (int k = 0; k < nb; k++)
		{
			if (begin[k] == gameObject)
			{
				memcpy(&begin[k], &begin[k + 1], nb - k);
				begin[nb] = nullptr;
				break;
			}
		}
		zone = (int*)world.getMaps()[pos];
		nb = *(zone++);
		begin = (GameObject**)zone;
		begin[nb] = this->gameObject;
	}
}


Mat4x4 Transform::getMatrix()
{
	return Mat4x4::translate(this->position) * Mat4x4::rotate(this->rotation) * Mat4x4::scale(this->scale);
}

int Transform::getMapPos()
{
	int HEIGHT_MAP = 100;
	return (HEIGHT_MAP * position.x / 100 + position.y / 100);
}

const glm::vec4& Transform::getPosition()
{
	return this->position;
}

float Transform::getDistSquare(const glm::vec4& p1, const glm::vec4& p2)
{

	return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p1.y);
}

void Transform::setGameObject(GameObject* gameObject)
{
	this->gameObject = gameObject;
}