#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(Transform* t, Mesh* m) :transform(t), mesh(m)
{
}


void MeshRenderer::render()
{
	this->mesh->draw(this->transform->getMatrix());
}


void MeshRenderer::renderAll(void* begin, int size)
{
	MeshRenderer* ptr = (MeshRenderer*)begin;
	for (int i = 0; i < size; i++)
	{
		ptr->render();
		ptr++;
	}
}