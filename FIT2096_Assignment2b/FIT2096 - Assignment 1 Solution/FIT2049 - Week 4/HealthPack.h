#pragma once

#ifndef HEALTHPACK_H
#define HEALTHPACK_H

#include "GameObject.h"

class HealthPack : public GameObject
{
private:

	bool m_isUsed;  // To determine if this health pack is used

	CBoundingBox m_boundingBox;

	// Respawn
	Vector3 m_spawnPoint;  // The place where it should respawn
	float m_respawnTime;
	void Respawn();  // Allow a healthpack to respawn

public:

	HealthPack(Mesh* mesh, Shader* shader, Texture* texture);
	~HealthPack();

	void Update(float timestep);

	// Collisions
	// Health Pack doesn't need to know about the player
	void OnPlayerCollisionEnter();
	void OnPlayerCollisionStay();
	void OnPlayerCollisionExit();

	// Mutators
	void SetIsUsed(bool value) { m_isUsed = value; }
	void SetSpawnPoint(Vector3 position) { m_spawnPoint = position; }

	// Accessors
	bool GetIsUsed() { return m_isUsed; }
	CBoundingBox GetBounds() { return m_boundingBox; }
	Vector3 GetSpawnPoint() { return m_spawnPoint; }

};

#endif
