#pragma once

#ifndef BULLET_H
#define BULLET_H

#include "PhysicsObject.h"

class Bullet : public PhysicsObject
{
private:
	const Vector3 initialPosition = Vector3(0.0f, -10.0f, 0.0f);  // Where a bullet should stay when it is not being used

	CBoundingBox m_boundingBox;  // Hitbox of a bullet

	bool isBeingUsed;  // For others to know if a bullet is being used
	float m_moveSpeed;  // How fast a bullet travel
	float m_timeInAir;  // How long does a bullet fly

	Vector3 m_flyTowards;

public:

	Bullet(Mesh* mesh, Shader* shader, Texture* texture, Vector3 position);

	void Update(float timestep);


	// Collisions with other objects (Bullet doesn't need to know who it hits, it just need to know it hits something)
	void OnEnemyCollisionEnter(GameObject* other);
	void OnEnemyCollisionStay(GameObject* other);
	void OnEnemyCollisionExit(GameObject* other);

	void OnPlayerCollisionEnter(GameObject* other);
	void OnPlayerCollisionStay(GameObject* other);
	void OnPlayerCollisionExit(GameObject* other);

	// Accessors
	CBoundingBox GetBounds() { return m_boundingBox; }
	bool GetBeingUsed() { return isBeingUsed; }
	Vector3 GetFlyTowards() { return m_flyTowards; }

	// Mutators
	void SetBeingUsed(bool value) { isBeingUsed = value; }
	void SetFlyTowards(Vector3 direction) { m_flyTowards = direction; }

};

#endif
