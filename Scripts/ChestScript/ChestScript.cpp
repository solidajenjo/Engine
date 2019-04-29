#include "ChestScript.h"

#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ComponentTransform.h"

#include "imgui.h"
#include "JSON.h"

ChestScript_API Script* CreateScript()
{
	ChestScript* instance = new ChestScript;
	return instance;
}

void ChestScript::Start()
{
	player = App->scene->FindGameObjectByName(playerName.c_str());
	playerBbox = &App->scene->FindGameObjectByName(player, playerBboxName.c_str())->bbox;

	myBbox = &App->scene->FindGameObjectByName(gameObject, myBboxName.c_str())->bbox;
}

void ChestScript::Update()
{
	// Check collision with player
	if (myBbox != nullptr && myBbox->Intersects(*playerBbox))
	{
		// Open chest
		gameObject->SetActive(true);
	}
}

void ChestScript::Expose(ImGuiContext* context)
{
	char* bboxName = new char[64];
	strcpy_s(bboxName, strlen(myBboxName.c_str()) + 1, myBboxName.c_str());
	ImGui::InputText("My BBox Name", bboxName, 64);
	myBboxName = bboxName;
	delete[] bboxName;

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

void ChestScript::Serialize(JSON_value* json) const
{
	assert(json != nullptr);
	json->AddString("playerName", playerName.c_str());
	json->AddString("playerBboxName", playerBboxName.c_str());
	json->AddString("myBboxName", myBboxName.c_str());
}

void ChestScript::DeSerialize(JSON_value* json)
{
	assert(json != nullptr);
	playerName = json->GetString("playerName");
	playerBboxName = json->GetString("playerBboxName");
	myBboxName = json->GetString("myBboxName");
}