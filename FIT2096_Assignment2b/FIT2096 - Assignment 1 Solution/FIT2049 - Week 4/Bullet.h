#pragma once

#ifndef BULLET_H
#define BULLET_H

#include "PhysicsObject.h"

class Bullet : public PhysicsObject
{
private:

	CBoundingBox m_boundingBox;  // Hitbox of a bullet

	bool isBeingUsed;  // For others to know if a bullet is being used
	float m_moveSpeed;  // How fast a bullet travel


public:

	Bullet(Mesh* mesh, Shader* shader, Texture* texture);



	void Update(float timestep);


	// Accessors
	bool GetBeingUsed() { return isBeingUsed; }

	// Mutators
	void SetBeingUsed(bool value) { isBeingUsed = value; }

};

#endif
