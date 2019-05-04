#include <Core/transform.h>
#include <Core/entity.h>

#ifdef EDITOR
#include <imgui.h>
#endif

#define FLAG_TO_UINT32(flag) static_cast<iga::uint32>(flag)

namespace iga {

	Transform::Transform() {
		// Initialize iga::Transform
		dirty_mask_ = 0b11111111;
		position_ = Vector3(0.f);
		rotation_ = Vector3(0.f);
		scale_ = Vector3(1.f, 1.f, 0.01f);
		image_aspect_ratio_ = 1.0f;
		aspect_scales_up_ = false;
	}

	Transform::Transform(std::weak_ptr<Entity> a_entity) :
		Transform() {
		// Initialize iga::Transform with iga::entity owner
		owner_ = a_entity;
	}

	Transform::~Transform() {
	
	}

	void Transform::OnLoad(std::shared_ptr<Transform> a_self) {
		CalculateTransform();
		for(std::weak_ptr<Transform> transform : children_) {
			if(std::shared_ptr<Transform> valid_transform = transform.lock()) {
				valid_transform->parent_ = a_self;
				valid_transform->OnLoad(valid_transform);

				if(std::shared_ptr<Entity> child_owner = valid_transform->owner_.lock()) {
					child_owner->has_parent_ = true;
				}
			}
		}
	}

	void Transform::GetTotalPosition(Vector3 & a_position, bool a_only_physics) {
		a_position += position_;
		if(std::shared_ptr<Transform> parent = parent_.lock()) {
			if(a_only_physics) {
				if(std::shared_ptr<Entity> parent_entity = parent->GetOwner().lock()) {
					if(parent_entity->has_physics_) {
						if(std::shared_ptr<Transform> parent_parent = parent->parent_.lock()) {
							if(std::shared_ptr<Entity> parent_parent_entity = parent_parent->GetOwner().lock()) {
								if(parent_parent_entity->has_physics_) {
									parent->GetTotalPosition(a_position, a_only_physics);
								}
							}
						}
					}
				}
			} else {
				parent->GetTotalPosition(a_position, a_only_physics);
			}
		}
	}

	const Vector3 Transform::GetWorldPosition() const {
		return world_position_.getXYZ();
	}

	const Vector3& Transform::GetPosition() const {
		return position_;
	}

    const Vector2 Transform::Get2DWorldPosition() const {
        return Vector2(world_position_.getX(), world_position_.getY());
    }

    const Vector2 Transform::Get2DPosition() const {
        return Vector2(position_.getX(), position_.getY());
    }

	float Transform::GetRotation() const {
		return rotation_.getZ();
	}

	float Transform::GetRotationX() const {
		return rotation_.getX();
	}

	float Transform::GetWorldRotation() const {
		return world_rotation_;
	}

	const Vector3& Transform::GetWorldScale() const {
		return world_scale_;
	}

	const Vector3& Transform::GetScale() const {
		return scale_;
	}

    const Vector2 Transform::Get2DWorldScale() const
    {
        return Vector2(world_scale_.getX(), world_scale_.getY());
    }

    const Vector2 Transform::Get2DScale() const
    {
        return Vector2(scale_.getX(), scale_.getY());
    }

	void Transform::SetPosition(const Vector3 &a_position) {
		// Set position and set dirty flag
		position_ = a_position;
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) | 
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) | 
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

    void Transform::SetPosition(const Vector2 & a_position)
    {
        // Set position and set dirty flag
        position_ = Vector3(a_position.getX(), a_position.getY(), position_.getZ());
        dirty_mask_ |=
            FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
            FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
            FLAG_TO_UINT32(DirtyFlags::TERRAIN);
    }

	void Transform::SetRotation(float a_rotation) {
		// Set rotation and set dirty flag
		rotation_.setZ(a_rotation);
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

	void Transform::SetRotationX(float a_rotation_x) {
		// Set rotation and set dirty flag
		rotation_.setX(a_rotation_x);
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM);
	}

	void Transform::SetScale(const Vector3 &a_scale) {
		// Set scale and set dirty flag
		scale_ = a_scale;
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_SIZE) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

    void Transform::SetScale(const Vector2 & a_scale)
    {
        // Set scale and set dirty flag
        scale_ = Vector3(a_scale.getX(), a_scale.getY(), scale_.getZ());
        dirty_mask_ |=
            FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
            FLAG_TO_UINT32(DirtyFlags::PHYSICS_SIZE) |
            FLAG_TO_UINT32(DirtyFlags::TERRAIN);
    }

	void Transform::SetDirty() {
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_SIZE) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

	void Transform::SetImageAspectRatio( float arg_aspect_ratio )
	{
		image_aspect_ratio_ = arg_aspect_ratio;
		dirty_mask_ |=
			FLAG_TO_UINT32( DirtyFlags::TRANSFORM ) |
			FLAG_TO_UINT32( DirtyFlags::PHYSICS_SIZE ) |
			FLAG_TO_UINT32( DirtyFlags::TERRAIN );
	}

	void Transform::Move(const Vector3 &a_distance) {
		// Add to position and set dirty flag
		position_ += a_distance;
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

    void Transform::Move(const Vector2 & a_distance)
    {
        // Add to position and set dirty flag
        position_ += Vector3(a_distance.getX(), a_distance.getY(), 0.0f);
        dirty_mask_ |=
            FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
            FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
            FLAG_TO_UINT32(DirtyFlags::TERRAIN);
    }

	void Transform::MoveX(float a_distance_x) {
		position_.setX(position_.getX() + a_distance_x);
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

	void Transform::MoveY(float a_distance_y) {
		position_.setY(position_.getY() + a_distance_y);
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

	void Transform::MoveZ(float a_distance_z) {
		position_.setZ(position_.getZ() + a_distance_z);
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

	void Transform::Rotate(float a_distance) {
		// Add to rotation and set dirty flag
		rotation_.setZ(rotation_.getZ() + a_distance);
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

	void Transform::RotateX(float a_distance_x) {
		// Add to rotation and set dirty flag
		rotation_.setX(rotation_.getX() + a_distance_x);
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM);
	}

	void Transform::Scale(const Vector3 &a_distance) {
		// Add to scale and set dirty flag
		scale_ += a_distance;
		dirty_mask_ |=
			FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
			FLAG_TO_UINT32(DirtyFlags::PHYSICS_SIZE) |
			FLAG_TO_UINT32(DirtyFlags::TERRAIN);
	}

    void Transform::Scale(const Vector2 & a_distance)
    {
        // Add to scale and set dirty flag
        scale_ += Vector3(a_distance.getX(), a_distance.getY(), 0.0f);
        dirty_mask_ |=
            FLAG_TO_UINT32(DirtyFlags::TRANSFORM) |
            FLAG_TO_UINT32(DirtyFlags::PHYSICS_SIZE) |
            FLAG_TO_UINT32(DirtyFlags::TERRAIN);
    }

    void Transform::Update() {
		// If parent is nullptr, then this is a root node
		if (!parent_.lock()) {
			// Calculate the world matrix for this Transform and traverse the tree
			CalculateTransformTree();
		}
	}

	void Transform::AddChild(const std::weak_ptr<Transform> a_child) {
		if (std::shared_ptr<Transform> child = a_child.lock()) {
			if (std::shared_ptr<Entity> owner = owner_.lock()) {
				// Set child's parent to this via pointer from Entity (otherwise must inherit from std::enable_shared_from_this)
				child->parent_ = owner->GetTransform();
				// Add child to vector
				children_.push_back(child);
				if(std::shared_ptr<Entity> child_owner = child->owner_.lock()) {
					child_owner->has_parent_ = true;
				}
			}
		}
	}

	void Transform::RemoveChild(std::weak_ptr<Transform> a_child) {
		std::shared_ptr<Transform> child;
		// Loop through children
		for (auto it = children_.begin(); it != children_.end(); ++it) {
			child = a_child.lock();
			// If the child has been found
			if (it->lock() == child) {
				// Set its parent to nullptr
				child->parent_ = std::weak_ptr<Transform>();
				// Remove it from the vector
				children_.erase(it);

				if(std::shared_ptr<Entity> child_owner = child->owner_.lock()) {
					child_owner->has_parent_ = false;
				}
				
				break;
			}
		}
	}

	const std::vector<std::weak_ptr<Transform>>& Transform::GetChildren() const {
		return children_;
	}

	const std::weak_ptr<Transform> Transform::GetParent() const {
		return parent_;
	}

	const std::weak_ptr<Entity> Transform::GetOwner() const {
		return owner_;
	}

	const Matrix4 & Transform::GetTransform() {
		return transform_;
	}

	float Transform::GetImageAspectRatio()
	{
		return image_aspect_ratio_;
	}

	Vector2 Transform::GetImageScaleCorrection() {
		Vector2 new_scale(1.0f, 1.0f);
		if (!aspect_scales_up_) {
			new_scale = Vector2(image_aspect_ratio_, 1.f);
		}
		else {
			if (image_aspect_ratio_ < 1.f) {
				new_scale = Vector2(1.f, 1.f / image_aspect_ratio_);
			}
			else {
				new_scale = Vector2(image_aspect_ratio_, 1.f);
			}
		}

		return new_scale;
	}

	void Transform::Inspect() {
#ifdef EDITOR
		if (ImGui::TreeNode("Transform")) {
			float position[3] = { position_.getX(), position_.getY(), position_.getZ() };
			float rotation[3] = { rotation_.getX(), 0.f, rotation_.getZ() };
			float scale[3] = { scale_.getX(), scale_.getY(), scale_.getZ() };

			ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.7f);
			ImGui::DragFloat3("Position", position, 0.1f, -10000.f, 10000.f);
			ImGui::DragFloat3("Rotation##rotation_xyz", rotation, 1.f, FLOAT32_MIN, FLOAT32_MAX);
			ImGui::DragFloat3("Scale", scale, 0.1f, -1000.f, 1000.f);
			ImGui::PopItemWidth();

			Vector3 new_position = Vector3(position[0], position[1], position[2]);
			if (new_position.getX() != position_.getX() ||
				new_position.getY() != position_.getY() ||
				new_position.getZ() != position_.getZ() ) {
				SetPosition(new_position);
			}

			if(rotation[0] != rotation_.getX()) {
				SetRotationX(rotation[0]);
			}

			if (rotation[2] != rotation_.getZ()) {
				SetRotation(rotation[2]);
			}

			Vector3 new_scale = Vector3(scale[0], scale[1], scale[2]);
			if (new_scale.getX() != scale_.getX() ||
				new_scale.getY() != scale_.getY() ||
				new_scale.getZ() != scale_.getZ()) {
				SetScale(new_scale);
			}

			ImGui::Checkbox("Aspect Ratio Scales Up", &aspect_scales_up_);

			ImGui::TreePop();
		}
#endif
	}

	void Transform::GetTotalRotation(float & a_rotation) {
		a_rotation += rotation_.getZ();
		if(std::shared_ptr<Transform> parent = parent_.lock()) {
			parent->GetTotalRotation(a_rotation);
		}
	}

	void Transform::GetTotalScale(Vector3 & a_scale) {
		a_scale = mulPerElem(scale_, a_scale);
		if(std::shared_ptr<Transform> parent = parent_.lock()) {
			parent->GetTotalScale(a_scale);
		}
	}

	void Transform::GetTotalProperties(Vector3 & a_position, Vector3 & a_scale, float & a_rotation, bool a_only_physics) {
		a_position += position_;
		a_rotation += rotation_.getZ();
		a_scale = mulPerElem(scale_, a_scale);
		if(std::shared_ptr<Transform> parent = parent_.lock()) {
			if(a_only_physics) {
				if(std::shared_ptr<Entity> parent_entity = parent->GetOwner().lock()) {
					if(parent_entity->has_physics_) {
						if(std::shared_ptr<Transform> parent_parent = parent->parent_.lock()) {
							if(std::shared_ptr<Entity> parent_parent_entity = parent_parent->GetOwner().lock()) {
								if(parent_parent_entity->has_physics_) {
									parent->GetTotalProperties(a_position, a_scale, a_rotation, a_only_physics);
								}
							}
						}
					}
				}
			} else {
				parent->GetTotalProperties(a_position, a_scale, a_rotation, a_only_physics);
			}
		}
	}

	void Transform::CalculateTransformTree() {
		bool set_children_dirty = false;

		// If Transform is dirty
		if (dirty_mask_ & static_cast<uint8>(DirtyFlags::TRANSFORM)) {
			// Re-calculate world matrix
			CalculateTransform();

			// Make sure children are re-calculated
			set_children_dirty = true;
		}

		// For each child
		for (auto it = children_.begin(); it != children_.end(); ++it) {
			// If child is valid
			if (std::shared_ptr<Transform> child = it->lock()) {
				// Set child dirty if necessary
				if (set_children_dirty) {
					child->dirty_mask_ |= dirty_mask_;
					if(owner_.lock()->has_physics_ && dirty_mask_ & static_cast<uint8>(DirtyFlags::PHYSICS_TRANSFORM)) {
						child->dirty_mask_ &= ~(static_cast<uint8>(Transform::DirtyFlags::PHYSICS_TRANSFORM));
					}
				}

				// Recursively calculate world matrices of children
				child->CalculateTransformTree();
			}
		}

		dirty_mask_ &= ~(static_cast<uint8>(DirtyFlags::TRANSFORM));
	}

	void Transform::CalculateTransform() {
		// If there is a parent, multiply by it's world matrix to get this Transform's world matrix
		if (std::shared_ptr<Transform> parent = parent_.lock()) {
			// Get world position
			world_position_ = parent->transform_ * Vector4(position_.getX(), position_.getY(), position_.getZ(), 1.0f);

			// Get world rotation
			float total_rotation = 0.f;
			GetTotalRotation(total_rotation);
			world_rotation_ = total_rotation;

			// Get world scale
			Vector3 total_scale(1.f);
			GetTotalScale(total_scale);
			world_scale_ = total_scale;
		} else {
			world_position_ = Vector4(position_.getX(), position_.getY(), position_.getZ(), 1.0f);
			world_rotation_ = rotation_.getZ();
			world_scale_ = scale_;
		}

		// Create transform matrix
		transform_ =
			Matrix4::translation(world_position_.getXYZ()) *
			Matrix4::rotationZ(DEG_TO_RAD(world_rotation_)) *
			Matrix4::rotationX(DEG_TO_RAD(rotation_.getX())) *
			Matrix4::scale(world_scale_);
	}

}