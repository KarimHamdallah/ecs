#include <iostream>
#include "Ecs/Ecs.h"

int main()
{
	Registry registry;

	Entity e1 = registry.CreateEntity();
	Entity e2 = registry.CreateEntity();
	Entity e3 = registry.CreateEntity();
	Entity e4 = registry.CreateEntity();

	registry.AddComponent<TextRendererComponent>(e1, "Karim");
	registry.AddComponent<SpriteRendererComponent>(e2, 1, 1, 0);
	registry.AddComponent<TransformComponent>(e1, 5, 3, 2);
	registry.AddComponent<SpriteRendererComponent>(e3, 1, 0, 1);
	registry.AddComponent<SpriteRendererComponent>(e4, 1, 0, 1);
	registry.AddComponent<TransformComponent>(e3, 6, 2, 9);
	registry.AddComponent<SpriteRendererComponent>(e1, 0, 1, 1);
	registry.AddComponent<TransformComponent>(e2, 1, 3, 8);
	registry.AddComponent<TransformComponent>(e4, 6, 2, 9);


	bool e1_has_transform = registry.HasComponent<TransformComponent>(e1); // true
	TransformComponent* tranf_e1 = registry.GetComponent<TransformComponent>(e1);
	std::cout << "e1 transform : " << tranf_e1->x << " , " << tranf_e1->y << " , " << tranf_e1->z << "\n";
	registry.RemoveComponent<SpriteRendererComponent>(e1);
	e1_has_transform = registry.HasComponent<TransformComponent>(e1); // false

	registry.RemoveEntity(e1);

	Entity e5 = registry.CreateEntity();
	registry.AddComponent<TransformComponent>(e5, 5, 5, 5);
	registry.AddComponent<SpriteRendererComponent>(e5, 1.0f, 1.0f, 1.0f);

	const auto& group = registry.group<TransformComponent, SpriteRendererComponent>();
	for (Entity e : group)
	{
		std::cout << e << " ";
	}
	std::cout << "\n";

	std::cout << "------------------------------\n";

	system("Pause");
	return 0;
}
