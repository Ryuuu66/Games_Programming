/*	FIT2096 - Assignment 1 Sample Solution
*	Tile.h
*	Created by Mike Yeates - 2017 - Monash University
*	A Tile represents a coloured cell on the board.
*/

#ifndef TILE_H
#define TILE_H

#include "GameObject.h"
#include "TextureManager.h"

// Define all the types of tiles we could be (naming these by function instead of appearance).
enum class TileType
{
	HEALTH,
	DAMAGE,
	TELEPORT,
	NORMAL,
	DISABLED,
	MONSTER_VAR1,
	MONSTER_VAR2,
	WALL,
	INVALID // Used if we query a tile which doesn't exist
};

class Tile : public GameObject
{
private:
	TileType m_type;
	TextureManager* m_textureManager;

	// We'll be animating so a second position is required
	Vector3 m_targetPosition;
	float m_timeUntilMove;
	float m_moveSpeed;

	TileType SelectType();
	Texture* GetTextureForType(TileType type);

	bool hasEnemy;  // To confirm if an enemy is on a tile



public:
	Tile();
	Tile(Mesh* mesh, Shader* shader, Vector3 pos, TextureManager* textureManager);
	Tile(Mesh* mesh, Shader* shader, Vector3 pos, TextureManager* textureManager, TileType type);
	~Tile();

	void Update(float timestep);

	// Instruct a tile to start falling from a specified height
	void DropFromHeight(float dropHeight, float stopHeight, float speed, float delay);

	TileType GetType() { return m_type; }
	void SetType(TileType type);

	void SetHasEnemy(bool value) { hasEnemy = value; }
	bool GetHasEnemy() { return hasEnemy; }


};

#endif

