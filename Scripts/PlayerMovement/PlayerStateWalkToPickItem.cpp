#include "PlayerMovement.h"

#include "Application.h"

#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleNavigation.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentAnimation.h"

#include "PlayerStateWalkToPickItem.h"
#include "PlayerStateIdle.h"

#include "ItemPicker.h"

#include "JSON.h"
#include <assert.h>
#include <string>
#include "imgui.h"
#include "Globals.h"
#include "debugdraw.h"

#define RECALC_PATH_TIME 0.3f

PlayerStateWalkToPickItem::PlayerStateWalkToPickItem(PlayerMovement* PM, const char* trigger):
	PlayerState(PM, trigger)
{
}

PlayerStateWalkToPickItem::~PlayerStateWalkToPickItem()
{
}

void PlayerStateWalkToPickItem::StopAndSwitchToIdle()
{
	//stop going after the item
	player->itemClicked = nullptr;
	//stop particles
	if (dustParticles)
	{
		dustParticles->SetActive(false);
	}
	//and set the player to idle
	player->currentState = player->idle;
	playerWalking = false;
}

void PlayerStateWalkToPickItem::Update()
{
	//check we really got the item in check
	if (player->itemClicked != nullptr)
	{
		itemPosition = player->itemClicked->gameobject->transform->position;
		math::float3 correctionPos(0, player->OutOfMeshCorrectionY, 0);
		if (player->App->navigation->FindPath(player->gameobject->transform->position, itemPosition,
			path, PathFindType::FOLLOW, correctionPos, defaultMaxDist, player->straightPathingDistance))
		{
			//case the player clicks outside of the floor mesh but we want to get close to the floors edge
			pathIndex = 0;
		}

	}
	if (path.size() > 0)
	{
		math::float3 currentPosition = player->gameobject->transform->GetPosition();
		while (pathIndex < path.size() && currentPosition.DistanceSq(path[pathIndex]) < MINIMUM_PATH_DISTANCE)
		{
			pathIndex++;
		}
		if (pathIndex < path.size())
		{
			player->gameobject->transform->LookAt(path[pathIndex]);
			math::float3 direction = (path[pathIndex] - currentPosition).Normalized();
			math::float3 finalWalkingSpeed = player->walkingSpeed * direction * player->App->time->gameDeltaTime;
			finalWalkingSpeed *= (1 + (player->stats.dexterity * 0.005f));
			player->gameobject->transform->SetPosition(currentPosition + finalWalkingSpeed);
			playerWalking = true;
			playerWalkingToHit = true;

			if (dustParticles)
			{
				dustParticles->SetActive(true);
			}
		}
		else if(currentPosition.Distance(itemPosition) < player->basicAttackRange/2)
		{
			//done walking, get the item
			player->itemClicked->pickedUpViaPlayer = true;
			StopAndSwitchToIdle();
		}
		else 
		{
			StopAndSwitchToIdle();
		}
	}
	//no path, stop
	else
	{
		StopAndSwitchToIdle();
	}
}

void PlayerStateWalkToPickItem::Enter()
{
	if (dustParticles)
	{
		dustParticles->SetActive(true);
		player->anim->controller->current->speed *= (1 + (player->stats.dexterity * 0.005f));
	}
}

void PlayerStateWalkToPickItem::CheckInput()
{
	if (player->IsUsingSkill() || (player->IsAttacking()))
	{
		player->itemClicked = nullptr;
		player->currentState = (PlayerState*)player->attack;
	}
	else if (player->IsMovingToAttack())
	{
		player->itemClicked = nullptr;
		player->currentState = (PlayerState*)player->walkToHit;
	}
	else if (player->IsMoving())
	{
		player->itemClicked = nullptr;
		player->currentState = (PlayerState*)player->walk;
	}
	else if (player->IsMovingToItem())
	{
		player->currentState = (PlayerState*)player->walkToPickItem;
	}
	else
	{
		player->itemClicked = nullptr;
		player->currentState = (PlayerState*)player->idle;
		if (dustParticles)
		{
			dustParticles->SetActive(false);
		}
	}
}