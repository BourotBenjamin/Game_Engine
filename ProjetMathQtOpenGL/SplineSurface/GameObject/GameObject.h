#pragma once
#include "Components/MeshRenderer.h"
#include "Components/RigidBody.h"
#include "Components/Transform.h"
#include "Components/Collider.h"
#include <vector>

class GameObject
{
public:
	//GameObject(Transform* transform, MeshRenderer* meshRenderer, RigidBody* rigidBody, Collider* collider, GameObject* parent);
	GameObject::GameObject(Transform* transform, MeshRenderer* meshRenderer, RigidBody* rigidBody, Collider* collider);
	~GameObject();
	static void** maps;
	const Collider* getCollider();
private:
	Transform* transform;
	MeshRenderer* meshRenderer;
	RigidBody* rigidBody;
	Collider* collider;
	/*GameObject* parent;
	std::vector<GameObject*> childs;*/
};
