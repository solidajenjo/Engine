#ifndef __GameObject_h__
#define __GameObject_h__

#include <vector>
#include "assimp/matrix4x4.h"
#include "Math/float4x4.h"

class Component;
class ComponentTransform;
enum ComponentType;
struct Texture;

class GameObject
{
public:
	GameObject(const char* name);
	GameObject(const aiMatrix4x4& transform, const char* filepath, const char* name);
	~GameObject();
	void Draw();
	void DrawProperties();
	void DrawHierarchy(int &obj_clicked, int i);
	void Delete();
	void Update();

	Component * CreateComponent(ComponentType type);
	void AddComponent(Component* component);
	Component * GetComponent(ComponentType type) const;
	std::vector<Component *> GetComponents(ComponentType type) const;
	void RemoveComponent(Component * component);
	void RemoveChild(GameObject* child);

	std::string GetFileFolder() const;
	float4x4 GetGlobalTransform() const;
	void DisableBox();

	void CleanUp();


private:
	float4x4 GetLocalTransform() const;
	void ModelTransform(unsigned int shader) const;
	AABB GetBoundingBox() const;

	void DrawBBox() const;


public:
	ComponentTransform * transform = nullptr;
	GameObject *parent = nullptr;
	std::vector<Component*> components;
	std::vector<GameObject*> children;
	std::string filepath = "";
	std::string name = "GameObject";

private:
	bool drawBBox = false;

};

#endif __GameObject_h__

