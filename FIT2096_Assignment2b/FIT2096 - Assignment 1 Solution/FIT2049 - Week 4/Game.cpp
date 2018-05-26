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
	m_renderer = NULL;
	m_currentCam = NULL;
	m_input = NULL;
	m_meshManager = NULL;
	m_textureManager = NULL;
	m_diffuseTexturedShader = NULL;
	m_gameBoard = NULL;
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

	// Refresh UI
	RefreshUI();


	m_collisionManager = new CollisionManager(&m_players, &m_enemies);

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
	m_player = new Player(m_meshManager->GetMesh("Assets/Meshes/player_capsule.obj"),
						  m_diffuseTexturedShader,
						  m_textureManager->GetTexture("Assets/Textures/tile_white.png"),
						  m_input,
						  m_gameBoard);

	m_players.push_back(m_player);

	// So the game can pass the enemy vector to the collision manager
	m_enemies = m_gameBoard->getEnemyVector();

}

void Game::Update(float timestep)
{
	// Look how short this function can be when we make objects responsible for their own logic.
	// Our only job out here is to Update the board and player, and check if the game is over.
	m_input->BeginUpdate();

	// Update audio
	m_audio->Update();

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

	m_input->EndUpdate();
}

void Game::Render()
{
	m_renderer->BeginScene(0.2f, 0.2f, 0.2f, 1.0f);

	// The board renders all of its tiles
	m_gameBoard->Render(m_renderer, m_currentCam);
	
	//m_player->Render(m_renderer, m_currentCam);  // Don't render the player in first person view
	
	// Draw UI here
	DrawUI();

	m_renderer->EndScene();		
}

void Game::CheckGameOver()
{
	// Checks the three conditions that can end the game and informs the user

	const char* msg = "";

	if (m_player->GetHealth() <= 0.0f)
	{
		msg = "You've run out of health.";
	}
	// In this game we don't limit player's moves
	/*else if (m_player->GetMovesRemaining() <= 0)
	{
		msg = "You've used up all your moves.";
	}*/
	else if (m_player->GetIsTrapped())
	{
		msg = "You're trapped.";
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

}


void Game::InitUI()
{
	m_spriteBatch = new SpriteBatch(m_renderer->GetDeviceContext());

	m_HealthBarSprite = m_textureManager->GetTexture("Assets/Textures/sprite_healthBar.png");
}

void Game::DrawUI()
{
	// Sprites don't use a shader 
	m_renderer->SetCurrentShader(NULL);

	CommonStates states(m_renderer->GetDevice());
	m_spriteBatch->Begin(SpriteSortMode_Deferred, states.NonPremultiplied());

	// Do UI drawing between the Begin and End calls

	// Let's draw some text over our game
	m_arialFont18->DrawString(m_spriteBatch, m_playerScoreText.c_str(), Vector2(500, 680), Color(0.0f, 0.0f, 0.0f), 0, Vector2(0, 0));

	RECT r1 = { 20, 20, m_player->GetHealth() * 2, 40 };

	// Here's how we draw a sprite over our game
	m_spriteBatch->Draw(m_HealthBarSprite->GetShaderResourceView(), r1, Color(0.0f, 1.0f, 0.0f));
	
	m_spriteBatch->End();
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
}
