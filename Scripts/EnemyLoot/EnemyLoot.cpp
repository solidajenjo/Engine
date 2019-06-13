#include "EnemyLoot.h"
#include "EnemyControllerScript.h"
#include "ItemPicker.h"
#include "Application.h"
#include "Resource.h"
#include "ModuleScene.h"
#include "ModuleScript.h"
#include "ModuleResourceManager.h"

#include "GameObject.h"


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
		go = items[i].first;
		//go->parent = App->scene->root;
		go->SetActive(true);
	}
	//items.clear();
}

void EnemyLoot::AddItem(std::string name, int drop)
{
	go = App->scene->FindGameObjectByName(name.c_str());
	items.push_back(std::make_pair(go, drop));
	go->parent = App->scene->selected;
	if (go->isActive())
	{
		go->SetActive(false);
	}
	go->transform = App->scene->selected->transform;
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
		ImGui::Text(items[i].first->name.c_str());
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

	char* imguiText = new char[64];
	strcpy(imguiText, goName.c_str());
	ImGui::InputText("##", imguiText, 64);
	goName = imguiText;

	ImGui::DragInt("Drop %", &drop, 0.01f, 0, 100);

	if(ImGui::Button("Add item"))
	{
		AddItem(imguiText,drop);
		goName = "GO Name";
		drop = 0;
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
