#include "ItemPicker.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"

#include "InventoryScript.h"

#include "JSON.h"
#include "imgui.h"
#include "Resource.h"
#include "ComponentAudioSource.h"

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

	GameObject* GOtemp = App->scene->FindGameObjectByName("itemPickedAudio");
	if (GOtemp != nullptr)
	{
		itemPickedAudio = GOtemp->GetComponent<ComponentAudioSource>();
		assert(itemPickedAudio != nullptr);
	}
	else
	{
		LOG("The Game Object 'itemPickedAudio' couldn't be found.");
	}
}

void ItemPicker::Update()
{
	if (gameobject->isActive() && gameobject->bbox.Intersects(*playerBbox))
	{
		if (inventoryScript->AddItem(item))
		{
			gameobject->SetActive(false);
			if (itemPickedAudio != nullptr) itemPickedAudio->Play();
		}
	}
}

void ItemPicker::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddString("name", name.c_str());
	json->AddString("sprite", sprite.c_str());
	json->AddInt("type", (int)type);
}

void ItemPicker::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	name = json->GetString("name");
	sprite = json->GetString("sprite");
	type = (ItemType)json->GetInt("type");
}