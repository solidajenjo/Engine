#include "EnemyLoot.h"
#include "ItemPicker.h"
#include "Application.h"
#include "Resource.h"
#include "ModuleScene.h"
#include "ModuleScript.h"
#include "ModuleResourceManager.h"


#pragma warning(disable : 4996)

EnemyLoot_API Script* CreateScript()
{
	EnemyLoot* instance = new EnemyLoot;
	return instance;
}

void EnemyLoot::Start()
{
}

void EnemyLoot::Update()
{
}

void EnemyLoot::GenerateLoot()
{
	for (int i = 0; i < items.size(); ++i)
	{
	}
}

void EnemyLoot::AddItem(ItemType type, std::string name, std::string sprite, int drop)
{
	Item item;
	item.name = name;
	item.sprite = sprite;
	item.type = type;
	items.emplace_back(std::make_pair(item, drop));
	go = App->scene->CreateGameObject(name.c_str(),App->scene->selected);
	//itemPicker->SetItem(type, name, sprite);
	script = App->scripting->GetScript("ItemPicker");
	script->SetGameObject(App->scene->selected);
}

void EnemyLoot::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::Separator();
	ImGui::Text("List of items to drop:");
	for (int i = 0; i < items.size(); ++i)
	{
		ImGui::Text("Item: ");
		ImGui::SameLine();
		ImGui::Text((items[i].first.name).c_str());
		ImGui::SameLine();
		ImGui::Separator();
		ImGui::SameLine();
		ImGui::Text("Drop:");
		ImGui::SameLine();
		ImGui::Text(std::to_string(items[i].second).c_str());
		
	}
	if (items.size() == 0)
	{
		ImGui::Text("No items!");
	}

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
	//name = imguiText;

	ImGui::DragInt("Drop %", &drop, 0.01f, 0, 100);

	if(ImGui::Button("Add item"))
	{
		AddItem(type, imguiText, sprite, drop);
	}

	
	/*ImGui::Checkbox("Draw Debug", &drawDebug);
	ImGui::Text("Patrol:");
	ImGui::InputFloat("Distance to activate", &activationDistance);
	ImGui::Text("Chase:");
	ImGui::InputFloat("Chase Speed", &chaseSpeed);
	ImGui::Text("Return:");
	ImGui::InputFloat("Return Distance", &returnDistance);
	ImGui::InputFloat("Return Speed", &returnSpeed);
	ImGui::Text("Attack:");
	ImGui::InputFloat("Attack Time", &attackDuration);
	ImGui::InputFloat("Attack Damage", &attackDamage);
	ImGui::Text("Cooldown:");
	ImGui::InputFloat("Cooldown Time", &cooldownTime);*/
}
