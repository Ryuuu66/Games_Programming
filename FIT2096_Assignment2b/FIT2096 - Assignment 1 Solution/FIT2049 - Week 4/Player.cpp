#include "Player.h"
#include "Monster.h"
#include "MathsHelper.h"

Player::Player()
{
	m_input = NULL;
	m_moveSpeed = 5.0f;
	m_currentBoard = NULL;
	m_health = 100.0f;
	m_score = 0;
	m_monstersDefeated = 0;
}

Player::Player(Mesh* mesh, Shader* shader, Texture* texture, InputController* input, GameBoard* board)
	: GameObject(mesh, shader, texture, Vector3::Zero)
{
	m_input = input;
	m_moveSpeed = 5.0f;
	m_currentBoard = board;
	m_health = 100.0f;
	m_score = 0;
	m_monstersDefeated = 0;

	TeleportToTileOfType(TileType::NORMAL);
}

Player::~Player() {}

void Player::Update(float timestep)
{
	// This is the first time a moving object in our game can rotate and change where
	// it's looking. Suddenly moving along the world axes is not useful to us anymore.
	// We need to work out what direction is forward for this particular object. 

	// We'll start by declaring a forward vector in world space
	Vector3 worldForward = Vector3(0, 0, 1);

	// Next we'll wrap up our Y rotation in a matrix (remember matrices transform vectors)
	Matrix heading = Matrix::CreateRotationY(m_rotY);

	// Finally, we'll transform our world forward vector by the heading matrix which 
	// will essentially spin it from a world forward into a local forward which takes
	// the object's rotation into account.
	Vector3 localForward = Vector3::TransformNormal(worldForward, heading);

	// Transform a world right vector from world space into local space
	Vector3 localRight = Vector3::TransformNormal(Vector3(1, 0, 0), heading);


	if (m_input->GetKeyHold('W'))
	{
		// Move along our local forward vector
		m_position += localForward * m_moveSpeed * timestep;
	}
	if (m_input->GetKeyHold('S'))
	{
		// Move along our local forward vector
		m_position -= localForward * m_moveSpeed * timestep;
	}
	if (m_input->GetKeyHold('A'))
	{
		// Move along our local right vector
		m_position -= localRight * m_moveSpeed * timestep;
	}
	if (m_input->GetKeyHold('D'))
	{
		// Move along our local right vector
		m_position += localRight * m_moveSpeed * timestep;
	}


	/*
	// Constantly step towards target position
	// Could add a distance check here, however seeing we only have one player we'll be fine
	m_position = Vector3::Lerp(m_position, m_targetPosition, timestep * m_moveSpeed);
	
	// Update player position for GameBoard
	m_currentBoard->SetCurrentPlayerPosition(m_position);

	// We need to identify the frame input was received so we can perform common logic
	// outside of the GetKeyDown IF statements below.
	bool didJustMove = false;

	if (m_input->GetKeyDown('W'))
	{
		if (CanMoveHere(m_targetPosition + Vector3(0, 0, 1)))
		{
			// Deactivate tile before we update the target position. This prevents the trail
			// of disabled tiles from getting ahead of the player. It always disables the tile
			// we are leaving as opposed to disabling the one we are moving onto.
			m_currentBoard->DeactivateTile(m_targetPosition.x, m_targetPosition.z);
			m_targetPosition += Vector3(0, 0, 1);
			didJustMove = true;
		}
	}
	if (m_input->GetKeyDown('S'))
	{
		if (CanMoveHere(m_targetPosition + Vector3(0, 0, -1)))
		{
			m_currentBoard->DeactivateTile(m_targetPosition.x, m_targetPosition.z);
			m_targetPosition += Vector3(0, 0, -1);
			didJustMove = true;
		}
	}
	if (m_input->GetKeyDown('A'))
	{
		if (CanMoveHere(m_targetPosition + Vector3(-1, 0, 0)))
		{
			m_currentBoard->DeactivateTile(m_targetPosition.x, m_targetPosition.z);
			m_targetPosition += Vector3(-1, 0, 0);
			didJustMove = true;
		}
	}
	if (m_input->GetKeyDown('D'))
	{
		if (CanMoveHere(m_targetPosition + Vector3(1, 0, 0)))
		{
			m_currentBoard->DeactivateTile(m_targetPosition.x, m_targetPosition.z);
			m_targetPosition += Vector3(1, 0, 0);
			didJustMove = true;
		}
	}

	
	if (didJustMove)
	{
		// We want to react once per move (not every frame)
		FinishTurn();
	}
	*/

}

void Player::FinishTurn()
{
	// Even though this is called in an Update function, it's essentially event-driven as it
	// is only called the frame we receive input from the keyboard.


	// Handle collisions here

	/*
	Player-Bullet
	Player-Enemy
	Player-Healthpack
	Player-Wall
	*/


	// React to tile we're standing on
	ReactToTile();
	CheckIfTrapped();

}

bool Player::CanMoveHere(Vector3 target)
{
	// Asks the GameBoard for the type of the target tile
	// We can't step onto a wall or a disabled tile

	TileType targetTileType = m_currentBoard->GetTileTypeForPosition(target.x, target.z);

	return targetTileType != TileType::DISABLED &&
		   targetTileType != TileType::WALL &&
		   targetTileType != TileType::INVALID;
}

void Player::CheckIfTrapped()
{
	// Using our target position, ask the GameBoard for the type of 
	// our surrounding tiles and check if we are surrounded by walls or grey tiles

	m_isTrapped = !CanMoveHere(m_targetPosition + Vector3(0, 0, 1)) &&  // Above
				  !CanMoveHere(m_targetPosition + Vector3(0, 0, -1)) && // Below
				  !CanMoveHere(m_targetPosition + Vector3(-1, 0, 0)) && // Left
				  !CanMoveHere(m_targetPosition + Vector3(1, 0, 0));    // Right
}

void Player::ReactToTile()
{
	TileType targetTileType = m_currentBoard->GetTileTypeForPosition(m_targetPosition.x, m_targetPosition.z);

	switch (targetTileType)
	{
	case TileType::HEALTH:
		// Deactivate the healthpack at target position
		m_currentBoard->GetHealthPack(m_targetPosition)->SetIsUsed(true);

		m_health += 5;
		break;
	case TileType::DAMAGE:
		m_health -= 10;
		break;
	case TileType::TELEPORT:
		TeleportToTileOfType(TileType::TELEPORT);
		break;
	case TileType::MONSTER_VAR1:

	//case TileType::MONSTER_VAR2:  // We only need one monster tile in this game
		// We'll react the same to both types of monster. TODO how could we differentiate them?
		if (m_currentBoard->GetEnemy(m_targetPosition) != NULL)
		{
			DoBattle(m_currentBoard->GetEnemy(m_targetPosition));
		}
		// DoMonsterBattle();
		break;
	default:
		break;
	}
}

void Player::TeleportToTileOfType(TileType type)
{
	Tile* destinationTile = m_currentBoard->GetRandomTileOfType(type);

	if (destinationTile)
	{
		// We need to set both the current position and the target
		// The only time the player remains still is when these two positions match
		m_targetPosition = destinationTile->GetPosition();
		m_position = destinationTile->GetPosition();

		// Tiles start up in the sky and fall down. Ensure player starts on the ground.
		m_targetPosition.y = 0.0f;
		m_position.y = 0.0f;
	}
}

int Player::Attack()
{
	// The player is much stronger than any monster on the board
	return MathsHelper::RandomRange(0, 15);
}

void Player::takeDamage(int amount)
{
	// "abs" keeps a value positive
	m_health -= abs(amount);
}


// This function is not used in this game
/*
void Player::DoMonsterBattle()
{
	// A battle takes place within a single frame.

	// Slightly hacky, but we only need a Monster when a battle is about to occur.
	// The Player creates a Monster then fights it. Asking the GameBoard for a Monster
	// would perhaps be a more proper way of doing this, however storing Monsters in Tiles
	// would be unnecessary work for this implementation.
	Monster monster = Monster();

	// We keep fighting until someone dies
	while (m_health > 0 && monster.IsAlive())
	{
		int playerAttackValue = Attack();
		int monsterAttackValue = monster.Attack();

		if (playerAttackValue > monsterAttackValue)
		{
			// Player wins the round - the monster will receive some damage
			monster.BeHit(playerAttackValue - monsterAttackValue);
		}
		else
		{
			// Monster wins round - the player will receive some damage
			takeDamage(monsterAttackValue - playerAttackValue);
		}
	}

	if (!monster.IsAlive())
	{
		// If the player won the overall battle, increment our score
		m_score += monster.GetSkill();
		m_monstersDefeated++;
	}
}
*/

void Player::DoBattle(Enemy* currentEnemy)
{
	// Default state, player attack first
	bool playerTurn = true;
	bool enemyTurn = false;

	// If both are alive, keep battle
	while (m_health > 0 && currentEnemy->IsAlive())
	{
		if (playerTurn && m_health > 0)
		{
			int playerAttackValue = Attack();

			currentEnemy->takeDamage(playerAttackValue);

			playerTurn = false;
			enemyTurn = true;
		}
		// Check if enemy is still alive after player's attack
		if (enemyTurn && currentEnemy->IsAlive())
		{
			int enemyAttackValue = currentEnemy->Attack();

			takeDamage(enemyAttackValue);

			enemyTurn = false;
			playerTurn = true;
		}
	}
	// Battle ends

	// If player still alive
	if (!currentEnemy->IsAlive())
	{
		m_score += currentEnemy->GetSkill();
		m_monstersDefeated += 1;
	}
}