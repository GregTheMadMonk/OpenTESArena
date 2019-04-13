#include "RenderMaterial.h"

RenderMaterial::RenderMaterial()
{
	this->setShader(nullptr);
}

RenderMaterial::RenderMaterial(Double3 (*shader)(const Double3&, const Double2&, const Double3&, const int&))
{
	this->setShader(shader);
}

void RenderMaterial::setShader(Double3 (*shader)(const Double3&, const Double2&, const Double3&, const int&))
{
	this->shade = shader;
}

uint32_t RenderMaterial::shaded(const Double3 &baseColor, const Double2 &texCoord, const Double3 &worldPosition, const int &time) const
{
	if (shade == nullptr) return static_cast<uint32_t>(
			((static_cast<uint8_t>(texCoord.x * 255.0)) << 16) |
			((static_cast<uint8_t>(texCoord.y * 255.0)) << 8) |
			((static_cast<uint8_t>(0.0))));

	const Double3 res = this->shade(baseColor, texCoord, worldPosition, time);

	return static_cast<uint32_t>(
		((static_cast<uint8_t>(res.x * 255.0)) << 16) |
		((static_cast<uint8_t>(res.y * 255.0)) << 8) |
		((static_cast<uint8_t>(res.z * 255.0))));
}
