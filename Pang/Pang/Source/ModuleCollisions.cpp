#include "ModuleCollisions.h"

#include "Application.h"

#include "ModuleRender.h"
#include "ModuleInput.h"

#include "ModuleScene.h"
#include "SDL/include/SDL_Scancode.h"

ModuleCollisions::ModuleCollisions(bool startEnabled) : Module(startEnabled)
{
	name = "COLLISIONS";

	for(uint i = 0; i < MAX_COLLIDERS; ++i) //Assign nullptr to all colliders at the constructor.
		colliders[i] = nullptr;

	matrix[Collider::Type::WALL][Collider::Type::WALL] = false;
	matrix[Collider::Type::WALL][Collider::Type::PLAYER] = true;
	matrix[Collider::Type::WALL][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::WALL][Collider::Type::PLAYER_SHOT] = true;
	matrix[Collider::Type::WALL][Collider::Type::BALLOON] = true;


	matrix[Collider::Type::PLAYER][Collider::Type::WALL] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::ENEMY] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::PLAYER_SHOT] = false;
	matrix[Collider::Type::PLAYER][Collider::Type::BALLOON] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::UNBREAKABLE_BLOCK] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::STAIR] = true;
	matrix[Collider::Type::PLAYER][Collider::Type::BOOSTERS] = true;

	matrix[Collider::Type::STAIR][Collider::Type::PLAYER] = true;

	matrix[Collider::Type::BOOSTERS][Collider::Type::PLAYER] = true;

	matrix[Collider::Type::PLAYER_SHOT][Collider::Type::WALL] = true;
	matrix[Collider::Type::PLAYER_SHOT][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::PLAYER_SHOT][Collider::Type::ENEMY] = true;
	matrix[Collider::Type::PLAYER_SHOT][Collider::Type::PLAYER_SHOT] = false;
	matrix[Collider::Type::PLAYER_SHOT][Collider::Type::BALLOON] = true;
	matrix[Collider::Type::PLAYER_SHOT][Collider::Type::BREAKABLE_BLOCK] = true;
	
	matrix[Collider::Type::BALLOON][Collider::Type::WALL] = true;
	matrix[Collider::Type::BALLOON][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::BALLOON][Collider::Type::PLAYER_SHOT] = true;
	matrix[Collider::Type::BALLOON][Collider::Type::BALLOON] = false;
	matrix[Collider::Type::BALLOON][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::BALLOON][Collider::Type::BREAKABLE_BLOCK] = true;

	matrix[Collider::Type::BREAKABLE_BLOCK][Collider::Type::PLAYER] = false;
	matrix[Collider::Type::BREAKABLE_BLOCK][Collider::Type::ENEMY] = false;
	matrix[Collider::Type::BREAKABLE_BLOCK][Collider::Type::PLAYER_SHOT] = true;
	matrix[Collider::Type::BREAKABLE_BLOCK][Collider::Type::BALLOON] = false;

}

// Destructor
ModuleCollisions::~ModuleCollisions()
{

}

update_status ModuleCollisions::PreUpdate()
{
	// Remove all colliders scheduled for deletion
	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if(colliders[i] != nullptr && colliders[i]->pendingToDelete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
			--colliderCount;
		}
	}

	Collider* c1;
	Collider* c2;

	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if(colliders[i] == nullptr)
			continue;

		c1 = colliders[i];

		// avoid checking collisions already checked
		for(uint k = i+1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if(colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if(c1->Intersects(c2->rect))
			{
				if(matrix[c1->type][c2->type] && c1->listener) 
					c1->listener->OnCollision(c1, c2);
				
				if(matrix[c2->type][c1->type] && c2->listener) 
					c2->listener->OnCollision(c2, c1);
			}
		}
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleCollisions::Update()
{
	if (App->input->keys[SDL_SCANCODE_F1] == KEY_DOWN)
		debug = !debug;

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleCollisions::PostUpdate()
{
	if (debug)
		DebugDraw();

	return update_status::UPDATE_CONTINUE;
}

void ModuleCollisions::DebugDraw()
{
	Uint8 alpha = 80;
	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if(colliders[i] == nullptr)
			continue;
		
		switch(colliders[i]->type)
		{
			case Collider::Type::NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
			case Collider::Type::WALL: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
			case Collider::Type::PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
			case Collider::Type::ENEMY: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
			case Collider::Type::PLAYER_SHOT: // yellow
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
			case Collider::Type::BALLOON: // magenta
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 255, alpha);
			break;
			case Collider::Type::BREAKABLE_BLOCK: //red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
			case Collider::Type::UNBREAKABLE_BLOCK: //blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
			case Collider::Type::STAIR: //blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
			case Collider::Type::BOOSTERS:
			App->render->DrawQuad(colliders[i]->rect, 255, 112, 40, alpha);
			break;
		}
	}
}

// Called before quitting
bool ModuleCollisions::CleanUp()
{
	LOG("Freeing all colliders");

	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if(colliders[i] != nullptr)
		{
			delete colliders[i];
			colliders[i] = nullptr;
			--colliderCount;
		}
	}

	return true;
}

Collider* ModuleCollisions::AddCollider(SDL_Rect rect, Collider::Type type, Module* listener)
{
	Collider* ret = nullptr;

	for(uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if(colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, listener);
			++colliderCount;
			break;
		}
	}

	return ret;
}

void ModuleCollisions::RemoveCollider(Collider* collider)
{
	LOG("///////////////////////////////////Remove Collider///////////////////////////////");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == collider)
		{
			delete colliders[i];
			colliders[i] = nullptr;
			--colliderCount;
		}
	}
}