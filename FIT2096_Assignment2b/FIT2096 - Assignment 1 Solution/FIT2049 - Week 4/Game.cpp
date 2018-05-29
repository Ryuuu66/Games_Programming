/*	FIT2096 - Assignment 1 Sample Solution
*	Game.h
*	Created by Mike Yeates - 2017 - Monash University
*/

#include "Game.h"
#include "TexturedShader.h"
#include "StaticObject.h"

#include "DirectXTK/CommonStates.h"
#include <sstream>

Game::Game()
{
	m_audio = NULL;
	m_backgroundMusic = NULL;
	m_renderer = NULL;
	m_currentCam = NULL;
	m_input = NULL;
	m_meshManager = NULL;
	m_textureManager = NULL;
	m_diffuseTexturedShader = NULL;
	m_gameBoard = NULL;
	
	m_stateMachine = NULL;
	m_startButton = NULL;
	m_timeTrialButton = NULL;
	m_quitButton = NULL;
}

Game::~Game() {}

bool Game::Initialise(Direct3D* renderer, AudioSystem* audio, InputController* input)
{
	m_audio = audio;
	m_renderer = renderer;	
	m_input = input;
	m_meshManager = new MeshManager();
	m_textureManager = new TextureManager();
	
	if (!InitShaders())
		return false;

	if (!LoadMeshes())
		return false;

	if (!LoadTextures())
		return false;

	if (!LoadAudio())
		return false;

	// Ready background music
	m_backgroundMusic = m_audio->Play("Assets/Sounds/rocket_race.mp3", false);

	if (m_backgroundMusic)
	{
		m_backgroundMusic->SetLoopCount(-1);
		m_backgroundMusic->SetPaused(false);
	}

	// Prepare the sprites
	LoadFonts();
	// Initialize UI
	InitUI();
	// Initialize Game World
	InitGameWorld();
	// Ready State Machine
	InitStates();

	m_countDown = 90.0f;
	m_isTimeTrial = false;

	m_collisionManager = new CollisionManager(&m_players, &m_enemies, &m_bullets, &m_healthPacks);

	// Create the camera after player has appeared
	//m_currentCam = new Camera();  // For third person view
	m_currentCam = new FirstPersonCamera(m_input, m_player->GetPosition() + Vector3(0.0f, 5.0f, 0.0f));  // For first person view

	return true;
}

bool Game::InitShaders()
{
	m_diffuseTexturedShader = new TexturedShader();
	if (!m_diffuseTexturedShader->Initialise(m_renderer->GetDevice(), L"Assets/Shaders/VertexShader.vs", L"Assets/Shaders/TexturedPixelShader.ps"))
		return false;

	return true;
}

void Game::LoadFonts()
{
	// There's a few different size fonts in there, you know
	m_arialFont12 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-12pt.spritefont");
	m_arialFont18 = new SpriteFont(m_renderer->GetDevice(), L"Assets/Fonts/Arial-18pt.spritefont");
}

bool Game::LoadAudio()
{
	if (!m_audio->Load("Assets/Sounds/rocket_race.mp3"))
		return false;

	return true;
}

bool Game::LoadMeshes()
{
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/floor_tile.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/wall_tile.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/player_capsule.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/progress_cube.obj"))
		return false;
	
	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ammoBlock.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/bullet.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/enemy.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ground.obj"))
		return false;

	if (!m_meshManager->Load(m_renderer, "Assets/Meshes/ruby.obj"))
		return false;

	return true;
}

bool Game::LoadTextures()
{
	if (!m_textureManager->Load(m_renderer, "Assets/Textures/button.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/tile_blue.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/tile_disabled.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/tile_green.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/tile_orange.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/tile_purple.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/tile_red.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/tile_white.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/bullet.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_red.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redDarker.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redLighter.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redOrange.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/gradient_redPink.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/ground.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/sprite_healthBar.png"))
		return false;

	if (!m_textureManager->Load(m_renderer, "Assets/Textures/sprite_hurtOverlay.png"))
		return false;

	return true;
}

void Game::InitGameWorld()
{
	// A GameBoard creates the world layout and manages the Tiles.
	// We pass it the Mesh and Texture managers as it will be creating tiles and walls
	m_gameBoard = new GameBoard(m_meshManager, m_textureManager, m_diffuseTexturedShader);


	// A player will select a random starting position.
	// We need to tell the player about the board it is standing on so it can validate movement
	// and ask the board what type of tile it is standing on.
	m_player = new Player(m_meshManager->GetMesh("Assets/Meshes/enemy.obj"),
						  m_diffuseTexturedShader,
						  m_textureManager->GetTexture("Assets/Textures/tile_white.png"),
						  m_input,
						  m_gameBoard);

	// To be passed into Collision Manager
	m_players.push_back(m_player);
	m_enemies = m_gameBoard->getEnemyVector();
	m_bullets = m_gameBoard->getBulletVector();
	m_healthPacks = m_gameBoard->getHealthPackVector();

}

void Game::InitStates()
{
	// TODO Create the state machine and register states here
	m_stateMachine = new StateMachine<GameStates, Game>(this, GameStates::MENU_STATE);


	m_stateMachine->RegisterState(GameStates::MENU_STATE, &Game::Menu_OnEnter,
		&Game::Menu_OnUpdate, &Game::Menu_OnRender, &Game::Menu_OnExit);

	m_stateMachine->RegisterState(GameStates::GAMEPLAY_STATE, &Game::Gameplay_OnEnter,
		&Game::Gameplay_OnUpdate, &Game::Gameplay_OnRender, &Game::Gameplay_OnExit);

	m_stateMachine->RegisterState(GameStates::TIMETRIAL_STATE, &Game::Timetrial_OnEnter,
		&Game::Timetrial_OnUpdate, &Game::Timetrial_OnRender, &Game::Timetrial_OnExit);

	m_stateMachine->RegisterState(GameStates::PAUSE_STATE, &Game::Pause_OnEnter,
		&Game::Pause_OnUpdate, &Game::Pause_OnRender, &Game::Pause_OnExit);
}

void Game::Update(float timestep)
{
	// Look how short this function can be when we make objects responsible for their own logic.
	// Our only job out here is to Update the board and player, and check if the game is over.
	m_input->BeginUpdate();

	// Update audio
	m_audio->Update();

	m_stateMachine->Update(timestep);

	/*
	// Use a static cast to update the rotY of player
	FirstPersonCamera* tempCam = static_cast<FirstPersonCamera*>(m_currentCam);
	m_player->SetYRotation(tempCam->GetHeading());
	
	m_player->Update(timestep);

	// Let gameboard knows current player position
	m_gameBoard->SetCurrentPlayerPosition(m_player->GetPosition());
	m_gameBoard->Update(timestep);

	// Check collisions
	if (m_collisionManager)
	{
		m_collisionManager->CheckCollisions();
	}

	CheckGameOver();

	// Refresh UI here
	RefreshUI();

	// Sometimes creating a whole new child of Camera is a bit overkill. Here
	// we're just telling our existing camera what to do (it has been modified to include
	// the catch-up mode using LERP and also sets its look-at internally each frame).

	//m_currentCam->SetTargetPosition(m_player->GetPosition() + Vector3(0, 5, -5));  // For third person view
	m_currentCam->SetTargetPosition(m_player->GetPosition() + Vector3(0, 1, 0));  // For first person view
	m_currentCam->Update(timestep);
	
	// Set audio listener after the camera has updated so its data is valid on the first frame
	m_audio->SetListener3DAttributes(m_currentCam->GetPosition(), m_currentCam->GetForward(), m_currentCam->GetUp(), m_currentCam->GetVelocity());
	*/

	m_input->EndUpdate();
}

void Game::Render()
{
	m_renderer->BeginScene(0.2f, 0.2f, 0.2f, 1.0f);

	m_stateMachine->Render();

	/*
	// The board renders all of its tiles
	m_gameBoard->Render(m_renderer, m_currentCam);
	
	//m_player->Render(m_renderer, m_currentCam);  // Don't render the player in first person view
	
	// Draw UI here
	DrawGameUI();
	*/

	m_renderer->EndScene();		
}

void Game::CheckGameOver()
{
	// Checks the three conditions that can end the game and informs the user

	const char* msg = "";

	if (m_isTimeTrial && m_countDown <= 0.0f)
	{
		msg = "Time's up!";
	}

	else if (m_player->GetHealth() <= 0.0f)
	{
		msg = "You've run out of health.";
	}

	// The only win condition
	else if (m_player->GetNumberOfMonstersDefeated() == 5)
	{
		msg = "You won!";
	}

	if (msg != "")
	{
		std::stringstream ss;
		ss << msg << " You scored " << m_player->GetScore() << " and defeated " << m_player->GetNumberOfMonstersDefeated() << " monsters.";

		// Message Boxes are a blocking call which makes life a little easier here
		MessageBox(NULL, ss.str().c_str(), "Game Over", MB_OK);
		PostQuitMessage(0);

		// From now on, an in-game UI should be used instead of a message box
	}
}

void Game::Shutdown()
{
	if (m_player)
	{
		delete m_player;
		m_player = NULL;
	}

	if (m_gameBoard)
	{
		delete m_gameBoard;
		m_gameBoard = NULL;
	}

	if (m_currentCam)
	{
		delete m_currentCam;
		m_currentCam = NULL;
	}

	if (m_meshManager)
	{
		m_meshManager->Release();
		delete m_meshManager;
		m_meshManager = NULL;
	}

	if (m_textureManager)
	{
		m_textureManager->Release();
		delete m_textureManager;
		m_textureManager = NULL;
	}

	if (m_diffuseTexturedShader)
	{
		m_diffuseTexturedShader->Release();
		delete m_diffuseTexturedShader;
		m_diffuseTexturedShader = NULL;
	}

	if (m_spriteBatch)
	{
		delete m_spriteBatch;
		m_spriteBatch = NULL;
	}

	if (m_arialFont12)
	{
		delete m_arialFont12;
		m_arialFont12 = NULL;
	}

	if (m_arialFont18)
	{
		delete m_arialFont18;
		m_arialFont18 = NULL;
	}

	if (m_audio)
	{
		m_audio->Shutdown();
		delete m_audio;
		m_audio = NULL;
	}

	if (m_backgroundMusic)
	{
		delete m_backgroundMusic;
		m_backgroundMusic = NULL;
	}

	if (m_startButton)
	{
		delete m_startButton;
		m_startButton = NULL;
	}
	if (m_timeTrialButton)
	{
		delete m_timeTrialButton;
		m_timeTrialButton = NULL;
	}
	if (m_quitButton)
	{
		delete m_quitButton;
		m_quitButton = NULL;
	}

	if (m_stateMachine)
	{
		delete m_stateMachine;
		m_stateMachine = NULL;
	}
}


void Game::InitUI()
{
	m_spriteBatch = new SpriteBatch(m_renderer->GetDeviceContext());

	m_HealthBarSprite = m_textureManager->GetTexture("Assets/Textures/sprite_healthBar.png");  // To show the healthbar

	// Prepare Menu UI
	Texture* buttonTexture = m_textureManager->GetTexture("Assets/Textures/button.png");
																							   // Also init any buttons here
	m_startButton = new Button(128, 64, buttonTexture, L"Start Game", Vector2(574, 385), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Transition into the Gameplay state (these buttons are only used on the menu screen)
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	});

	m_timeTrialButton = new Button(128, 64, buttonTexture, L"Time Trial", Vector2(574, 600), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Transition into the Gameplay state (these buttons are only used on the menu screen)
		m_stateMachine->ChangeState(GameStates::TIMETRIAL_STATE);

		m_isTimeTrial = true;
	});

	m_quitButton = new Button(128, 64, buttonTexture, L"Quit", Vector2(706, 385), m_spriteBatch, m_arialFont12, m_input, [this]
	{
		// Tell windows to send a WM_QUIT message into our message pump
		PostQuitMessage(0);
	});

}

void Game::DrawMenuUI()
{
	BeginUI();

	m_startButton->Render();
	m_timeTrialButton->Render();
	m_quitButton->Render();
	
	// The game title
	m_arialFont18->DrawString(m_spriteBatch, L"FIT2096 Assignment 2B", Vector2(500, 100), Color(1.0f, 1.0f, 1.0f), 0, Vector2(0, 0));

	EndUI();
}

void Game::DrawPauseUI()
{
	BeginUI();

	m_arialFont18->DrawString(m_spriteBatch, L"Paused", Vector2(605, 10), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));
	
	EndUI();
}

void Game::DrawGameUI()
{
	BeginUI();

	// Do UI drawing between the Begin and End calls
	// This is for time trial mode
	if (m_isTimeTrial)
	{
		m_arialFont18->DrawString(m_spriteBatch, m_countDownTimerText.c_str(), Vector2(1100, 10), Color(1.0f, 1.0f, 1.0f), 0, Vector2(0, 0));
	}

	// So player knows they could pause
	m_arialFont18->DrawString(m_spriteBatch, L"P to toggle pause", Vector2(1080, 680), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));

	// Here to show the Player's Score
	m_arialFont18->DrawString(m_spriteBatch, m_playerScoreText.c_str(), Vector2(500, 680), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));

	// Here's how we draw a sprite over our game
	RECT r1 = { 20, 20, m_player->GetHealth() * 2, 40 };
	m_spriteBatch->Draw(m_HealthBarSprite->GetShaderResourceView(), r1, Color(0.0f, 1.0f, 0.0f));
	
	EndUI();
}

void Game::RefreshUI()
{
	// Ensure text in UI matches latest scores etc (call this after data changes)
	// Concatenate data into our label string using a wide string stream
	if (m_player)
	{
		// Update player's score
		std::wstringstream ss;

		// Round to two decimal places for neater output
		ss << "Player's Score: " << floorf(m_player->GetScore() * 100) / 100;

		m_playerScoreText = ss.str();
	}

	// Only update this if is time trial
	if (m_isTimeTrial)
	{
		std::wstringstream ss1;

		ss1 << "Timer: " << roundf(m_countDown * 100) / 100;

		m_countDownTimerText = ss1.str();
	}
}

void Game::BeginUI()
{
	// Sprites don't use a shader 
	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());
}

void Game::EndUI()
{
	m_spriteBatch->End();
}


// All states
void Game::Menu_OnEnter()
{
	OutputDebugString("Menu OnEnter\n");
}

void Game::Menu_OnUpdate(float timestep)
{
	// Only update the buttons in menu state
	m_startButton->Update();
	m_timeTrialButton->Update();
	m_quitButton->Update();
}

void Game::Menu_OnRender()
{
	DrawMenuUI();
}

void Game::Menu_OnExit()
{
	OutputDebugString("Menu OnExit\n");
}


void Game::Gameplay_OnEnter()
{
	OutputDebugString("GamePlay OnEnter\n");
}

void Game::Gameplay_OnUpdate(float timestep)
{
	// Use a static cast to update the rotY of player
	FirstPersonCamera* tempCam = static_cast<FirstPersonCamera*>(m_currentCam);
	m_player->SetYRotation(tempCam->GetHeading());

	m_player->Update(timestep);

	// Let gameboard knows current player position
	m_gameBoard->SetCurrentPlayerPosition(m_player->GetPosition());
	m_gameBoard->Update(timestep);

	// Check collisions
	if (m_collisionManager)
	{
		m_collisionManager->CheckCollisions();
	}

	CheckGameOver();

	// Refresh UI here
	RefreshUI();

	// Sometimes creating a whole new child of Camera is a bit overkill. Here
	// we're just telling our existing camera what to do (it has been modified to include
	// the catch-up mode using LERP and also sets its look-at internally each frame).

	//m_currentCam->SetTargetPosition(m_player->GetPosition() + Vector3(0, 5, -5));  // For third person view
	m_currentCam->SetTargetPosition(m_player->GetPosition() + Vector3(0, 1, 0));  // For first person view
	m_currentCam->Update(timestep);

	// Set audio listener after the camera has updated so its data is valid on the first frame
	m_audio->SetListener3DAttributes(m_currentCam->GetPosition(), m_currentCam->GetForward(), m_currentCam->GetUp(), m_currentCam->GetVelocity());

	// Should we pause
	if (m_input->GetKeyDown('P'))
	{
		m_stateMachine->ChangeState(GameStates::PAUSE_STATE);
	}
}

void Game::Gameplay_OnRender()
{
	// The board renders all of its tiles
	m_gameBoard->Render(m_renderer, m_currentCam);

	//m_player->Render(m_renderer, m_currentCam);  // Don't render the player in first person view

	// Draw UI here
	DrawGameUI();
}

void Game::Gameplay_OnExit()
{
	OutputDebugString("GamePlay OnExit\n");
}


void Game::Timetrial_OnEnter()
{
	OutputDebugString("TimeTrial OnEnter\n");
}

void Game::Timetrial_OnUpdate(float timestep)
{
	// The only difference with normal game play is the count down timer
	m_countDown -= timestep;

	Gameplay_OnUpdate(timestep);
}

void Game::Timetrial_OnRender()
{
	Gameplay_OnRender();
}

void Game::Timetrial_OnExit()
{
	OutputDebugString("TimeTrial OnExit\n");
}


void Game::Pause_OnEnter()
{
	OutputDebugString("Pause OnEnter\n");
}

void Game::Pause_OnUpdate(float timestep)
{
	// Check if we should exit pause
	if (m_input->GetKeyDown('P'))
	{
		m_stateMachine->ChangeState(GameStates::GAMEPLAY_STATE);
	}
}

void Game::Pause_OnRender()
{
	// Keep drawing the game when paused (it's not being updated though which is what freezes it)
	Gameplay_OnRender();

	// In addition to the game, draw some UI to say we're paused
	DrawPauseUI();
}

void Game::Pause_OnExit()
{
	OutputDebugString("Pause OnExit\n");
}

