#include <Core/ray.h>
#include <Core/scene.h>
#include <Graphics/sprite_component.h>
#include <ig_art_engine.h>
#include <algorithm>

#ifdef _WIN32
#undef max
#undef min
#endif // _WIN32


std::weak_ptr<iga::Entity> iga::Raycast::GetEntityHitByRay(Ray a_ray) {
	// Get main scene
	if (std::shared_ptr<Scene> scene = GetGame()->GetCurrentScene().lock()) {
		// Get camera
		if (std::shared_ptr<CameraComponent> camera = GetRenderer()->GetActiveCamera().lock()) {
			// Get the entity that owns the camera
			if (std::shared_ptr<Entity> camera_entity = camera->GetOwner().lock()) {
				// Set distance to closest selected entity to highest number
				float closest_distance_sqr = FLOAT32_MAX;
				// Recursively walk the tree
				std::weak_ptr<Entity> entity = GetEntityHitByRayInTree(a_ray, &closest_distance_sqr, scene, camera_entity);

				// Return entity hit or nullptr
				return entity;
			}
		}
	}

	// Couldn't get main scene so return nullptr
	return std::weak_ptr<Entity>();
}

bool iga::Raycast::AABBRayIntersect(Ray a_ray, Vector3 a_box_min, Vector3 a_box_max, float *a_t_out) {
	// Inverse of the ray direction (1 / dir)
	Vector3 inverse_dir;

	// Set the inverse direction
	inverse_dir.setX(1.f / a_ray.direction.getX());
	inverse_dir.setY(1.f / a_ray.direction.getY());
	inverse_dir.setZ(1.f / a_ray.direction.getZ());

	float t1 = (a_box_min.getX() - a_ray.origin.getX()) * inverse_dir.getX();
	float t2 = (a_box_max.getX() - a_ray.origin.getX()) * inverse_dir.getX();
	float t3 = (a_box_min.getY() - a_ray.origin.getY()) * inverse_dir.getY();
	float t4 = (a_box_max.getY() - a_ray.origin.getY()) * inverse_dir.getY();
	float t5 = (a_box_min.getZ() - a_ray.origin.getZ()) * inverse_dir.getZ();
	float t6 = (a_box_max.getZ() - a_ray.origin.getZ()) * inverse_dir.getZ();

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0) {
		if (a_t_out) {
			*a_t_out = tmax;
		}
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) {
		if (a_t_out) {
			*a_t_out = tmax;
		}
		return false;
	}

	if (a_t_out) {
		*a_t_out = tmin;
	}
	return true;
}

bool iga::Raycast::OBBRayIntersect(Ray a_ray, Vector3 a_box_min, Vector3 a_box_max, std::weak_ptr<Entity> a_entity, float * a_t_out) {
	if (auto entity = a_entity.lock()) {
		if (auto transform = entity->GetTransform().lock()) {
			Ray ray;
			Matrix4 inverse_transform;
			inverse_transform = transform->transform_;
			inverse_transform = inverse(inverse_transform);
			
			for (size_t i = 0; i < 4; ++i) {
				for (size_t j = 0; j < 4; ++j) {
					if (isnan(inverse_transform.getElem(static_cast<int>(i), static_cast<int>(j)))) {
						return false;
					}
				}
			}

			Vector4 origin = inverse_transform * Vector4(a_ray.origin, 1.f);
			ray.origin = origin.getXYZ();
			Vector4 end = inverse_transform * Vector4(a_ray.end, 1.f);
			ray.end = end.getXYZ();
			Vector4 direction = end - origin;
			ray.direction = normalize(direction.getXYZ());
			ray.length = length(direction.getXYZ());

			if (AABBRayIntersect(ray, a_box_min, a_box_max, a_t_out)) {
				Vector4 intersection = Vector4(ray.origin + (ray.direction * (*a_t_out)), 1.f);
				*a_t_out = length(transform->transform_ * intersection);
				*a_t_out *= length(transform->scale_);
				return true;
			}
		}
	}

	return false;
}

const bool iga::Raycast::ScreenPointToRay(float a_screen_pos_x, float a_screen_pos_y, float a_screen_width, float a_screen_height, Ray& a_ray_out) {
	// Get the camera
	if (std::shared_ptr<CameraComponent> camera = GetRenderer()->GetActiveCamera().lock()) {
		if (std::shared_ptr<Entity> camera_entity = camera->GetOwner().lock()) {
			if (std::shared_ptr<Transform> camera_transform = camera_entity->GetTransform().lock()) {
				// NDC coordinates - from (-1, -1) at bottom left, to (1, 1) at top right
				Vector4 input;
				input.setX((a_screen_pos_x / a_screen_width * 2.f) - 1.f);
				input.setY(-1.f * ((a_screen_pos_y / a_screen_height * 2.f) - 1.f));
				// Set Z to 0 for point on near plane
				input.setZ(0.f);
				input.setW(1.f);

				Matrix4 camera_view = camera->GetView();
				// Inverse view projection matrix
				Matrix4 inverse_view_projection = inverse(camera->GetProjection() * camera_view);
				// Multiply input by inverse view projection to get world coordinates
				Vector4 out = inverse_view_projection * input;
				if (out.getW() == 0.f) {
					return false;
				}

				// Divide by 1 / W 
				out.setW(1.f / out.getW());
				a_ray_out.origin.setX(out.getX() * out.getW());
				a_ray_out.origin.setY(out.getY() * out.getW());
				a_ray_out.origin.setZ(out.getZ() * out.getW());

				// Set Z to 1 for point on far plane
				input.setZ(1.f);
				// Multiply input by inverse view projection to get world coordinates
				out = inverse_view_projection * input;
				if (out.getW() == 0.f) {
					return false;
				}

				// Divide by 1 / W 
				out.setW(1.f / out.getW());
				a_ray_out.end.setX(out.getX() * out.getW());
				a_ray_out.end.setY(out.getY() * out.getW());
				a_ray_out.end.setZ(out.getZ() * out.getW());

				// Get ray direction as vector
				a_ray_out.direction = a_ray_out.end - a_ray_out.origin;
				// Get the length of the direction vector
				a_ray_out.length = length(a_ray_out.direction);
				// Normalize the direction so it is a unit vector
				a_ray_out.direction = normalize(a_ray_out.direction);

				return true;
			}
		}
	}

	// If the camera is null return false
	return false;
}

std::weak_ptr<iga::Entity> iga::Raycast::GetEntityHitByRayInTree(Ray a_ray, float * a_distance_sqr, std::shared_ptr<Scene> a_scene, std::shared_ptr<Entity> a_current_camera_entity) {
	std::weak_ptr<Entity> closest_selected_entity = std::weak_ptr<Entity>();
	Vector3 box_min, box_max, position, scale, scale_half;
	// Set distance to closest selected entity to highest number
	float closest_distance_sqr = FLOAT32_MAX;
	float ray_length_squared = std::powf(a_ray.length, 2);

	// Loop through all scenes
	for (int i = 0; i < a_scene->scene_vector_.size(); ++i) {
		// Walk the tree of scenes
		// If a child scene returned a valid entity that was hit
		if (std::shared_ptr<Entity> entity = GetEntityHitByRayInTree(a_ray, &closest_distance_sqr, a_scene->scene_vector_[i], a_current_camera_entity).lock()) {
			// If the distance is less than the current closest distance
			if (closest_distance_sqr < *a_distance_sqr) {
				// Set the closest entity to this
				closest_selected_entity = entity;
				// Set the closest distance to this
				*a_distance_sqr = closest_distance_sqr;
			}
		}
	}

	// Loop through all entities in this scene
	for (int i = 0; i < a_scene->entity_vector_.size(); ++i) {
		// Only check for entity hit if this entity is not the current camera
		// Otherwise this will always hit closest since the view is from the current camera
		if (a_scene->entity_vector_[i] != a_current_camera_entity && a_scene->entity_vector_[i]->IsInspectorSelectable()) {
			// Get the transform
			if (std::shared_ptr<Transform> transform = a_scene->entity_vector_[i]->GetTransform().lock()) {
				position = transform->GetWorldPosition();
				Vector2 scale_correction = transform->GetImageScaleCorrection();
				scale = Vector3(scale_correction.getX(), scale_correction.getY(), 1.f);
				scale_half.setX(scale.getX() * 0.5f);
				scale_half.setY(scale.getY() * 0.5f);
				scale_half.setZ(scale.getZ() * 0.5f);

				// Calculate box bounds
				box_min = Vector3(-scale_half.getX(), -scale_half.getY(), -scale_half.getZ());
				box_max = Vector3(scale_half.getX(), scale_half.getY(), scale_half.getZ());

				float intersection_dist;
				if (OBBRayIntersect(a_ray, box_min, box_max, a_scene->entity_vector_[i], &intersection_dist)) {
					Vector3 intersection = a_ray.origin + (a_ray.direction * intersection_dist);
					// Store the distance between the rays origin and the position of the transform
					float distance_sqr = lengthSqr(a_ray.origin - intersection);

					//Check if the hit is in range of the length of the ray.
					// If this distance is less than the current shortest distance
					if (distance_sqr < ray_length_squared && distance_sqr < *a_distance_sqr) {
						//Set the closest entity to this
						closest_selected_entity = a_scene->entity_vector_[i];
						//Set the closest distance to this
						*a_distance_sqr = distance_sqr;
					}
				}
			}
		}
	}
	// Return Entity or nullptr
	return closest_selected_entity;
}
