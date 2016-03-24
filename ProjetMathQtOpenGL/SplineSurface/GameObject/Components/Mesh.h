#pragma once
#include "GL/glew.h"
#include "../../Mat4x4.h"
#include "Transform.h"
#include <tiny_obj_loader.h>
class Mesh
{
public:
	Mesh(GLuint& program, std::string& file);
	void LoadFromObj(std::string& file, bool noNormal);
	void draw(Mat4x4& worldMatrix);
	~Mesh();
private:
	int elemsCount;
	GLuint VBO, EBO, program, VAO, worldLocation;
};