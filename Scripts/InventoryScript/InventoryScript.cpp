#include "InventoryScript.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleUI.h"

#include "ComponentImage.h"
#include "ComponentTransform2D.h"

#include "GameObject.h"
#include "Viewport.h"


InventoryScript_API Script* CreateScript()
{
	InventoryScript* instance = new InventoryScript;
	return instance;
}

void InventoryScript::Start()
{
	inventory = App->scene->FindGameObjectByName("Inventory");
	GameObject* slots = App->scene->FindGameObjectByName(inventory, "InventorySlots");
	assert(slots != nullptr);
	slotsTransform = slots->GetComponentsInChildren(ComponentType::Transform2D);

	slotsTransform.emplace_back(App->scene->FindGameObjectByName(inventory, "HelmetSlot")->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName(inventory, "BodySlot")->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName(inventory, "PantsSlot")->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName(inventory, "BootsSlot")->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName(inventory, "WeaponSlot")->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName(inventory, "AmuletSlot")->GetComponent<Transform2D>());
	slotsTransform.emplace_back(App->scene->FindGameObjectByName(inventory, "RingSlot")->GetComponent<Transform2D>());

	std::list<GameObject*> l = App->scene->FindGameObjectByName(inventory, "Items")->children;

	items = { std::begin(l), std::end(l) };
	//items = App->scene->FindGameObjectByName(inventory, "Items")->children;
	
}

void InventoryScript::Update()
{
	if (!inventory->isActive()) return;

	math::float2 mouse = reinterpret_cast<const float2&>(App->input->GetMousePosition());
	float screenX = mouse.x - App->renderer->viewGame->winPos.x - (App->ui->currentWidth * .5f);
	float screenY = mouse.y - App->renderer->viewGame->winPos.y - (App->ui->currentHeight * .5f);

	for (int i = 0; i < items.size(); ++i)
	{
		if (!items[i]->isActive()) continue;

		Transform2D* rectTransform = items[i]->GetComponent<Transform2D>();
		ComponentImage* image = items[i]->GetComponent<ComponentImage>();
		math::float2 pos = rectTransform->getPosition();
		math::float2 size = rectTransform->getSize();
		float buttonX = pos.x;
		float buttonY = pos.y;

		math::float2 buttonMin = float2(buttonX - size.x *.5f, -buttonY - size.y *.5f);
		math::float2 buttonMax = float2(buttonX + size.x *.5f, -buttonY + size.y *.5f);

		if (screenX > buttonMin.x && screenX < buttonMax.x && screenY > buttonMin.y && screenY < buttonMax.y)
			image->isHovered = true;
		else
			image->isHovered = false;

		if (image->isHovered && App->input->GetMouseButtonDown(1) == KEY_DOWN)
		{
			image->isPressed = true;
			initialitemPos = pos;
		}

		if (image->isPressed)
		{
			math::float2 newPos = math::float2(screenX, -screenY);
			rectTransform->SetPositionUsingAligment(newPos);

		}

		if (image->isPressed && App->input->GetMouseButtonDown(1) == KEY_UP)
		{
			bool moved = false;
			image->isPressed = false;
			
			for (int j = 0; j < TOTAL_SLOTS; ++j)
			{
				math::float2 posSlot = ((Transform2D*)slotsTransform[j])->getPosition();
				math::float2 sizeSlot = ((Transform2D*)slotsTransform[j])->getSize();
				math::float2 slotMin = float2(posSlot.x - sizeSlot.x *.5f, -posSlot.y - sizeSlot.y *.5f);
				math::float2 slotMax = float2(posSlot.x + sizeSlot.x *.5f, -posSlot.y + sizeSlot.y *.5f);
				if (screenX > slotMin.x && screenX < slotMax.x && screenY > slotMin.y && screenY < slotMax.y)
				{
					if (i >= INVENTARY_SLOTS)
					{
						//Check if is correct item slot
					}

					moved = true;
					math::float2 newPos = math::float2(posSlot.x, posSlot.y);
					rectTransform->SetPositionUsingAligment(newPos);

					for (GameObject* secondItem : items)
					{
						if (secondItem->UUID != items[i]->UUID)
						{
							math::float2 posSecond = secondItem->GetComponent<Transform2D>()->getPosition();
							if (posSecond.x == posSlot.x && posSecond.y == posSlot.y)
							{
								secondItem->GetComponent<Transform2D>()->SetPositionUsingAligment(initialitemPos);
								GameObject* tmp(std::move(secondItem));
								items[j] = std::move(items[i]);
								items[i] = std::move(tmp);
								tmp = nullptr;

								return;
							}
						}
					}

					break;
				}
			}

			if (!moved)
			{
				rectTransform->SetPositionUsingAligment(initialitemPos);
				initialitemPos = math::float2::zero;
				return;
			}

		}
	}
}