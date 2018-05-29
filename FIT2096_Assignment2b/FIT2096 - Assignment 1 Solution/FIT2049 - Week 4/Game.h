/*	FIT2096 - Example Code
*	Game.h
*	Created by Elliott Wilson & Mike Yeates - 2016 - Monash University
*	This class is the heart of our game and is also where our game logic will reside
*	It contains the overall Update and Render method for the whole game
*	You should eventually split some game logic into other objects which Game will manage
*	Watch the size of this class - it can grow out of control very fast!
*/

#ifndef GAME_H
#define GAME_H

#include "AudioSystem.h"
#include "Button.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Enemy.h"
#include "FirstPersonCamera.h"
#include "InputController.h"
#include "CollisionManager.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "GameObject.h"
#include "GameBoard.h"
#include "Player.h"

#include "StateMachine.h"

#include "DirectXTK/SpriteBatch.h"
#include "DirectXTK/SpriteFont.h"

#include <vector>

class Game
{
private:
	
	// Declare different states in the game
	enum class GameStates
	{
		MENU_STATE,
		GAMEPLAY_STATE,
		TIMETRIAL_STATE,
		PAUSE_STATE,
	};

	// Our state machine is a generic class (we need to tell it what types it manages).
	// It knows about two things - our states, and also who ownes those states.
	StateMachine<GameStates, Game>* m_stateMachine;

	// MENU State
	Button* m_startButton;
	Button* m_timeTrialButton;
	Button* m_quitButton;
	
	// TIMETRIAL State
	float m_countDown;
	bool m_isTimeTrial;

	// Game audio system
	AudioSystem* m_audio;
	AudioClip* m_backgroundMusic;

	Camera* m_currentCam;
	Direct3D* m_renderer;
	InputController* m_input;	
	MeshManager* m_meshManager;
	TextureManager* m_textureManager;
	CollisionManager* m_collisionManager;

	Shader* m_diffuseTexturedShader;

	// Our game data. The Game class only needs to manage three objects for this game.
	GameBoard* m_gameBoard;
	Player* m_player;
	// Pass these to collision manager
	std::vector<Player*> m_players;
	std::vector<Enemy*> m_enemies;
	std::vector<Bullet*> m_bullets;
	std::vector<HealthPack*> m_healthPacks;	

	// Sprites / Text Fonts
	Texture* m_HealthBarSprite;
	SpriteBatch* m_spriteBatch;
	SpriteFont* m_arialFont12;
	SpriteFont* m_arialFont18;

	// Wide strings use more than 8 bits per character so they can capture more symbols
	// Windows loves them and as such so does Direct3D and its text helpers
	std::wstring m_playerScoreText;
	std::wstring m_countDownTimerText;


	// Splitting initialisation up into several steps
	// Initialisation Helpers
	bool InitShaders();
	bool LoadAudio();
	bool LoadMeshes();
	bool LoadTextures();
	void LoadFonts();
	void InitGameWorld();
	void InitStates();
	

	// UI drawing helpers
	void InitUI();
	void DrawMenuUI();
	void DrawGameUI();
	void DrawPauseUI();
	void RefreshUI();
	void BeginUI();
	void EndUI();


	void CheckGameOver();

	// Every state in our game will have four callbacks
	// We register these with the StateMachine and it calls them for us
	void Menu_OnEnter();
	void Menu_OnUpdate(float timestep);
	void Menu_OnRender();
	void Menu_OnExit();

	void Gameplay_OnEnter();
	void Gameplay_OnUpdate(float timestep);
	void Gameplay_OnRender();
	void Gameplay_OnExit();

	void Timetrial_OnEnter();
	void Timetrial_OnUpdate(float timestep);
	void Timetrial_OnRender();
	void Timetrial_OnExit();

	void Pause_OnEnter();
	void Pause_OnUpdate(float timestep);
	void Pause_OnRender();
	void Pause_OnExit();

public:
	Game();	
	~Game();

	bool Initialise(Direct3D* renderer, AudioSystem* audio, InputController* input); //The initialise method will load all of the content for the game (meshes, textures, etc.)

	void Update(float timestep);	//The overall Update method for the game. All gameplay logic will be done somewhere within this method
	void Render();					//The overall Render method for the game. Here all of the meshes that need to be drawn will be drawn

	void Shutdown(); //Cleanup everything we initialised
};

#endif