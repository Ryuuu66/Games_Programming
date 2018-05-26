#pragma once

#ifndef HEALTHPACK_H
#define HEALTHPACK_H

#include "GameObject.h"

class HealthPack : public GameObject
{
private:

	bool m_isUsed;  // To determine if this health pack is used


public:

	HealthPack(Mesh* mesh, Shader* shader, Texture* texture);
	~HealthPack();

	void Update(float timestep);

	// Mutators
	void SetIsUsed(bool value) { m_isUsed = value; }

	// Accessors
	bool GetIsUsed() { return m_isUsed; }

};

#endif
