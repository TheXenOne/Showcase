#pragma once

#include <defines.h>

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

/**
* @file component.h
* @author Meine Zeinstra
* @author Ben Landor
* @date 27 feb 2018
* @brief iga::Component class header.
*
* iga::Component is a base class that holds modular functionality, that can be contained in an iga::Entity.
* Sub-classes must implement the pure virtual functions OnCreate, OnUpdate, and PostUpdate.
*/

/**
*  IGArt namespace
*/
namespace iga {

	class Entity;
	class BaseRenderingComponent;
	class CameraComponent;
	class SpriteComponent;
	class PhysicsBodyComponent;

	/**
	*  iga::Component is an object holding modular functionality, that can be contained in an iga::Entity.
	*/
	class Component {

		friend Entity;

	public:
		/**
		*   Cereal versions of iga::Component.
		*/
		enum class Version {
			DEFAULT = 0, /**< Default normal version. */
			LATEST /**< Latest version. */
		};

		/**
		*  Default constructor.
		*/
		Component();
		/**
		*  Custom constructor with iga::entity that will own the component.
		*  @param a_entity is a weak pointer to an iga::Entity that is to be the owner of this component.
		*/
		Component(std::weak_ptr<Entity> a_entity);
		/**
		*  Removes the copy constructor.
		*/
		Component(Component& a_component) = delete;
		/**
		*  Move constructor.
		*/
		Component(Component&& a_component);
		/**
		*  Move assignment operator.
		*/
		Component& operator=(Component&& a_component);
		/**
		*  Virtual destructor.
		*/
		virtual ~Component();

		/**
		*  A pure virtual function that is called when the component is first created.
		*  @param a_on_load Wether the component is loaded by a file or created in run-time. True is loaded by a file. False (default value) is created in run-time.
		*/
		virtual void OnCreate(bool a_on_load = false) = 0;
		/**
		*  A pure virtual function that is called once every frame.
		*/
		virtual void Update() = 0;
		/**
		*  A pure virtual function that is called once every frame, after the main iga::Component::Update.
		*/
		virtual void PostUpdate() = 0;

		/**
		*  Gets this iga::Component's owner.
		*  @return A weak pointer to the iga::Entity that owns this.
		*/
		std::weak_ptr<Entity> GetOwner() const;

		/**
		*  Gets whether this iga::Component is enabled.
		*  @return A bool signifying whether this iga::Component is enabled.
		*/
		const bool IsEnabled() const;
		/**
		*  Sets whether this iga::Component is enabled.
		*  @param a_enabled is a const bool signifying whether this iga::Component is enabled.
		*/
		void SetEnabled(const bool a_enabled);
		/**
		*  Gets whether the type name of this Component.
		*  @return A bool specifying the type name of this Component.
		*/
		const std::string& GetTypeName() const;

		/**
		*  Used to show information in the editor : Inspector
		*/
		virtual void Inspect() /*override*/ = 0;

		/**
		*  Serializes this component for saving to disk.
		*/
		template<class Archive>
		void serialize(Archive &a_archive, uint32 const a_version);

	protected:
		std::string component_type_; /**< A string containing the name of the component type. */
		std::weak_ptr<Entity> owner_; /**< Weak pointer to the iga::Entity that owns this. */
		bool enabled_; /**< A bool defining whether the component is enabled/active. */

	private:

	};

	template<class Archive>
	inline void Component::serialize(Archive &a_archive, uint32 const a_version) {
		switch(static_cast<Version>(a_version)) {
			case Version::LATEST:
			case Version::DEFAULT:
				a_archive(cereal::make_nvp("enabled", enabled_));
				a_archive(cereal::make_nvp("component_name", component_type_));
				break;
		}
	}

}

CEREAL_CLASS_VERSION(iga::Component, (static_cast<iga::uint32>(iga::Component::Version::LATEST) - 1));


namespace detail
{
	typedef void ( *constructLambda )( iga::Entity* );
	typedef std::pair<std::string, constructLambda> NameComponentPair;
}

class StaticComponentVector
{
public:
	static std::map<std::string, detail::constructLambda>* component_vector_;
};

#ifdef EDITOR
#define SUBSCRIBECOMPONENT(classArg)																																						\
namespace classArg##classArg{																																								\
template <typename C> struct SubStruct																																						\
{																																															\
	SubStruct();																																											\
};																																															\
template <typename C> SubStruct<C>::SubStruct()																																				\
{																																															\
	if( StaticComponentVector::component_vector_ == nullptr )																																\
	{																																														\
		StaticComponentVector::component_vector_ = new std::map<std::string, ::detail::constructLambda>();																					\
	}																																														\
	StaticComponentVector::component_vector_->insert( std::make_pair( std::string( #classArg ), []( iga::Entity* arg_entity ) {	arg_entity->NewComponent<classArg>();} ) );					\
}																																															\
static SubStruct<classArg> s;																																								\
}
#else
#define SUBSCRIBECOMPONENT(argClass)
#endif // EDITOR
