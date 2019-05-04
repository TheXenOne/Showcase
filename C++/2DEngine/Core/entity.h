#pragma once

#include <defines.h>

#include <Utility\bitflag.h>

#include <memory>
#include <vector>
#include <string>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

/**
 * @file entity.h
 * @author Meine Zeinstra
 * @author Jens Hagen
 * @author Ben Landor
 * @date 27 feb 2018
 * @brief iga::Entity class header.
 *
 * iga::Entity is an object in the scene. It has a iga::Transform by default.
 * It can have components that give the entities functionality.
 * iga::Entity is the only class that can create, add and destroy components.
 */

/**
 *  IGArt namespace
 */
namespace iga {

	class Scene;
	class Transform;
	class Component;

	/**
	*  iga::Entity class. This class is an object in the scene.
	*  @see iga::Scene
	*/
	class Entity final {

		friend Scene;
#ifdef EDITOR
		friend class Editor;
#endif // EDITOR
		friend Transform;
		friend class PhysicsBodyComponent;

	public:
		/**
		*   Cereal versions of iga::Entity
		*/
		enum class Version {
			DEFAULT = 0, /**< Default normal version. */
			HIERARCHY_VISIBLE, /** Visible in hierarchy version */
			TAGGING, /** Entities have tags */
			SELECTABLE, /** Saves whether entities can be selected in with the mouse in editor */
			LATEST /**< Latest version. */
		};

		/**
		* Constructor
		*/
		Entity();
		/**
		* Destructor
		*/
		~Entity();
		/**
		*  Removes the copy constructor since iga::Entity can't be copied.
		*/
		Entity(Entity& a_entity) = delete;
		/**
		*  Removes the copy assigment since iga::Entity can't be copied.
		*/
		Entity& operator=(const Entity& a_entity) = delete;

		/**
		* move constructor
		*/
		Entity( Entity&& arg_entity );
		/**
		* move assignment operator
		*/
		Entity& operator=( Entity&& arg_entity );

		/**
		*  Gets called after creation.
		*  All initialization should happen in here.
		*  @param a_on_load Wether the entity is loaded by a file or created in run-time. True is loaded by a file. False (default value) is created in run-time.
		*/
		void OnCreate(bool a_on_load = false);

		/**
		*  Updates the iga::Entity, its components and iga::Transform
		*/
		void Update();
		/**
		*  Post Update call, 
		*/
		void PostUpdate();

		/**
		*  Get the ID of iga::Entity
		*  @return A const unsigned int with the ID of the entity
		*/
		const size_t GetID() const;
		/**
		*  Get the name of iga::Entity
		*  @return A const std::string& with the name of the entity
		*/
		const std::string& GetName() const;
		/**
		*  Set the name of the iga::Entity
		*  @param const std::string& with the new name for the entity
		*/
		void SetName(const std::string& a_name);
		/**
		*  Get if the iga::Entity is enabled
		*  @return const bool
		*/
		const bool GetEnabled() const;
		/**
		*  Set is the iga::Entity is enabled
		*  @param const bool
		*/
		void SetEnabled(const bool a_enabled);

		/**
		*  Gets all tags that are enabled.
		*  @param const bool
		*/
		const BitFlag GetAllTags();

		/**
		*  Set the bitmask directly. destroying tags that are already set.
		*  @param const bool
		*/
		void SetAllTags(BitFlag a_tags);

		/**
		*  true if specific flag inside of the mask is set.
		*  @param const bool
		*/
		const bool HasTag(BitFlag a_tag);

		/**
		*  Set specific flag. (add on top of others).
		*  @param const bool
		*/
		void SetTag(BitFlag a_tag);

		/**
		*  Gets whether the iga::Entity has a parent or not.
		*  @return Whether the iga::Entity has a parent or not.
		*/
		bool HasParent() const;
		/**
		*  Gets whether the iga::Entity has physics or not.
		*  @return Whether the iga::Entity has physics or not.
		*/
		bool HasPhysics() const;

		/**
		*  Get the iga::Scene the iga::Entity is currently in
		*  @return std::weak_ptr<Scene> to the iga::Scene
		*  @see iga::Scene
		*/
		std::weak_ptr<Scene> GetScene() const;
		/**
		*  Get the iga::Transform of the iga::Entity
		*  @return std::weak_ptr<Transform> to the iga::Transform
		*  @see iga::Transform
		*/
		std::weak_ptr<Transform> GetTransform() const;
		/**
		*  @brief Create new iga::Component of specified type
		*  @details Create new iga::Component of specified type. adds it to the components vector of this iga::Entity.
		*  Only one type of iga::Component can be on a single iga::Entity
		*  @return std::weak_ptr<T> to the iga::Component of type.
		*  @see iga::Component
		*/
		template<class T>
		std::weak_ptr<T> NewComponent();
		/**
		*  Get iga::Component of specified type
		*  @return std::weak_ptr<T> to the iga::Component of type
		*  @see iga::Component
		*/
		template<class T>
		std::weak_ptr<T> GetComponent();
		/**
		*  Delete iga::Component of specified type
		*  @see iga::Component
		*/
		template<class T>
		void DeleteComponent();

		/**
		*  Sets wether the iga::Entity is visible in the hierarchy or not.
		*  his function only functions when EDITOR is defined
		*   @param a_in_hierarchy Wether the iga::Entity is visible in the hierarchy or not. True is visible in the hierarchy. False is invisible in the hierarchy.
		*/
		void SetVisibleInHierarchy(bool a_in_hierarchy);

		/**
		*  Gets whether the iga::Entity is visible hierarchy or not.
		*  @return A bool specifying whether the iga::Entity is visible in the hierarchy or not. True is visible in the hierarchy. False is invisible in the hierarchy.
		*/
		bool IsVisibleInHierarchy();
		/**
		*  Gets whether the iga::Entity is selectable by the inspector.
		*  @return A bool specifying whether the iga::Entity is selectable by the inspector.
		*/
		bool IsInspectorSelectable();
		/**
		*  Sets whether the iga::Entity is selectable by the inspector.
		*  @param a_inspector_selectable A bool specifying whether the iga::Entity is selectable by the inspector.
		*/
		void SetInspectorSelectable(bool a_inspector_selectable);

		/**
		*  Used to show information in the editor : inspector window.
		*/
		void Inspect();

		/**
		*  Used to save and load iga::Entity.
		*/
		template<class Archive>
		void serialize(Archive &a_archive, uint32 const a_version);

	protected:


	private:
		/**
		* Constructor
		* @param std::weak_ptr<Scene>
		* @see iga::Scene
		*/
		Entity( std::weak_ptr<Scene> a_scene );
		
		size_t id_; /**< The ID of the iga::Entity. */
		std::string name_; /**< The Name of the iga::Entity. */
		bool is_enabled_; /**< State of iga::Entity enabled. */
		iga::BitFlag tags_; /**< storage for tags that are set for certain entities. */

		static size_t entity_number_; /**< Number used for name of entity. Required so entity can be selected in Editor/Hierarchy if no name is set by user. */

		bool in_hierarchy_; /**< Specifies if the entity will show in the Editor heirarchy. */
#ifdef EDITOR
		bool inspector_selectable_; /**< Specifies whether the entity is selectable using the mouse. (Editor only). */
#endif // EDITOR

		std::weak_ptr<Entity> self_; /**< Pointer to it's own iga::Entity weak pointer. */
		std::weak_ptr<Scene> scene_; /**< Pointer to the iga::Scene the iga::Entity is currenly in. */
		std::shared_ptr<Transform> transform_; /**< Pointer to the iga::Transform of the iga::Entity. */

		std::vector<std::shared_ptr<Component>> components_; /**< Vector of std::shared_ptr<Component> of the iga::Entity. */

		bool has_parent_; /**< Used to easier check if this entity has a parent or not. */
		bool has_physics_; /**< Used to easier check if this entity has physics or not. */
	};


	template<class T>
	inline std::weak_ptr<T> Entity::NewComponent() {
		std::weak_ptr<T> wp = GetComponent<T>();
		if( !wp.expired() )
		{
			return wp;
		}
		std::shared_ptr<Component> sp = std::shared_ptr<Component>( new T(self_) );
		sp->OnCreate();
		components_.push_back( sp );
		return std::static_pointer_cast<T>(sp);
	}

	template<class T>
	inline std::weak_ptr<T> Entity::GetComponent() {
		for( size_t i = 0, size = components_.size(); i < size; i++ )
		{
			if( dynamic_cast<T*>(components_[i].get()))
			{
				return std::static_pointer_cast< T >( components_[i] );
			}
		}
		return std::weak_ptr<T>();
	}

	template<class T>
	inline void Entity::DeleteComponent() {
		size_t index;
		for( size_t i = 0, size = components_.size(); i < size; i++ )
		{
			if( dynamic_cast<T*>( components_[i].get() ) )
			{
				index = i;
				break;	
			}
		}
		components_.erase( components_.begin() + index );
	}

	template<class Archive>
	inline void Entity::serialize(Archive &a_archive, uint32 const a_version) {
		switch(static_cast<Version>(a_version)) {
			case Version::LATEST:
			case Version::SELECTABLE:
#ifdef EDITOR
				a_archive(cereal::make_nvp("inspector_selectable", inspector_selectable_));
#endif // EDITOR
			case Version::TAGGING:
				a_archive(cereal::make_nvp("tags", tags_));
			case Version::HIERARCHY_VISIBLE:
				a_archive(cereal::make_nvp("hierarchy_visible", in_hierarchy_));
			case Version::DEFAULT:
				a_archive(cereal::make_nvp("name", name_));
				a_archive(cereal::make_nvp("enabled", is_enabled_));
				a_archive(cereal::make_nvp("tranform", transform_));
				a_archive(cereal::make_nvp("components", components_));
				break;
		}
	}

};

CEREAL_CLASS_VERSION(iga::Entity, (static_cast<iga::uint32>(iga::Entity::Version::LATEST) - 1));
