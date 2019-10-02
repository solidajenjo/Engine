#include "LootDropScript.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ResourcePrefab.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "Item.h"

#include "imgui.h"
#include "JSON.h"

LootDropScript_API Script* CreateScript()
{
	LootDropScript* instance = new LootDropScript;
	return instance;
}

void LootDropScript::Start()
{
	itemListGO = App->scene->FindGameObjectByName("ItemsList");
	if (itemListGO == nullptr)
		LOG("Warning: ItemList GO couldn't be found, items will spawn with root as a parent.");
}

void LootDropScript::Expose(ImGuiContext* context)
{
	for (int i = 0; i < itemList.size(); ++i)
	{
		ImGui::PushID(i);
		ImGui::Text("Item %i: ", i + 1);
		char* itemName = new char[64];
		strcpy_s(itemName, strlen(itemList[i].c_str()) + 1, itemList[i].c_str());
		ImGui::InputText("Prefab Name", itemName, 64);
		itemList[i] = itemName;
		delete[] itemName;
		ImGui::PopID();
	}

	if (ImGui::Button("Add item"))
	{
		itemList.push_back("");
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete item"))
	{
		itemList.pop_back();
	}
}

void LootDropScript::Serialize(JSON_value* json) const
{
	json->AddInt("NumberOfItems", itemList.size());

	std::string name;
	for (int i = 0; i < itemList.size(); ++i)
	{
		name = "ItemPrefabName" + std::to_string(i);
		json->AddString(name.c_str(), itemList[i].c_str());
	}
}

void LootDropScript::DeSerialize(JSON_value* json)
{
	int numItems = json->GetInt("NumberOfItems");

	std::string name;
	for (int i = 0; i < numItems; ++i)
	{
		name = "ItemPrefabName" + std::to_string(i);
		itemList.push_back(json->GetString(name.c_str(), ""));
	}
}

void LootDropScript::DropItems()
{
	for (int i = 0; i < itemList.size(); ++i)
	{
		if (itemListGO != nullptr)
		{
			math::float3 position = gameobject->transform->position + positionOffset;
			GameObject* spawnedItem = App->scene->Spawn(itemList[i].c_str(), position, itemListGO->transform->rotation, itemListGO);
			spawnedItem->transform->Rotate(rotationOffset);
		}
		else
			App->scene->Spawn(itemList[i].c_str());
	}
}

void LootDropScript::DropItemsInCircle(float radius)
{
	// 2 * pi is full circle, divide it between items to drop
	float angleStep = (2 * 3.14f) / itemList.size();
	float angle = 0.0f;
	for (int i = 0; i < itemList.size(); ++i)
	{
		if (itemListGO != nullptr)
		{
			// Apply position offset
			math::float3 position = gameobject->transform->position + positionOffset;

			// Position in circle = PosCenter + radius*cos(angle)
			position = math::float3(position.x + radius * cos(angle), position.y, position.z + radius * sin(angle));
			GameObject* spawnedItem = App->scene->Spawn(itemList[i].c_str(), position, itemListGO->transform->rotation, itemListGO);

			// Apply rotation offset
			spawnedItem->transform->Rotate(rotationOffset);
			angle += angleStep;
		}
		else
			App->scene->Spawn(itemList[i].c_str());
	}
}

void LootDropScript::DropItemsInSemiCircle(float radius)
{
	// pi is half of a circle, divide it between items to drop
	float angleStep = 3.14f / (itemList.size() - 1);
	float angle = 0.0f;
	for (int i = 0; i < itemList.size(); ++i)
	{
		if (itemListGO != nullptr)
		{
			// Apply position offset
			math::float3 position = gameobject->transform->position + positionOffset;

			// Position in circle = PosCenter + radius*cos(angle)
			position = math::float3(position.x + radius * cos(angle), position.y, position.z + radius * sin(angle));
			GameObject* spawnedItem = App->scene->Spawn(itemList[i].c_str(), position, itemListGO->transform->rotation, itemListGO);

			// Apply rotation offset
			spawnedItem->transform->Rotate(rotationOffset);
			angle += angleStep;
		}
		else
			App->scene->Spawn(itemList[i].c_str());
	}
}
