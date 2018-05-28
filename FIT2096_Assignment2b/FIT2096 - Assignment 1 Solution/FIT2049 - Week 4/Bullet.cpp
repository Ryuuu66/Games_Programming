#include "Bullet.h"

Bullet::Bullet(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position)
	:PhysicsObject(mesh, shader, texture, position)
{
	isBeingUsed = false;
	m_moveSpeed = 1.0f;
	m_timeInAir = 0.0f;

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());

	m_flyTowards = Vector3::Zero;
}


void Bullet::Update(float timestep)
{
	if (isBeingUsed)
	{
		m_timeInAir += timestep;

		// If the bullet flys for a certain time
		if (m_timeInAir >= 5.0f)  
		{
			isBeingUsed = false;
			m_position = initialPosition;
			m_timeInAir = 0.0f;  // Reset time in air
		}
		// Still flying
		else
		{
			m_position += m_flyTowards * m_moveSpeed * timestep;
		}
	}
	
	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());
}
