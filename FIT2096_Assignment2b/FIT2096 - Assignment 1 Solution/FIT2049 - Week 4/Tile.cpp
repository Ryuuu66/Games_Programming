#include "Tile.h"
#include "MathsHelper.h"

Tile::Tile()
{
	m_type = TileType::NORMAL;
	m_textureManager = NULL;
	m_timeUntilMove = 0.0f;
}

Tile::Tile(Mesh* mesh, Shader* shader, Vector3 pos, TextureManager* textureManager)
	: GameObject(mesh, shader, NULL, pos)
{
	m_textureManager = textureManager;
	m_targetPosition = pos;
	m_timeUntilMove = 0.0f;

	hasEnemy = false;

	// Also sets our texture
	SetType(SelectType());
	
	DropFromHeight(40.0f, 0.0f, 3.0f, MathsHelper::RandomRange(0.0f, 2.0f));
}

Tile::Tile(Mesh* mesh, Shader* shader, Vector3 pos, TextureManager* textureManager, TileType type)
	: GameObject(mesh, shader, NULL, pos)
{
	m_type = type;
	m_textureManager = textureManager;
	m_targetPosition = pos;
	m_timeUntilMove = 0.0f;

	SetType(type);

	DropFromHeight(40.0f, 0.0f, 3.0f, MathsHelper::RandomRange(0.0f, 2.0f));
}

Tile::~Tile() {}

void Tile::Update(float timestep)
{
	if (m_timeUntilMove > 0.0f)
	{
		// Not ready to fall yet
		m_timeUntilMove -= timestep;
	}
	else
	{
		// We're falling!
		m_position = Vector3::Lerp(m_position, m_targetPosition, timestep * m_moveSpeed);
	}
}

TileType Tile::SelectType()
{
	// A Tile is responsible for selecting its own type (colour) using basic random numbers
	// Higher probability for normal white tiles than the rest

	int roll = MathsHelper::RandomRange(1, 100);

	if (roll < 75)
		return TileType::NORMAL;
	// The board doesn't need to choose monster tile, just set random white tile into monster tile
	//else if (roll < 75)
		//return TileType::MONSTER_VAR1;  
	else if (roll < 80)
		return TileType::DAMAGE;
	else if (roll < 85)
		return TileType::HEALTH;
	else if (roll < 88)
		return TileType::TELEPORT;
	else
		return TileType::DISABLED;
}

Texture* Tile::GetTextureForType(TileType type)
{
	// Asks the texture manager for the texture matching our type (i.e. red texture for "damage" type)

	switch (type)
	{
	case TileType::HEALTH:
		return m_textureManager->GetTexture("Assets/Textures/tile_green.png");
	case TileType::DAMAGE:
		return m_textureManager->GetTexture("Assets/Textures/tile_red.png");
	case TileType::TELEPORT:
		return m_textureManager->GetTexture("Assets/Textures/tile_blue.png");
	case TileType::DISABLED:
		return m_textureManager->GetTexture("Assets/Textures/tile_disabled.png");
	case TileType::MONSTER_VAR1:
		return m_textureManager->GetTexture("Assets/Textures/tile_orange.png");
	// We only need one monster tile in this game
	//case TileType::MONSTER_VAR2:
		//return m_textureManager->GetTexture("Assets/Textures/tile_purple.png");
	case TileType::NORMAL:
		return m_textureManager->GetTexture("Assets/Textures/tile_white.png");
	case TileType::WALL:
		return m_textureManager->GetTexture("Assets/Textures/tile_disabled.png");
	default:
		return m_textureManager->GetTexture("Assets/Textures/tile_white.png");
	}
}

void Tile::SetType(TileType type)
{
	m_type = type;
	m_texture = GetTextureForType(m_type);
}

void Tile::DropFromHeight(float dropHeight, float stopHeight, float speed, float delay)
{
	// Instruct a tile to start falling from a specified height

	// Snap to drop height
	m_position.y = dropHeight;

	// Set target y to stop height (this is the resting height where the fall stops)
	m_targetPosition = Vector3(m_targetPosition.x, stopHeight, m_targetPosition.z);

	m_moveSpeed = speed;
	m_timeUntilMove = delay;
}