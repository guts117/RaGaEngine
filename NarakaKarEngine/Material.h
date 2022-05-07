#pragma once

#include <GL/glew.h>

class Material
{
public:
	Material();
	Material(GLuint alb);
	Material(GLuint alb, GLuint metal);
	Material(GLuint alb, GLuint metal, GLuint norm);
	Material(GLuint alb, GLuint metal, GLuint norm, GLuint rough);
	Material(GLuint alb, GLuint metal, GLuint norm, GLuint rough, GLuint para);
	Material(GLuint alb, GLuint metal, GLuint norm, GLuint rough, GLuint para, GLuint glo);


	void UseMaterial(GLuint albedoLocation);
	void UseMaterial(GLuint albedoLocation, GLuint metallicLocation);
	void UseMaterial(GLuint albedoLocation, GLuint metallicLocation, GLuint normalLocation);
	void UseMaterial(GLuint albedoLocation, GLuint metallicLocation, GLuint normalLocation, GLuint roughLocation);
	void UseMaterial(GLuint albedoLocation, GLuint metallicLocation, GLuint normalLocation, GLuint roughLocation, GLuint parallaxLocation);
	void UseMaterial(GLuint albedoLocation, GLuint metallicLocation, GLuint normalLocation, GLuint roughLocation, GLuint parallaxLocation, GLuint glowLocation);

	~Material();

private:

	GLuint metallic = 0;
	GLuint albedo =0;
	GLuint normal = 0;
	GLuint roughness = 0;
	GLuint parallax = 0;
	GLuint glow = 0;
};

