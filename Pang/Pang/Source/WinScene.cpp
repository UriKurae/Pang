﻿#include "WinScene.h"

#include "Application.h"
#include "ModuleFadeToBlack.h"
#include "ModuleTextures.h"
#include "ModuleRender.h"
#include "ModuleAudio.h"
#include "ModuleInput.h"

#include "ModuleCollisions.h"
#include "ModulePlayer.h"
#include "ModuleFonts.h"
#include <stdio.h>

#include "SDL\include\SDL.h"

#include <SDL\include\SDL_scancode.h>

WinScene::WinScene(bool startEnabled) : Module(startEnabled)
{
	name = "WIN SCENE";
}

WinScene::~WinScene()
{

}

// Load assets
bool WinScene::Start()
{
	LOG("Loading background assets");

	bool ret = true;

	bgTexture = App->textures->Load("Assets/UI/WinAnimations.png");
	App->audio->PlayMusic("Assets/Sound/Sounds_Gameplay/Level_Complete.ogg", 1.0f);


	char lookupTable2[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!✕-:©✕ " };
	winIndex = App->fonts->Load("Assets/UI/Fonts/Pang_font.png", lookupTable2, 1);

	App->render->camera.x = 0;
	App->render->camera.y = 0;

	if (App->player->scene1 == true) {
		winAnim.PushBack({ 0, 0, 194, 96 });
		winAnim.PushBack({ 194, 0, 194, 96 });
		winAnim.speed = 0.1f;
		winAnim.loop = true;
	}
	if (App->player->scene2 == true) {
		winAnim2.PushBack({ 0, 96, 194, 96 });
		winAnim2.speed = 0.1f;
		winAnim2.loop = true;
	}
	

	App->input->Enable();
	

	return ret;
}

update_status WinScene::Update()
{

	if (App->input->keys[SDL_SCANCODE_RETURN] == KEY_STATE::KEY_DOWN && App->player->scene1 == true)
	{
		App->fade->FadeToBlack(this, (Module*)App->scene2, 90);
	}

	if (App->input->keys[SDL_SCANCODE_RETURN] == KEY_STATE::KEY_DOWN && App->player->scene2 == true)
	{
		App->fade->FadeToBlack(this, (Module*)App->sceneIntro, 90);
	}

	winAnim.Update();

	return update_status::UPDATE_CONTINUE;
}

// Update: draw background
update_status WinScene::PostUpdate()
{
	// Draw everything --------------------------------------
	sprintf_s(bonusText, 6, "%d", App->player->timeBonus);
	if (App->player->scene1 == true) {
		App->render->Blit(bgTexture, 95, 32, &(winAnim.GetCurrentFrame()), 0.2f);
		App->fonts->BlitText(168, 145, winIndex, "1STAGE");
	}
	if (App->player->scene2 == true) {
		App->render->Blit(bgTexture, 95, 32, &(winAnim2.GetCurrentFrame()), 0.2f);
		App->fonts->BlitText(168, 145, winIndex, "2STAGE");
	}
	App->fonts->BlitText(102, 170, winIndex, "TIME BONUS");
	App->fonts->BlitText(214, 170, winIndex, bonusText);
	App->fonts->BlitText(263, 170, winIndex, "PTS.");
	App->fonts->BlitText(102, 186, winIndex, "NEXT EXTEND");
	App->fonts->BlitText(214, 186, winIndex, "20000");
	App->fonts->BlitText(263, 186, winIndex, "PTS.");

	return update_status::UPDATE_CONTINUE;
}

bool WinScene::CleanUp()
{
	SDL_DestroyTexture(bgTexture);
	
	App->fonts->UnLoad(winIndex);

	winAnim.Reset();

	return true;
}