#include "ItemPicker.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"

#include "InventoryScript.h"
#include "PlayerMovement.h"

#include "JSON.h"
#include "imgui.h"
#include "Resource.h"

#pragma warning(disable : 4996)

ItemPicker_API Script* CreateScript()
{
	ItemPicker* instance = new ItemPicker;
	return instance;
}

void ItemPicker::Expose(ImGuiContext* context)
{

	ImGui::SetCurrentContext(context);
	const char * types[] = { "NONE","QUICK","KEY","MATERIAL","WEAPON","HELMET","CHEST","PANTS","BOOTS","AMULET","RING" };

	if (ImGui::BeginCombo("Type", types[(int)type]))
	{
		for (int n = 0; n < 11; n++)
		{
			bool isSelected = ((int)type == n);
			if (ImGui::Selectable(types[n], isSelected) && (int)type != n)
			{
				type = (ItemType)n;
			}
			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	if (textureFiles.empty())
	{
		textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);
	}

	//texture selector
	if (ImGui::BeginCombo("Texture", sprite.c_str()))
	{
		bool none_selected = (sprite == "None");
		if (ImGui::Selectable("None", none_selected))
		{
			sprite = "None";
		}
		if (none_selected)
			ImGui::SetItemDefaultFocus();
		for (int n = 0; n < textureFiles.size(); n++)
		{
			bool is_selected = (sprite == textureFiles[n]);
			if (ImGui::Selectable(textureFiles[n].c_str(), is_selected) && !is_selected)
			{
				sprite = textureFiles[n].c_str();
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	
	char* imguiText = new char[64];
	strcpy(imguiText, name.c_str());
	ImGui::InputText("##", imguiText, 64);
	name = imguiText;
	delete[] imguiText;
	

	item.stats.Expose("Item Stats");
}

void ItemPicker::Start()
{
	inventoryScript = App->scene->FindGameObjectByName("Inventory")->GetComponent<InventoryScript>();
	GameObject* playerGO = App->scene->FindGameObjectByName("Player");
	playerBbox = &App->scene->FindGameObjectByName("PlayerMesh", playerGO)->bbox;

	item.name = this->name;
	item.description = this->description;
	item.sprite = this->sprite;
	item.type = this->type;
}

void ItemPicker::Update()
{
	if (gameobject->isActive() && gameobject->bbox.Intersects(*playerBbox))
	{
		if (inventoryScript->AddItem(item))
			gameobject->SetActive(false);
	}
}

void ItemPicker::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddString("name", name.c_str());
	json->AddString("sprite", sprite.c_str());
	json->AddInt("type", (int)type);
	json->AddInt("isEquipped", (int)item.isEquipped);
	json->AddFloat("health", item.stats.health);
	json->AddFloat("mana", item.stats.mana);
	json->AddInt("strength", item.stats.strength);
	json->AddInt("dexterity", item.stats.dexterity);
	json->AddFloat("hpRegen", item.stats.hpRegen);
	json->AddFloat("manaRegen", item.stats.manaRegen);
}

void ItemPicker::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	name = json->GetString("name");
	sprite = json->GetString("sprite");
	type = (ItemType)json->GetInt("type");
	item.isEquipped = json->GetInt("isEquipped");
	item.stats.health = json->GetFloat("health");
	item.stats.mana = json->GetFloat("mana");
	item.stats.strength = json->GetInt("strength");
	item.stats.dexterity = json->GetInt("dexterity");
	item.stats.hpRegen = json->GetFloat("hpRegen");
	item.stats.manaRegen = json->GetFloat("manaRegen");
}