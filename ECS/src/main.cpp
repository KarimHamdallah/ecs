#include <iostream>
#include "Ecs/Ecs.h"

// Components

struct TransformComponent : Component
{
public:
	TransformComponent() = default;
	TransformComponent(TransformComponent&) = default;
	TransformComponent(float x, float y, float z)
		: x(x), y(y), z(z) {}

	float x, y, z = 0.0f;


	static ComponentType GetType() { return ComponentType::TransformComponent; }
};

struct SpriteRendererComponent : Component
{
public:
	SpriteRendererComponent() = default;
	SpriteRendererComponent(SpriteRendererComponent&) = default;
	SpriteRendererComponent(float r, float g, float b)
		: r(r), g(g), b(b) {}

	float r, g, b = 1.0f;


	static ComponentType GetType() { return ComponentType::SpriteRendererComponent; }
};

struct TextRendererComponent : Component
{
public:
	TextRendererComponent() = default;
	TextRendererComponent(TextRendererComponent&) = default;
	TextRendererComponent(const std::string& text)
		: text(text) {}

	std::string text = "";


	static ComponentType GetType() { return ComponentType::TextRendererComponent; }
};

struct RigidBodyComponent : Component
{
public:
	RigidBodyComponent() = default;
	RigidBodyComponent(RigidBodyComponent&) = default;
	RigidBodyComponent(float gravity)
		: gravity(gravity) {}

	float gravity = 0.0f;


	static ComponentType GetType() { return ComponentType::RigidBodyComponent; }
};


int main()
{
	// Creat Registry
	Registry reg;
	// Creat Entity
	Entity e1 = reg.CreateEntity();
	Entity e2 = reg.CreateEntity();
	Entity e3 = reg.CreateEntity();

	// Add Componenet Or ReplaceComponenet If Present
	reg.AddComponent<TransformComponent>(e1, 20.0f, 50.0f, 30.0f);
	reg.AddComponent<SpriteRendererComponent>(e1, 1.0f, 0.0f, 0.0f);
	reg.AddComponent<TransformComponent>(e2, 2.0f, 5.0f, 3.0f); // Add
	reg.AddComponent<TransformComponent>(e2, 2.0f, 5.0f, 4.0f); // Replace

	// DestryEntity
	bool destroy = false;
	if (destroy)
		reg.RemoveEntity(e1);

	// Has Componenet
	bool has = reg.HasComponent<SpriteRendererComponent>(e1);
	std::cout << "Entity With ID " << e1 << " " << (has ? ("Has") : ("Hasn't")) << " SpriteRendererComponenet\n";

	// Get Componenet
	if (has)
	{
		SpriteRendererComponent& spriterenderer = *reg.GetComponent<SpriteRendererComponent>(e1);
		std::cout << "Color : r = " << spriterenderer.r << ", g = " << spriterenderer.g << ", b = " << spriterenderer.b << "\n";
	}
	std::cout << "------------------------------\n";


	// Remove Component
	bool remove = false;
	if (remove)
		reg.RemoveComponent<TransformComponent>(e2);

	// Query

	{ // Get All Entities That Have Transform Components and print EntityID With It's Transform Componenet Data
		auto group = reg.get<TransformComponent>();
		for (Entity entity : group)
		{
			TransformComponent& transform = *reg.GetComponent<TransformComponent>(entity);
			std::cout << "EntityID = " << entity << " Has Transform Component With Data >> ";
			std::cout << "x = " << transform.x << ", y = " << transform.y << ", z = " << transform.z << "\n";
		}
	}

	std::cout << "------------------------------\n";

	{// Do This Function For All Entities That Have Transform Component
		reg.do_for<TransformComponent>([&](Entity Entity_id, TransformComponent& transform)
			{
				std::cout << "EntityID = " << Entity_id << " Has Transform Component With Data >> ";
				std::cout << "x = " << transform.x << ", y = " << transform.y << ", z = " << transform.z << "\n";

				// Examples From My Game Engine(SnapEngine) -- Inside My UI->Render(); & Serializer->SerializerScene();
				/*
				   UI::DrawFloat3("Position", &transform.x, &transform.y, &transform.z);
				   Serializer::SerializerTransformComponenet(Entity_id, transform);
				*/
			});
	}


	std::cout << "------------------------------\n";

	system("Pause");
	return 0;
}
