#ifndef __MODULE_PLAYER_H__
#define __MODULE_PLAYER_H__

#include "Module.h"
#include "Animation.h"
#include "p2Point.h"
#include "ModuleInput.h"
#include "Application.h"


struct SDL_Texture;
struct Collider;

class ModulePlayer : public Module
{
public:
	// Constructor
	ModulePlayer(bool startEnabled);

	// Destructor
	~ModulePlayer();

	// Called when the module is activated
	// Loads the necessary textures for the player
	bool Start() override;

	// Called at the middle of the application loop
	// Processes new input and handles player movement
	update_status Update() override;

	// Called at the end of the application loop
	// Performs the render call of the player sprite
	update_status PostUpdate() override;

	bool CleanUp() override;

	// Collision callback, called when the player intersects with another collider
	void OnCollision(Collider* c1, Collider* c2) override;

	//void upStairs();
	//void downStairs();
	void checkUnbreakable();
	void checkIfNeedToFall();

	void checkIfWall();

public:
	// Position of the player in the map
	iPoint position;
	iPoint tile = { position.x / TILE_SIZE, position.y / TILE_SIZE };
		
	//Position of the mouse
	int mouseX;
	int mouseY;

	// The speed in which we move the player (pixels per frame)
	int speed = 2;

	// The player spritesheet loaded into an SDL_Texture
	SDL_Texture* texture = nullptr;
	SDL_Texture* timeTexture = nullptr;
	SDL_Texture* gameOverTexture = nullptr;
	SDL_Texture* readyTexture = nullptr;

	// The pointer to the current player animation
	// It will be switched depending on the player's movement direction
	Animation* currentAnimation = nullptr;

	// A set of animations
	Animation idleAnim;
	Animation upAnim;
	Animation downAnim;
	Animation leftAnim;
	Animation rightAnim;
	Animation shotAnim;
	Animation deadAnimLeft;
	Animation deadAnimRight;
	Animation idleStairs;
	Animation idleTopStairs;

	// The player's collider
	Collider* collider = nullptr;

	// A flag to detect when the player has been destroyed
	bool destroyed = false;

	// A countdown to when the player gets destroyed. After a while, the game exits
	uint destroyedCountdown = 180;

	// Font score index
	uint score = 000;
	uint time = 100;

	uint count = 0;
	uint ready = 3;
	int uiIndex;
	int timeIndex;
	char scoreText[10] = { "\0" };
	char timeText[3] = { "\0" };
	char lifeText[3] = { "\0" };
	uint timeBonus = 0;

	//Lifes and management
	int lifes = 3;
	int cont = 0;
	SDL_Texture* lifesTexture1 = nullptr;
	SDL_Rect lifesTextureRect = { 142, 42, 16, 16 };

	//Booleans for scenes
	bool scene1 = false;
	bool scene2 = false;
	bool scene3 = false;
	bool scene4 = false;
	bool scene5 = false;
	bool scene6 = false;

	//Boolean for God Mode
	bool godMode = false;

	//Shield booster (Protects one hit)
	int inmunityTime = 0;
	bool inmunity = false;

	//Clock booster
	int stopTime = 0;

	//Dynamite Booster

	bool dynamite = false;
	int dynamiteBoosterDelay = 0;
	int dynamiteBoosterStop = 0;

	//Doubleshot Booster
	bool doubleshot = false;

	//Current weapon Normal Harpoon = 0, Hookshot = 1, Gunshot = 2, DoubleShot = 3.
	int currWeapon = 0;

	int countForIdleAnim = 10;


	//Enum class for Enemy Type
	enum class ENEMY_TYPE lastBalloon;
};

#endif //!__MODULE_PLAYER_H__