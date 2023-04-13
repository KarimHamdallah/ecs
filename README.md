# ecs
fast ecs system for games and game engines


ECS Or Entity Component System / DesignPattern is largley used in games, it considered way faster and easier to think about than Traditional OOP
So I Created This Light weight STL based ECS Library For using it in my games and my own game engine,
I will update it instantly.

**CURRENT FEATURES:**
- CreatEntity.
- DestroyEntity.
- AddOrReplaceComponent.
- RemoveComponent.
- HasComponent.
- 3 Different Ways To Query For Specific Component.

**FUTURE PALN:**
- Make Query More Faster By Using Components Archetypes.
- Add Query For More Than One Componenet.
- Add Systems and Required Componenets For These Systems.
- Testing it in real game engine or game.

**CurrentDesign:**
![ecs](/ECS/assets/CurrentDesign.png?raw=true)

**FutureDesign (Archetypes):**
![ecs](/ECS/assets/FutureDesign.png?raw=true)

**HOW TO USE IT:**
```c++
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
```
```c++
- Output:
Entity With ID 0 Has SpriteRendererComponenet
Color : r = 1, g = 0, b = 0, a = 1
------------------------------
EntityID = 0 Has Transform Component With Data >> x = 20, y = 50, z = 30
EntityID = 1 Has Transform Component With Data >> x = 2, y = 5, z = 4
------------------------------
EntityID = 0 Has Transform Component With Data >> x = 20, y = 50, z = 30
EntityID = 1 Has Transform Component With Data >> x = 2, y = 5, z = 4
------------------------------
EntityID = 0 Has Transform Component With Data >> x = 20, y = 50, z = 30
EntityID = 1 Has Transform Component With Data >> x = 2, y = 5, z = 4
------------------------------
Press any key to continue . . .
```


**What Is Archetypes**
- Let's Assume We Have 1000000 Entities in our secne and 1000 of them having TransformComponent and 500 of them having SpriteRendererComponent and we need to loop throught all entities that have both componenets:
* First Solution: my friend mike is a stuipid programmer who suggests to creat entity group of all entites that have TransformComponent and at every loop we can confirm if it has SpriteRendererComponent or not by using HasComponent<SpriteRendererComponent>(entityid) function. in this case mike will loop through 1000 entities.
* Second Solution: my friend jason is a smart programmer who suggests to creat entity group of all entites that have SpriteRendererComponent and at every loop we can confirm if it has TransformComponent or not by using HasComponent<TransformComponent>(entityid) function. in this case jason will loop through 500 entities.

But Wait....

we have only 2 entities which have both TransformComponent and SpriteRendererComponent.

Hmmmm....... Shit.

Don't Be Upset, We Introduce you To Components Archetypes, Which is a table of entities that have a specific set of componenets.
so if we need to creat entity and Add to it both TransformComponent and SpriteRendererComponent, you can creat Archetype Table for Theses Componenets and use this archetype to add and remove these componenets to these objects. by this way you make sure that our Archetype Holds All Entities having both of these componenets.
if you want to add another component like rigidbody for example you can use registry no problem.

Archetype Solution: By using Archetypes We loop through only 2 entities having both TransformComponent and SpriteRendererComponent.

Easy...

**Archtype Problem**
- You Have To Use Archetypes for every entity in ur scene you need to query for, so that entites which have components added without archetypes will not included in query, but that not be problem at all as 90% of time we query for having one specific component and 10% of times we query for more than one component which we can use archetypes for these specific cases.

   ** Archetype Example **
   ```c++
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
   ```
