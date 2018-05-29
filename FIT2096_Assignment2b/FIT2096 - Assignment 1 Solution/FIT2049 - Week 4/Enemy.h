#pragma once

#ifndef ENEMY_H
#define ENEMY_H

#include "GameObject.h"
#include "Bullet.h"
#include <vector>

class Enemy : public GameObject
{
private:

	// Enemy's variables
	int m_health;
	int m_skill;
	bool m_isAlive;
	CBoundingBox m_boundingBox;
	std::vector<Bullet*> m_bullets;  // Enemy will also know the vector of bullets

	// Use them to make sure the enemy choose a point on the board
	float Board_Width;  
	float Board_Height;

	// The movement of enemy
	Vector3 m_randomPoint;
	bool m_isMoving;
	float m_moveSpeed;
	int m_moveLogic;

	// Different enemies have different moving logic
	void move1();
	void move2();
	void move3();
	void move4();
	void move5();

	// Enemy must look at the player
	Vector3 m_playerPosition;

	// Function to be called in Update function, a bullet will shoot out
	void Shoot();
	float shootCounter;  // Enemy will only shoot when the counter reach 0

	// Helper functions
	float RandomRange(float min, float max);

public:

	Enemy();
	Enemy(int newHealth, int newSkill, Mesh* mesh, Shader* shader, Texture* texture);
	Enemy(int newHealth, int newSkill, int newMoveLogic, Mesh* mesh, Shader* shader, Texture* texture);
	~Enemy();

	void takeDamage(int amount);

	void Update(float timestep);

	// Collisions with other objects (Enemy doesn't need to know the collision with enemy)
	void OnBulletCollisionEnter(Bullet* other);  // Take damage here
	void OnBulletCollisionStay(Bullet* other);
	void OnBulletCollisionExit(Bullet* other);


	// Accessors
	bool IsAlive() { return m_isAlive; }
	int GetSkill() { return m_skill; }
	Vector3 GetPlayerPosition() { return m_playerPosition; }
	CBoundingBox GetBounds() { return m_boundingBox; }

	// Mutators
	void SetPlayerPosition(Vector3 newPos) { m_playerPosition = newPos; }
	void SetBoardWidth(int width) { Board_Width = (float) width; }
	void SetBoardHeight(int height) { Board_Height = (float) height; }
	void SetBulletVector(std::vector<Bullet*> the_vector) { m_bullets = the_vector; }
};


#endif
