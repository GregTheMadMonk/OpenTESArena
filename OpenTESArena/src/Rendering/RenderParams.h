#ifndef RENDER_PARAMS_H
#define RENDER_PARAMS_H

#include <cstdint>

// Render params masks
namespace RenderParams
{
	constexpr uint32_t PostProcessing = 0b1;
	constexpr uint32_t Bloom = 0b10; 
}

#endif
