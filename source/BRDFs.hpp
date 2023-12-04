#pragma once

#include "ColorRGB.h"
#include "Mathematics.hpp"
#include "Vector3.h"

ColorRGB Lambert(float diffuseReflectance, const ColorRGB& diffuseColor)
{
	return diffuseReflectance * diffuseColor / PI;
}

ColorRGB Phong(const ColorRGB& specularReflectance, float phongExponent, const Vector3& lightDirection, const Vector3& viewDirection, const Vector3& normal)
{
	const Vector3 reflectedLightDirection{ Vector3::Reflect(lightDirection, normal) };
	const float negatedDot{ -Vector3::Dot(reflectedLightDirection, viewDirection) };
	if (negatedDot <= 0.0f)
		return BLACK;

	return specularReflectance * powf(negatedDot, phongExponent);
}