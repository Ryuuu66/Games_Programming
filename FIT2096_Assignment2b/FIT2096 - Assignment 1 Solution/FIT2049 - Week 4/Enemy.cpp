#include "Enemy.h"
#include "MathsHelper.h"


Enemy::Enemy()
{
	m_health = 100;
	m_skill = MathsHelper::RandomRange(3, 10);
	m_isAlive = true;

	m_isMoving = false;

	shootCounter = 5.0f;
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
	m_isMoving = false;

	shootCounter = 5.0f;
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

	shootCounter = 5.0f;
	m_isMoving = false;
	m_moveLogic = newMoveLogic;
	// Move speed of enemy depends on their move logic
	if (m_moveLogic == 1)
	{
		m_moveSpeed = 0.05f;
	}
	else if (m_moveLogic == 2)
	{
		m_moveSpeed = 0.04f;
	}
	else if (m_moveLogic == 3)
	{
		m_moveSpeed = 0.03f;
	}
	else if (m_moveLogic == 4)
	{
		m_moveSpeed = 0.02f;
	}
	else if (m_moveLogic == 5)
	{
		m_moveSpeed = 0.01f;
	}
}

Enemy::~Enemy()
{

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
	if (IsAlive())
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

		// Handle the shooting of enemy
		shootCounter -= timestep;

		if (shootCounter <= 0.0f)
		{
			Shoot();
			shootCounter = 5.0f;
		}
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
		m_isMoving = true;

		Vector3 directionToPlayer = m_playerPosition - m_position;

		directionToPlayer.Normalize();

		m_position += directionToPlayer * m_moveSpeed;
	}
	else
	{
		m_isMoving = false;
	}
}

// Constantly run away from the player
void Enemy::move2()
{
	m_isMoving = true;

	Vector3 directionToPlayer = m_playerPosition - m_position;

	directionToPlayer.Normalize();

	Vector3 targetPosition = m_position - directionToPlayer * m_moveSpeed;

	if (targetPosition.x >= 1.0f && targetPosition.x <= Board_Width - 1
		&& targetPosition.z >= 1.0f && targetPosition.z <= Board_Height - 1)
	{
		m_position -= directionToPlayer * m_moveSpeed;
	}
}

// Constantly moving to a random point on the board
void Enemy::move3()
{
	if (m_isMoving)
	{
		Vector3 directionToPoint = m_randomPoint - m_position;
		directionToPoint.Normalize();
		m_position += directionToPoint * m_moveSpeed;

		float distanceToPoint = Vector3::Distance(m_position, m_randomPoint);
		// Stop moving if close enough
		if (distanceToPoint <= 0.1f)
		{
			m_isMoving = false;
		}
	}
	else if (!m_isMoving)
	{
		// Generate a random point
		m_isMoving = true;

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
		m_isMoving = true;

		Vector3 directionToPlayer = m_playerPosition - m_position;

		directionToPlayer.Normalize();

		m_position += directionToPlayer * m_moveSpeed;
	}
	else
	{
		m_isMoving = false;
	}
}

// Stand still until the player get close, then run away to a random point
void Enemy::move5()
{
	float distanceToPlayer = Vector3::Distance(m_playerPosition, m_position);

	if (m_isMoving)
	{
		Vector3 directionToPoint = m_randomPoint - m_position;
		directionToPoint.Normalize();
		m_position += directionToPoint * m_moveSpeed;

		float distanceToPoint = Vector3::Distance(m_position, m_randomPoint);
		// Stop moving if close enough
		if (distanceToPoint <= 0.01f)
		{
			m_isMoving = false;
		}
	}
	else if (!m_isMoving && distanceToPlayer <= 5.0f)
	{
		// Generate a random point
		m_isMoving = true;

		float pointX = RandomRange(1.0, Board_Width);
		float pointZ = RandomRange(1.0, Board_Height);

		m_randomPoint = Vector3(pointX, 0.0, pointZ);
	}
}

void Enemy::Shoot()
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
		// Offset from pivot of enemy to the gun
		Vector3 m_offset = Vector3(-0.133f, 1.2f, 1.137f);

		Matrix heading = Matrix::CreateRotationY(m_rotY);

		Matrix lookAtRotation = heading;

		// The offset is still based on the world local, transform it into enemy's local
		Vector3 spawnAt = Vector3::TransformNormal(m_offset, lookAtRotation);

		spawnAt += m_position;

		Vector3 flyTowards = Vector3::TransformNormal(Vector3(0, 0, 10), heading);  // This is the direction enemy is facing

		the_bullet->SetPosition(spawnAt);                // Need to add some offset so the bullet appear in front of player and will not trigger hitbox

		the_bullet->SetFlyTowards(flyTowards);           // Fly towards the direction player is facing

		the_bullet->SetYRotation(m_rotY);                // Rotate the bullet to the correct direction

		the_bullet->SetBeingUsed(true);  // So the bullet will start updating
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

// Collisions
void Enemy::OnBulletCollisionEnter(Bullet* other)
{
	OutputDebugString("Enemy-Bullet Collision Enter\n");

	int damage = rand() % 6 + 3;  // Damage to enemy is between 3 - 8

	takeDamage(damage);

	// If enemy got killed, move them to other position
	if (!IsAlive())
	{
		m_position = Vector3(10.0f, -10.0f, 0.0f);  // Sink the enemy to the ground
	}
}

void Enemy::OnBulletCollisionStay(Bullet* other)
{
	OutputDebugString("Enemy-Bullet Collision Stay\n");
}

void Enemy::OnBulletCollisionExit(Bullet* other)
{
	OutputDebugString("Enemy-Bullet Collision Exit\n");
}



