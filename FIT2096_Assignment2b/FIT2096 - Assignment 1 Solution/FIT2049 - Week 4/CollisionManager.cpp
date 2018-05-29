#include "CollisionManager.h"

CollisionManager::CollisionManager(std::vector<Player*>* players, std::vector<Enemy*>* enemies, std::vector<Bullet*>* bullets, std::vector<HealthPack*>* healthPacks)
{
	m_players = players;
	m_enemies = enemies;
	m_bullets = bullets;
	m_healthPacks = healthPacks;

	// Clear our arrays to 0 (NULL)
	memset(m_currentCollisions, 0, sizeof(m_currentCollisions));
	memset(m_previousCollisions, 0, sizeof(m_previousCollisions));

	m_nextCurrentCollisionSlot = 0;
}

void CollisionManager::CheckCollisions()
{
	// Check all collisions
	PlayerToEnemy();
	PlayerToBullet();
	EnemyToBullet();
	PlayerToHealthPack();

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

void CollisionManager::PlayerToBullet()
{
	// Here we check each player against each bullet
	for (unsigned int i = 0; i < m_players->size(); i++)
	{
		for (unsigned int j = 0; j < m_bullets->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Player* player = (*m_players)[i];
			Bullet* bullet = (*m_bullets)[j];

			// Only check collision if a bullet is being used
			if (bullet->GetBeingUsed())
			{
				CBoundingBox playerBounds = player->GetBounds();
				CBoundingBox bulletBounds = bullet->GetBounds();

				// Are they colliding this frame?
				bool isColliding = CheckCollision(playerBounds, bulletBounds);

				// Were they colliding last frame?
				bool wasColliding = ArrayContainsCollision(m_previousCollisions, player, bullet);

				// For this part, enemy doesn't need to know it collides with a player
				// But a player must know it collides with an enemy
				if (isColliding)
				{
					// Register the collision
					AddCollision(player, bullet);

					if (wasColliding)
					{
						// Collision Stay
						player->OnBulletCollisionStay(bullet);
						bullet->OnPlayerCollisionStay(player);
					}
					else
					{
						// Collision Enter
						player->OnBulletCollisionEnter(bullet);
						bullet->OnPlayerCollisionEnter(player);
					}
				}
				else
				{
					if (wasColliding)
					{
						// Collision Exit
						player->OnBulletCollisionExit(bullet);
						bullet->OnPlayerCollisionExit(player);
					}
				}
			}
		}
	}
}

void CollisionManager::EnemyToBullet()
{
	// Here we check each enemy against each bullet
	for (unsigned int i = 0; i < m_enemies->size(); i++)
	{
		for (unsigned int j = 0; j < m_bullets->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Enemy* enemy = (*m_enemies)[i];
			Bullet* bullet = (*m_bullets)[j];

			// Only check collision if a bullet is being used
			if (bullet->GetBeingUsed())
			{
				CBoundingBox enemyBounds = enemy->GetBounds();
				CBoundingBox bulletBounds = bullet->GetBounds();

				// Are they colliding this frame?
				bool isColliding = CheckCollision(enemyBounds, bulletBounds);

				// Were they colliding last frame?
				bool wasColliding = ArrayContainsCollision(m_previousCollisions, enemy, bullet);

				// For this part, enemy doesn't need to know it collides with a player
				// But a player must know it collides with an enemy
				if (isColliding)
				{
					// Register the collision
					AddCollision(enemy, bullet);

					if (wasColliding)
					{
						// Collision Stay
						enemy->OnBulletCollisionStay(bullet);
						bullet->OnEnemyCollisionStay(enemy);	
					}
					else
					{
						// Collision Enter
						enemy->OnBulletCollisionEnter(bullet);
						bullet->OnEnemyCollisionEnter(enemy);
					}
				}
				else
				{
					if (wasColliding)
					{
						// Collision Exit
						enemy->OnBulletCollisionExit(bullet);
						bullet->OnEnemyCollisionExit(enemy);
					}
				}
			}

		}

	}
}

void CollisionManager::PlayerToHealthPack()
{
	// Here we check each player against each health pack
	for (unsigned int i = 0; i < m_players->size(); i++)
	{
		for (unsigned int j = 0; j < m_healthPacks->size(); j++)
		{
			// Don't need to store pointer to these objects again but favouring clarity
			// Can't index into these directly as they're a pointer to a vector. We need to dereference them first
			Player* player = (*m_players)[i];
			HealthPack* healthPack = (*m_healthPacks)[j];

			// Only check collision if a health pack is available
			if (!healthPack->GetIsUsed())
			{
				CBoundingBox playerBounds = player->GetBounds();
				CBoundingBox healthBounds = healthPack->GetBounds();

				// Are they colliding this frame?
				bool isColliding = CheckCollision(playerBounds, healthBounds);

				// Were they colliding last frame?
				bool wasColliding = ArrayContainsCollision(m_previousCollisions, player, healthPack);

				// For this part, enemy doesn't need to know it collides with a player
				// But a player must know it collides with an enemy
				if (isColliding)
				{
					// Register the collision
					AddCollision(player, healthPack);

					if (wasColliding)
					{
						// Collision Stay
						player->OnHealthPackCollisionStay(healthPack);
						healthPack->OnPlayerCollisionStay();
					}
					else
					{
						// Collision Enter
						player->OnHealthPackCollisionEnter(healthPack);
						healthPack->OnPlayerCollisionEnter();
					}
				}
				else
				{
					if (wasColliding)
					{
						// Collision Exit
						player->OnHealthPackCollisionExit(healthPack);
						healthPack->OnPlayerCollisionExit();
					}
				}

			}

		}

	}
}

