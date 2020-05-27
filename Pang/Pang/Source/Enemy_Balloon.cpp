#include "Enemy_Balloon.h"
#include "Application.h"
#include "ModuleCollisions.h"
#include "Particle.h"
#include "ModuleAudio.h"
#include "ModuleParticles.h"
#include "ModuleScene.h"
#include "ModuleScene2.h"
#include "ModuleScene3.h"
#include "ModuleScene4.h"
#include "ModuleScene5.h"
#include "ModuleScene6.h"
#include "ModuleEnemies.h"
#include "ModulePlayer.h"
#include "ModuleHarpoon_Shot.h"
#include "ModuleHarpoon_HookShot.h"

#include "ModuleTileset.h"
#include "Application.h"

#include "SDL/include/SDL.h"


Enemy_Balloon::Enemy_Balloon(int x, int y, enum class ENEMY_TYPE type) : Enemy(x, y)
{

	ballonExplosion = App->audio->LoadFx("Assets/Sound/FX/DestroyBalls.wav");

	tipoBalloon = type;
	if (App->player->scene1 || App->player->scene4) {
		veryBigBalloonAnim.PushBack({ 207,112, 48, 40 });
		bigBalloonAnim.PushBack({ 257,119, 32, 26 });
		smallBalloonAnim.PushBack({ 292,125,16,14 });
		verySmallBalloonAnim.PushBack({ 312,129,8,7 });
	}
	if (App->player->scene2 || App->player->scene5) {
		veryBigBalloonAnim.PushBack({ 206, 9, 48, 40 });
		bigBalloonAnim.PushBack({ 257, 16, 32, 26 });
		smallBalloonAnim.PushBack({ 291,22,16,14 });
		verySmallBalloonAnim.PushBack({ 311,26,8,7 });
	}
	if (App->player->scene3 || App->player->scene6)
	{
		veryBigBalloonAnim.PushBack({ 206, 57, 48, 40 });
		bigBalloonAnim.PushBack({ 257, 64, 32, 26 });
		smallBalloonAnim.PushBack({ 291,70,16,14 });
		verySmallBalloonAnim.PushBack({ 311,74,8,7 });
	}
	particleDeathVeryBig.anim.PushBack({ 4,142,48,39 });
	particleDeathVeryBig.anim.PushBack({ 60,148,28,25 });
	particleDeathVeryBig.anim.PushBack({ 96,143,41,37 });
	particleDeathVeryBig.anim.PushBack({ 145,137,48,46 });
	particleDeathVeryBig.anim.speed = 0.25f;
	particleDeathVeryBig.anim.loop = false;

	particleDeathBig.anim.PushBack({ 4,85,32,26 });
	particleDeathBig.anim.PushBack({ 44,88,20,15 });
	particleDeathBig.anim.PushBack({ 72,85,27,26 });
	particleDeathBig.anim.PushBack({ 107,83,32,30 });
	particleDeathBig.anim.speed = 0.25f;
	particleDeathBig.anim.loop = false;

	particleDeathSmall.anim.PushBack({ 4,44,16,14 });
	particleDeathSmall.anim.PushBack({ 28,47,9,8 });
	particleDeathSmall.anim.PushBack({ 45,43,15,15 });
	particleDeathSmall.anim.PushBack({ 68,43,16,16 });
	particleDeathSmall.anim.speed = 0.25f;
	particleDeathSmall.anim.loop = false;

	particleDeathVerySmall.anim.PushBack({ 4,12,5,5 });
	particleDeathVerySmall.anim.PushBack({ 17,12,7,6 });
	particleDeathVerySmall.anim.PushBack({ 32,11,10,8 });
	particleDeathVerySmall.anim.speed = 0.25f;
	particleDeathVerySmall.anim.loop = false;

	currentAnim = &veryBigBalloonAnim;


	if (tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON)
	{
		collider = App->collisions->AddCollider({ position.x, position.y, 48, 40 }, Collider::Type::BALLOON, (Module*)App->enemies);
		currentAnim = &veryBigBalloonAnim;
	}
	if (tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON2)
	{
		collider = App->collisions->AddCollider({ position.x, position.y, 48, 40 }, Collider::Type::BALLOON, (Module*)App->enemies);
		currentAnim = &veryBigBalloonAnim;
	}
	else if (tipoBalloon == ENEMY_TYPE::BIGBALLOON)
	{
		collider = App->collisions->AddCollider({ position.x, position.y, 32, 26 }, Collider::Type::BALLOON, (Module*)App->enemies);
		currentAnim = &bigBalloonAnim;
	}
	else if (tipoBalloon == ENEMY_TYPE::BIGBALLOON2)
	{
		collider = App->collisions->AddCollider({ position.x, position.y, 32, 26 }, Collider::Type::BALLOON, (Module*)App->enemies);
		currentAnim = &bigBalloonAnim;
	}
	else if (tipoBalloon == ENEMY_TYPE::SMALLBALLOON)
	{
		collider = App->collisions->AddCollider({ position.x, position.y, 16, 14 }, Collider::Type::BALLOON, (Module*)App->enemies);
		currentAnim = &smallBalloonAnim;
	}
	else if (tipoBalloon == ENEMY_TYPE::SMALLBALLOON2)
	{
		collider = App->collisions->AddCollider({ position.x, position.y, 16, 14 }, Collider::Type::BALLOON, (Module*)App->enemies);
		currentAnim = &smallBalloonAnim;
	}
	else if (tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON)
	{
		collider = App->collisions->AddCollider({ position.x, position.y, 8, 7 }, Collider::Type::BALLOON, (Module*)App->enemies);
		currentAnim = &verySmallBalloonAnim;
	}
	else if (tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON2)
	{
		collider = App->collisions->AddCollider({ position.x, position.y, 8, 7 }, Collider::Type::BALLOON, (Module*)App->enemies);
		currentAnim = &verySmallBalloonAnim;
	}
}

void Enemy_Balloon::Update()
{
	if (App->player->ready == 0) {
		MRUA();
		balloonBounce();

		if (App->player->destroyed)
		{
			collider->pendingToDelete = true;
		}

		Enemy::Update();
	}
}


void Enemy_Balloon::MRUA()
{
	if (!App->player->destroyed)
	{
		if (tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON2 || tipoBalloon == ENEMY_TYPE::BIGBALLOON2 || tipoBalloon == ENEMY_TYPE::SMALLBALLOON2 || tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON2) {
			position.x -= speedX;
			position.y -= (speedY + gravity);
			speedY -= gravity;
		}
		else {
			position.x += speedX;
			position.y -= (speedY + gravity);
			speedY -= gravity;
		}
	}
}

void Enemy_Balloon::balloonBounce()
{
	//PHYSICS MOTIONS


	iPoint tile = { position.x / TILE_SIZE, position.y / TILE_SIZE };
	if (tile.x < 0) { tile.x = 0; }
	if (tile.y < 0) { tile.y = 0; }
	//LOG("%d     %d", tile.x, tile.y);

	//FLOOR COLLISION
	if (this->tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON || this->tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON2)
	{
		for (int i = 1; i < 6; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + 5, tile.x + i).id == ModuleTileset::TileType::WALL)
			{
				//position.y -= 5;
				speedY = 4.5f;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::BIGBALLOON || this->tipoBalloon == ENEMY_TYPE::BIGBALLOON2)
	{
		for (int i = 1; i < 4; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + 3.5f, tile.x + i).id == ModuleTileset::TileType::WALL)
			{
				speedY = 4.5f;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::SMALLBALLOON || this->tipoBalloon == ENEMY_TYPE::SMALLBALLOON2)
	{
		for (int i = 1; i < 2; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + 2, tile.x + i).id == ModuleTileset::TileType::WALL)
			{
				speedY = 4.5f;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON || this->tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON2)
	{
		if (App->tileset->getTileLevel(tile.y + 1, tile.x).id == ModuleTileset::TileType::WALL)
		{
			speedY = 4.5f;
		}
	}


	//RIGHT WALL COLLISION

	if (this->tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON || this->tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON2)
	{
		for (int i = 0; i < 5; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + i, tile.x + 6).id == ModuleTileset::TileType::WALL)
			{
				speedX = -speedX;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::BIGBALLOON || this->tipoBalloon == ENEMY_TYPE::BIGBALLOON2)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + i, tile.x + 4).id == ModuleTileset::TileType::WALL)
			{
				speedX = -speedX;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::SMALLBALLOON || this->tipoBalloon == ENEMY_TYPE::SMALLBALLOON2)
	{
		for (int i = 0; i < 2; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + i, tile.x + 2.5f).id == ModuleTileset::TileType::WALL)
			{
				speedX = -speedX;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON || this->tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON2)
	{
		if (App->tileset->getTileLevel(tile.y, tile.x).id == ModuleTileset::TileType::WALL)
		{
			speedX = -speedX;
		}
	}



	//LEFT WALL COLLISION

	if (this->tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON || this->tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON2)
	{
		for (int i = 0; i < 5; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + i, tile.x).id == ModuleTileset::TileType::WALL)
			{
				speedX = -speedX;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::BIGBALLOON || this->tipoBalloon == ENEMY_TYPE::BIGBALLOON2)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + i, tile.x).id == ModuleTileset::TileType::WALL)
			{
				speedX = -speedX;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::SMALLBALLOON || this->tipoBalloon == ENEMY_TYPE::SMALLBALLOON2)
	{
		for (int i = 0; i < 2; ++i)
		{
			if (App->tileset->getTileLevel(tile.y + i, tile.x).id == ModuleTileset::TileType::WALL)
			{
				speedX = -speedX;
				break;
			}
		}
	}
	else if (this->tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON || this->tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON2)
	{
		if (App->tileset->getTileLevel(tile.y, tile.x + 1).id == ModuleTileset::TileType::WALL)
		{
			speedX = -speedX;
		}
	}
}


void Enemy_Balloon::OnCollision(Collider* c2) {

	SDL_Rect r = collider->rect;

	if (c2->type == Collider::Type::PLAYER_SHOT) {
		if (c2->Intersects(r) == true) {

			collider->pendingToDelete = true;
			App->enemies->balloon.balloonsOnScene--;
			App->harpoon->totalColliders--;


			if (tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON || tipoBalloon == ENEMY_TYPE::VERYBIGBALLOON2)
			{
				App->player->lastBalloon = ENEMY_TYPE::VERYBIGBALLOON;
				App->particles->AddParticle(particleDeathVeryBig, position.x, position.y, Collider::Type::NONE, 0);
				App->audio->PlayFx(ballonExplosion);
				App->enemies->AddEnemy(ENEMY_TYPE::BIGBALLOON, position.x, position.y);
				App->enemies->AddEnemy(ENEMY_TYPE::BIGBALLOON2, position.x, position.y);
				App->player->score += 100;
				App->player->cont++;
			}
			else if (tipoBalloon == ENEMY_TYPE::BIGBALLOON || tipoBalloon == ENEMY_TYPE::BIGBALLOON2)
			{
				if (App->player->lastBalloon == ENEMY_TYPE::BIGBALLOON && tipoBalloon == ENEMY_TYPE::BIGBALLOON2 || App->player->lastBalloon == tipoBalloon) {
					App->player->cont++;
					if (App->player->count <= 3) {
						App->player->score += 200 * App->player->cont;
					}
					else {
						App->player->score += 200 * 3;
					}
				}
				else {
					App->player->score += 200;
					App->player->cont = 1;
				}
				App->player->lastBalloon = ENEMY_TYPE::BIGBALLOON;

				App->particles->AddParticle(particleDeathBig, position.x, position.y, Collider::Type::NONE, 0);
				App->audio->PlayFx(ballonExplosion);
				App->enemies->AddEnemy(ENEMY_TYPE::SMALLBALLOON, position.x, position.y);
				App->enemies->AddEnemy(ENEMY_TYPE::SMALLBALLOON2, position.x, position.y);

			}
			else if (tipoBalloon == ENEMY_TYPE::SMALLBALLOON || tipoBalloon == ENEMY_TYPE::SMALLBALLOON2)
			{
				if (App->player->lastBalloon == ENEMY_TYPE::SMALLBALLOON && tipoBalloon == ENEMY_TYPE::SMALLBALLOON2 || App->player->lastBalloon == tipoBalloon) {
					App->player->cont++;
					if (App->player->count <= 3) {
						App->player->score += 300 * App->player->cont;
					}
					else {
						App->player->score += 300 * 3;
					}
				}
				else {
					App->player->score += 300;
					App->player->cont = 1;
				}
				App->player->lastBalloon = ENEMY_TYPE::SMALLBALLOON;

				App->particles->AddParticle(particleDeathSmall, position.x, position.y, Collider::Type::NONE, 0);
				App->audio->PlayFx(ballonExplosion);
				App->enemies->AddEnemy(ENEMY_TYPE::VERYSMALLBALLOON, position.x, position.y);
				App->enemies->AddEnemy(ENEMY_TYPE::VERYSMALLBALLOON2, position.x, position.y);

			}
			else if (tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON || tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON2)
			{
				if (App->player->lastBalloon == ENEMY_TYPE::VERYSMALLBALLOON && tipoBalloon == ENEMY_TYPE::VERYSMALLBALLOON2 || App->player->lastBalloon == tipoBalloon) {
					App->player->cont++;
					if (App->player->count <= 3) {
						App->player->score += 400 * App->player->cont;
					}
					else {
						App->player->score += 400 * 3;
					}
				}
				else {
					App->player->score += 400;
					App->player->cont = 1;
				}
				App->player->lastBalloon = ENEMY_TYPE::VERYSMALLBALLOON;

				App->particles->AddParticle(particleDeathVerySmall, position.x, position.y, Collider::Type::NONE, 0);
				App->audio->PlayFx(ballonExplosion);

			}
		}

	}

}