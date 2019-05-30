#include "EnemyControllerScript.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "ModuleNavigation.h"

#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentTransform.h"
#include "ComponentBoxTrigger.h"

#include "PlayerMovement.h"

#include "DamageController.h"
#include "EnemyLifeBarController.h"

#include "imgui.h"
#include "JSON.h"

#define MINIMUM_PATH_DISTANCE 400.0f
#define MOVE_REFRESH_TIME 0.3f

EnemyControllerScript_API Script* CreateScript()
{
	EnemyControllerScript* instance = new EnemyControllerScript;
	return instance;
}

void EnemyControllerScript::Start()
{
	// Look for enemy BBox
	GameObject* enemyBBox = App->scene->FindGameObjectByName(gameobject, myBboxName.c_str());
	if (enemyBBox == nullptr)
	{
		LOG("The GO %s couldn't be found \n", myBboxName);
	}
	else
	{
		myBbox = &enemyBBox->bbox;
		if (myBbox == nullptr)
		{
			LOG("The GameObject %s has no bbox attached \n", enemyBBox->name);
		}
	}

	GameObject* myRenderGO = App->scene->FindGameObjectByName(gameobject, myBboxName.c_str());
	if (myRenderGO != nullptr)
		myRender = (ComponentRenderer*)myRenderGO->GetComponent<ComponentRenderer>();

	// Look for player and his BBox
	player = App->scene->FindGameObjectByName(playerName.c_str());
	if (player == nullptr)
	{
		LOG("The GO %s couldn't be found \n", playerName);
	}
	else
	{
		playerHitBox = player->GetComponent<ComponentBoxTrigger>();
		if (playerHitBox == nullptr)
		{
			LOG("The GameObject %s has no bbox attached \n", player->name);
		}

		playerMovement = (PlayerMovement*)player->GetComponentInChildren(ComponentType::Script);
	}

	// Look for Component Animation of the enemy
	anim = (ComponentAnimation*)gameobject->GetComponentInChildren(ComponentType::Animation);
	if (anim == nullptr)
	{
		LOG("No child of the GameObject %s has an Animation component attached \n", gameobject->name);
	}

	GameObject* damageGO = App->scene->FindGameObjectByName("Damage");
	if (damageGO == nullptr)
	{
		LOG("Damage controller GO couldn't be found \n");
	}
	else
	{
		damageController = damageGO->GetComponent<DamageController>();
		if (damageController == nullptr)
		{
			LOG("Damage controller couldn't be found \n");
		}
	}

	GameObject* enemyLifeGO = App->scene->FindGameObjectByName("EnemyLife");
	if (enemyLifeGO == nullptr)
	{
		LOG("Enemy controller GO couldn't be found \n");
	}
	else
	{
		enemyLifeBar = enemyLifeGO->GetComponent<EnemyLifeBarController>();
		if (enemyLifeBar != nullptr)
		{
			LOG("Damage controller couldn't be found \n");
		}
	}

	hpBoxTrigger = (ComponentBoxTrigger*)gameobject->GetComponentInChildren(ComponentType::BoxTrigger);
	if (hpBoxTrigger == nullptr)
	{
		LOG("No child of the GameObject %s has a boxTrigger component attached \n", gameobject->name);
	}

	GameObject* attackGameObject = App->scene->FindGameObjectByName(gameobject, "HitBoxAttack");
	assert(attackGameObject != nullptr);

	attackBoxTrigger = (ComponentBoxTrigger*)attackGameObject->GetComponentInChildren(ComponentType::BoxTrigger);
	if (attackBoxTrigger == nullptr)
	{
		LOG("No child of the GameObject %s has a boxTrigger component attached \n", attackGameObject->name);
	}
	else
	{
		attackBoxTrigger->Enable(false);
	}
}

void EnemyControllerScript::Update()
{
	math::float3 closestPoint;
	if (App->scene->Intersects(closestPoint, myBboxName.c_str()))
	{
		if(enemyLifeBar != nullptr)
			enemyLifeBar->SetLifeBar(maxHealth, actualHealth, EnemyLifeBarType::NORMAL, "Skeleton");

		if (myRender != nullptr)
			myRender->highlighted = true;
	}
	else
	{
		if(myRender != nullptr)
			myRender->highlighted = false;
	}
}

void EnemyControllerScript::Expose(ImGuiContext* context)
{
	char* bboxName = new char[64];
	strcpy_s(bboxName, strlen(myBboxName.c_str()) + 1, myBboxName.c_str());
	ImGui::InputText("My BBox Name", bboxName, 64);
	myBboxName = bboxName;
	delete[] bboxName;

	if (ImGui::InputInt("Health", &maxHealth))
	{
		actualHealth = maxHealth;
	}

	ImGui::Separator();
	ImGui::Text("Player:");
	char* goName = new char[64];
	strcpy_s(goName, strlen(playerName.c_str()) + 1, playerName.c_str());
	ImGui::InputText("playerName", goName, 64);
	playerName = goName;
	delete[] goName;

	char* targetBboxName = new char[64];
	strcpy_s(targetBboxName, strlen(playerBboxName.c_str()) + 1, playerBboxName.c_str());
	ImGui::InputText("Player BBox Name", targetBboxName, 64);
	playerBboxName = targetBboxName;
	delete[] targetBboxName;
}

void EnemyControllerScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddString("playerName", playerName.c_str());
	json->AddString("playerBboxName", playerBboxName.c_str());
	json->AddString("myBboxName", myBboxName.c_str());
	json->AddInt("health", maxHealth);
}

void EnemyControllerScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	playerName = json->GetString("playerName");
	playerBboxName = json->GetString("playerBboxName");
	myBboxName = json->GetString("myBboxName");
	maxHealth = json->GetInt("health", maxHealth);
	actualHealth = maxHealth;
}

void EnemyControllerScript::TakeDamage(unsigned damage)
{
	if (!isDead)
	{
		if (actualHealth - damage < 0 )
		{
			actualHealth = 0;
			gameobject->SetActive(false);
			
		}
		else
		{
			actualHealth -= damage;
		}

		if (actualHealth <= 0)
		{
			isDead = true;
		}
		damageController->AddDamage(gameobject->transform, damage, 2);
	}
}

inline math::float3 EnemyControllerScript::GetPosition() const
{
	assert(gameobject->transform != nullptr);
	return gameobject->transform->GetGlobalPosition();
}

inline math::Quat EnemyControllerScript::GetRotation() const
{
	assert(gameobject->transform != nullptr);
	return gameobject->transform->GetRotation();
}

inline math::float3 EnemyControllerScript::GetPlayerPosition() const
{
	assert(player->transform != nullptr);
	return player->transform->GetGlobalPosition();
}

inline float EnemyControllerScript::GetDistanceTo(math::float3& position) const
{
	math::float3 enemyDistance = GetPosition();
	return enemyDistance.Distance(position);
}

inline float EnemyControllerScript::GetDistanceTo2D(math::float3& position) const
{
	math::float3 enemyDistance = GetPosition();
	enemyDistance.y = position.y;
	return enemyDistance.Distance(position);
}

inline float EnemyControllerScript::GetDistanceToPlayer2D() const
{
	math::float3 enemyPosition = GetPosition();
	math::float3 playerPosition = GetPlayerPosition();
	enemyPosition.y = playerPosition.y;
	return enemyPosition.Distance(playerPosition);
}

inline bool EnemyControllerScript::IsCollidingWithPlayer() const
{
	assert(myBbox != nullptr && playerHitBox != nullptr);
	return myBbox->Intersects(*playerHitBox->GetOBB());
}

void EnemyControllerScript::Move(float speed, math::float3& direction) const
{
	math::float3 movement = direction.Normalized() * speed * App->time->gameDeltaTime;
	gameobject->transform->SetPosition(gameobject->transform->GetPosition() + movement);
}

void EnemyControllerScript::Move(float speed, float& refreshTime, math::float3 position, std::vector<float3>& path) const
{
	if (refreshTime > MOVE_REFRESH_TIME)
	{
		refreshTime = 0.0f;
		App->navigation->FindPath(gameobject->transform->position, position, path);
	}
	if (path.size() > 0)
	{
		math::float3 currentPosition = gameobject->transform->GetPosition();
		while (path.size() > 0 && currentPosition.DistanceSq(path[0]) < MINIMUM_PATH_DISTANCE)
		{
			path.erase(path.begin());
		}
		if (path.size() > 0)
		{
			gameobject->transform->LookAt(path[0]);
			math::float3 direction = (path[0] - currentPosition).Normalized();
			gameobject->transform->SetPosition(currentPosition + speed * direction * App->time->gameDeltaTime);
		}
	}
	refreshTime += App->time->gameDeltaTime;
}

void EnemyControllerScript::MoveTowards(float speed) const
{
	math::float3 movement = gameobject->transform->front.Normalized() * -speed * App->time->gameDeltaTime;
	gameobject->transform->SetPosition(gameobject->transform->GetPosition() + movement);
}

void EnemyControllerScript::LookAt2D(math::float3& position)
{
	math::float3 auxPos = position;
	auxPos.y = GetPosition().y;
	gameobject->transform->LookAt(auxPos);
}

void EnemyControllerScript::OnTriggerEnter(GameObject* go)
{
	if (go == player)
	{
		auto overlaper = attackBoxTrigger->overlapList.find(playerHitBox);
		if (overlaper != attackBoxTrigger->overlapList.end() && overlaper->second == OverlapState::PostIdle)
		{
			playerMovement->Damage(5);
		}
	}

	if (go->name == "HitBoxAttack")
	{
		TakeDamage(10);
	}
}