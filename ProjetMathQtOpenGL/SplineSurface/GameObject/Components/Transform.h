#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <glm.hpp>
#include <vector>
#include "../../Mat4x4.h"
#include "../../World.h"
#include <gtc/quaternion.hpp>

class GameObject;
class Transform
{
public:
	Transform(World* world);
	Transform(World* world, float x, float y, float z);
	~Transform();
	void move(const glm::vec4& offset);
	Mat4x4 getMatrix();
	int getMapPos();
	const glm::vec4& getPosition();
	static float getDistSquare(const glm::vec4& p1, const glm::vec4& p2);
	void setGameObject(GameObject* gameObject);
private:
	glm::vec4 position;
	glm::vec4 scale;
	glm::vec4 rotation;
	GameObject* gameObject;
	World* world;
};
#endif

