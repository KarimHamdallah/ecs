#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>

#define ECS_ASSERT(cond, msg) do{ if(cond){} else { std::cerr << msg << "\n"; __debugbreak(); } }while(0)


using EntityID = std::size_t;
using ComponentID = std::size_t;

class System {};
class Component {};

inline ComponentID GetUniqueComponentTypeID()
{
	static std::atomic<std::size_t> ComponentTypeID = 0u;
	return ComponentTypeID++;
}

template<typename T>
inline ComponentID GetComponentTypeID()
{
	bool IsComponent = (std::is_base_of<Component, T>::value);
	ECS_ASSERT(IsComponent, "Passed Type Is Not A Component!");
	static const ComponentID TypeID = GetUniqueComponentTypeID();
	return TypeID;
}


struct EntityGroup
{
	EntityGroup() = default;
	EntityGroup(const std::unordered_map<EntityID, std::shared_ptr<Component>>& MapRef)
		: map(MapRef) {}

	template<typename T>
	inline std::pair<EntityID, T&> get(const std::pair< EntityID, std::shared_ptr<Component>>& Slot)
	{
		return { Slot.first, *static_cast<T*>(map.at(Slot.first).get()) };
	}

	std::unordered_map<EntityID, std::shared_ptr<Component>>::iterator begin() { return map.begin(); }
	std::unordered_map<EntityID, std::shared_ptr<Component>>::iterator end() { return map.end(); }
private:
	std::unordered_map<EntityID, std::shared_ptr<Component>> map;
};


class Archetype
{
public:
	Archetype() = default;

	void AddTo(EntityID entity)
	{
		m_Archetype.push_back(entity);
	}
	
	void Erase(EntityID entity)
	{
		auto it = std::find(m_Archetype.begin(), m_Archetype.end(), entity);
		m_Archetype.erase(it);
	}
	
	
	std::vector<EntityID>::iterator begin() { return m_Archetype.begin(); }
	std::vector<EntityID>::iterator end() { return m_Archetype.end(); }
private:
	std::vector<EntityID> m_Archetype;
};



class Registry
{
public:
	Registry() = default;
	EntityID CreatEntity() { return EntityCounter++; }

	template<typename T>
	inline bool HasComponent(EntityID Entity)
	{
		ComponentID comp_id = GetComponentTypeID<T>();
		auto it = Components[comp_id].find(Entity);
		return it != Components[comp_id].end();
	}

	/*
    Add Component Will override Component If Already The Same Component Type Added Before
    */
	template<typename T, typename... Args>
	inline void AddComponent(EntityID Entity, Args&&... args)
	{
		//ECS_ASSERT(!HasComponent<T>(Entity), "Entity Already Has Component!");
		auto Component = std::make_shared<T>(std::forward<Args>(args)...);
		ComponentID comp_id = GetComponentTypeID<T>();
		Components[comp_id][Entity] = Component;
	}

	template<typename T>
	inline T& GetComponent(EntityID Entity)
	{
		//------- Has Component -------------
		ComponentID comp_id = GetComponentTypeID<T>();
		auto it = Components[comp_id].find(Entity);
		//------- Get Component -------------
		ECS_ASSERT(it != Components[comp_id].end(), "Entity Dosen't Have Component!");
		return *static_cast<T*>(it->second.get());
	}

	template<typename T>
	inline T& GetComponentWithoutAssertion(EntityID Entity)
	{
		ComponentID comp_id = GetComponentTypeID<T>();
		return *static_cast<T*>(Components.at(comp_id).at(Entity).get());
	}

	template<typename T>
	inline void RemoveComponent(EntityID Entity)
	{
		//------- Has Component -------------
		ComponentID comp_id = GetComponentTypeID<T>();
		auto it = Components[comp_id].find(Entity);
		//------- Remove Component Slot -------------
		if (it != Components[comp_id].end())
			Components[comp_id].erase(Entity);
	}

	template<typename T>
	inline EntityGroup get_group()
	{
		ComponentID comp_id = GetComponentTypeID<T>();
		auto it = Components.find(comp_id);
		if (it != Components.end())
			return EntityGroup(Components.at(comp_id));

		return EntityGroup();
	}

	template<typename T, typename Func>
	inline void do_for(Func func)
	{
		EntityGroup group = this->get_group<T>();
		for (auto entity : group)
		{
			auto& [entityid, CompRef] = group.get<T>(entity);
			func(entityid, CompRef);
		}
	}


	// TODO:: Think About Faster Way
	void DestroyEntity(EntityID Entity)
	{
		for (auto& Comp : Components)
		{
			//------- Has Component -------------
			ComponentID comp_id = Comp.first;
			auto it = Components[comp_id].find(Entity);
			//------- Remove Component Slot -------------
			if (it != Components[comp_id].end())
				Components[comp_id].erase(Entity);
		}
	}

private:
	std::size_t EntityCounter = 0;
	std::unordered_map<ComponentID, std::unordered_map<EntityID, std::shared_ptr<Component>>> Components;
	std::vector<std::shared_ptr<System>> Systems;
};

/*
* Archetypes
*/

enum class ComponentPos
{
	FIRST_COMPONENT = 0,
	SECOND_COMPONENT
};

template<typename T1, typename T2>
class ArchetypeSystem
{
public:
	ArchetypeSystem(Registry& reg)
		: reg(reg) {}

	template<typename... Args>
	inline void AddFirstComponents(EntityID entity, Args&&... args)
	{
		if (!reg.HasComponent<T1>(entity))
			reg.AddComponent<T1>(entity, std::forward<Args>(args)...);
		if (reg.HasComponent<T2>(entity))
			archetype.AddTo(entity);
	}

	template<typename... Args>
	inline void AddSecondComponents(EntityID entity, Args&&... args)
	{
		if (!reg.HasComponent<T2>(entity))
			reg.AddComponent<T2>(entity, std::forward<Args>(args)...);
		if (reg.HasComponent<T1>(entity))
			archetype.AddTo(entity);
	}

	inline void RemoveComponenet(EntityID entity, ComponentPos WhichComponenet)
	{
		switch (WhichComponenet)
		{
		case ComponentPos::FIRST_COMPONENT: reg.RemoveComponent<T1>(entity); break;
		case ComponentPos::SECOND_COMPONENT: reg.RemoveComponent<T2>(entity); break;
		default: break;
		}

		archetype.Erase(entity);
	}

	std::vector<EntityID>::iterator begin() { return archetype.begin(); }
	std::vector<EntityID>::iterator end() { return archetype.end(); }


	std::pair<T1&, T2&> get(EntityID Entity)
	{
		T1& FirstComponent = reg.GetComponentWithoutAssertion<T1>(Entity);
		T2& SecondComponent = reg.GetComponentWithoutAssertion<T2>(Entity);

		return { FirstComponent, SecondComponent };
	}

private:
	Registry& reg;
	Archetype archetype;
};