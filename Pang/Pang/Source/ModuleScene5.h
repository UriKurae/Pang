#pragma once

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;
struct Collider;

class ModuleScene5 : public Module
{

public:

	ModuleScene5(bool startEnabled);

	~ModuleScene5();


	// Called when the module is activated
	// Loads the necessary textures for the map background
	bool Start() override;

	update_status Update() override;

	// Called at the end of the application loop.
	// Performs the render call of all the parts of the scene's background
	update_status PostUpdate() override;

	bool CleanUp();

	void pushBlocksAnims();

public:

	uint countDownToFade = 180;
	int toFade = 5;

	bool destroyedBlockTopLeft = false;
	bool destroyedBlockBottomLeft = false;
	bool destroyedBlockMiddle = false;
	bool destroyedBlockTopRight = false;
	bool destroyedBlockBottomRight = false;

	// The background sprite sheet loaded into an SDL_Texture
	SDL_Texture* bgTexture = nullptr;

	//The foreground sprite sheet loaded into an SDL_Texture
	SDL_Texture* fgTexture = nullptr;

	Animation blockVer;

	//Animation pushes
	Animation blockDestroyTopLeft;
	Animation blockDestroyBottomLeft;
	Animation blockDestroyMiddle;
	Animation blockDestroyTopRight;
	Animation blockDestroyBottomRight;
	   
	Animation* currentAnim = nullptr;

	//Foregrounds when dying
	SDL_Texture* deathTexture1 = nullptr;
	SDL_Texture* deathTexture2 = nullptr;

	//Change the music
	bool musicBool1 = false;
	bool musicBool2 = false;

};
