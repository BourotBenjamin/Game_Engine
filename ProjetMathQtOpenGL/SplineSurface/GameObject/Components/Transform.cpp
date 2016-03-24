#include "Transform.h"
#include "../GameObject.h"

Transform::Transform()
{
}


Transform::~Transform()
{
}


void Transform::move(const glm::vec4& offset)
{
	int HEIGHT_MAP = 100;
	int oldpos = (HEIGHT_MAP * position.x / 100 + position.y / 100);
	this->position += offset;
	int pos = (HEIGHT_MAP * position.x / 100 + position.y / 100);
	if (pos != oldpos)
	{
		int* zone = (int*)GameObject::maps[oldpos];
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
		int* zone = (int*)GameObject::maps[pos];
		int nb = *(zone++);
		(GameObject*)zone[nb] = this->gameObject;
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