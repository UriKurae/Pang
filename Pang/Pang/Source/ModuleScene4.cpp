#include "ModuleScene4.h"

#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModuleCollisions.h"
#include "ModuleEnemies.h"
#include "Enemy_Balloon.h"
#include "ModuleFadeToBlack.h"
#include "ModulePlayer.h"
#include "ModuleHarpoon_Shot.h"
#include "ModuleHarpoon_HookShot.h"
#include "ModuleGunShot.h"
#include "ModuleHarpoon_DoubleShot.h"
#include "ModuleInput.h"
#include "ModuleFonts.h"
#include "ModuleTextures.h"
#include "IntroScene.h"
#include "ModuleTileset.h"
#include "ModuleBoosters.h"

#include "SDL/include/SDL.h"
#include "SDL/include/SDL_scancode.h"
#include "SDL_mixer/include/SDL_mixer.h"

ModuleScene4::ModuleScene4(bool startEnabled) : Module(startEnabled)
{
	name = "LEVEL 4";
}

ModuleScene4::~ModuleScene4()
{
}

bool ModuleScene4::Start()
{
	LOG("Loading background assets");

	bool ret = true;

	fgTexture = App->textures->Load("Assets/Items&Weapons/BlockSprites.png"); //fg on 2st Level is invisible
	++activeTextures; ++totalTextures;

	block.PushBack({ 221, 40, 8, 32 });

	countDownToFade = 300;
	bgTexture = App->textures->Load("Assets/Backgrounds/Mt.Keirin(Day).png");
	++activeTextures; ++totalTextures;
	deathTexture1 = App->textures->Load("Assets/Foregrounds/Foreground_Death_1.png");
	++activeTextures; ++totalTextures;
	deathTexture2 = App->textures->Load("Assets/Foregrounds/Foreground_Death_2.png");
	++activeTextures; ++totalTextures;
	


	stairs.PushBack({ 241, 284, 22, 16 });

	App->audio->PlayMusic("Assets/Sound/Soundtracks/MtKeirin.ogg", 0.0f);

	App->player->scene1 = false;
	App->player->scene2 = false;
	App->player->scene3 = false;
	App->player->scene4 = true;
	App->player->scene5 = false;
	App->player->scene6 = false;


	App->player->Enable();
	App->enemies->Enable();
	App->collisions->Enable();
	App->tileset->Enable();
	App->boosters->Enable();

	//ADD ENEMIES
	App->enemies->AddEnemy(ENEMY_TYPE::VERYBIGBALLOON, 152, 16);

	App->player->uiIndex = 0;

	App->player->score = 0;

	App->enemies->balloon.balloonsOnScene = 1;

	//Buff=App->collisions->AddCollider({ 20, 190, 10, 10 }, Collider::Type::PLAYER); FOR TESTS DON'T ERASE PLS

	musicBool1 = true;
	musicBool2 = true;

	return ret;
}

update_status ModuleScene4::Update()
{
	//LOG("Balloons On Stage %d", App->scene->balloonsOnScene);
	if (App->input->keys[SDL_SCANCODE_F11] == KEY_STATE::KEY_DOWN)
	{
		App->enemies->balloon.balloonsOnScene = 0;
	}

	if (App->enemies->balloon.balloonsOnScene == 0)
	{
		App->fade->FadeToBlack((Module*)App->scene4, (Module*)App->winScene, 60);
	}

	if (App->player->time == 50 && musicBool1 == true) {
		musicBool1 = false;
		App->audio->PlayMusic("Assets/Sound/Soundtracks/GettingLate.ogg", 0.0f);
	}

	if (App->player->time == 20 && musicBool2 == true) {
		musicBool2 = false;
		App->audio->PlayMusic("Assets/Sound/Soundtracks/OutOfTime!.ogg", 0.0f);
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScene4::PostUpdate()
{
	App->render->Blit(bgTexture, 0, 0, NULL);

	App->render->Blit(fgTexture, 161, 184, &(stairs.GetCurrentFrame()), 1.0f);
	App->render->Blit(fgTexture, 161, 168, &(stairs.GetCurrentFrame()), 1.0f);
	App->render->Blit(fgTexture, 161, 156, &(stairs.GetCurrentFrame()), 1.0f);

	App->render->Blit(fgTexture, 201, 184, &(stairs.GetCurrentFrame()), 1.0f);
	App->render->Blit(fgTexture, 201, 168, &(stairs.GetCurrentFrame()), 1.0f);
	App->render->Blit(fgTexture, 201, 156, &(stairs.GetCurrentFrame()), 1.0f);

	if (destroyedBlockLeft == false)
	{
		App->render->Blit(fgTexture, 184, 156, &(block.GetCurrentFrame()), 1.0f);
	}

	if (destroyedBlockRight == false)
	{
		App->render->Blit(fgTexture, 192, 156, &(block.GetCurrentFrame()), 1.0f);
	}



	//This could be more clean 
	//Animation to stop the scene with the death 
	if (App->player->lifes == 0) {
		App->render->Blit(App->player->gameOverTexture, 150, 99, NULL);
	}

	if (App->player->destroyed || App->player->time == 0)
	{
		countDownToFade--;
	}
	if (countDownToFade > 220 && countDownToFade < 230)
	{
		App->render->Blit(deathTexture1, 0, 0, NULL);
	}
	else if (countDownToFade > 215 && countDownToFade < 220)
	{
		App->render->Blit(deathTexture2, 0, 0, NULL);
	}
	else if (countDownToFade == 180)
	{
		if (App->player->lifes > 0)
		{
			App->fade->FadeToBlack((Module*)App->scene4, (Module*)App->scene4, 60);
		}
		else
		{
			App->fade->FadeToBlack((Module*)App->scene4, (Module*)App->sceneIntro, 60);
		}
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene4::CleanUp()
{
	LOG("---------------------------------------------- CleanUp ModuleScene ----------------------------------------------")

		activeTextures = activeColliders = activeFonts = activeFx = 0;


	App->player->Disable();
	App->enemies->Disable();
	App->collisions->Disable();
	App->tileset->Disable();
	App->boosters->Disable();
	App->harpoon->Disable();
	App->doubleShot->Disable();
	App->gunShot->Disable();
	App->hookShot->Disable();

	App->sceneIntro->countdown = 1;

	App->textures->Unload(bgTexture);
	--totalTextures;
	App->textures->Unload(deathTexture1);
	--totalTextures;
	App->textures->Unload(deathTexture2);
	--totalTextures;
	App->textures->Unload(App->boosters->texture);
	--totalTextures;

	App->collisions->RemoveCollider(App->boosters->typeBooster->collider);
	--App->boosters->totalColliders;

	App->textures->Unload(App->enemies->texture);
	--App->enemies->totalTextures;

	return true;
}
