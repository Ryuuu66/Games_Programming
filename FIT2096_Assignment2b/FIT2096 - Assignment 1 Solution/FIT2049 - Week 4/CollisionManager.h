#pragma once

#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <vector>
#include "Collisions.h"
#include "Player.h"
#include "Enemy.h"

#define MAX_ALLOWED_COLLISIONS 2048

class CollisionManager
{
private:
	// Check collision of player, enemies, bullets, healthpacks, walls, etc
	std::vector<Player*>* m_players;
	std::vector<Enemy*>* m_enemies;
	std::vector<Bullet*>* m_bullets;
	std::vector<HealthPack*>* m_healthPacks;

	GameObject* m_currentCollisions[MAX_ALLOWED_COLLISIONS];

	// We need to know what objects were colliding last frame so we can determine if a collision has just begun or ended
	GameObject* m_previousCollisions[MAX_ALLOWED_COLLISIONS];

	int m_nextCurrentCollisionSlot;

	// Check if we already know about two objects colliding
	bool ArrayContainsCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second);

	// Register that a collision has occurred
	void AddCollision(GameObject* first, GameObject* second);

	// Collision check helpers
	void PlayerToEnemy();
	void PlayerToBullet();
	void EnemyToBullet();
	void PlayerToHealthPack();


public:
	CollisionManager(std::vector<Player*>* players, std::vector<Enemy*>* enemies, std::vector<Bullet*>* bullets, std::vector<HealthPack*>* healthPacks);
	void CheckCollisions();

};

#endif


