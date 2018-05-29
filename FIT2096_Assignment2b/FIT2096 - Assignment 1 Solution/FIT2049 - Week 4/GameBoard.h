/*	FIT2096 - Assignment 1 Sample Solution
*	GameBoard.h
*	Created by Mike Yeates - 2017 - Monash University
*	Generates the board and manages a collection of Tiles.
*/

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include "Enemy.h"
#include "HealthPack.h"
#include "Tile.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include <vector>

class GameBoard
{
private:
	MeshManager* m_meshManager;
	TextureManager* m_textureManager;
	Shader* m_texturedShader;

	// How many tiles does this board manage
	const static int BOARD_WIDTH = 30;
	const static int BOARD_HEIGHT = 30;

	// Game objects handled by GameBoard
	std::vector<Enemy*> m_enemies;  // A vector of enemies
	std::vector<Bullet*> m_bullets;  // A vector of bullets
	std::vector<HealthPack*> m_healthPacks;  // A vector of health packs
	
	// Storing tiles in a 2D array to make neighbour checking easier
	Tile* m_tiles[BOARD_HEIGHT][BOARD_WIDTH];

	Vector3 currentPlayerPosition;  // Need this to rotate the enemies

	void Generate();  // Generate the ground
	void AddWalls();  // Called in the function above, to generate the walls

	void GenerateEnemies();  // Generate five enemies
	void PutEnemies();       // Put enemies on enemy tiles
	int enemyTileCount = 0;  // Keep track of how many enemy tile has been spawned

	void GenerateHealthPacks();  // Generate health packs on all health tiles
	void GenerateBullets();  // Generate bullets to be used
	
public:
	GameBoard();
	GameBoard(MeshManager* meshManager, TextureManager* textureManager, Shader* tileShader);
	~GameBoard();

	void Update(float timestep);
	void Render(Direct3D* renderer, Camera* camera);

	TileType GetTileTypeForPosition(int x, int z);
	Tile* GetRandomTileOfType(TileType type);
	Tile* GetEmptyEnemyTile(TileType type);  // Used to find an empty red tile to spawn an enemy

	Enemy* GetEnemy(Vector3 position);  // Used to get an enemy for player according to position
	HealthPack* GetHealthPack(Vector3 position);  // Used to get the healthpack at player's target position

	// Mutators
	void SetCurrentPlayerPosition(Vector3 pos) { currentPlayerPosition = pos; }

	// Accessors
	Vector3 GetCurrentPlayerPosition() { return currentPlayerPosition; }
	int GetEnemyTileCount() { return enemyTileCount; }
	int GetDeadEnemyAmount();  // Used to check if all enemies are dead

	std::vector<Enemy*> getEnemyVector() { return m_enemies; }
	std::vector<Bullet*> getBulletVector() { return m_bullets; }
	std::vector<HealthPack*> getHealthPackVector() { return m_healthPacks; }
};

#endif

