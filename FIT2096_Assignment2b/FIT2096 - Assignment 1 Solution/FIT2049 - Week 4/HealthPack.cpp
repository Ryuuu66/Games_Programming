#include "HealthPack.h"

HealthPack::HealthPack(Mesh* mesh, Shader* shader, Texture* texture)
{
	m_isUsed = false;

	m_mesh = mesh;
	m_shader = shader;
	m_texture = texture;

	SetUniformScale(2.0f);
}

HealthPack::~HealthPack()
{

}

void HealthPack::Update(float timestep)
{

}
