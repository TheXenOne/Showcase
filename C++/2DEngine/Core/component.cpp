#include <Core/component.h>

#include <utility>

#ifdef EDITOR
std::map<std::string, detail::constructLambda>* StaticComponentVector::component_vector_ = component_vector_ == nullptr ? new std::map<std::string, detail::constructLambda>() : component_vector_;
#endif // EDITOR

namespace iga {

	Component::Component() {
		enabled_ = true;
	}

	Component::Component(std::weak_ptr<Entity> a_entity) {
		owner_ = a_entity;
		enabled_ = true;
	}

	Component::Component(Component&& a_component) {
		owner_ = a_component.owner_;
		enabled_ = a_component.enabled_;
	}

	Component& Component::operator=(Component&& a_component) {
		if (this != &a_component) {
			owner_ = std::move(a_component.owner_);
			enabled_ = a_component.enabled_;
		}

		return *this;
	}

	Component::~Component() {

	}

	std::weak_ptr<Entity> Component::GetOwner() const {
		return owner_;
	}

	const bool Component::IsEnabled() const {
		return enabled_;
	}

	void Component::SetEnabled(const bool a_enabled) {
		enabled_ = a_enabled;
	}

	const std::string & Component::GetTypeName() const {
		return component_type_;
	}

	void Component::Inspect() {
#ifdef EDITOR
#endif
	}

}