#include "HealthPack.h"

HealthPack::HealthPack(Mesh* mesh, Shader* shader, Texture* texture) :
	GameObject(mesh, shader, texture)
{
	m_isUsed = false;

	m_mesh = mesh;
	m_shader = shader;
	m_texture = texture;

	SetUniformScale(2.0f);

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());

	m_spawnPoint = Vector3::Zero;
	m_respawnTime = 10.0f;
}

HealthPack::~HealthPack()
{

}

void HealthPack::Update(float timestep)
{
	if (m_isUsed)  // If it is already used, countdown
	{
		m_respawnTime -= timestep;

		if (m_respawnTime <= 0.0f)
		{
			Respawn();
			m_isUsed = false;
			m_respawnTime = 10.0f;
		}
	}

	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());
}

void HealthPack::Respawn()
{
	m_position = m_spawnPoint;
}

// Collisions
void HealthPack::OnPlayerCollisionEnter()
{
	OutputDebugString("HealthPack-Player Collision Enter\n");
	
	m_isUsed = true;
	// Move to other places
	
	m_position = Vector3(-10.0f, -10.0f, 0.0f);  // Sink it to the ground
}

void HealthPack::OnPlayerCollisionStay()
{
	OutputDebugString("HealthPack-Player Collision Stay\n");
}

void HealthPack::OnPlayerCollisionExit()
{
	OutputDebugString("HealthPack-Player Collision Exit\n");
}

