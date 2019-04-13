#ifndef RENDERMATERIAL_H
#define RENDERMATERIAL_H

#include "../Math/Vector2.h"
#include "../Math/Vector3.h"

// Serves the function of modifying render color
// Can be used to create effects like blinking, greyscaling etc without modifying the base texture

class RenderMaterial
{
private:
	// returns the resulting pixel color
	Double3 (*shade)(const Double3 &baseColor, const Double2 &texCoord, const Double3 &worldPosition, const int &time);
public:
	RenderMaterial();
	RenderMaterial(Double3 (*shader)(const Double3&, const Double2&, const Double3&, const int&));

	void setShader(Double3 (*shader)(const Double3&, const Double2&, const Double3&, const int&));

	uint32_t shaded(const Double3 &baseColor, const Double2 &texCoord, const Double3&, const int &time) const;
};

#endif
