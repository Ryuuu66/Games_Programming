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
	: GameObject(mesh, shader, texture, Vector3::Zero)
{
	m_health = newHealth;
	m_skill = newSkill;
	m_isAlive = true;

	m_mesh = mesh;
	m_shader = shader;
	m_texture = texture;

	m_boundingBox = CBoundingBox(m_position + m_mesh->GetMin(), m_position + m_mesh->GetMax());

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
		m_moveSpeed = 0.01f;
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

	// Keep bounds up to date with position
	m_boundingBox.SetMin(m_position + m_mesh->GetMin());
	m_boundingBox.SetMax(m_position + m_mesh->GetMax());
}

// Constantly move towards the player
void Enemy::move1()
{
	float distanceToPlayer = Vector3::Distance(m_playerPosition, m_position);

	if (distanceToPlayer >= 0.01f)
	{
		isMoving = true;

		Vector3 directionToPlayer = m_playerPosition - m_position;

		directionToPlayer.Normalize();

		m_position += directionToPlayer * m_moveSpeed;
	}
	else
	{
		isMoving = false;
	}
}

// Constantly run away from the player
void Enemy::move2()
{
	isMoving = true;

	Vector3 directionToPlayer = m_playerPosition - m_position;

	directionToPlayer.Normalize();

	m_position -= directionToPlayer * m_moveSpeed;
}

// Constantly moving to a random point on the board
void Enemy::move3()
{
	if (isMoving)
	{
		Vector3 directionToPoint = m_randomPoint - m_position;
		directionToPoint.Normalize();
		m_position += directionToPoint * m_moveSpeed;

		float distanceToPoint = Vector3::Distance(m_position, m_randomPoint);
		// Stop moving if close enough
		if (distanceToPoint <= 0.01f)
		{
			isMoving = false;
		}
	}
	else if (!isMoving)
	{
		// Generate a random point
		isMoving = true;

		float pointX = RandomRange(1.0, Board_Width);
		float pointZ = RandomRange(1.0, Board_Height);

		m_randomPoint = Vector3(pointX, 0.0, pointZ);
	}
}

// Constantly move to some point near the player
void Enemy::move4()
{
	float distanceToPlayer = Vector3::Distance(m_playerPosition, m_position);

	if (distanceToPlayer >= 5.0f)
	{
		isMoving = true;

		Vector3 directionToPlayer = m_playerPosition - m_position;

		directionToPlayer.Normalize();

		m_position += directionToPlayer * m_moveSpeed;
	}
	else
	{
		isMoving = false;
	}
}

// Stand still until the player get close, then run away to a random point
void Enemy::move5()
{
	float distanceToPlayer = Vector3::Distance(m_playerPosition, m_position);

	if (isMoving)
	{
		Vector3 directionToPoint = m_randomPoint - m_position;
		directionToPoint.Normalize();
		m_position += directionToPoint * m_moveSpeed;

		float distanceToPoint = Vector3::Distance(m_position, m_randomPoint);
		// Stop moving if close enough
		if (distanceToPoint <= 0.01f)
		{
			isMoving = false;
		}
	}
	else if (!isMoving && distanceToPlayer <= 5.0f)
	{
		// Generate a random point
		isMoving = true;

		float pointX = RandomRange(1.0, Board_Width);
		float pointZ = RandomRange(1.0, Board_Height);

		m_randomPoint = Vector3(pointX, 0.0, pointZ);
	}
}



float Enemy::RandomRange(float min, float max)
{
	// rand gives us a number between 0 and RAND_MAX
	// Divide by RAND_MAX moves it into a 0-1 range
	// max - min gives us the range our number needs to be in
	// This is still relative to 0 though, so we shift it accross by min to move into the actual range

	return ((rand() / float(RAND_MAX)) * (max - min)) + min;
}

