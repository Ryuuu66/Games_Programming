#include "Player.h"
#include "Monster.h"
#include "MathsHelper.h"

Player::Player()
{
	m_input = NULL;
	m_moveSpeed = 7.0f;
	m_currentBoard = NULL;
	m_health = 100.0f;
	m_score = 0;
	m_monstersDefeated = 0;

	shootCounter = 0.0f;
}

Player::Player(Mesh* mesh, Shader* shader, Texture* texture, InputController* input, GameBoard* board)
	: GameObject(mesh, shader, texture, Vector3::Zero)
{
	m_input = input;
	m_moveSpeed = 7.0f;
	m_currentBoard = board;
	m_health = 100.0f;
	m_score = 0;
	m_monstersDefeated = 0;

	TeleportToTileOfType(TileType::NORMAL);

	shootCounter = 0.0f;
	m_bullets = m_currentBoard->getBulletVector();

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());
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
	// Left mouse button to shoot
	if (m_input->GetMouseDown(0) && shootCounter <= 0.0f)
	{
		Shoot();
		shootCounter = 1.0f;
	}
	shootCounter -= timestep;

	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());

	CheckWinCondition();

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

void Player::takeDamage(int amount)
{
	// "abs" keeps a value positive
	m_health -= abs(amount);
}

void Player::CheckWinCondition()
{
	m_monstersDefeated = m_currentBoard->GetDeadEnemyAmount();
}

// The shoot function of a player
void Player::Shoot()
{
	Bullet* the_bullet = NULL;

	for (int i = 0; i < m_bullets.size(); i++)
	{
		if (m_bullets[i]->GetBeingUsed() == false)
		{
			the_bullet = m_bullets[i];
			break;
		}
	}

	if (the_bullet)
	{
		Vector3 m_offset = Vector3(0.0f, 0.0f, 1.0f);    // The offset for player bullet
		
		Vector3 m_view = m_position + Vector3(0, 1, 0);  // This is the position of the camera

		Matrix heading = Matrix::CreateRotationY(m_rotY);

		Matrix lookAtRotation = heading;

		// The offset is still based on the world local, transform it into player's local
		Vector3 spawnAt = Vector3::TransformNormal(m_offset, lookAtRotation);

		spawnAt += m_view;                               // Now it is a point slightly in front of the player

		Vector3 flyTowards = Vector3::TransformNormal(Vector3(0, 0, 10), heading);  // This is the direction player is facing

		the_bullet->SetPosition(spawnAt);                // Need to add some offset so the bullet appear in front of player and will not trigger hitbox
		                                                 
		the_bullet->SetFlyTowards(flyTowards);           // Fly towards the direction player is facing

		the_bullet->SetYRotation(m_rotY);                // Rotate the bullet to the correct direction

		the_bullet->SetBeingUsed(true);  // So the bullet will start updating
	}
}

// Collisions
void Player::OnEnemyCollisionEnter(Enemy* other)
{
	OutputDebugString("Player-Enemy Collision Enter\n");

	// Player got instant killed here
	m_health = 0;
}

void Player::OnEnemyCollisionStay(Enemy* other)
{
	OutputDebugString("Player-Enemy Collision Stay\n");
}

void Player::OnEnemyCollisionExit(Enemy* other)
{
	OutputDebugString("Player-Enemy Collision Exit\n");
}

void Player::OnBulletCollisionEnter(Bullet* other)
{
	OutputDebugString("Player-Bullet Collision Enter\n");

	int damage = rand() % 10 + 5;  // Damage to player is between 5 to 14

	takeDamage(damage);
}

void Player::OnBulletCollisionStay(Bullet* other)
{
	OutputDebugString("Player-Bullet Collision Stay\n");
}

void Player::OnBulletCollisionExit(Bullet* other)
{
	OutputDebugString("Player-Bullet Collision Exit\n");
}

void Player::OnHealthPackCollisionEnter(HealthPack* other)
{
	OutputDebugString("Player-HealthPack Collision Enter\n");
	// Restore health
	int health = rand() % 10 + 5;
	
	m_health += health;
	
	if (m_health > MaxHealth)
	{
		m_health = MaxHealth;
	}
}

void Player::OnHealthPackCollisionStay(HealthPack* other)
{
	OutputDebugString("Player-HealthPack Collision Stay\n");
}

void Player::OnHealthPackCollisionExit(HealthPack* other)
{
	OutputDebugString("Player-HealthPack Collision Exit\n");
}


