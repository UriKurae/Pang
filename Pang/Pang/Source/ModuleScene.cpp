#include "ModuleScene.h"

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
#include "ModuleBoosters.h"
#include "IntroScene.h"
#include "ModuleTileset.h"

#include "SDL/include/SDL.h"
#include "SDL/include/SDL_scancode.h"
#include "SDL_mixer/include/SDL_mixer.h"



ModuleScene::ModuleScene(bool startEnabled) : Module(startEnabled)
{
	name = "LEVEL 1";
}

ModuleScene::~ModuleScene()
{

}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading background assets");

	bool ret = true;

	countDownToFade = 300;
	bgTexture = App->textures->Load("Assets/Backgrounds/Mt.Fuji(Day).png");
	++activeTextures; ++totalTextures;
	deathTexture1 = App->textures->Load("Assets/Foregrounds/Foreground_Death_1.png");
	++activeTextures; ++totalTextures;
	deathTexture2 = App->textures->Load("Assets/Foregrounds/Foreground_Death_2.png");
	++activeTextures; ++totalTextures;
	

	App->audio->PlayMusic("Assets/Sound/Soundtracks/MtFuji.ogg", 0.0f);

	App->player->Enable();
	App->enemies->Enable();
	App->collisions->Enable();
	App->boosters->Enable();
	App->tileset->Enable();

	//ADD ENEMIES
	App->enemies->AddEnemy(ENEMY_TYPE::VERYBIGBALLOON, 50, 20);
	
	App->player->uiIndex = 0;
	
	App->player->score = 0;

	App->player->scene1 = true;
	App->player->scene2 = false;
	App->player->scene3 = false;
	App->player->scene4 = false;
	App->player->scene5 = false;
	App->player->scene6 = false;

	musicBool1 = true;
	musicBool2 = true;

	App->enemies->balloon.balloonsOnScene = 1;

	//Buff=App->collisions->AddCollider({ 20, 190, 10, 10 }, Collider::Type::PLAYER); FOR TESTS DON'T ERASE PLS

	return ret;
}

update_status ModuleScene::Update()
{
	//LOG("Balloons On Stage %d", App->scene->balloonsOnScene);
	if (App->input->keys[SDL_SCANCODE_F11] == KEY_STATE::KEY_DOWN)
	{
		App->enemies->balloon.balloonsOnScene = 0;
	}

	if (App->enemies->balloon.balloonsOnScene == 0)
	{
		App->fade->FadeToBlack((Module*)App->scene, (Module*)App->winScene, 60);
	}

//	LOG("enemyOnStage == %d", balloonsOnScene);
	//LOG("Lifes: ---------%d---------", App->player->lifes);

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

// Update: draw background
update_status ModuleScene::PostUpdate()
{
	// Draw everything --------------------------------------

	App->render->Blit(bgTexture, 0, 0, NULL);

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
			App->fade->FadeToBlack((Module*)App->scene, (Module*)App->scene, 60);
		}
		else
		{
			App->fade->FadeToBlack((Module*)App->scene, (Module*)App->sceneIntro, 60);
		}
	}



	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
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
	
	App->textures->Unload(App->boosters->texture);
	--App->boosters->totalTextures;

	App->textures->Unload(bgTexture);
	--totalTextures;
	App->textures->Unload(deathTexture1);
	--totalTextures;
	App->textures->Unload(deathTexture2);
	--totalTextures;

	App->collisions->RemoveCollider(App->boosters->typeBooster->collider);
	--App->boosters->totalColliders;

	App->textures->Unload(App->enemies->texture);
	--App->enemies->totalTextures;

	return true;

}