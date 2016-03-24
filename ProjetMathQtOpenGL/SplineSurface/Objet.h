#ifndef OBJET_H
#define OBJET_H

#define _USE_MATH_DEFINES
#include <iostream>
#include <stdio.h>
#include <list>
#include <vector>
#include "Point.h"
#include <math.h>
#include <cmath>
#include <memory>
#define GLEW_STATIC 1
#include <GL\glew.h>
#include "Shader.h"
#include "EsgiShader.h"
#include "Mat4x4.h"
#include <tiny_obj_loader.h>
#include <time.h>


struct Particle
{
	//float Type;
	Point Pos;
	Point Vel;
	float LifetimeMillis;
};

class Objet
{
protected:

	
	Mat4x4 worldMatrix;
	// mesh
	GLuint VBO;
	GLuint EBO;
	GLuint ElementCount;
	GLenum PrimitiveType;
	GLuint VAO;
	// material
	GLuint textureObj;
	GLuint specularObj;
	GLuint normalObj;

	//Shader * m_shader;
	
	
	bool isLookat = false;

	Point ambiant;
	Point diffuse;
	Point specular;
	float shininess;

	bool isDiffuseMap = false;
	bool isNormalMap = false;
	bool isSpecularMap = false;

	bool isParticul = false;

	float time;

	//particule system related
	bool isFirst;
	GLuint particuleUpdateVAO[2];
	GLuint particuleRenderVAO[2];
	GLuint particleBuffer[2];
	GLuint transformFeedback[2];
	uint32_t nbParticules;

	GLuint randomTex;

public:
	// transform
	Point position;
	Point rotation;
	Point scale;
	Point lookat;

	uint32_t currVB;
	uint32_t currTFB;
	uint32_t currSource;

	Objet(std::string objfile, bool noNorma = false);
	Objet(std::vector<std::string>& v);
	Objet(std::string objfile,std::string normalMap);
	Objet(std::string objfile, uint32_t nbParticules, EsgiShader& programUpdate, std::string vertexShader, std::string fragmentShader, std::string geometryShader );
	~Objet();

	//void setupFeedback(GLuint program);

	void setPost(Point p){ position = p; }
	void setRot(Point p){ rotation = p; }
	void setScale(Point p){ scale = p; }
	void setLookAtPoint(Point p);
	void unDoneLookAt(){ isLookat = false; }
	void LoadFromObj(std::string objfile, bool noNorma = false);
	void render(GLuint& program, GLuint shadowText = 0);
	void renderCubeMap(GLuint& program);
	void renderNormalMap(GLuint& program);
	void renderBillBoardParticules(GLuint& program);

	void UpdateMaterial(GLuint& program);

	void UpdateTransform();
	void UpdateParticules(GLuint& program, float deltaTime);
	
	float RandomFloat()
	{
		float Max = RAND_MAX;
		return ((float)rand() / Max);
	}
};

#endif