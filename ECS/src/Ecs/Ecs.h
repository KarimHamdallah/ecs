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