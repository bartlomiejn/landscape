#ifndef LANDSCAPE_LIGHT_H
#define LANDSCAPE_LIGHT_H

#include <glm/glm.hpp>

class DirectionalLight
{
public:
	DirectionalLight(
		glm::vec3 dir, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec
	):
		direction(dir),
		ambient(amb),
		diffuse(diff),
		specular(spec)
	{};
	
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class PointLight
{
public:
	PointLight(
		glm::vec3 pos, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec,
		float att_const = 1.0, float att_lin = 0.09,
		float att_quad = 0.032
	):
		position(pos),
		ambient(amb),
		diffuse(diff),
		specular(spec),
		att_constant(att_const),
		att_linear(att_lin),
		att_quadratic(att_quad)
	{};

	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	float att_constant;
	float att_linear;
	float att_quadratic;
};

class SpotLight
{
public:
	/// Creates a spot light.
	///
	/// \param pos Position of the light
	/// \param dir Direction of the light
	/// \param amb Ambient light component
	/// \param diff Diffuse component
	/// \param spec Specular component
	/// \param cutoff_cos Cosine of the cutoff angle in radians
	/// \param att_const Attenuation constant parameter
	/// \param att_lin Attenuation linear parameter
	/// \param att_quad Attenuation quadratic parameter
	SpotLight(
		glm::vec3 pos, glm::vec3 dir, glm::vec3 amb, glm::vec3 diff,
		glm::vec3 spec, float cutoff_cos, float outer_cutoff_cos,
		float att_const = 1.0, float att_lin = 0.027,
		float att_quad = 0.0028
	):
		position(pos),
		direction(dir),
		ambient(amb),
		diffuse(diff),
		specular(spec),
		cut_off_cos(cutoff_cos),
		outer_cut_off_cos(outer_cutoff_cos),
		att_constant(att_const),
		att_linear(att_lin),
		att_quadratic(att_quad)
	{};
	
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	
	float cut_off_cos;
	float outer_cut_off_cos;
	
	float att_constant;
	float att_linear;
	float att_quadratic;
};

// Attenuation table
//
// Dist	Const	Linear	Quadratic
// 7	1.0	0.7	1.8
// 13	1.0	0.35	0.44
// 20	1.0	0.22	0.20
// 32	1.0	0.14	0.07
// 50	1.0	0.09	0.032
// 65	1.0	0.07	0.017
// 100	1.0	0.045	0.0075
// 160	1.0	0.027	0.0028
// 200	1.0	0.022	0.0019
// 325	1.0	0.014	0.0007
// 600	1.0	0.007	0.0002
// 3250	1.0	0.0014	0.000007

#endif //LANDSCAPE_LIGHT_H
