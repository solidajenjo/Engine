#include "PlayerMovement.h"

#include "Application.h"
#include "ModuleInput.h"

#include "ComponentTransform.h"
#include "GameObject.h"

//#include "JSON.h"
#include <assert.h>
#include "imgui.h"
#include <iostream>
#include "Globals.h"

PlayerMovement_API PlayerMovement* CreateScript()
{
	PlayerMovement* instance = new PlayerMovement;
	return instance;
}

void PlayerMovement::Expose(ImGuiContext* context)
{
	ImGui::SetCurrentContext(context);
	ImGui::InputFloat("speed", &speed);
}

void PlayerMovement::Update()
{
	if(App->input->IsKeyPressed(SDL_SCANCODE_W))
	{
		gameObject->transform->position.z -= speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_S))
	{
		gameObject->transform->position.z += speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_A))
	{
		gameObject->transform->position.x -= speed;
	}
	if (App->input->IsKeyPressed(SDL_SCANCODE_D))
	{
		gameObject->transform->position.x += speed;
	}
	App->SetTimer();
	//LOG("PLAYER POSITION: (%d,%d)", gameObject->transform->position.x, gameObject->transform->position.z);
}

//void PlayerMovement::Serialize(JSON * json) const //TODO: Create template base method
//{
//	assert(json != nullptr); //JSON is null
//	JSON_value* variables = json->CreateValue();
//	variables->AddFloat("speed", speed);
//	json->AddValue("variables", *variables);
//}
//
//void PlayerMovement::DeSerialize(JSON * json)
//{
//	JSON_value* variables = json->GetValue("variables");
//	speed = variables->GetFloat("speed");
//}
