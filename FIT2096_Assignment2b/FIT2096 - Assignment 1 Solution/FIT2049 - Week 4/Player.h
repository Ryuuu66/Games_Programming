/*	FIT2096 - Assignment 1 Sample Solution
*	Player.h
*	Created by Mike Yeates - 2017 - Monash University
*	A Player listens to the keyboard and responds to arrow key presses.
*	It LERPs itself between cells and also asks the GameBoard where it can move.
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "Bullet.h"
#include "GameObject.h"
#include "InputController.h"
#include "GameBoard.h"

class Player : public GameObject
{
private:
	// A Player should listen for its own input
	InputController* m_input;
	CBoundingBox m_boundingBox;


	// We'll be animating our movement so a second position is required
	Vector3 m_targetPosition;
	float m_moveSpeed;
	
	// Which board is the player currently on
	GameBoard* m_currentBoard;
	std::vector<Bullet*> m_bullets;

	// Game variables
	float m_health;
	int m_score;
	int m_monstersDefeated;

	// Ask the GameBoard if we are allowed to move to a particular tile
	bool CanMoveHere(Vector3 target);
	
	// Housekeeping after we receive input
	void FinishTurn();

	// Used to spawn the player in a random position and teleport between blue tiles
	void TeleportToTileOfType(TileType type);

	// For Monster battles
	int Attack();
	void takeDamage(int amount);
	// void DoMonsterBattle();  // This function is not used in this game
	void DoBattle(Enemy* currentEnemy);

public:
	Player();
	Player(Mesh* mesh, Shader* shader, Texture* texture, InputController* input, GameBoard* board);
	~Player();

	void Update(float timestep);

	// Function to be called in Update function, a bullet will shoot out
	void Shoot();




	// Collisions of player with other objects
	void OnEnemyCollisionEnter(Enemy* other);  // Got instant killed here
	void OnEnemyCollisionStay(Enemy* other);
	void OnEnemyCollisionExit(Enemy* other);

	void OnBulletCollisionEnter(Bullet* other);  // Take damage here
	void OnBulletCollisionStay(Bullet* other);
	void OnBulletCollisionExit(Bullet* other);

	void OnHealthPackCollisionEnter(HealthPack* other);  // Restore health here
	void OnHealthPackCollisionStay(HealthPack* other);
	void OnHealthPackCollisionExit(HealthPack* other);


	// The Game class will use these to determine if the game should end
	float GetHealth() { return m_health; }

	// Game will use these to output info to the player
	int GetNumberOfMonstersDefeated() { return m_monstersDefeated; }
	int GetScore() { return m_score; }

	// Accessors
	CBoundingBox GetBounds() { return m_boundingBox; }

};

#endif
