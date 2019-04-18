#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"
#include "../Math/Vector4.h"

// Serves the function of modifying render color
// Can be used to create effects like blinking, greyscaling etc without modifying the base texture

class RenderMaterial
{
private:
	// "Shaders" return the resulting pixel colors
	Double3 (*shaderEmission)(const Double3 &texColor,const Double2 &texCoord, const Double3 &worldPosition, const Double3 &normal, const int &time);
	Double3 (*shaderBase)(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &normal, const int &time);
	double (*shaderOpacity)(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time);
	Double3 (*shaderDistant)(const Double3 &texColor, const Double2 &texCoord, const double &theta, const int &time);
public:
	// MaterialDomain defines material behaviour
	enum class MaterialDomain
	{
		Unlit,		// processes emissive color
		Lit,		// processes emissive color & base color (with lighting)
		Transparent,	// same as Lit, but with opacity
		Distant		// almost same as Unlit, but normal is not passed
	};

	// Helper functions
	static uint32_t toColorInt(const double &alpha, const Double3 &color);
	
	MaterialDomain domain;

	// Constructors
	RenderMaterial();
	RenderMaterial(Double3 (*baseShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&)); // obsolete, use creators
private:
	// Private constructor. To create materials, use static creators
	RenderMaterial(const MaterialDomain &materialDomain,
			Double3 (*emissionShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
			Double3 (*baseShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
			double (*opacityShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
			Double3 (*distantShader)(const Double3&, const Double2&, const double&, const int&));
public:
	// creators
	static RenderMaterial createUnlit(Double3 (*unlitShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&));
	static RenderMaterial createLit(Double3 (*litShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&), 
					Double3 (*emissiveShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&));
	static RenderMaterial createTransparent(Double3 (*litShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
						Double3 (*emissiveShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&),
						double (*opacityShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&));
	static RenderMaterial createDistant(Double3 (*distantShader)(const Double3&, const Double2&, const double&, const int&));

	// set shadersi
	void setBaseShader(Double3 (*baseShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&));
	void setEmissionShader(Double3 (*emissiveShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&));
	void setOpacityShader(double (*opacityShader)(const Double3&, const Double2&, const Double3&, const Double3&, const int&));
	void setDistantShader(Double3 (*distantShader)(const Double3&, const Double2&, const double&, const int&));

	// returns the resulting pixel emissive color (for external call)
	Double4 shadedPixelEmission(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time) const;

	// returns the resulting lit pixel color 
	Double4 shadedPixelBase(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition,  const Double3 &worldNormal, const int &time) const;

	// returns the pixel opacity
	double shadedPixelOpacity(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal,  const int &time) const;

	// returns the distant pixel color
	Double4 shadedPixelDistant(const Double3 &texColor, const Double2 &texCoord, const double &theta, const int &time) const;	

	// full shaded info (is uint64_t a good choice?)
	uint64_t shadedPixel(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int& time) const;

	// pixel screen color
	uint32_t shadedPixelScreen(const Double3 &texColor, const Double2 &texCoord, const Double3 &worldPosition, const Double3 &worldNormal, const int &time) const;
};

#endif
