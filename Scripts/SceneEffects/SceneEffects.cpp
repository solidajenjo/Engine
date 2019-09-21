#include "SceneEffects.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleTime.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "JSON.h"
#include "imgui.h"

SceneEffects_API Script* CreateScript()
{
	SceneEffects* instance = new SceneEffects;
	return instance;
}

void SceneEffects::Expose(ImGuiContext * context)
{
	if (ImGui::BeginCombo("Behaviours", item_current, 0)) 
	{
		for (int n = 0; n < IM_ARRAYSIZE(behaviours); n++)
		{
			bool is_selected = (item_current == behaviours[n]);
			if (ImGui::Selectable(behaviours[n], is_selected))
			{
				item_current = behaviours[n];
				behaviour = static_cast<Behaviours>((int)Behaviours::NONE + n);
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();   
		}
		ImGui::EndCombo();
	}
	switch (behaviour)
	{
	case Behaviours::BATS:
		ImGui::InputInt("Bat amount ", &batAmount);		
		break;
	case Behaviours::STORM:
		break;
	}
}

void SceneEffects::Start()
{
	switch (behaviour)
	{
	case Behaviours::BATS:
	{
		for (GameObject* wayPoint : gameobject->children)
		{
			waypoints.emplace_back(wayPoint->transform->position);
		}
		for (int i = 0; i < batAmount; ++i)
		{
			Bat newBat;
			newBat.destination = waypoints[rand() % waypoints.size()];
			newBat.gameobject = App->scene->Spawn("BatPrefab", gameobject);
			assert(newBat.gameobject);
			bats.push_back(newBat);
		}

		break;
	}
	case Behaviours::STORM:
		break;
	}
}

void SceneEffects::Update()
{
	switch (behaviour)
	{
	case Behaviours::BATS:
	{
		for (Bat& bat : bats)
		{
			if (bat.gameobject->transform->GetGlobalPosition().Distance(bat.destination) < BAT_STOP_DISTANCE)
				bat.destination = waypoints[rand() % waypoints.size()];			

			math::float3 direction = (bat.destination - bat.gameobject->transform->position).Normalized();
			bat.gameobject->transform->position +=  direction * BAT_SPEED * App->time->gameDeltaTime;
			bat.gameobject->transform->SetRotation(Quat::LookAt(-math::float3::unitZ, direction, math::float3::unitY, math::float3::unitY));
		}
		break;
	}
	case Behaviours::STORM:
		break;
	}
}

void SceneEffects::Serialize(JSON_value* json) const
{
	json->AddInt("behaviour", (int)behaviour);
	json->AddInt("batAmount", batAmount);
}

void SceneEffects::DeSerialize(JSON_value* json)
{
	behaviour = static_cast<Behaviours>(json->GetInt("behaviour", (int)behaviour));
	batAmount = json->GetInt("batAmount", batAmount);
}
