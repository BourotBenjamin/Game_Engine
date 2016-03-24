#pragma once
#include "Courbe.h"
#include <QtOpenGL\qglWidget>
class Spline :
	public Courbe
{
protected:
	std::vector<int> m_T;		//Knot vector

	int p;						//degree of the spline
	int n;

	inline float alpha1(float t, int i, int p){ if ((m_T[i + p] - m_T[i]) == 0)return 0.f;			else return((t - m_T[i]) / ((m_T[i + p] - m_T[i]))*1.f); }
	inline float alpha2(float t, int i, int p){ if ((m_T[i + 1 + p] - m_T[i + 1]) == 0)return 0.f;	else return(((m_T[i + 1 + p] - t)) / ((m_T[i + 1 + p] - m_T[i + 1])*1.f)); }
	
	
public:
	Spline(std::string vertexShader, std::string fragmentShader);
	Spline(float timeMs);
	~Spline();

	void calculateT();						//compute Knot Vector
	void computePointInCourbe();			
	float DeBoor(int p, int i, float t);	//DeBoor algorithm to get controle point factor
	void load();							
	void drawCourbe(Mat4x4 projection, Mat4x4 modelView);

};

