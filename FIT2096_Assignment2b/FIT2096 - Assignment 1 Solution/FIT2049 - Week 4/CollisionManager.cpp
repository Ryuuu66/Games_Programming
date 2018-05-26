#include "CollisionManager.h"

CollisionManager::CollisionManager(std::vector<Player*>* players, std::vector<Enemy*>* enemies)
{
	m_players = players;
	m_enemies = enemies;

	// Clear our arrays to 0 (NULL)
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	memset(m_previousCollisions, 0, sizeof(m_previousCollisions));

	m_nextCurrentCollisionSlot = 0;
}

void CollisionManager::CheckCollisions()
{
	// Check all collisions
	PlayerToEnemy();


	// Move all current collisions into previous
	memcpy(m_previousCollisions, m_currentCollisions, sizeof(m_currentCollisions));

	// Clear out current collisions
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));

	// Now current collisions is empty, we'll start adding from the start again
	m_nextCurrentCollisionSlot = 0;

}

bool CollisionManager::ArrayContainsCollision(GameObject* arrayToSearch[], GameObject* first, GameObject* second)
{
	// See if these two GameObjects appear one after the other in specified collisions array
	// Stop one before length so we don't overrun as we'll be checking two elements per iteration
	for (int i = 0; i < MAX_ALLOWED_COLLISIONS - 1; i += 2)
	{
		if ((arrayToSearch[i] == first && arrayToSearch[i + 1] == second) ||
			arrayToSearch[i] == second && arrayToSearch[i + 1] == first)
		{
			// Found them!
			return true;
		}
	}

	// These objects were not colliding last frame
	return false;
}

void CollisionManager::AddCollision(GameObject* first, GameObject* second)
{
	// Add the two colliding objects to the current collisions array
	// We keep track of the next free slot so no searching is required
	m_currentCollisions[m_nextCurrentCollisionSlot] = first;
	m_currentCollisions[m_nextCurrentCollisionSlot + 1] = second;

	m_nextCurrentCollisionSlot += 2;
}

void CollisionManager::PlayerToEnemy()
{
	// Here we check each player against each enemy
	for (unsigned int i = 0; i < m_players->size(); i++)
	{
		for (unsigned int j = 0; j < m_enemies->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Player* player = (*m_players)[i];
			Enemy* enemy = (*m_enemies)[j];

			CBoundingBox playerBounds = player->GetBounds();
			CBoundingBox enemyBounds = enemy->GetBounds();

			// Are they colliding this frame?
			bool isColliding = CheckCollision(playerBounds, enemyBounds);

			// Were they colliding last frame?
			bool wasColliding = ArrayContainsCollision(m_previousCollisions, player, enemy);

			// For this part, enemy doesn't need to know it collides with a player
			// But a player must know it collides with an enemy
			if (isColliding)
			{
				// Register the collision
				AddCollision(player, enemy);

				if (wasColliding)
				{
					// Collision Stay
					player->OnEnemyCollisionStay(enemy);
				}
				else
				{
					// Collision Enter
					player->OnEnemyCollisionEnter(enemy);
				}
			}
			else
			{
				if (wasColliding)
				{
					// Collision Exit
					player->OnEnemyCollisionExit(enemy);
				}

			}
		}
	}
}