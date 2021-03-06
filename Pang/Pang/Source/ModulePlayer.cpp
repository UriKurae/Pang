﻿#include "ModulePlayer.h"

#include "Application.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleParticles.h"
#include "ModuleAudio.h"
#include "ModuleCollisions.h"
#include "ModuleScene.h"
#include "ModuleScene2.h"
#include "ModuleHarpoon_Shot.h"
#include "ModuleHarpoon_HookShot.h"
#include "ModuleFadeToBlack.h"
#include "ModuleBoosters.h"
#include "ModuleHarpoon_DoubleShot.h"
#include "ModuleGunShot.h"
#include "ModuleTileset.h"

#include "Enemy.h"
#include "ModuleEnemies.h"

#include "ModuleFonts.h"

#include "IntroScene.h"

#include "SDL/include/SDL.h"
#include "SDL/include/SDL_scancode.h"


#include <stdio.h>
#include "ModuleWindow.h"

ModulePlayer::ModulePlayer(bool startEnabled) : Module(startEnabled)
{
	name = "PLAYER";

	// idle animation - just one sprite
	idleAnim.PushBack({ 0, 110, 26, 32 });

	// move left
	leftAnim.PushBack({ 379, 0, 30, 31 });
	leftAnim.PushBack({ 416, 0, 28, 31 });
	leftAnim.PushBack({ 448, 0, 30, 32 });
	leftAnim.PushBack({ 482, 0, 30, 31 });
	leftAnim.PushBack({ 516, 0, 30, 32 });
	leftAnim.speed = 0.3f;

	//move right
	rightAnim.PushBack({ 0, 0, 30, 32 });
	rightAnim.PushBack({ 33, 0, 30, 32 });
	rightAnim.PushBack({ 68, 0, 30, 32 });
	rightAnim.PushBack({ 102, 0, 28, 32 });
	rightAnim.PushBack({ 136, 0, 30, 32 });
	rightAnim.speed = 0.3f;

	//move upStairs
	upAnim.PushBack({ 0, 34, 25, 32 });
	upAnim.PushBack({ 37, 34, 24, 32 });
	upAnim.PushBack({ 69, 34, 26, 32 });
	upAnim.PushBack({ 103, 34, 26, 32 });
	upAnim.PushBack({ 4, 75, 24, 31 });
	upAnim.PushBack({ 35, 75, 27, 31 });
	upAnim.PushBack({ 69, 76, 27, 30 });
	upAnim.speed = 0.2f;
	upAnim.loop = true;

	//move downStairs
	downAnim.PushBack({ 69, 76, 27, 30 });
	downAnim.PushBack({ 35, 75, 27, 31 });
	downAnim.PushBack({ 4, 75, 24, 31 });
	downAnim.PushBack({ 103, 34, 26, 32 });
	downAnim.PushBack({ 69, 34, 26, 32 });
	downAnim.PushBack({ 37, 34, 24, 32 });
	downAnim.PushBack({ 0, 34, 25, 32 });
	downAnim.speed = 0.2f;
	downAnim.loop = true;

	//idle stairs anim
	idleStairs.PushBack({ 103,34,26,32 });

	//idle top stairs anim
	idleTopStairs.PushBack({ 104, 81, 28, 26 });

	//left death animation
	deadAnimLeft.PushBack({ 69, 110 ,41, 30 });

	//right death animation
	deadAnimRight.PushBack({ 120, 110, 41, 30 });

	//shot animation
	shotAnim.PushBack({ 32, 113, 27, 33 });
}

ModulePlayer::~ModulePlayer()
{

}

bool ModulePlayer::Start()
{
	LOG("Loading player textures");

	bool ret = true;
	bool dynamite = false;
	bool godMode = false;

	currWeapon = 0;

	ready = 3;

	stopTime = 0;
	inmunityTime = 0;

	texture = App->textures->Load("Assets/Movement/Sprite_Sheet_Movement.png");
	timeTexture = App->textures->Load("Assets/UI/Time.png");
	++activeTextures; ++totalTextures;
	currentAnimation = &idleAnim;

	gameOverTexture = App->textures->Load("Assets/UI/GameOver.png");
	++activeTextures; ++totalTextures;
	readyTexture = App->textures->Load("Assets/UI/Ready.png");
	++activeTextures; ++totalTextures;

	//Load textures for the player's life
	lifesTexture1 = App->textures->Load("Assets/Movement/Sprite_Sheet_Movement.png");
	++activeTextures; ++totalTextures;

	//SET SPAWN POSITION FOR PLAYER
	if (scene4 == true) {
		position.x = 80;
		position.y = SCREEN_HEIGHT - 77;
	}
	else
	{
		position.x = (SCREEN_WIDTH / 2) - 20;
		position.y = SCREEN_HEIGHT - 77;
	}

	collider = App->collisions->AddCollider({ position.x, position.y, 26, 32 }, Collider::Type::PLAYER, this);
	++activeColliders; ++totalColliders;
	destroyed = false;
	time = 100;
	//timeMusic = 3300;

	char lookupTable[] = { "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!✕-:©✕ " };
	uiIndex = App->fonts->Load("Assets/UI/Fonts/Pang_font.png", lookupTable, 1);
	++activeFonts; ++totalFonts;
	char timeTable[] = { "0123456789" };
	timeIndex = App->fonts->Load("Assets/UI/CountdownNumbers.png", timeTable, 1);
	++activeFonts; ++totalFonts;


	//Enable all weapons

	App->harpoon->Enable();
	App->hookShot->Enable();
	App->gunShot->Enable();
	App->doubleShot->Enable();

	speed = 2;

	return ret;
}

void ModulePlayer::checkUnbreakable()
{
	LOG("%i  %i", tile.x, tile.y);
	for (int i = 0; i < 3; ++i)
	{
		if (App->tileset->getTileLevel(tile.y + i, tile.x + 3).id == ModuleTileset::TileType::UNBREAKABLE)
		{
			position.x--;
		}
		else if (App->tileset->getTileLevel(tile.y + i, tile.x).id == ModuleTileset::TileType::UNBREAKABLE)
		{
			position.x++;
		}
	}
}

void ModulePlayer::checkIfNeedToFall()
{
	if (position.y < SCREEN_HEIGHT - 77)
	{
		if (App->tileset->getTileLevel(tile.y + 3, tile.x + 1).id != ModuleTileset::TileType::STAIRS &&
			App->tileset->getTileLevel(tile.y + 3, tile.x + 1).id != ModuleTileset::TileType::TOP_STAIRS &&
			App->tileset->getTileLevel(tile.y + 3, tile.x + 1).id != ModuleTileset::TileType::UNBREAKABLE)
		{
			if (currentAnimation != &idleAnim)
			{
				idleAnim.Reset();
				currentAnimation = &idleAnim;
			}

			speed = 0;
			position.y++;
			collider->SetPos(position.x, position.y);

			if (App->tileset->getTileLevel(tile.y + 5, tile.x - 1).id == ModuleTileset::TileType::WALL)
			{
				speed = 2;
			}
		}
	}
}

void ModulePlayer::checkIfWall()
{
	if (App->tileset->getTileLevel(tile.y, tile.x).id == ModuleTileset::TileType::WALL)
	{
		position.x += 2;
	}
	else if (App->tileset->getTileLevel(tile.y, tile.x + 3).id == ModuleTileset::TileType::WALL)
	{
		position.x -= 2;
	}
}

update_status ModulePlayer::Update()
{
	GamePad& pad = App->input->pads[0];
	tile = { position.x / TILE_SIZE, position.y / TILE_SIZE };

	//TO GET THE MOUSE POSITION, SDL_GETMouseState, MUST FIX THE FOR SOME FUCKING REASON THE BALLS WONT SPAWN.
	if (App->input->keys[SDL_SCANCODE_V] == KEY_STATE::KEY_DOWN)
	{
		SDL_GetMouseState(&mouseX, &mouseY);

		App->enemies->AddEnemy(ENEMY_TYPE::VERYBIGBALLOON, (mouseX), (mouseY));
	}

	//Debug key for gamepad rumble testing purposes
	if (App->input->keys[SDL_SCANCODE_1] == KEY_STATE::KEY_DOWN)
	{
		App->input->ShakeController(0, 12, 0.33f);
	}

	//Debug key for gamepad rumble testing purposes
	if (App->input->keys[SDL_SCANCODE_2] == KEY_STATE::KEY_DOWN)
	{
		App->input->ShakeController(0, 36, 0.66f);
	}

	//Debug key for gamepad rumble testing purposes
	if (App->input->keys[SDL_SCANCODE_3] == KEY_STATE::KEY_DOWN)
	{
		App->input->ShakeController(0, 60, 1.0f);
	}

	//Press button G for God Mode
	if (App->input->keys[SDL_SCANCODE_G] == KEY_STATE::KEY_DOWN)
	{
		godMode = !godMode;
	}

	count++;

	if (count % 30 == 0 && ready > 0) {
		ready--;
	}

	if (ready == 0) {
		if (count % 60 == 0 && time > 0 && App->enemies->balloon.balloonsOnScene > 0 && destroyed == false) {
			time--;
		}

		if (App->enemies->balloon.balloonsOnScene == 0) {
			timeBonus = time * 100;
		}

		if (App->input->keys[SDL_SCANCODE_LALT] == KEY_STATE::KEY_REPEAT && App->input->keys[SDL_SCANCODE_H] == KEY_STATE::KEY_DOWN) {
			
			if (currWeapon == 0) {
				currWeapon = 1;
			}

			else if (currWeapon == 1) {
				currWeapon = 2;
			}

			else if (currWeapon == 2) {
				currWeapon = 3;
			}

			else if (currWeapon == 3) {
				currWeapon = 0;
			}
		}

		//Detect inputs
		if (App->input->keys[SDL_SCANCODE_A] == KEY_STATE::KEY_REPEAT || pad.l_x < 0 || pad.left)
		{
			if (!destroyed && App->tileset->getTileLevel(tile.y + 4, tile.x + 1).id != ModuleTileset::TileType::STAIRS)
			{
				position.x -= speed;
				collider->SetPos(position.x + 5, position.y);

				if (currentAnimation != &leftAnim)
				{
					leftAnim.Reset();
					currentAnimation = &leftAnim;
				}
			}
		}

		if (App->input->keys[SDL_SCANCODE_D] == KEY_STATE::KEY_REPEAT || pad.l_x > 0 || pad.right)
		{
			if (!destroyed && App->tileset->getTileLevel(tile.y + 4, tile.x + 1).id != ModuleTileset::TileType::STAIRS &&
				App->tileset->getTileLevel(tile.y + 4, tile.x + 1).id != ModuleTileset::TileType::EMPTY)
			{

				position.x += speed;
				collider->SetPos(position.x, position.y);

				if (currentAnimation != &rightAnim)
				{
					rightAnim.Reset();
					currentAnimation = &rightAnim;

				}
			}
		}

		if ((App->input->keys[SDL_SCANCODE_SPACE] == KEY_STATE::KEY_DOWN || pad.a) && ready == 0)
		{
			if (currentAnimation != &shotAnim && App->harpoon->destroyed == true)
			{
				if (App->enemies->balloon.balloonsOnScene == 0)
				{
					idleAnim.Reset();
					currentAnimation = &idleAnim;
				}
				else
				{
					shotAnim.Reset();
					currentAnimation = &shotAnim;
				}
			}
		}
		//SOLVED SHOT ANIMATION IF SPACE IS ON REPEAT
		if (App->input->keys[SDL_SCANCODE_SPACE] == KEY_STATE::KEY_REPEAT || pad.a)
		{
			countForIdleAnim--;
			if (countForIdleAnim == 0)
			{
				currentAnimation = &idleAnim;
				countForIdleAnim = 10;
			}
		}

		//if space is on repeat and movement detected
		if ((App->input->keys[SDL_SCANCODE_SPACE] == KEY_STATE::KEY_REPEAT) && App->input->keys[SDL_SCANCODE_A] == KEY_STATE::KEY_REPEAT)
		{
			currentAnimation = &leftAnim;
		}
		if ((App->input->keys[SDL_SCANCODE_SPACE] == KEY_STATE::KEY_REPEAT) && App->input->keys[SDL_SCANCODE_D] == KEY_STATE::KEY_REPEAT)
		{
			currentAnimation = &rightAnim;
		}

		
		//Detect when S and D or S and A are pressed at the same time
		if ((App->input->keys[SDL_SCANCODE_S] == KEY_STATE::KEY_REPEAT || pad.down) && (App->input->keys[SDL_SCANCODE_D] == KEY_STATE::KEY_REPEAT || pad.right))
		{
			currentAnimation = &rightAnim;
		}
		if ((App->input->keys[SDL_SCANCODE_S] == KEY_STATE::KEY_REPEAT || pad.right) && (App->input->keys[SDL_SCANCODE_A] == KEY_STATE::KEY_REPEAT || pad.left))
		{
			currentAnimation = &leftAnim;
		}


		//Detect when A and D are pressed at the same time and set the current animation to idle
		if (App->input->keys[SDL_SCANCODE_A] == KEY_STATE::KEY_REPEAT && App->input->keys[SDL_SCANCODE_D] == KEY_STATE::KEY_REPEAT ||
			pad.left && pad.right)
		{
			idleAnim.Reset();
			currentAnimation = &idleAnim;
			position.x = position.x;
		}
		// If no movement detected, set the current animation back to idle
		if (App->input->keys[SDL_SCANCODE_A] == KEY_STATE::KEY_IDLE &&
			App->input->keys[SDL_SCANCODE_D] == KEY_STATE::KEY_IDLE &&
			App->input->keys[SDL_SCANCODE_SPACE] == KEY_STATE::KEY_IDLE &&
			App->input->keys[SDL_SCANCODE_W] == KEY_STATE::KEY_IDLE &&
			App->input->keys[SDL_SCANCODE_S] == KEY_STATE::KEY_IDLE && !pad.right && !pad.left && !pad.up && !pad.down && pad.l_x == 0 && pad.l_y == 0 && !pad.a)
		{
			if (App->tileset->getTileLevel(tile.y + 4, tile.x + 1).id == ModuleTileset::TileType::STAIRS)
			{
				currentAnimation = &idleStairs;
			}
			else
			{
				currentAnimation = &idleAnim;
				collider->SetPos(position.x, position.y);
			}
		}




		//----------------------------------------STAIRS CODE----------------------------------------//

		//UP STAIRS

		if (App->input->keys[SDL_SCANCODE_W] == KEY_STATE::KEY_REPEAT || pad.l_y < 0 || pad.up)
		{
			if (!destroyed)
			{

				if (App->tileset->getTileLevel(tile.y + 3, tile.x + 1).id == ModuleTileset::TileType::STAIRS)
				{
					if (currentAnimation != &upAnim)
					{
						upAnim.Reset();
						currentAnimation = &upAnim;

					}

					position.y--;
				}


				if (App->tileset->getTileLevel(tile.y + 3, tile.x + 1).id == ModuleTileset::TileType::TOP_STAIRS)
				{
					if (currentAnimation != &idleAnim)
					{
						idleAnim.Reset();
						currentAnimation = &idleAnim;
					}
					position.y = 124;
				}
				collider->SetPos(position.x, position.y);
				checkIfNeedToFall();
			}
		}

		//DOWN STAIRS

		if ((App->input->keys[SDL_SCANCODE_S] == KEY_STATE::KEY_REPEAT || pad.l_y > 0 || pad.down) && !destroyed)
		{

			if (App->tileset->getTileLevel(tile.y + 3, tile.x + 1).id == ModuleTileset::TileType::STAIRS &&
				position.y < SCREEN_HEIGHT - 77)
			{
				if (currentAnimation != &downAnim)
				{
					downAnim.Reset();
					currentAnimation = &downAnim;

				}
				position.y++;
			}

			if (App->tileset->getTileLevel(tile.y + 4, tile.x + 1).id == ModuleTileset::TileType::TOP_STAIRS ||
				App->tileset->getTileLevel(tile.y + 4, tile.x + 1).id == ModuleTileset::TileType::STAIRS)
			{
				if (currentAnimation != &downAnim)
				{
					downAnim.Reset();
					currentAnimation = &downAnim;
				}

				position.y++;
			}

			if (App->tileset->getTileLevel(tile.y + 4, tile.x + 1).id == ModuleTileset::TileType::WALL && 
				App->tileset->getTileLevel(tile.y+3, tile.x+1).id == ModuleTileset::TileType::STAIRS)
			{
				if (currentAnimation != &idleAnim)
				{
					idleAnim.Reset();
					currentAnimation = &idleAnim;
				}
			}
			collider->SetPos(position.x, position.y);

			checkIfNeedToFall();

		}

		checkIfNeedToFall();
		checkUnbreakable();
		checkIfWall();

		currentAnimation->Update();

		if (inmunityTime < 181 && inmunityTime > 0) { inmunityTime--; }

		if (stopTime > 0) { stopTime--; }

		if (dynamiteBoosterStop > 0)
		{
			dynamiteBoosterStop--;
		}
		else
		{
			dynamite = false;
		}

	}


	return update_status::UPDATE_CONTINUE;
}

update_status ModulePlayer::PostUpdate()
{
	if (inmunityTime == 181)
	{
		App->boosters->inmunityAnim = &App->boosters->shieldInmunity;
		App->render->Blit(App->boosters->texture, position.x - 2, position.y - 7, &(App->boosters->inmunityAnim->GetCurrentFrame()), 1.0f);
	}

	SDL_Rect rect = currentAnimation->GetCurrentFrame();

	if (ready == 3 || ready == 1) {
		App->render->Blit(App->player->readyTexture, 150, 99, NULL);
	}

	if (!destroyed)
	{
		App->render->Blit(texture, position.x, position.y, &rect);
	}

	if (App->input->keys[SDL_SCANCODE_F9] == KEY_STATE::KEY_DOWN)
	{
		destroyed = true;
		lifes--;

		App->collisions->Disable();
		App->sceneIntro->countdown = 1;

	}

	if (destroyed == true) //Blit the dead animation
	{

		currentAnimation = &deadAnimLeft;

		rect = currentAnimation->GetCurrentFrame();

		App->render->Blit(texture, position.x, position.y, &rect);

		//App->fade->FadeToBlack((Module*)App->scene, (Module*)App->sceneIntro, 60);

	}

	sprintf_s(scoreText, 10, "%d", score);
	sprintf_s(timeText, 5, "%3d", time);

	App->fonts->BlitText(81, 216, uiIndex, scoreText);
	App->fonts->BlitText(25, 208, uiIndex, "PLAYER-1");

	if (scene1 == true)
	{
		App->fonts->BlitText(161, 228, uiIndex, "1-1 STAGE");
		App->fonts->BlitText(158, 208, uiIndex, "MT.FUJI");
	}
	else if (scene2 == true)
	{
		App->fonts->BlitText(161, 228, uiIndex, "1-2 STAGE");
		App->fonts->BlitText(158, 208, uiIndex, "MT.FUJI");
	}
	else if (scene3 == true)
	{
		App->fonts->BlitText(161, 228, uiIndex, "1-3 STAGE");
		App->fonts->BlitText(158, 208, uiIndex, "MT.FUJI");
	}
	else if (scene4 == true)
	{
		App->fonts->BlitText(161, 228, uiIndex, "2-4 STAGE");
		App->fonts->BlitText(158, 208, uiIndex, "MT.KEIRIN");
	}
	else if (scene5 == true)
	{
		App->fonts->BlitText(161, 228, uiIndex, "2-5 STAGE");
		App->fonts->BlitText(158, 208, uiIndex, "MT.KEIRIN");
	}
	else if (scene6 == true)
	{
		App->fonts->BlitText(161, 228, uiIndex, "2-6 STAGE");
		App->fonts->BlitText(158, 208, uiIndex, "MT.KEIRIN");
	}
	App->fonts->BlitText(151, 236, uiIndex, "HI: 100000");
	App->fonts->BlitText(272, 208, uiIndex, "PLAYER-2");
	App->render->Blit(timeTexture, 269, 9, NULL);
	App->fonts->BlitText(334, 9, timeIndex, timeText);

	//Print the lifes of the player, the for is for the icons, the next lines are for the number


	for (int i = 0; i < lifes; i++)
	{
		if (i == 0)
		{
			App->render->Blit(lifesTexture1, 25, 227, &lifesTextureRect, 0, false);
		}
		else if (i < 4)
		{
			App->render->Blit(lifesTexture1, 25 + (16 * i), 227, &lifesTextureRect, 0, false);
		}
	}

	sprintf_s(lifeText, 3, "%d", lifes);
	if (lifes > 4) { App->fonts->BlitText(89, 235, uiIndex, lifeText); }


	return update_status::UPDATE_CONTINUE;
}

void ModulePlayer::OnCollision(Collider* c1, Collider* c2)
{
	if (c2->type == Collider::Type::WALL) {
		if (c1->rect.x > c2->rect.x && c1->rect.x < c2->rect.x + c2->rect.w) {
			position.x = position.x + 2;
		}

		if (c1->rect.x + c1->rect.w > c2->rect.x && c1->rect.x + c1->rect.w < c2->rect.x + c2->rect.w) {
			position.x = position.x - 2;
		}

		if (c1->rect.y + c1->rect.h > c2->rect.y) {
			position.y = 168;
		}

		collider->SetPos(position.x, position.y);
	}

	if (c2->type == Collider::Type::BALLOON && godMode == false && inmunityTime == 0)
	{
		destroyed = true;

		c1->pendingToDelete = true;
		c2->pendingToDelete = true;
		lifes--;
	}

	if (c2->type == Collider::Type::BALLOON && godMode == false && inmunityTime == 181)
	{
		inmunityTime--;
	}
	if (c2->type == Collider::Type::BOOSTERS) {

		//Normal boosters
		if (c2 == App->boosters->typeBooster[SHIELD].collider)
		{
			inmunityTime = 181;
		}

		if (c2 == App->boosters->typeBooster[CLOCK].collider)
		{
			stopTime = 420;
		}

		if (c2 == App->boosters->typeBooster[DYNAMITE].collider)
		{
			dynamite = true;
			dynamiteBoosterStop = 100;
		}

		if (c2 == App->boosters->typeBooster[DOUBLESHOT].collider)
		{
			doubleshot = true;
			currWeapon = 3;
		}

		if (c2 == App->boosters->typeBooster[EXTRALIFE].collider)
		{
			App->player->lifes++;
		}


		//Gun Boosters
		if (c2 == App->boosters->typeBooster[GUN].collider)
		{
			currWeapon = 2;
			doubleshot = false;

		}

		if (c2 == App->boosters->typeBooster[HOOK].collider)
		{
			currWeapon = 1;
			doubleshot = false;
		}
		for (int i = 0; i < MAX; i++)
		{
			if (App->boosters->typeBooster[i].booster == true && c2 == App->boosters->typeBooster[i].collider)
			{
				App->boosters->typeBooster[i].collider->pendingToDelete = true;
				App->boosters->typeBooster[i].booster = false;
				break;
			}
		}
	}
}

bool ModulePlayer::CleanUp()
{
	activeTextures = activeColliders = activeFonts = activeFx = 0;

	App->fonts->UnLoad(uiIndex);
	--totalFonts;
	App->fonts->UnLoad(timeIndex);
	--totalFonts;
	App->textures->Unload(texture);
	--totalTextures;
	App->collisions->RemoveCollider(collider);
	--totalColliders;
	App->textures->Unload(lifesTexture1);
	--totalTextures;
	App->textures->Unload(readyTexture);
	--totalTextures;
	App->textures->Unload(gameOverTexture);
	--totalTextures;
	App->textures->Unload(timeTexture);
	--totalTextures;

	App->harpoon->Disable();
	App->hookShot->Disable();
	App->gunShot->Disable();
	App->doubleShot->Disable();

	return true;
}