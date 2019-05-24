#include "Application.h"

#include "ModuleTime.h"
#include "ModuleParticles.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "ModuleResourceManager.h"
#include "ModuleScene.h"

#include "ComponentTrail.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "GameObject.h"
#include "ResourceTexture.h"

#include "HashString.h"
#include "imgui.h"
#include "debugdraw.h"
#include "JSON.h"


#define None "None Selected"

ComponentTrail::ComponentTrail(GameObject* gameobject) : Component(gameobject, ComponentType::Trail)
{
	if (!gameobject->transform)
	{
		gameobject->CreateComponent(ComponentType::Transform);
	}
	modules.push_back(new PMSizeOverTime());
	App->particles->AddTrailRenderer(this);
}

ComponentTrail::ComponentTrail(const ComponentTrail& component) : Component(component)
{
	if (!gameobject->transform)
	{
		gameobject->CreateComponent(ComponentType::Transform);
	}
	width = component.width;
	duration = component.duration;
	minDistance = component.minDistance;

	textureName = component.textureName;
	if (textureName != "None Selected")
	{
		texture = (ResourceTexture*)App->resManager->GetByName(textureName.c_str(), TYPE::TEXTURE);
	}

	App->particles->AddTrailRenderer(this);
}

void ComponentTrail::Update()
{
	unsigned trailPoints = trail.size();
	for (unsigned i = 0u; i < trailPoints; ++i)
	{
		TrailPoint point = trail.front();
		point.remainingTime -= App->time->gameDeltaTime;

		if (point.remainingTime > 0)
		{
			trail.push(point);
		}

		trail.pop();
	}

	math::float3 pos = gameobject->transform->GetGlobalPosition();
	if (trail.size() == 0 || trail.back().Distance(pos) > minDistance)
	{
		if (trail.size() == 0)
		{
			TrailPoint newPoint(duration, pos, width);
			trail.push(newPoint);
		}
		else			
		{			
			if (App->scene->maincamera)
			{
				TrailPoint newPoint(duration, gameobject->transform->GetGlobalPosition(), trail.back().position, width, (App->scene->maincamera->frustum->pos - gameobject->transform->GetGlobalPosition()).Normalized());
				trail.push(newPoint);
			}
			else
			{
				TrailPoint newPoint(duration, gameobject->transform->GetGlobalPosition(), trail.back().position, width, gameobject->transform->right);
				trail.push(newPoint);
			}
		}
	}
}

void ComponentTrail::DrawProperties()
{
	ImGui::PushID(this);
	if (ImGui::CollapsingHeader("Trail Renderer", ImGuiTreeNodeFlags_DefaultOpen))
	{
		//texture selector
		if (ImGui::BeginCombo("Texture", texture != nullptr ? texture->GetName() : None))
		{
			bool none_selected = (texture == nullptr);
			if (ImGui::Selectable(None, none_selected))
			{
				if (texture != nullptr)
				{
					App->resManager->DeleteResource(texture->GetUID());
					texture = nullptr;
				}
			}
			if (none_selected)
				ImGui::SetItemDefaultFocus();

			textureFiles = App->resManager->GetResourceNamesList(TYPE::TEXTURE, true);

			for (int n = 0; n < textureFiles.size(); n++)
			{
				bool is_selected = (texture != nullptr && (HashString(texture->GetName()) == HashString(textureFiles[n].c_str())));
				if (ImGui::Selectable(textureFiles[n].c_str(), is_selected) && !is_selected)
				{
					// Delete previous texture
					if (texture != nullptr)
						App->resManager->DeleteResource(texture->GetUID());

					texture = (ResourceTexture*)App->resManager->GetByName(textureFiles[n].c_str(), TYPE::TEXTURE);
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (texture != nullptr)
		{
			ImGui::Image((ImTextureID)texture->gpuID, { 200,200 }, { 0,1 }, { 1,0 });
		}

		ImGui::InputFloat("Width", &width, .01f, .1f);
		ImGui::InputFloat("Duration", &duration, .01f, .1f);
		ImGui::InputFloat("Min. point distance", &minDistance, .01f, .1f);
		ImGui::ColorEdit4("Color", trailColor.ptr());
		for (ParticleModule* pm : modules)
		{
			pm->InspectorDraw();
		}
	}
	ImGui::PopID();
}

void ComponentTrail::Save(JSON_value* value) const
{
	Component::Save(value);
	value->AddFloat("width", width);
	value->AddFloat("duration", duration);
	value->AddFloat("minDistance", minDistance);
	value->AddFloat4("trailColor", trailColor);
	value->AddUint("textureUID", (texture != nullptr) ? texture->GetUID() : 0u);
	
	value->AddInt("sizeOT", modules[0]->enabled);
	
}

void ComponentTrail::Load(JSON_value* value)
{
	Component::Load(value);
	width = value->GetFloat("width");
	duration = value->GetFloat("duration");
	minDistance = value->GetFloat("minDistance");
	trailColor = value->GetFloat4("trailColor");
	unsigned uid = value->GetUint("textureUID");
	texture = (ResourceTexture*)App->resManager->Get(uid);

	modules[0]->enabled = value->GetInt("sizeOT");

}

ComponentTrail * ComponentTrail::Clone() const
{
	return new ComponentTrail(*this);
}

ComponentTrail::~ComponentTrail()
{
	App->particles->RemoveTrailRenderer(this);
	if (texture != nullptr)
	{
		App->resManager->DeleteResource(texture->GetUID());
		texture = nullptr;
	}
}
