#include <vector>
#include <bitset>
#include <unordered_map>
#include <set>
#include <list>
#include <memory>

namespace
{
	uint32_t MaxEntityCount = 1000;
}

using Entity = uint32_t;
class Component {};
enum class ComponentType
{
	TransformComponent = 0,
	SpriteRendererComponent,
	TextRendererComponent,
	FinalCount
};

using EntitySignature = std::bitset<64>;
using ArchetypeLayout = std::bitset<64>;

class Archetype
{
public:
	Archetype() = default;
	void PushEntity(Entity e) { m_Entities.insert(e); }
	void RemoveEntity(Entity e) { m_Entities.erase(e); }

	std::set<Entity>::iterator begin() { return m_Entities.begin(); }
	std::set<Entity>::iterator end() { return m_Entities.end(); }

	bool isEmpty() { return m_Entities.size() == 0; }
	const std::set<Entity>& GetSet() { return m_Entities; }

private:
	std::set<Entity> m_Entities;
};

class Registry
{
public:
	Registry() = default;
	Entity CreateEntity()
	{
		if (!RemovedEntities.empty())
		{
			uint32_t id = RemovedEntities.front();
			RemovedEntities.pop_front();
			return id;
		}
		return EntityCounter++;
	}

	template<typename T, typename... Args>
	void AddComponent(Entity entity, Args&&... args)
	{
		// Assert If T is A Component
		static_assert(std::is_base_of<Component, T>::value, "T is not a Component");
		
		ComponentType ComponentType = T::GetType();

		// Creat Component
		std::shared_ptr<T> Comp = std::make_shared<T>(std::forward<Args>(args)...);

		// Add Component To ComponentsMap
		auto it = ComponentsMap.find(ComponentType);
		if (it != ComponentsMap.end())
		{
			// Component Array Is Found
			{ // dynamic
				if (entity < it->second.size())
					it->second[entity] = Comp;
				else
					it->second.push_back(Comp);
			}

			/*
			{ // static
				it->second[entity] = Comp;
			}
			*/
		}
		else
		{
			// Component Array Not Found And Creat One
			std::vector<std::shared_ptr<Component>> ComponentArray;
			ComponentArray.resize(MaxEntityCount, nullptr);
			ComponentArray[entity] = Comp;
			ComponentsMap[ComponentType] = ComponentArray;
		}

		// Entity Signature
		AddEntitySignature(entity, ComponentType);
		// Update Archetypes
		UpdateArchetypes(entity);
	}

	template<typename T>
	T* GetComponent(Entity entity)
	{
		// Assert If T is A Component
		static_assert(std::is_base_of<Component, T>::value, "T is not a Component");

		ComponentType ComponentType = T::GetType();

		// Get Component Array From ComponentsMap
		auto it = ComponentsMap.find(ComponentType);
		if (it != ComponentsMap.end())
		{
			// Component Array Is Found
			std::shared_ptr<Component> Comp = it->second[entity];
			return static_cast<T*>(Comp.get());
		}
		else
		{
			return static_cast<T*>(nullptr);
		}
	}

	template<typename T>
	bool HasComponent(Entity entity)
	{
		// Assert If T is A Component
		static_assert(std::is_base_of<Component, T>::value, "T is not a Component");
		
		ComponentType ComponentType = T::GetType();

		auto it = EntitySignatures.find(entity);
		if (it != EntitySignatures.end())
		{
			// EntitySignatures Array Is Found
			return it->second.test((int)ComponentType);
		}
		else
		{
			// No Signatures Means No Components
			return false;
		}
	}

	template<typename T>
	void RemoveComponent(Entity entity)
	{
		// Assert If T is A Component
		static_assert(std::is_base_of<Component, T>::value, "T is not a Component");

		ComponentType ComponentType = T::GetType();

		auto sig_it = EntitySignatures.find(entity);
		if (sig_it != EntitySignatures.end())
		{
			// Signature Found
			if (sig_it->second.test((int)ComponentType)) // Entity Has Component
			{
				ComponentsMap[ComponentType][entity].reset();
				RemoveEntitySignature(entity, ComponentType, sig_it->second);
			}
		}
	}

	void RemoveEntity(Entity entity)
	{
		auto sig_it = EntitySignatures.find(entity);
		if (sig_it != EntitySignatures.end())
		{
			EntitySignature sig = sig_it->second;
			for (auto& pair : ComponentsMap)
			{
				if (sig.test((int)pair.first))
				{
					pair.second[entity].reset();
				}
			}

			// Remove Entity Form Holding Archetype
			const ArchetypeLayout& layout = Entity_ArchetypeLayout.at(entity);
			Archetype& arc = Archetypes[layout];
			arc.RemoveEntity(entity);

			// Remove Archetype if Empty
			if (arc.isEmpty())
				Archetypes.erase(layout);

			// Remove From Entity Signatures
			EntitySignatures.erase(entity);

			// Remove From Entity_ArchetypeLayout
			Entity_ArchetypeLayout.erase(entity);

			// Add To Removed Entities
			RemovedEntities.push_front(entity);
		}
	}


	template<typename T1, typename T2>
	std::set<Entity> group()
	{
		std::set<Entity> ret;

		int Type1 = (int)T1::GetType();
		int Type2 = (int)T2::GetType();

		for (auto& pair : Archetypes)
		{
			if (pair.first.test(Type1) && pair.first.test(Type2))
			{
				const std::set<Entity>& set = pair.second.GetSet();
				ret.insert(set.begin(), set.end());
			}
		}

		return ret;
	}



private:
	std::unordered_map<ComponentType, std::vector<std::shared_ptr<Component>>> ComponentsMap;
	std::unordered_map<Entity, EntitySignature> EntitySignatures;
	std::unordered_map<ArchetypeLayout, Archetype> Archetypes;
	std::unordered_map<Entity, ArchetypeLayout> Entity_ArchetypeLayout;
	std::list<Entity> RemovedEntities;


	uint32_t EntityCounter = 0u;


	// EntitySignature
	void AddEntitySignature(Entity entity, ComponentType ComponentType)
	{
		auto it = EntitySignatures.find(entity);
		if (it != EntitySignatures.end())
		{
			// before Modifying Signature
			ArchetypeLayout layout = it->second;
			auto arch_it = Archetypes.find(layout);
			if (arch_it != Archetypes.end())
			{
				// Erase this entity From Holding Archetype
				arch_it->second.RemoveEntity(entity);
				
				// Remove Archetype if Empty
				if (arch_it->second.isEmpty())
					Archetypes.erase(arch_it->first);
			}

			// Signature Found
			it->second.set((int)ComponentType);

			// And Push it To new One After Modification
			UpdateArchetypes(entity);
		}
		else
		{
			// Creat Signature
			EntitySignature seg;
			seg.set((int)ComponentType);
			EntitySignatures[entity] = seg;
		}
	}

	// EntitySignature
	void RemoveEntitySignature(Entity entity, ComponentType ComponentType, EntitySignature& sig)
	{
		// before Modifying Signature
		ArchetypeLayout layout = sig;
		auto arch_it = Archetypes.find(layout);
		if (arch_it != Archetypes.end())
		{
			// Erase this entity From Holding Archetype
			arch_it->second.RemoveEntity(entity);

			// Remove Archetype if Empty
			if (arch_it->second.isEmpty())
				Archetypes.erase(arch_it->first);
		}

		// Signature Found
		sig.set((int)ComponentType, 0);

		// And Push it To new One After Modification
		UpdateArchetypes(entity);
	}

	void UpdateArchetypes(Entity entity)
	{
		ArchetypeLayout layout = EntitySignatures[entity];
		auto arch_it = Archetypes.find(layout);
		if (arch_it != Archetypes.end()) // Check For Suitable Archetype
		{
			arch_it->second.PushEntity(entity);
		}
		else // Creat New Archetype
		{
			Archetype arc;
			arc.PushEntity(entity);
			Archetypes[layout] = arc;
		}

		// When Push Entity
		Entity_ArchetypeLayout[entity] = layout;
	}
};

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
