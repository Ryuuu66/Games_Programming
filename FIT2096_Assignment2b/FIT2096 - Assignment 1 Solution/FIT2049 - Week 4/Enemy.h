#pragma once

#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"

class Enemy : public GameObject
{
private:

	// Enemy's variables
	int m_health;
	int m_skill;
	bool m_isAlive;

	// Use them to make sure the enemy choose a point on the board
	int Board_Width;  
	int Board_Height;

	// The movement of enemy
	float m_moveSpeed;
	int m_moveLogic;

	// Enemy must look at the player
	Vector3 m_playerPosition;


public:

	Enemy();
	Enemy(int newHealth, int newSkill, Mesh* mesh, Shader* shader, Texture* texture);
	Enemy(int newHealth, int newSkill, int newMoveLogic, Mesh* mesh, Shader* shader, Texture* texture);
	~Enemy();

	int Attack();
	void takeDamage(int amount);

	void Update(float timestep);

	// Different enemies have different moving logic
	void move1();
	void move2();
	void move3();
	void move4();
	void move5();


	// Accessors
	bool IsAlive() { return m_isAlive; }
	int GetSkill() { return m_skill; }
	Vector3 GetPlayerPosition() { return m_playerPosition; }

	// Mutators
	void SetPlayerPosition(Vector3 newPos) { m_playerPosition = newPos; }

};


#endif
