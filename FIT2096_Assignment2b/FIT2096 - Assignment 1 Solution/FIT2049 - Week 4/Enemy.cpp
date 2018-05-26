#include "Enemy.h"
#include "MathsHelper.h"


Enemy::Enemy()
{
	m_health = 100;
	m_skill = MathsHelper::RandomRange(3, 10);
	m_isAlive = true;
}

Enemy::Enemy(int newHealth, int newSkill, Mesh* mesh, Shader* shader, Texture* texture)
{
	m_health = newHealth;
	m_skill = newSkill;
	m_isAlive = true;

	m_mesh = mesh;
	m_shader = shader;
	m_texture = texture;

	m_moveSpeed = 0.001f;
	m_moveLogic = 0;
}

Enemy::Enemy(int newHealth, int newSkill, int newMoveLogic, Mesh* mesh, Shader* shader, Texture* texture)
{
	m_health = newHealth;
	m_skill = newSkill;
	m_isAlive = true;

	m_mesh = mesh;
	m_shader = shader;
	m_texture = texture;

	m_moveLogic = newMoveLogic;
	// Move speed of enemy depends on their move logic
	if (m_moveLogic == 1)
	{
		m_moveSpeed = 0.005f;
	}
	else if (m_moveLogic == 2)
	{
		m_moveSpeed = 0.004f;
	}
	else if (m_moveLogic == 3)
	{
		m_moveSpeed = 0.003f;
	}
	else if (m_moveLogic == 4)
	{
		m_moveSpeed = 0.002f;
	}
	else if (m_moveLogic == 5)
	{
		m_moveSpeed = 0.001f;
	}
}

Enemy::~Enemy()
{

}


int Enemy::Attack()
{
	// A enemy's attack power is limited to its skill
	return MathsHelper::RandomRange(0, m_skill);
}


void Enemy::takeDamage(int amount)
{
	// "abs" keeps a value positive
	m_health -= abs(amount);

	if (m_health <= 0)
	{
		m_isAlive = false;
	}
}

void Enemy::Update(float timestep)
{
	// TODO Let enemy faces the player
	Vector3 directionToPlayer = m_playerPosition - m_position;
	// Normalize vector
	directionToPlayer.Normalize();
	// Calculate the angle the enemy should be facing
	// There are a couple of ways to do this, atan2 is fairly straightforward
	m_rotY = atan2(directionToPlayer.x, directionToPlayer.z);


	// Handle the movement of enemies
	if (m_moveLogic == 1)
	{
		move1();
	}
	else if (m_moveLogic == 2)
	{
		move2();
	}
	else if (m_moveLogic == 3)
	{
		move3();
	}
	else if (m_moveLogic == 4)
	{
		move4();
	}
	else if (m_moveLogic == 5)
	{
		move5();
	}
}

// Constantly move towards the player
void Enemy::move1()
{
	float distanceToPlayer = Vector3::Distance(m_playerPosition, m_position);

	if (distanceToPlayer >= 0.01f)
	{
		Vector3 directionToPlayer = m_playerPosition - m_position;

		directionToPlayer.Normalize();

		m_position += directionToPlayer * m_moveSpeed;
	}
}

// Constantly run away from the player
void Enemy::move2()
{
	Vector3 directionToPlayer = m_playerPosition - m_position;

	directionToPlayer.Normalize();

	m_position -= directionToPlayer * m_moveSpeed;
}

// Constantly moving to a random point on the board
void Enemy::move3()
{

}

// Constantly move to some point near the player
void Enemy::move4()
{
	float distanceToPlayer = Vector3::Distance(m_playerPosition, m_position);

	if (distanceToPlayer >= 5.0f)
	{
		Vector3 directionToPlayer = m_playerPosition - m_position;

		directionToPlayer.Normalize();

		m_position += directionToPlayer * m_moveSpeed;
	}
}

// Stand still until the player get close, then run away to a random point
void Enemy::move5()
{
	float distanceToPlayer = Vector3::Distance(m_playerPosition, m_position);

	if (distanceToPlayer <= 5.0f)
	{

	}

}

