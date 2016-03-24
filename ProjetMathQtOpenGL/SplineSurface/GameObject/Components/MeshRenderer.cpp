#include "MeshRenderer.h"


MeshRenderer::MeshRenderer(Transform& t, Mesh& m) :transform(t), mesh(m)
{
}


void MeshRenderer::render()
{
	this->mesh.draw(this->transform.getMatrix());
}
