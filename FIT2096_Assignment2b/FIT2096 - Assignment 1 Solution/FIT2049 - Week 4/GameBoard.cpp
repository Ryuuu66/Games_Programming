#include "GameBoard.h"
#include "MathsHelper.h"
#include <vector>

GameBoard::GameBoard()
{
	m_meshManager = NULL;
	m_textureManager = NULL;
	m_texturedShader = NULL;
}

GameBoard::GameBoard(MeshManager* meshManager, TextureManager* textureManager, Shader* tileShader)
{
	m_meshManager = meshManager;
	m_textureManager = textureManager;
	m_texturedShader = tileShader;
	
	// Generate GameBoard
	Generate();
	// Generate enemies
	GenerateEnemies();
	// Put enemies
	PutEnemies();

	// Generate HealthPacks
	GenerateHealthPacks();
}

GameBoard::~GameBoard()
{
	for (unsigned int z = 0; z < BOARD_HEIGHT; z++)
	{
		for (unsigned int x = 0; x < BOARD_WIDTH; x++)
		{
			delete m_tiles[z][x];
			m_tiles[z][x] = NULL;
		}
	}
	// Delete enemies
	for (int i = 0; i < m_enemies.size(); i++)
	{
		delete m_enemies[i];
		m_enemies[i] = NULL;
	}
	// Delete HealthPacks
	for (int i = 0; i < m_healthPacks.size(); i++)
	{
		delete m_healthPacks[i];
		m_healthPacks[i] = NULL;
	}
}

void GameBoard::Update(float timestep)
{
	// Update all the tiles we manage.
	// Our tiles will have an animation so they need to be Updated each frame.

	for (unsigned int z = 0; z < BOARD_HEIGHT; z++)
	{
		for (unsigned int x = 0; x < BOARD_WIDTH; x++)
		{
			m_tiles[z][x]->Update(timestep);
		}
	}
	// Update enemies
	for (int i = 0; i < m_enemies.size(); i++)
	{
		if (m_enemies[i]->IsAlive())
		{
			// Enemies update current player position
			m_enemies[i]->SetPlayerPosition(currentPlayerPosition);

			m_enemies[i]->Update(timestep);
		}
	}
	// Update HealthPacks
	for (int i = 0; i < m_healthPacks.size(); i++)
	{
		if (!m_healthPacks[i]->GetIsUsed())
		{
			m_healthPacks[i]->Update(timestep);
		}
	}
}

void GameBoard::Render(Direct3D* renderer, Camera* camera)
{
	// Render all the tiles we manage

	for (unsigned int z = 0; z < BOARD_HEIGHT; z++)
	{
		for (unsigned int x = 0; x < BOARD_WIDTH; x++)
		{
			m_tiles[z][x]->Render(renderer, camera);
		}
	}
	// Render enemies
	for (int i = 0; i < m_enemies.size(); i++)
	{
		if (m_enemies[i]->IsAlive())
		{
			m_enemies[i]->Render(renderer, camera);
		}
	}
	// Render HealthPacks
	for (int i = 0; i < m_healthPacks.size(); i++)
	{
		if (!m_healthPacks[i]->GetIsUsed())
		{
			m_healthPacks[i]->Render(renderer, camera);
		}
	}
}

void GameBoard::Generate()
{
	// Just a plain old square world for now. In the week eight lecture, we'll
	// implement an algorithm which creates cave like structures.

	// In this function, I need to make sure only five enemy tiles can be generated

	for (unsigned int z = 1; z < BOARD_HEIGHT - 1; z++)
	{
		for (unsigned int x = 1; x < BOARD_WIDTH - 1; x++)
		{
			bool tileTypeValid = false;
			while (!tileTypeValid)
			{
				tileTypeValid = true;

				// We give a tile its mesh and shader, but it picks its own texture
				m_tiles[z][x] = new Tile(m_meshManager->GetMesh("Assets/Meshes/floor_tile.obj"),
					m_texturedShader,
					Vector3(x, 0, z),
					m_textureManager);

				/*
				// If is enemy tile and count < 5
				if ((m_tiles[z][x]->GetType() == TileType::MONSTER_VAR1) && enemyTileCount < 5)
				{
					enemyTileCount += 1;
				}
				// If is enemy tile but count >= 5
				else if ((m_tiles[z][x]->GetType() == TileType::MONSTER_VAR1) && enemyTileCount >= 5)
				{	
					// Recreate a tile
					tileTypeValid = false;

					delete m_tiles[z][x];
					m_tiles[z][x] = NULL;
				}
				*/
			}	
		}
	}
	AddWalls();

	// All enemy tiles will now be created from normal tiles

	// If we don't have 5 enemy tiles yet
	if (enemyTileCount < 5)
	{
		int enemyTileLeft = 5 - enemyTileCount;
		for (int i = 0; i < enemyTileLeft; i++)
		{
			// Pick a random white tile
			Tile* tileChosen = GetRandomTileOfType(TileType::NORMAL);  // <---- This function can only be called after the walls are created, as this function also consider the walls
			// Set the type to enemy tile
			tileChosen->SetType(TileType::MONSTER_VAR1);
			// +1 to enemyTileCount
			enemyTileCount += 1;
		}
	}
	// Till here we must already have five enemy tiles on gameBoard
}

void GameBoard::AddWalls()
{
	// Adds a strip of walls around the outer edge of the world

	// Outer walls horizontal
	for (unsigned int x = 0; x < BOARD_WIDTH; x++)
	{
		// Top 
		m_tiles[BOARD_HEIGHT-1][x] = new Tile(m_meshManager->GetMesh("Assets/Meshes/wall_tile.obj"),
			m_texturedShader,
			Vector3(x, 0, BOARD_HEIGHT-1),
			m_textureManager,
			TileType::WALL);

		// Bottom
		m_tiles[0][x] = new Tile(m_meshManager->GetMesh("Assets/Meshes/wall_tile.obj"),
			m_texturedShader,
			Vector3(x, 0, 0),
			m_textureManager,
			TileType::WALL);
	}

	// Outer walls vertical (avoding corners so we don't double up)
	for (unsigned int z = 1; z < BOARD_HEIGHT - 1; z++)
	{
		// Left
		m_tiles[z][0] = new Tile(m_meshManager->GetMesh("Assets/Meshes/wall_tile.obj"),
			m_texturedShader,
			Vector3(0, 0, z),
			m_textureManager,
			TileType::WALL);

		// Right
		m_tiles[z][BOARD_WIDTH-1] = new Tile(m_meshManager->GetMesh("Assets/Meshes/wall_tile.obj"),
			m_texturedShader,
			Vector3(BOARD_WIDTH - 1, 0, z),
			m_textureManager,
			TileType::WALL);
	}
}

void GameBoard::DeactivateTile(int x, int z)
{
	m_tiles[z][x]->SetType(TileType::DISABLED);
}

TileType GameBoard::GetTileTypeForPosition(int x, int z)
{
	// Index directly into our 2D array using the passed in position.

	// It's possible we may accidentally check a tile outside of the board. 
	// Even though walls will prevent this, we'll still be defensive.

	if ((x < 0 || x >= BOARD_WIDTH) ||
		(z < 0 || z >= BOARD_HEIGHT))
	{
		return TileType::INVALID;
	}

	return m_tiles[z][x]->GetType();
}

Tile* GameBoard::GetRandomTileOfType(TileType type)
{
	std::vector<Tile*> shortlist;

	// Find all tiles matching the type we want
	for (unsigned int z = 0; z < BOARD_HEIGHT; z++)
	{
		for (unsigned int x = 0; x < BOARD_WIDTH; x++)
		{
			if (m_tiles[z][x]->GetType() == type)
			{
				shortlist.push_back(m_tiles[z][x]);
			}
		}
	}

	// There are no more tiles left matching this type
	if (shortlist.size() == 0)
		return NULL;

	// Return a random tile from the shortlist
	return shortlist[MathsHelper::RandomRange(0, shortlist.size() - 1)];
}

// Find an enemy tile which has no enemy yet
Tile* GameBoard::GetEmptyEnemyTile(TileType type)
{
	std::vector<Tile*> shortlist;

	// Find all tiles matching the type we want
	for (unsigned int z = 0; z < BOARD_HEIGHT; z++)
	{
		for (unsigned int x = 0; x < BOARD_WIDTH; x++)
		{
			if (m_tiles[z][x]->GetType() == type &&
				m_tiles[z][x]->GetHasEnemy() == false)
			{
				shortlist.push_back(m_tiles[z][x]);
			}
		}
	}

	// There are no more tiles left matching this type
	if (shortlist.size() == 0)
		return NULL;

	// Return a random tile from the shortlist
	return shortlist[MathsHelper::RandomRange(0, shortlist.size() - 1)];
}


void GameBoard::GenerateEnemies()
{
	m_enemies.push_back(new Enemy(20, 2, 1, m_meshManager->GetMesh("Assets/Meshes/enemy.obj"),
		m_texturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_red.png")));

	m_enemies.push_back(new Enemy(40, 4, 2, m_meshManager->GetMesh("Assets/Meshes/enemy.obj"),
		m_texturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_redDarker.png")));

	m_enemies.push_back(new Enemy(60, 6, 3, m_meshManager->GetMesh("Assets/Meshes/enemy.obj"),
		m_texturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_redLighter.png")));

	m_enemies.push_back(new Enemy(80, 8, 4, m_meshManager->GetMesh("Assets/Meshes/enemy.obj"),
		m_texturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_redOrange.png")));

	m_enemies.push_back(new Enemy(90, 10, 5, m_meshManager->GetMesh("Assets/Meshes/enemy.obj"),
		m_texturedShader, m_textureManager->GetTexture("Assets/Textures/gradient_redPink.png")));

	for (int i = 0; i < m_enemies.size(); i++)
	{
		m_enemies[i]->SetBoardWidth(BOARD_WIDTH);
		m_enemies[i]->SetBoardHeight(BOARD_HEIGHT);
	}
}

void GameBoard::PutEnemies()
{
	for (int i = 0; i < m_enemies.size(); i++)
	{
		Tile* destinationTile = GetEmptyEnemyTile(TileType::MONSTER_VAR1);

		if (destinationTile)
		{
			destinationTile->SetHasEnemy(true);

			m_enemies[i]->SetPosition(destinationTile->GetPosition());

			m_enemies[i]->SetYPosition(0.0f);  // To ensure enemy spawn on the ground
		}
	}
}

Enemy* GameBoard::GetEnemy(Vector3 position)
{
	for (int i = 0; i < m_enemies.size(); i++)
	{
		if (m_enemies[i]->GetPosition() == position)
		{
			return m_enemies[i];
		}
	}
	
	return NULL;  // This should not happen
}

void GameBoard::GenerateHealthPacks()
{
	for (unsigned int z = 0; z < BOARD_HEIGHT; z++)
	{
		for (unsigned int x = 0; x < BOARD_WIDTH; x++)
		{
			// If it is a health tile, put a health pack there
			if (m_tiles[z][x]->GetType() == TileType::HEALTH)
			{
				HealthPack* h1 = new HealthPack(m_meshManager->GetMesh("Assets/Meshes/ammoBlock.obj"), 
					m_texturedShader, m_textureManager->GetTexture("Assets/Textures/tile_green.png"));

				h1->SetPosition(m_tiles[z][x]->GetPosition());
				h1->SetYPosition(0.0f);

				m_healthPacks.push_back(h1);
			}
		}
	}

}

HealthPack* GameBoard::GetHealthPack(Vector3 position)
{
	for (int i = 0; i < m_healthPacks.size(); i++)
	{
		if (m_healthPacks[i]->GetPosition() == position)
		{
			return m_healthPacks[i];
		}
	}

	return NULL;  // This should not happen
}
