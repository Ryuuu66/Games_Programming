#pragma once

#ifndef BULLET_H
#define BULLET_H

#include "PhysicsObject.h"

class Bullet : public PhysicsObject
{
private:

	CBoundingBox m_boundingBox;  // Hitbox of a bullet

	float m_moveSpeed;  // How fast a bullet travel

public:

	Bullet();



	void Update(float timestep);

};

#endif
