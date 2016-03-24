#include "Objet.h"
#include "Quaternion.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

void setupFeedback(GLuint program)
{
	const GLchar* Varyings[3];
	Varyings[0] = "out_Position";
	Varyings[1] = "out_Velocity";
	Varyings[2] = "out_Age";
	glTransformFeedbackVaryings(program, 2, Varyings, GL_INTERLEAVED_ATTRIBS);
}

Objet::Objet(std::string objfile, bool noNormal) :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0, 0, 1)
{
	LoadFromObj(objfile, noNormal);
	worldMatrix.identity();
}

Objet::Objet(std::string objfile, uint32_t nbParticules, EsgiShader& programUpdate, std::string vertexShader, std::string fragmentShader, std::string geometryShader) :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0, 0, 1),
currTFB(1), currVB(0), currSource(0)
{
	LoadFromObj(objfile);
	isParticul = true;
	this->nbParticules = nbParticules;

	std::vector<Particle> Particles(nbParticules);
	
	for (int i = 0; i < nbParticules; ++i)
	{
		Particles[i].Pos = Point();
		Particles[i].Vel = Point(RandomFloat(), RandomFloat(), RandomFloat());
		Particles[i].LifetimeMillis = 0.0f;
	}

	//Particles[0].Type = PARTICLE_TYPE_LAUNCHER;
	Particles[0].Pos = position;
	Particles[0].Vel = Point(0.0f, 0.0001f, 0.0f);
	Particles[0].LifetimeMillis = 0.0f;

	/*glGenTransformFeedbacks(2, transformFeedback);
	glGenBuffers(2, particleBuffer);

	for (unsigned int i = 0; i < 2; i++) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particles), Particles.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[i]);
	}*/

	glGenBuffers(2, particleBuffer);
	glGenVertexArrays(2, particuleUpdateVAO);
	glGenTransformFeedbacks(2, transformFeedback);
	GLsizei stride = sizeof(float)* 3;

	for (int i = 0; i < 2; i++)
	{
		stride = sizeof(Particle);
		glBindVertexArray(particuleUpdateVAO[i]);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, nbParticules * stride, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[i]);
		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
		glEnableVertexAttribArray(0);
		// velocity
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)12);
		glEnableVertexAttribArray(1);
		// Age
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)24);
		glEnableVertexAttribArray(2);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*const GLchar* Varyings[3];
	Varyings[0] = "Position";
	Varyings[1] = "Velocity";
	Varyings[2] = "Age";

	auto prog = programUpdate.getProgramID();
	bool isNVIDIA;

	glGetString(GL_VENDOR)[0] == 'N' ? isNVIDIA = true : isNVIDIA = false;

	if (isNVIDIA)
	{
		
		glUseProgram(prog);

		GLint* Varyings2 = new int[3];
		Varyings2[0] = glGetVaryingLocationNV(prog, Varyings[0]);
		Varyings2[1] = glGetVaryingLocationNV(prog, Varyings[1]);
		Varyings2[2] = glGetVaryingLocationNV(prog, Varyings[2]);

		glTransformFeedbackVaryingsNV(prog, 3, Varyings2, GL_INTERLEAVED_ATTRIBS);
		glUseProgram(0);

		delete[] Varyings2;
	}

	else
	{
		glTransformFeedbackVaryings(prog, 3, Varyings, GL_INTERLEAVED_ATTRIBS);
	}
	

	
	if (programUpdate.link())
		std::cout << "OOOOWWWW SHIT!";*/

	if (vertexShader.size())
		programUpdate.LoadVertexShader(vertexShader.c_str());
	//if (fragmentShader.size())
	//	programUpdate.LoadFragmentShader(fragmentShader.c_str());
	if (geometryShader.size())
		programUpdate.LoadGeometryShader(geometryShader.c_str());
	programUpdate.SetPreLinkCallback(setupFeedback);
	programUpdate.Create();


	Point* pRandomData = new Point[nbParticules];
	for (unsigned int i = 0; i < nbParticules; i++) {
		pRandomData[i].x = RandomFloat();
		pRandomData[i].y = RandomFloat();
		pRandomData[i].z = RandomFloat();
	}

	glGenTextures(1, &randomTex);
	glBindTexture(GL_TEXTURE_1D, randomTex);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, nbParticules, 0, GL_RGB, GL_FLOAT, pRandomData);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

	delete[] pRandomData;

	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[0]);
	float* particles = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (int i = 0; i < nbParticules; i++)
	{
		float rndx = (rand() / (float)RAND_MAX)*30.f - 15.f;
		float rndy = (rand() / (float)RAND_MAX)*5.f - 2.5f;
		float rndz = (rand() / (float)RAND_MAX)*5.f - 2.5f;
		//pos
		*particles++ = rndx;
		*particles++ = rndy;
		*particles++ = rndz;
		//velocity
		*particles++ = rndx;//0.f;
		*particles++ = rndx;//0.f;
		*particles++ = rndx;//0.f;
		//age
		*particles++ = 0.f;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	//glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[1]);
	//particles = (float *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	//for (int i = 0; i < nbParticules; i++)
	//{
	//	float rndx = (rand() / (float)RAND_MAX)*30.f - 15.f;
	//	float rndy = (rand() / (float)RAND_MAX)*5.f - 2.5f;
	//	float rndz = (rand() / (float)RAND_MAX)*5.f - 2.5f;
	//	//pos
	//	*particles++ = rndx;
	//	*particles++ = rndy;
	//	*particles++ = rndz;
	//	//velocity
	//	*particles++ = 0.f;
	//	*particles++ = 0.f;
	//	*particles++ = 0.f;
	//	//age
	//	*particles++ = 0.f;
	//}
	//glUnmapBuffer(GL_ARRAY_BUFFER);
	
}


Objet::Objet(std::vector<std::string>& v) :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0, 0, 1)
{
	static const float skyboxVertices[] = {
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	int w, h, comp;

	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);

	for (int faceIndex = 0; faceIndex < 6; ++faceIndex)
	{
		const char * filename = v[faceIndex].c_str();
		uint8_t * data = stbi_load(filename, &w, &h, &comp, STBI_rgb_alpha);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// RAPPEL: sizeof(skyboxVertices) fonctionne -cad qu'on obtient la taille totale du tableau-
	//			du fait que skyboxVertices est un tableau STATIC et donc que le compilateur peut deduire
	//			sa taille lors de la compilation. Autrement sizeof aurait du renvoyer la taille du pointeur.
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	
	worldMatrix.identity();
}

Objet::Objet(std::string diffuse, std::string normal) :position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1), lookat(0,0,1)
{
	//m_shader = shader;
	worldMatrix.identity();
	Point pos1(-1.0, 1.0, 0.0);
	Point pos2(-1.0, -1.0, 0.0);
	Point pos3(1.0, -1.0, 0.0);
	Point pos4(1.0, 1.0, 0.0);
	// texture coordinates
	Point uv1(0.0, 1.0,0);
	Point uv2(0.0, 0.0,0);
	Point uv3(1.0, 0.0,0);
	Point uv4(1.0, 1.0,0);
	// normal vector
	Point nm(0.0, 0.0, 1.0);

	// calculate tangent/bitangent vectors of both triangles
	Point tangent1, bitangent1;
	Point tangent2, bitangent2;
	// - triangle 1
	Point edge1 = pos2 - pos1;
	Point edge2 = pos3 - pos1;
	Point deltaUV1 = uv2 - uv1;
	Point deltaUV2 = uv3 - uv1;

	GLfloat f = 1.0f / (deltaUV1.Getx() * deltaUV2.Gety() - deltaUV2.Getx() * deltaUV1.Gety());

	tangent1.Setx(f * (deltaUV2.Gety() * edge1.Getx() - deltaUV1.Gety() * edge2.Getx()));
	tangent1.Sety(f * (deltaUV2.Gety() * edge1.Gety() - deltaUV1.Gety() * edge2.Gety()));
	tangent1.Setz(f * (deltaUV2.Gety() * edge1.Getz() - deltaUV1.Gety() * edge2.Getz()));
	tangent1.normalize();

	bitangent1.Setx(f * (-deltaUV2.Getx() * edge1.Getx() + deltaUV1.Getx() * edge2.Getx()));
	bitangent1.Sety(f * (-deltaUV2.Getx() * edge1.Gety() + deltaUV1.Getx() * edge2.Gety()));
	bitangent1.Setz(f * (-deltaUV2.Getx() * edge1.Getz() + deltaUV1.Getx() * edge2.Getz()));
	bitangent1.normalize();

	// - triangle 2
	edge1 = pos3 - pos1;
	edge2 = pos4 - pos1;
	deltaUV1 = uv3 - uv1;
	deltaUV2 = uv4 - uv1;

	f = 1.0f / (deltaUV1.Getx() * deltaUV2.Gety() - deltaUV2.Getx() * deltaUV1.Gety());

	tangent2.Setx(f * (deltaUV2.Gety() * edge1.Getx() - deltaUV1.Gety() * edge2.Getx()));
	tangent2.Sety(f * (deltaUV2.Gety() * edge1.Gety() - deltaUV1.Gety() * edge2.Gety()));
	tangent2.Setz(f * (deltaUV2.Gety() * edge1.Getz() - deltaUV1.Gety() * edge2.Getz()));
	tangent2.normalize();


	bitangent2.Setx(f * (-deltaUV2.Getx() * edge1.Getx() + deltaUV1.Getx() * edge2.Getx()));
	bitangent2.Sety(f * (-deltaUV2.Getx() * edge1.Gety() + deltaUV1.Getx() * edge2.Gety()));
	bitangent2.Setz(f * (-deltaUV2.Getx() * edge1.Getz() + deltaUV1.Getx() * edge2.Getz()));
	bitangent2.normalize();

	std::vector<uint32_t> indices{ 1, 2, 3, 1, 3, 4 };
	ElementCount = 2;
	//ebo
	//123   134
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	GLfloat quadVertices[] = {
		// Positions            // normal         // TexCoords  // Tangent                          // Bitangent
		pos1.Getx(), pos1.Gety(), pos1.Getz(), nm.Getx(), nm.Gety(), nm.Getz(), uv1.Getx(), uv1.Gety(), tangent1.Getx(), tangent1.Gety(), tangent1.Getz(), bitangent1.Getx(), bitangent1.Gety(), bitangent1.Getz(),
		pos2.Getx(), pos2.Gety(), pos2.Getz(), nm.Getx(), nm.Gety(), nm.Getz(), uv2.Getx(), uv2.Gety(), tangent1.Getx(), tangent1.Gety(), tangent1.Getz(), bitangent1.Getx(), bitangent1.Gety(), bitangent1.Getz(),
		pos3.Getx(), pos3.Gety(), pos3.Getz(), nm.Getx(), nm.Gety(), nm.Getz(), uv3.Getx(), uv3.Gety(), tangent1.Getx(), tangent1.Gety(), tangent1.Getz(), bitangent1.Getx(), bitangent1.Gety(), bitangent1.Getz(),
		pos4.Getx(), pos4.Gety(), pos4.Getz(), nm.Getx(), nm.Gety(), nm.Getz(), uv4.Getx(), uv4.Gety(), tangent2.Getx(), tangent2.Gety(), tangent2.Getz(), bitangent2.Getx(), bitangent2.Gety(), bitangent2.Getz()
	};
	// Setup plane VAO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));

	glGenTextures(1, &textureObj);
	glBindTexture(GL_TEXTURE_2D, textureObj);

	// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
	// autrement le Texture Object est considere comme invalide
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int w, h;
	uint8_t *data = stbi_load(diffuse.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	glGenTextures(1, &normalObj);
	glBindTexture(GL_TEXTURE_2D, normalObj);

	// il est obligatoire de specifier une valeur pour GL_TEXTURE_MIN_FILTER
	// autrement le Texture Object est considere comme invalide
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//int w, h;
	data = stbi_load(normal.c_str(), &w, &h, nullptr, STBI_rgb_alpha);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}
	worldMatrix.identity();
}

Objet::~Objet()
{
	if (textureObj)
		glDeleteTextures(1, &textureObj);
	if (isSpecularMap)
		glDeleteTextures(1, &specularObj);
	if (isNormalMap)
		glDeleteTextures(1, &normalObj); 
	if (VAO)
		glDeleteVertexArrays(1, &VAO);
	if (VBO)
		glDeleteBuffers(1, &VBO);
	if (EBO)
		glDeleteBuffers(1, &EBO);
}

void Objet::LoadFromObj(std::string file, bool noNormal)
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

	ElementCount = indices.size();

	if (materials[0].diffuse_texname.size())
	{
		isDiffuseMap = true;
	}

	uint32_t stride = 0;

	if (positions.size()) {
		stride += 3 * sizeof(float);
	}
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
	}

	const auto count = positions.size() / 3;
	const auto totalSize = count * stride;

	//calcul des tangentes et bitangente
	if(isNormalMap)
	{
		tangentes.reserve(positions.size());
		biTangentes.reserve(positions.size());
		for(int i = 0; i < positions.size(); ++i)
		{
			tangentes.push_back(0);
			biTangentes.push_back(0);
		}
		for(int i = 0; i < indices.size(); i+=3)
		{
			// Shortcuts for vertices
			Point v0 = Point(positions[(indices[i]*3)+0], positions[(indices[i]*3)+1], positions[(indices[i]*3)+2]);
			Point v1 = Point(positions[((indices[i+1])*3)+0], positions[((indices[i+1])*3)+1], positions[((indices[i+1])*3)+2]);
			Point v2 = Point(positions[((indices[i+2])*3)+0], positions[((indices[i+2])*3)+1], positions[((indices[i+2])*3)+2]);

			// Shortcuts for UVs
			Point uv0 = Point(texcoords[((indices[i])* 2) + 0], texcoords[((indices[i])* 2) + 1], 0);
			Point uv1 = Point(texcoords[((indices[i + 1]) * 2) + 0], texcoords[((indices[i + 1]) * 2) + 1], 0);
			Point uv2 = Point(texcoords[((indices[i + 2]) * 2) + 0], texcoords[((indices[i + 2]) * 2) + 1], 0);

			// Edges of the triangle : postion delta
			Point deltaPos1 = v1-v0;
			Point deltaPos2 = v2-v0;

			// UV delta
			Point deltaUV1 = uv1-uv0;
			Point deltaUV2 = uv2-uv0;

			float r = 1.0f / (deltaUV1.Getx() * deltaUV2.Gety() - deltaUV1.Gety() * deltaUV2.Getx());
			Point tangent = (deltaPos1 * deltaUV2.Gety()   - deltaPos2 * deltaUV1.Gety())*r;
			Point bitangent = (deltaPos2 * deltaUV1.Getx()   - deltaPos1 * deltaUV2.Getx())*r;

			tangentes[(indices[i]*3)]       += tangent.Getx();
			tangentes[(indices[i]*3)+1]     += tangent.Gety();
			tangentes[(indices[i]*3)+2]     += tangent.Getz();

			tangentes[((indices[i+1])*3)]   += tangent.Getx();
			tangentes[((indices[i+1])*3)+1] += tangent.Gety();
			tangentes[((indices[i+1])*3)+2] += tangent.Getz();

			tangentes[((indices[i+2])*3)]   += tangent.Getx();
			tangentes[((indices[i+2])*3)+1] += tangent.Gety();
			tangentes[((indices[i+2])*3)+2] += tangent.Getz();

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

		
	}


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
		}
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
		}
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	uint32_t offset = 3 * sizeof(float);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, nullptr);
	glEnableVertexAttribArray(0);
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
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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
	}
	
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
	}

	if(isNormalMap)
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
	}
}

void Objet::render(GLuint& program, GLuint shadowText)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());

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
	{
		auto spec = glGetUniformLocation(program, "u_isSpecular");
		glUniform1i(spec, 0);
	}

	if (shadowText != 0)
	{
		auto shadowLoc = glGetUniformLocation(program, "u_shadowMap");
		glUniform1i(shadowLoc, 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadowText);
	}

	if(isNormalMap)
	{
		auto bumpTexLocation = glGetUniformLocation(program, "u_normalMap");
		glUniform1i(bumpTexLocation, 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalObj);
	}

	glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void Objet::renderNormalMap(GLuint& program)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");
	

	glBindTexture(GL_TEXTURE_2D, textureObj);
	glBindTexture(GL_TEXTURE_2D, normalObj);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());

	glDrawElements(GL_TRIANGLES, ElementCount, GL_UNSIGNED_INT, 0);
}

void Objet::renderCubeMap(GLuint& program)
{

	auto cubemapIndex = glGetUniformLocation(program, "u_CubeMap");
	glUniform1f(cubemapIndex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObj);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(VAO);
	// Tres important ! D'une part, comme la cubemap represente un environnement distant
	// il n'est pas utile d'ecrire dans le depth buffer (on est toujours au plus loin)
	// cependant il faut quand effectuer le test de profondeur (donc on n'a pas glDisable(GL_DEPTH_TEST)).
	// Neamoins il faut legerement changer l'operateur du test dans le cas ou 
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 8 * 2 * 3);
	// on retabli ensuite les render states par defaut
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void Objet::renderBillBoardParticules(GLuint& program)
{
	auto worldLocation = glGetUniformLocation(program, "u_worldMatrix");

	UpdateTransform();
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, worldMatrix.getMatrix());

	if (isDiffuseMap)
	{
		UpdateMaterial(program);
		auto diffTexLocation = glGetUniformLocation(program, "u_sampler");
		glUniform1i(diffTexLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureObj);
	}

	// on affiche le vbo non transforme
	glBindVertexArray(particuleUpdateVAO[currTFB]);
	//glDrawArrays(GL_POINTS, 0, nbParticules);
	glDrawTransformFeedback(GL_POINTS, transformFeedback[currTFB]);
	glBindVertexArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[currTFB]);

	//glEnableVertexAttribArray(0);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);  // position

	//glDrawTransformFeedback(GL_POINTS, transformFeedback[currTFB]);

	//glDisableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Objet::UpdateMaterial(GLuint& program)
{
	GLint matAmbientLoc = glGetUniformLocation(program, "u_ambient");
	GLint matDiffuseLoc = glGetUniformLocation(program, "u_diffuse");
	GLint matSpecularLoc = glGetUniformLocation(program, "u_specular");
	GLint matShininessLoc = glGetUniformLocation(program, "u_shininess");

	glUniform3f(matAmbientLoc, ambiant.Getx(), ambiant.Gety(), ambiant.Getz());
	glUniform3f(matDiffuseLoc, diffuse.Getx(), diffuse.Gety(), diffuse.Getz());
	glUniform3f(matSpecularLoc, specular.Getx(), specular.Gety(), specular.Getz());
	glUniform1f(matShininessLoc, shininess);
}

void Objet::UpdateTransform()
{ 
	
	auto t = Mat4x4::translate(position.x, position.y, position.z);
	Quaternion r = Quaternion::eulerAngle(rotation);
	auto s = Mat4x4::scale(scale.x, scale.y, scale.z);
	worldMatrix.identity();
	worldMatrix *= t;
	worldMatrix *= r.to_mat4();
	worldMatrix *= s;
}


void Objet::UpdateParticules(GLuint& program, float deltaTime)
{
	time += deltaTime;

	GLint timeLoc = glGetUniformLocation(program, "u_time");
	GLint deltaTimeLoc = glGetUniformLocation(program, "u_deltaTime");
	glUniform1f(timeLoc, time);
	glUniform1f(deltaTimeLoc, deltaTime);

	auto randTexLocation = glGetUniformLocation(program, "u_random");
	glUniform1i(randTexLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, randomTex);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(particuleUpdateVAO[currVB]);
	//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[currTFB]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[currTFB]);
	glBeginTransformFeedback(GL_POINTS);
	//glDrawArrays(GL_POINTS, 0, nbParticules);
	if (isFirst) {
		glDrawArrays(GL_POINTS, 0, nbParticules);

		isFirst = false;
	}
	else {
		glDrawTransformFeedback(GL_POINTS, transformFeedback[currVB]);
	}
	glEndTransformFeedback();
	//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	glDisable(GL_RASTERIZER_DISCARD);


	//glEnable(GL_RASTERIZER_DISCARD);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[currVB]);
	//glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[currTFB]);

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);                          // type
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)12);         // position
	//glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)24);        // velocity

	//glBeginTransformFeedback(GL_POINTS);

	//if (isFirst) {
	//	glDrawArrays(GL_POINTS, 0, 1);

	//	isFirst = false;
	//}
	//else {
	//	glDrawTransformFeedback(GL_POINTS, transformFeedback[currVB]);
	//}

	//glEndTransformFeedback();

	//glDisableVertexAttribArray(0);
	//glDisableVertexAttribArray(1);
	//glDisableVertexAttribArray(2);
	//glDisableVertexAttribArray(3);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glDisable(GL_RASTERIZER_DISCARD);
}

void Objet::setLookAtPoint(Point p)
{
	auto q = Quaternion::RotationBetweenVectors(lookat, p);
	rotation += q.toEulerAngle();
	lookat = p;
	isLookat = true;
}