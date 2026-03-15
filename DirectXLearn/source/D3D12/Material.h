#pragma once
#include "Texture.h"

struct Material {
	Texture texture{};
};

class MaterialGenerator {
public:
	MaterialGenerator() = default;
	~MaterialGenerator() = default;

	MaterialGenerator(const MaterialGenerator&) = delete;
	MaterialGenerator operator=(const MaterialGenerator&) = delete;

	static Material generateMaterial(const Texture& texture);
};