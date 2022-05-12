#include "Material.h"

Material::Material() {
}

Material::Material( GLuint alb) {
	albedo = alb;
}

Material::Material(GLuint alb, GLuint metal)
{
	albedo = alb;
	metallic = metal;
}

Material::Material(GLuint alb, GLuint metal, GLuint norm)
{
	albedo = alb;
	metallic = metal;
	normal = norm;
}

Material::Material(GLuint alb, GLuint metal, GLuint norm, GLuint rough)
{
	albedo = alb;
	metallic = metal;
	normal = norm;
	roughness = rough;
}

Material::Material(GLuint alb, GLuint metal, GLuint norm, GLuint rough, GLuint para)
{
	albedo = alb;
	metallic = metal;
	normal = norm;
	roughness = rough;
	parallax = para;
}

Material::Material(GLuint alb, GLuint metal, GLuint norm, GLuint rough, GLuint para, GLuint glo)
{
	albedo = alb;
	metallic = metal;
	normal = norm;
	roughness = rough;
	parallax = para;
	glow = glo;
}


void Material::UseMaterial(GLuint albedoLocation) {
	glUniform1i(albedoLocation, albedo);
}

void Material::UseMaterial(GLuint albedoLocation, GLuint metallicLocation)
{
	glUniform1i(albedoLocation, albedo);
	glUniform1i(metallicLocation, metallic);

}

void Material::UseMaterial(GLuint albedoLocation, GLuint metallicLocation, GLuint normalLocation)
{
	glUniform1i(albedoLocation, albedo);
	glUniform1i(metallicLocation, metallic);
	glUniform1i(normalLocation, normal);
}

void Material::UseMaterial(GLuint albedoLocation, GLuint metallicLocation, GLuint normalLocation, GLuint roughnessLocation)
{
	glUniform1i(albedoLocation, albedo);
	glUniform1i(metallicLocation, metallic);
	glUniform1i(normalLocation, normal);
	glUniform1i(roughnessLocation, roughness);
}

void Material::UseMaterial(GLuint albedoLocation, GLuint metallicLocation, GLuint normalLocation, GLuint roughnessLocation, GLuint parallaxLocation)
{
	glUniform1i(albedoLocation, albedo);
	glUniform1i(metallicLocation, metallic);
	glUniform1i(normalLocation, normal);
	glUniform1i(roughnessLocation, roughness);
	glUniform1i(parallaxLocation, parallax);
}

void Material::UseMaterial(GLuint albedoLocation, GLuint metallicLocation, GLuint normalLocation, GLuint roughnessLocation, GLuint parallaxLocation, GLuint glowLocation)
{
	glUniform1i(albedoLocation, albedo);
	glUniform1i(metallicLocation, metallic);
	glUniform1i(normalLocation, normal);
	glUniform1i(roughnessLocation, roughness);
	glUniform1i(parallaxLocation, parallax);
	glUniform1i(glowLocation, glow);

}

Material::~Material() {

}