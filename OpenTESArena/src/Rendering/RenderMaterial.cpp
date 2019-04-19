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

Double4 RenderMaterial::shadedPixelEmission(const Double3 &baseColor, const Double3 &emissiveColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time) const
{
	// Result is Double4(Red, Green, Blue, Intensity)
	if (this->shaderEmission == nullptr) return Double4(0.0, 0.0, 0.0, 1.0);

	const Double3 rescolor = this->shaderEmission(baseColor, texCoord, worldPosition, worldNormal, time) + emissiveColor;
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

Double4 RenderMaterial::shadedPixelBase(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const Double3 &shading, const int &time) const
{
	// Result is Double4(Red, Green, Blue, Opacity)
	if (this->shaderBase == nullptr) return Double4(0.0, 0.0, 0.0, 1.0);

	const Double3 res = [this, texColor, texCoord, worldPosition, worldNormal, shading, time]()
	{
		const Double3 base = this->shaderBase(texColor, texCoord, worldPosition, worldNormal, time);
		return Double3(base.x * std::min(shading.x, 1.0),
				base.y * std::min(shading.y, 1.0),
				base.z * std::min(shading.z, 1.0));
	}();

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

Double4 RenderMaterial::shadedPixelScreen(const Double3 &baseColor, const Double3 &emissiveColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const Double3 &shading, const int &time) const
{
	// Returns Double4(R, G, B, A)
	if (this->domain == MaterialDomain::Distant) return this->shadedPixelDistant(emissiveColor, texCoord, worldPosition.x, time);

	if (this->domain == MaterialDomain::Unlit)
		return this->shadedPixelEmission(baseColor, emissiveColor, texCoord, worldPosition, worldNormal, time);

	const Double4 emission = [this, baseColor, emissiveColor, texCoord, worldPosition, worldNormal, time]()
	{
		if (this->shaderEmission == nullptr) return Double4(0.0, 0.0, 0.0, 1.0);
		return this->shadedPixelEmission(baseColor, emissiveColor, texCoord, worldPosition, worldNormal, time);
	}();

	const Double4 base = [this, baseColor, texCoord, worldPosition, worldNormal, shading, time]()
	{
		if (this->shaderBase == nullptr) Double4(0.0, 0.0, 0.0, 1.0);
		return this->shadedPixelBase(baseColor, texCoord, worldPosition, worldNormal, shading, time);
	}();

	return Double4(
			std::min(1.0, std::max(emission.x, base.x)),
			std::min(1.0, std::max(emission.y, base.y)),
			std::min(1.0, std::max(emission.z, base.z)),
			base.w);
}
