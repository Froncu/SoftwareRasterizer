#pragma once

struct ColorRGB
{
public:
	ColorRGB operator*(const ColorRGB& color) const;
	ColorRGB operator*(float scalar) const;
	ColorRGB operator/(const ColorRGB& color) const;
	ColorRGB operator/(float scalar) const;
	ColorRGB operator+(const ColorRGB& color) const;
	ColorRGB operator-(const ColorRGB& color) const;

	const ColorRGB& operator*=(const ColorRGB& color);
	const ColorRGB& operator*=(float scalar);
	const ColorRGB& operator/=(const ColorRGB& color);
	const ColorRGB& operator/=(float scalar);
	const ColorRGB& operator+=(const ColorRGB& color);
	const ColorRGB& operator-=(const ColorRGB& color);

	ColorRGB GetMaxToOne() const;
	ColorRGB& MaxToOne();

	float
		red,
		green,
		blue;
};

ColorRGB operator*(float scalar, const ColorRGB& color);

static constexpr ColorRGB
RED{ 1.0f, 0.0f, 0.0f },
BLUE{ 0.0f, 0.0f, 1.0f },
GREEN{ 0.0f, 1.0f, 0.0f },
YELLOW{ 1.0f, 1.0f, 0.0f },
CYAN{ 0.0f, 1.0f, 1.0f },
MAGENTA{ 1.0f, 0.0f, 1.0f },
WHITE{ 1.0f, 1.0f, 1.0f },
BLACK{ 0.0f, 0.0f, 0.0f },
GRAY{ 0.5f ,0.5f ,0.5f };