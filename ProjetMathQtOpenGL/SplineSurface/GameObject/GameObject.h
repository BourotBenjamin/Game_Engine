#pragma once
#include "Components/MeshRenderer.h"
#include "Components/RigidBody.h"
#include "Components/Transform.h"
#include <vector>

class GameObject
{
public:
	GameObject();
	~GameObject();
private:
	Transform* transform;
	MeshRenderer* meshRenderer;
	RigidBody* rigidBody;
	GameObject* parent;
	std::vector<GameObject*> childs;
};
