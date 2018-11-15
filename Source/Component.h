#ifndef __Component_h__
#define __Component_h__

class GameObject;

enum ComponentType
{
	Transform,
	Mesh,
	Material,
	Light
};

class Component
{
public:
	Component(GameObject* gameobject, ComponentType type);
	virtual ~Component();

	virtual void DrawProperties() {};
	virtual void Enable()
	{
		enabled = true;
	}

	virtual void Update() {}
	virtual void Disable()
	{
		enabled = false;
	}

public:
	GameObject* gameobject = nullptr;
	ComponentType type;
	bool enabled = true;
};

#endif __Component_h__