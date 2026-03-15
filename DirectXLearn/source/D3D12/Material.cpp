#include "pch.h"
#include "Material.h"

Material MaterialGenerator::generateMaterial(const Texture& texture)
{
	Material mat{};
	mat.texture = texture;

	return mat;
}
