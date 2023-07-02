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
- 3 Different Ways To Query For Specific Component/Components  <get<T>, group<T1,T2>, do_for<T>>.
- Make Query More Faster By Using Components Archetypes.
- Add Query For More Than One Componenet.

**FUTURE PALN:**
- Add Systems and Required Componenets For These Systems.
- Testing it in real game engine or game.

**OldDesign:**
![ecs](/ECS/assets/CurrentDesign.png?raw=true)

**FutureDesign (Archetypes):**
![ecs](/ECS/assets/FutureDesign.png?raw=true)

**CurrentDesign (Mix Between Archetypes & HashMaps):**
this make it easier to set Signatures Into Archetypes and get entities ids from looping through Archetypes.
example : group<TransformComponent, SpriteRendererComponent>  >> return set of <uint32_t> each one is an entity index which have both TransformComponent and SpriteRendererComponent.
we can use these indices to get components from registry hash maps without looping through all registry entities or hash tables of both components. 

**HOW TO USE IT:**
```c++
#include <iostream>
#include "Ecs/Ecs.h"

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
Press any key to continue . . .
```


**WHAT IS ARCHTYPES**
- Let's Assume We Have 1000000 Entities in our secne and 1000 of them having TransformComponent and 500 of them having SpriteRendererComponent and we need to loop throught all entities that have both componenets:
* First Solution: some one may suggests to creat entity group of all entites that have TransformComponent and at every loop we can confirm if it has SpriteRendererComponent or not by using HasComponent<SpriteRendererComponent>(entityid) function. in this case he will loop through 1000 entities.
* Second Solution: another one may suggests to creat entity group of all entites that have SpriteRendererComponent and at every loop we can confirm if it has TransformComponent or not by using HasComponent<TransformComponent>(entityid) function. in this case he will loop through 500 entities.

But Wait....

we have only 2 entities which have both TransformComponent and SpriteRendererComponent.

Hmmmm....... Shit.

Don't Be Upset, We Introduce you To Components Archetypes, Which is a table of entities that have a specific set of componenets.
so if we need to creat entity and Add to it both TransformComponent and SpriteRendererComponent, you can creat Archetype Table for Theses Componenets and use this archetype to add and remove these componenets to these objects. by this way you make sure that our Archetype Holds All Entities having both of these componenets.
if you want to add another component like rigidbody for example you can use registry no problem.

Archetype Solution: By using Archetypes We loop through only 2 entities having both TransformComponent and SpriteRendererComponent.

Easy...

our ecs now has dynamic archetype tables that represents all entities having same signature (Special Set Of Components).
