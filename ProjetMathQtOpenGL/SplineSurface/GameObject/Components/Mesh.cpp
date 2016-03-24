#include "Mesh.h"


Mesh::Mesh(GLuint& program, std::string& file)
{
	worldLocation = glGetUniformLocation(program, "u_worldMatrix");
	this->program = program;
	this->LoadFromObj(file, false);
}

void Mesh::LoadFromObj(std::string& file, bool noNormal)
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, file.c_str());
	const std::vector<unsigned int>& indices = shapes[0].mesh.indices;
	const std::vector<float>& positions = shapes[0].mesh.positions;
	const std::vector<float>& normals = shapes[0].mesh.normals;
	const std::vector<float>& texcoords = shapes[0].mesh.texcoords;

	std::vector<float> tangentes;
	std::vector<float> biTangentes;

	if (file == "cyborg.obj")
		materials[0].bump_texname = "cyborg_normal.png";

	elemsCount = indices.size();
	/*
	if (materials[0].diffuse_texname.size())
	{
	isDiffuseMap = true;
	}*/

	uint32_t stride = 0;

	if (positions.size()) {
		stride += 3 * sizeof(float);
	}/*
	 if (isDiffuseMap) {
	 stride += 3 * sizeof(float);
	 }
	 if (isDiffuseMap) {
	 stride += 2 * sizeof(float);
	 }
	 if (materials[0].specular_texname.size())
	 isSpecularMap = true;
	 if (materials[0].bump_texname.size() && noNormal == false)
	 {
	 isNormalMap = true;
	 stride += 6 * sizeof(float);
	 }
	 if (materials[0].diffuse_texname.size())
	 {
	 isDiffuseMap = true;
	 }

	 if (isDiffuseMap)
	 {
	 ambiant = Point(materials[0].ambient[0], materials[0].ambient[1], materials[0].ambient[2]);
	 diffuse = Point(materials[0].diffuse[0], materials[0].diffuse[1], materials[0].diffuse[2]);
	 specular = Point(materials[0].specular[0], materials[0].specular[1], materials[0].specular[2]);
	 shininess = materials[0].shininess;
	 }*/

	const auto count = positions.size() / 3;
	const auto totalSize = count * stride;

	//calcul des tangentes et bitangente
	/*
	if (isNormalMap)
	{
	tangentes.reserve(positions.size());
	biTangentes.reserve(positions.size());
	for (int i = 0; i < positions.size(); ++i)
	{
	tangentes.push_back(0);
	biTangentes.push_back(0);
	}
	for (int i = 0; i < indices.size(); i += 3)
	{
	// Shortcuts for vertices
	Point v0 = Point(positions[(indices[i] * 3) + 0], positions[(indices[i] * 3) + 1], positions[(indices[i] * 3) + 2]);
	Point v1 = Point(positions[((indices[i + 1]) * 3) + 0], positions[((indices[i + 1]) * 3) + 1], positions[((indices[i + 1]) * 3) + 2]);
	Point v2 = Point(positions[((indices[i + 2]) * 3) + 0], positions[((indices[i + 2]) * 3) + 1], positions[((indices[i + 2]) * 3) + 2]);

	// Shortcuts for UVs
	Point uv0 = Point(texcoords[((indices[i]) * 2) + 0], texcoords[((indices[i]) * 2) + 1], 0);
	Point uv1 = Point(texcoords[((indices[i + 1]) * 2) + 0], texcoords[((indices[i + 1]) * 2) + 1], 0);
	Point uv2 = Point(texcoords[((indices[i + 2]) * 2) + 0], texcoords[((indices[i + 2]) * 2) + 1], 0);

	// Edges of the triangle : postion delta
	Point deltaPos1 = v1 - v0;
	Point deltaPos2 = v2 - v0;

	// UV delta
	Point deltaUV1 = uv1 - uv0;
	Point deltaUV2 = uv2 - uv0;

	float r = 1.0f / (deltaUV1.Getx() * deltaUV2.Gety() - deltaUV1.Gety() * deltaUV2.Getx());
	Point tangent = (deltaPos1 * deltaUV2.Gety() - deltaPos2 * deltaUV1.Gety())*r;
	Point bitangent = (deltaPos2 * deltaUV1.Getx() - deltaPos1 * deltaUV2.Getx())*r;

	tangentes[(indices[i] * 3)] += tangent.Getx();
	tangentes[(indices[i] * 3) + 1] += tangent.Gety();
	tangentes[(indices[i] * 3) + 2] += tangent.Getz();

	tangentes[((indices[i + 1]) * 3)] += tangent.Getx();
	tangentes[((indices[i + 1]) * 3) + 1] += tangent.Gety();
	tangentes[((indices[i + 1]) * 3) + 2] += tangent.Getz();

	tangentes[((indices[i + 2]) * 3)] += tangent.Getx();
	tangentes[((indices[i + 2]) * 3) + 1] += tangent.Gety();
	tangentes[((indices[i + 2]) * 3) + 2] += tangent.Getz();

	biTangentes[(indices[i] * 3)] += bitangent.Getx();
	biTangentes[(indices[i] * 3) + 1] += bitangent.Gety();
	biTangentes[(indices[i] * 3) + 2] += bitangent.Getz();

	biTangentes[((indices[i + 1]) * 3)] += bitangent.Getx();
	biTangentes[((indices[i + 1]) * 3) + 1] += bitangent.Gety();
	biTangentes[((indices[i + 1]) * 3) + 2] += bitangent.Getz();

	biTangentes[((indices[i + 2]) * 3)] += bitangent.Getx();
	biTangentes[((indices[i + 2]) * 3) + 1] += bitangent.Gety();
	biTangentes[((indices[i + 2]) * 3) + 2] += bitangent.Getz();

	}


	}*/


	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, GL_STATIC_DRAW);

	// glMapBuffer retourne un pointeur sur la zone memoire allouee par glBufferData 
	// du Buffer Object qui est actuellement actif - via glBindBuffer(<cible>, <id>)
	// il est imperatif d'appeler glUnmapBuffer() une fois que l'on a termine car le
	// driver peut tres bien etre amener a modifier l'emplacement memoire du BO.
	float* vertices = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (auto index = 0; index < count; ++index)
	{
		if (positions.size()) {
			memcpy(vertices, &positions[index * 3], 3 * sizeof(float));
			vertices += 3;
		}/*
		 if (isDiffuseMap) {
		 memcpy(vertices, &normals[index * 3], 3 * sizeof(float));
		 vertices += 3;
		 }
		 if (isDiffuseMap) {
		 memcpy(vertices, &texcoords[index * 2], 2 * sizeof(float));
		 vertices += 2;
		 }
		 if (isNormalMap)
		 {
		 memcpy(vertices, &tangentes[index * 3], 3 * sizeof(float));
		 vertices += 3;
		 memcpy(vertices, &biTangentes[index * 3], 3 * sizeof(float));
		 vertices += 3;
		 }*/
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	uint32_t offset = 3 * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, nullptr);
	glEnableVertexAttribArray(0);/*
								 if (isDiffuseMap) {
								 glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (GLvoid *)offset);
								 glEnableVertexAttribArray(1);
								 offset += 3 * sizeof(float);
								 }
								 if (isDiffuseMap) {
								 glVertexAttribPointer(2, 2, GL_FLOAT, false, stride, (GLvoid *)offset);
								 glEnableVertexAttribArray(2);
								 offset += 2 * sizeof(float);
								 }
								 if (isNormalMap)
								 {
								 glVertexAttribPointer(3, 3, GL_FLOAT, false, stride, (GLvoid *)offset);
								 glEnableVertexAttribArray(3);
								 offset += 3 * sizeof(float);

								 glVertexAttribPointer(4, 3, GL_FLOAT, false, stride, (GLvoid *)offset);
								 glEnableVertexAttribArray(4);
								 offset += 3 * sizeof(float);
								 }*/
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/*
	if (isDiffuseMap)
	{
	glGenTextures(1, &textureObj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObj);

	// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
	// autrement le Texture Object est considere comme invalide
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int w, h;
	uint8_t *data = stbi_load(materials[0].diffuse_texname.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
	if (data) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	}
	}*/
	/*
	if (isSpecularMap)
	{
	glGenTextures(1, &specularObj);
	glBindTexture(GL_TEXTURE_2D, specularObj);

	// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
	// autrement le Texture Object est considere comme invalide
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int w, h;
	uint8_t *data = stbi_load(materials[0].specular_texname.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
	if (data) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	}
	}*/
	/*
	if (isNormalMap)
	{
	glGenTextures(1, &normalObj);
	glBindTexture(GL_TEXTURE_2D, normalObj);

	// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
	// autrement le Texture Object est considere comme invalide
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int w, h;
	uint8_t *data = stbi_load(materials[0].bump_texname.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
	if (data) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
	}
	}*/
}


Mesh::~Mesh()
{
}


void Mesh::draw(Mat4x4& worldMatrix)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());
	/*
	if (isDiffuseMap)
	{
	UpdateMaterial(program);
	auto diffTexLocation = glGetUniformLocation(program, "u_sampler");
	glUniform1i(diffTexLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureObj);
	}

	if (isSpecularMap)
	{
	auto specTexLocation = glGetUniformLocation(program, "u_specularMap");
	glUniform1i(specTexLocation, 1);
	auto spec = glGetUniformLocation(program, "u_isSpecular");
	glUniform1i(spec, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, specularObj);
	}
	else
	{*/
	auto spec = glGetUniformLocation(program, "u_isSpecular");
	glUniform1i(spec, 0);
	//}
	/*
	if (shadowText != 0)
	{
	auto shadowLoc = glGetUniformLocation(program, "u_shadowMap");
	glUniform1i(shadowLoc, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowText);
	}

	if (isNormalMap)
	{
	auto bumpTexLocation = glGetUniformLocation(program, "u_normalMap");
	glUniform1i(bumpTexLocation, 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, normalObj);
	}*/

	glDrawElements(GL_TRIANGLES, elemsCount, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}
