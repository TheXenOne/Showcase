#include <Core\entity.h>
#include <Core\component.h>
#include <Core\transform.h>

#ifdef EDITOR
#include <imgui.h>
#endif

#include <utility>

namespace iga {

	size_t Entity::entity_number_ = 0;


	Entity::Entity() {

		// Set name so entity can be selected in Hierarchy if no name is set by user
		name_ = "Entity" + std::to_string(entity_number_);
		id_ = ++entity_number_;

#ifdef EDITOR
		in_hierarchy_ = true;
		inspector_selectable_ = true;
#endif

		has_parent_ = false;
		has_physics_ = false;

	}

	Entity::Entity(std::weak_ptr<Scene> a_scene) :
		Entity() {
		scene_ = a_scene;
		is_enabled_ = true;
		id_ = ++entity_number_;

		has_parent_ = false;
		has_physics_ = false;
	}

	Entity::~Entity() {

	}

	Entity::Entity(Entity && arg_entity) {
		id_ = std::move(arg_entity.id_);
		name_ = std::move(arg_entity.name_);
		is_enabled_ = std::move(arg_entity.is_enabled_);

		scene_ = std::move(arg_entity.scene_);
		transform_ = std::move(arg_entity.transform_);

		components_ = std::move(arg_entity.components_);
	}

	Entity& Entity::operator=(Entity && arg_entity) {
		if (this != &arg_entity) {
			id_ = std::move(arg_entity.id_);
			name_ = std::move(arg_entity.name_);
			is_enabled_ = std::move(arg_entity.is_enabled_);

			scene_ = std::move(arg_entity.scene_);
			transform_ = std::move(arg_entity.transform_);

			components_ = std::move(arg_entity.components_);
		}
		return *this;
	}

	void Entity::OnCreate(bool a_on_load) {
		if (a_on_load) {
			transform_->OnLoad(transform_);
			for (std::shared_ptr<Component> component : components_) {
				component->owner_ = self_;
				component->OnCreate(true);
			}
		}
		else {
			transform_ = std::shared_ptr<Transform>(new Transform(self_));
		}
	}

	void Entity::Update() {
		if (is_enabled_) {
			for (size_t i = 0, size = components_.size(); i < size; i++) {
				components_[i]->Update();
			}
			transform_->Update();
		}
	}

	void Entity::PostUpdate() {
		if (is_enabled_) {
			for (size_t i = 0, size = components_.size(); i < size; i++) {
				components_[i]->PostUpdate();
			}
		}
	}

	const size_t Entity::GetID() const {
		return id_;
	}

	const std::string & Entity::GetName() const {
		return name_;
	}

	void Entity::SetName(const std::string & a_name) {
		name_ = a_name;
	}

	const bool Entity::GetEnabled() const {
		return is_enabled_;
	}

	void Entity::SetEnabled(bool a_enabled) {
		is_enabled_ = a_enabled;
	}

	const BitFlag Entity::GetAllTags()
	{
		return tags_;
	}

	void Entity::SetAllTags(BitFlag a_tags)
	{
		tags_ = a_tags;
	}

	const bool Entity::HasTag(BitFlag a_tag)
	{
		return tags_.HasAny(a_tag);
	}

	void Entity::SetTag(BitFlag a_tag)
	{
		tags_.Set(a_tag);
	}

	bool Entity::HasParent() const {
		return has_parent_;
	}

	bool Entity::HasPhysics() const {
		return has_physics_;
	}

	std::weak_ptr<Scene> Entity::GetScene() const {
		return scene_;
	}

	std::weak_ptr<Transform> Entity::GetTransform() const {
		return transform_;
	}

#ifdef EDITOR
	void Entity::Inspect() {
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.7f);
		iga::int32 t = tags_.Get();
		ImGui::InputInt("Entity Tags", (int*)&t);
		tags_ = t;
		ImGui::Separator();
		// Transform
		transform_->Inspect();
		ImGui::Separator();
		size_t index_remove_component = 0;
		bool delete_is_pressed = false;
		// Components
		for (int i = 0; i < components_.size(); ++i) {
			if (ImGui::TreeNode(components_[i]->GetTypeName().c_str())) {
				components_[i]->Inspect();

				if (ImGui::Button("Delete", ImVec2(50, ImGui::GetTextLineHeightWithSpacing()))) {
					index_remove_component = i;
					delete_is_pressed = true;
				}
				if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
					ImGui::SetTooltip("Removes the component from this entity and deletes it.");
				}
				ImGui::TreePop();
			}

			ImGui::Separator();
		}

		ImGui::Checkbox("Is Selectable with Mouse", &inspector_selectable_);

		if (delete_is_pressed) {
			components_.erase(components_.begin() + index_remove_component);
		}
		
	}

	void Entity::SetVisibleInHierarchy(bool a_in_hierarchy) {
		in_hierarchy_ = a_in_hierarchy;
	}

	bool Entity::IsVisibleInHierarchy() {
		return in_hierarchy_;
	}

	bool Entity::IsInspectorSelectable() {
		return inspector_selectable_;
	}

	void Entity::SetInspectorSelectable(bool a_inspector_selectable) {
		inspector_selectable_ = a_inspector_selectable;
	}
#else
	void Entity::Inspect() {}

	void Entity::SetVisibleInHierarchy(bool a_in_hierarchy) {
		UNUSED(a_in_hierarchy); // Unused variable for RELEASE Build.
	}

	bool Entity::IsVisibleInHierarchy() {
		return false;
	}

	void Entity::SetInspectorSelectable(bool a_inspector_selectable) {
		UNUSED(a_inspector_selectable);
	}

	bool Entity::IsInspectorSelectable() {
		return false;
	}

#endif


};
