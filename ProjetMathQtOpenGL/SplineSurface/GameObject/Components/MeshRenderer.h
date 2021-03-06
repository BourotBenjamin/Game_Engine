#pragma once
#include "GL/glew.h"
#include "../../Mat4x4.h"
#include "Transform.h"
#include "Mesh.h"
#include <tiny_obj_loader.h>
class MeshRenderer
{
public:
	MeshRenderer(Transform* t, Mesh* m);
	void render();
	~MeshRenderer();
	static void MeshRenderer::renderAll(void* begin, int size);
private:
	Transform* transform;
	Mesh* mesh;
};

