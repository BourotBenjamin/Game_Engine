#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(Transform& t, Mesh& m) :transform(t), mesh(m)
{
}


void MeshRenderer::render()
{
	this->mesh.draw(this->transform.getMatrix());
}


void MeshRenderer::renderAll(MeshRenderer* begin, int size)
{
	for (int i = 0; i < size; i++)
	{
		begin->render();
		begin++;
	}
}