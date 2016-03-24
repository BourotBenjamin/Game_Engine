#include "GameObject.h"

/*
GameObject::GameObject(Transform* transform, MeshRenderer* meshRenderer, RigidBody* rigidBody, Collider* collider, GameObject* parent) :transform(transform), meshRenderer(meshRenderer), rigidBody(rigidBody), collider(collider), parent(parent)
{
	parent->childs.push_back(this);
	transform->setGameObject(this);
}*/

GameObject::GameObject(Transform* transform, MeshRenderer* meshRenderer, RigidBody* rigidBody, Collider* collider) : transform(transform), meshRenderer(meshRenderer), rigidBody(rigidBody), collider(collider)
{
	//parent = nullptr;
	transform->setGameObject(this);
}

const Collider* GameObject::getCollider()
{
	return collider;
}


GameObject::~GameObject()
{
}
