#pragma once

#ifndef FIRSTPERSONCAMERA_H
#define FIRSTPERSONCAMERA_H

#include "Camera.h"
#include "Player.h"
#include "InputController.h"

class FirstPersonCamera : public Camera
{
private:
	InputController* m_input;

	float m_moveSpeed;
	float m_rotationSpeed;
	float m_heightChangeSpeed;

	float m_heading;
	float m_pitch;


public:
	FirstPersonCamera();
	FirstPersonCamera(InputController* input, Vector3 startPos);
	~FirstPersonCamera();
	void Update(float timestep);

	// Mutators


	// Accessors
	float GetHeading() { return m_heading; }

};



#endif
