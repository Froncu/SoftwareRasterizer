#include "ColorRGB.h"

#include <cmath>

#pragma region Operators
ColorRGB ColorRGB::operator*(const ColorRGB& color) const
{
	return ColorRGB(red * color.red, green * color.green, blue * color.blue);
}

ColorRGB ColorRGB::operator*(float scalar) const
{
	return ColorRGB(red * scalar, green * scalar, blue * scalar);
}

ColorRGB ColorRGB::operator/(const ColorRGB& color) const
{
	return ColorRGB(red / color.red, green / color.green, blue / color.blue);
}

ColorRGB ColorRGB::operator/(float scalar) const
{
	return ColorRGB(red / scalar, green / scalar, blue / scalar);
}

ColorRGB ColorRGB::operator+(const ColorRGB& color) const
{
	return ColorRGB(red + color.red, green + color.green, blue + color.blue);
}

ColorRGB ColorRGB::operator-(const ColorRGB& color) const
{
	return ColorRGB(red - color.red, green - color.green, blue - color.blue);
}

const ColorRGB& ColorRGB::operator*=(const ColorRGB& color)
{
	*this = *this * color;
	return *this;
}

const ColorRGB& ColorRGB::operator*=(float scalar)
{
	*this = *this * scalar;
	return *this;
}

const ColorRGB& ColorRGB::operator/=(const ColorRGB& color)
{
	*this = *this / color;
	return *this;
}

const ColorRGB& ColorRGB::operator/=(float scalar)
{
	*this = *this / scalar;
	return *this;
}

const ColorRGB& ColorRGB::operator+=(const ColorRGB& color)
{
	*this = *this + color;
	return *this;
}

const ColorRGB& ColorRGB::operator-=(const ColorRGB& color)
{
	*this = *this - color;
	return *this;
}

ColorRGB operator*(float scalar, const ColorRGB& color)
{
	return color * scalar;
}
#pragma endregion



#pragma region Public Methods
ColorRGB ColorRGB::GetMaxToOne() const
{
	const float maxValue{ std::max(red, std::max(green, blue)) };
	if (maxValue > 1.0f)
		return *this / maxValue;

	return *this;
}

ColorRGB& ColorRGB::MaxToOne()
{
	*this = GetMaxToOne();
	return *this;
}
#pragma endregion