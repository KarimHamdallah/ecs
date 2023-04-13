#include <iostream>
#include "Ecs/Ecs.h"

// Setup Componenets
struct TransformComponent : Component
{
	TransformComponent() = default;
	TransformComponent(float x, float y, float z)
		: x(x), y(y), z(z) {}

	float x, y, z;
};

struct SpriteRendererComponent : Component
{
	SpriteRendererComponent() = default;
	SpriteRendererComponent(float r, float g, float b, float a)
		: r(r), g(g), b(b), a(a) {}

	float r, g, b, a;
};

struct RigidBodyComponent : Component
{
	RigidBodyComponent() = default;
	RigidBodyComponent(float gravity)
		: gravity(gravity) {}

	float gravity = 9.8f;
};

int main()
{
	// Creat Registry
	Registry reg;
	// Creat Entity
	EntityID e1 = reg.CreatEntity();
	EntityID e2 = reg.CreatEntity();
	EntityID e3 = reg.CreatEntity();
	// Add Componenet Or ReplaceComponenet If Present
	reg.AddComponent<TransformComponent>(e1, 20.0f, 50.0f, 30.0f);
	reg.AddComponent<SpriteRendererComponent>(e1, 1.0f, 0.0f, 0.0f, 1.0f);
	reg.AddComponent<TransformComponent>(e2, 2.0f, 5.0f, 3.0f); // Add
	reg.AddComponent<TransformComponent>(e2, 2.0f, 5.0f, 4.0f); // Replace

	/*
	* Using Archetypes
	*/

	EntityID e4 = reg.CreatEntity();
	EntityID e5 = reg.CreatEntity();
	EntityID e6 = reg.CreatEntity();
	EntityID e7 = reg.CreatEntity();

	ArchetypeSystem<TransformComponent, SpriteRendererComponent> transf_sprrend_archetype(reg);
	transf_sprrend_archetype.AddFirstComponents(e4, 20.0f, 30.0, 40.0f);
	transf_sprrend_archetype.AddSecondComponents(e4, 1.0f, 0.0, 0.0f, 1.0f);
	reg.AddComponent<RigidBodyComponent>(e4, 9.8f);

	transf_sprrend_archetype.AddFirstComponents(e5, 12.0f, 21.0, 11.0f);
	transf_sprrend_archetype.AddSecondComponents(e5, 0.0f, 1.0, 0.0f, 1.0f);

	transf_sprrend_archetype.AddFirstComponents(e6, 70.0f, 18.0, 350.0f);
	transf_sprrend_archetype.AddSecondComponents(e6, 0.0f, 0.0, 1.0f, 1.0f);

	transf_sprrend_archetype.AddFirstComponents(e7, 20.0f, 30.0, 40.0f);
	transf_sprrend_archetype.AddSecondComponents(e7, 1.0f, 0.0, 0.0f, 1.0f);

	//transf_sprrend_archetype.RemoveComponenet(e4, ComponentPos::FIRST_COMPONENT);


	{// Query With Archetypes
		for (EntityID entityid : transf_sprrend_archetype)
		{
			auto&[transform, spriterenderer] = transf_sprrend_archetype.get(entityid);
			std::cout << "EntityID = " << entityid << " Has Transform Component With Data >> ";
			std::cout << "x = " << transform.x << ", y = " << transform.y << ", z = " << transform.z << "\n";
			std::cout << "EntityID = " << entityid << " Has SpriteRenderer Component With Data >> ";
			std::cout << "Color : r = " << spriterenderer.r << ", g = " << spriterenderer.g << ", b = " << spriterenderer.b << ", a = " << spriterenderer.a << "\n";
			std::cout << "--------\n";
		}
	}










	// DestryEntity
	bool destroy = false;
	if (destroy)
		reg.DestroyEntity(e1);
	
	// Has Componenet
	bool has = reg.HasComponent<SpriteRendererComponent>(e1);
	std::cout << "Entity With ID " << e1 << " " << (has ? ("Has") : ("Hasn't")) << " SpriteRendererComponenet\n";

	// Get Componenet
	if (has)
	{
		SpriteRendererComponent& spriterenderer = reg.GetComponent<SpriteRendererComponent>(e1);
		std::cout << "Color : r = " << spriterenderer.r << ", g = " << spriterenderer.g << ", b = " << spriterenderer.b << ", a = " << spriterenderer.a << "\n";
	}
	std::cout << "------------------------------\n";


	// Remove Component
	bool remove = false;
	if(remove)
		reg.RemoveComponent<TransformComponent>(e2);

	// Query

	{ // Get All Entities That Have Transform Components and print EntityID With It's Transform Componenet Data
		EntityGroup group = reg.get_group<TransformComponent>();
		for (auto& [entityid, comp] : group)
		{
			TransformComponent& transform = *static_cast<TransformComponent*>(comp.get());
			std::cout << "EntityID = " << entityid << " Has Transform Component With Data >> ";
			std::cout << "x = " << transform.x << ", y = " << transform.y << ", z = " << transform.z << "\n";
		}
	}

	std::cout << "------------------------------\n";

	{ // Get All Entities That Have Transform Components and print EntityID With It's Transform Componenet Data
		EntityGroup group = reg.get_group<TransformComponent>();
		for (auto entity : group)
		{
			auto& [entityid, transform] = group.get<TransformComponent>(entity);
			std::cout << "EntityID = " << entityid << " Has Transform Component With Data >> ";
			std::cout << "x = " << transform.x << ", y = " << transform.y << ", z = " << transform.z << "\n";

			// Examples From My Game Engine(SnapEngine) -- Inside My Scene->Render();
			/*
			   if(reg.HasComponenet<SpriteRendererComponenet>())
			   {
			      auto& sprite_renderer = reg.GetComponenet<SpriteRendererComponenet>(Entity_id);
			      Renderer::DrawSprite(transform, sprite_renderer);
			   }
			*/
		}
	}

	std::cout << "------------------------------\n";

	{// Do This Function For All Entities That Have Transform Component
		reg.do_for<TransformComponent>([&](EntityID Entity_id, TransformComponent& transform)
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