#include "RenderMaterial.h"

RenderMaterial::RenderMaterial():RenderMaterial(MaterialDomain::Unlit, nullptr, nullptr, nullptr, nullptr)
{ }

RenderMaterial::RenderMaterial(Double3 (*baseShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&)):RenderMaterial(MaterialDomain::Lit, nullptr, baseShader, nullptr, nullptr)
{ }

RenderMaterial::RenderMaterial(const MaterialDomain &materialDomain,
				Double3 (*emissionShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
				Double3 (*baseShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
				double (*opacityShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
				Double3 (*distantShader)(const Double3&, const Double2&, const double&, const int&))
{
	this->domain = materialDomain;
	this->setEmissionShader(emissionShader);
	this->setBaseShader(baseShader);
	this->setOpacityShader(opacityShader);	
	this->setDistantShader(distantShader);
}

RenderMaterial RenderMaterial::createUnlit(Double3 (*unlitShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&))
{
	return RenderMaterial(MaterialDomain::Unlit, unlitShader, nullptr, nullptr, nullptr);
}

RenderMaterial RenderMaterial::createLit(Double3 (*litShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
						Double3 (*emissiveShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&))
{
	return RenderMaterial(MaterialDomain::Lit, emissiveShader, litShader, nullptr, nullptr);
}

RenderMaterial RenderMaterial::createTransparent(Double3 (*litShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
						Double3 (*emissiveShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
						double (*opacityShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&))
{
	return RenderMaterial(MaterialDomain::Transparent, emissiveShader, litShader, opacityShader, nullptr);
}

RenderMaterial RenderMaterial::createDistant(Double3 (*distantShader)(const Double3&, const Double2&, const double&, const int&))
{
	return RenderMaterial(MaterialDomain::Distant, nullptr, nullptr, nullptr, distantShader);
}

void RenderMaterial::setEmissionShader(Double3 (*emissionShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&))
{
	this->shaderEmission = emissionShader;
}

void RenderMaterial::setBaseShader(Double3 (*baseShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&))
{
	this->shaderBase = baseShader;
}

void RenderMaterial::setOpacityShader(double (*opacityShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&))
{
	this->shaderOpacity = opacityShader;
}

void RenderMaterial::setDistantShader(Double3 (*distantShader)(const Double3&, const Double2&, const double&, const int&))
{
	this->shaderDistant = distantShader;
}

Double4 RenderMaterial::shadedPixelEmission(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time) const
{
	if (this->shaderEmission == nullptr) return Double4(0.0, 0.0, 0.0, 1.0);

	const Double3 rescolor = this->shaderEmission(texColor, texCoord, worldPosition, worldNormal, time);
	const double amp = std::max(std::max(rescolor.x, rescolor.y), rescolor.z);

	if (amp <= 1.0) return Double4(
			rescolor.x,
			rescolor.y,
			rescolor.z,
			1.0);

	return Double4(
		rescolor.x / amp,
		rescolor.y / amp,
		rescolor.z / amp,
		amp);
}

Double4 RenderMaterial::shadedPixelBase(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time) const
{
	// put in result 4 bytes: inverted opacity if transparent, 1 byte R, 1 byte G, 1 byte B, where R, G, B are cropped at level of 255
	if (this->shaderBase == nullptr) return Double4(0.0, 0.0, 0.0, 1.0);

	const Double3 res = this->shaderBase(texColor, texCoord, worldPosition, worldNormal, time);

	if ((this->domain == MaterialDomain::Transparent) && (this->shaderOpacity != nullptr)) 
		return Double4(
			(res.x > 1.0)?1.0:res.x,
			(res.y > 1.0)?1.0:res.y,
			(res.z > 1.0)?1.0:res.z,	
			this->shaderOpacity(texColor, texCoord, worldPosition, worldNormal, time));


	return Double4(
		(res.x > 1.0)?1.0:res.x,
		(res.y > 1.0)?1.0:res.y,
		(res.z > 1.0)?1.0:res.z,
		1.0);	
}

double RenderMaterial::shadedPixelOpacity(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time) const
{
	// put in result opacity from 0 to 1.0
	if (this->shaderOpacity == nullptr) return 1.0;

	const double opacity = this->shaderOpacity(texColor, texCoord, worldPosition, worldNormal, time);

	return (opacity > 1.0)?1.0:opacity;
}

Double4 RenderMaterial::shadedPixelDistant(const Double3 &texColor, const Double2 &texCoord, const double &theta, const int &time) const
{
	// result format is basically the same as in shadedPixelEmission
	if (this->shaderDistant == nullptr) return Double4(0.0, 0.0, 0.0, 1.0);

	const Double3 rescolor = this->shaderDistant(texColor, texCoord, theta, time);
	const double amp = std::max(std::max(rescolor.x, rescolor.y), rescolor.z);

	if (amp <= 1.0) return Double4(
			rescolor.x,
			rescolor.y,
			rescolor.z,
			1.0);

	return Double4(
		rescolor.x,
		rescolor.y,
		rescolor.z,
		amp);

}

uint64_t RenderMaterial::shadedPixel(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time) const
{
	// Result format is 4 bytes of base color followed by 4 bytes of emissive color. Basically, it is everything you can know about shader calculation result
	
	const uint32_t emissive = [this, texColor, texCoord, worldPosition, worldNormal, time]()
	{
		if (this->domain == MaterialDomain::Distant) return this->shadedPixelDistant(texColor, texCoord, worldPosition.x, time).toARGB(); // put distant theta in worldPosition.x
		
		return this->shadedPixelEmission(texColor, texCoord, worldPosition, worldNormal, time).toARGB();
	}();

	if ((this->domain == MaterialDomain::Unlit) || (this->domain == MaterialDomain::Distant)) return static_cast<uint64_t>(emissive);

	const uint32_t base = this->shadedPixelBase(texColor, texCoord, worldPosition, worldNormal, time).toARGB();

	return (static_cast<uint64_t>(base) << 32) | static_cast<uint64_t>(emissive);
}

uint32_t RenderMaterial::shadedPixelScreen(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time) const
{
	// Returns 0x00RRGGBB without any additional info
	if (this->domain == MaterialDomain::Distant) return this->shadedPixelDistant(texColor, texCoord, worldPosition.x, time).toARGB();

	if (this->domain == MaterialDomain::Unlit)
		return static_cast<uint32_t>(this->shadedPixelEmission(texColor, texCoord, worldPosition, worldNormal, time).toARGB() & static_cast<uint32_t>(0x00ffffff));

	const Double3 emission = [this, texColor, texCoord, worldPosition, worldNormal, time]()
	{
		if (this->shaderEmission == nullptr) return Double3(0.0, 0.0, 0.0);
		return this->shaderEmission(texColor, texCoord, worldPosition, worldNormal, time);
	}();

	const Double3 base = [this, texColor, texCoord, worldPosition, worldNormal, time]()
	{
		if (this->shaderBase == nullptr) Double3(0.0, 0.0, 0.0);
		return this->shaderBase(texColor, texCoord, worldPosition, worldNormal, time);
	}();

	// @todo: base shader
	return static_cast<uint32_t>(
			(static_cast<uint8_t>(std::min(1.0, std::max(emission.x, base.x)) * 255.0) << 16) |
			(static_cast<uint8_t>(std::min(1.0, std::max(emission.y, base.y)) * 255.0) << 8) |
			(static_cast<uint8_t>(std::min(1.0, std::max(emission.z, base.z)) * 255.0)));

	// emission in Lit & Transparent materials beats base shading color
	//return static_cast<uint32_t>(this->shadedPixelEmission(texColor, texCoord, worldPosition, worldNormal, time) & static_cast<uint32_t>(0x00ffffff));

}
