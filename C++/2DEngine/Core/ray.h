#pragma once

#include <defines.h>

#include <vectormath/vectormath.hpp>

#include <memory>

/**
* @file ray.h
* @author Ben Landor
* @date 20 mar 2018
* @brief iga::Raycast class header.
*
* iga::Raycast provides functions for raycasting.
* iga::Ray provides a container for the data required to cast a ray.
*
*/

/**
*  IGArt namespace
*/
namespace iga {

	class Entity;
	class Scene;

	/**
	*  iga::Ray contains ray data.
	*/
	struct Ray {
		Vector3 origin; /**< Origin of the ray on the near z plane. */
		Vector3 end; /**< End of the ray on the far z plane. */
		Vector3 direction; /**< Direction the ray is being cast in. */
		float length; /**< Length of the ray. */
	};

	/**
	*  iga::Raycast provides functions for raycasting.
	*/
	class Raycast {
	public:
		/**
		*  Gets the closest iga::Entity hit by the given ray.
		*  @param a_ray is the ray being cast.
		*  @return A weak pointer to the iga::Entity hit, will be invalid if no hit.
		*  @see iga::Scene.
		*  @see iga::CameraComponent.
		*  @see iga::Entity.
		*/
		std::weak_ptr<Entity> GetEntityHitByRay(Ray a_ray);

		/**
		*  Gets the result of an AABB-Ray intersection test.
		*  @param a_ray is the ray being cast.
		*  @param a_box_min is the minimum bound of the box.
		*  @param a_box_max is the maximum bound of the box.
		*  @param a_t_out is the distance at which the collision occurs.
		*  @return A bool specifying if there was a collision.
		*/
		static bool AABBRayIntersect(Ray a_ray, Vector3 a_box_min, Vector3 a_box_max, float *a_t_out = nullptr);

		/**
		*  Gets the result of an OBB-Ray intersection test.
		*  @param a_ray is the ray being cast.
		*  @param a_box_min is the minimum bound of the box in local space.
		*  @param a_box_max is the maximum bound of the box in local space.
		*  @param a_entity is the entity whose transform will be used for the intersection test.
		*  @param a_t_out is the distance at which the collision occurs.
		*  @return A bool specifying if there was a collision.
		*/
		static bool OBBRayIntersect(Ray a_ray, Vector3 a_box_min, Vector3 a_box_max, std::weak_ptr<Entity> a_entity, float *a_t_out = nullptr);

		/**
		*  Converts a screen position to a ray, based on the currently active camera.
		*  @param a_screen_pos_x is the x position to be converted to a ray.
		*  @param a_screen_pos_y is the y position to be converted to a ray.
		*  @param a_screen_width is the width of the screen.
		*  @param a_screen_height is the height of the screen.
		*  @param a_ray_out is the ray returned from the function.
		*  @return A bool specifying if there was the conversion was successful.
		*  @see iga::CameraComponent.
		*  @see iga::Entity.
		*  @see iga::Transform.
		*/
		const bool ScreenPointToRay(float a_screen_pos_x, float a_screen_pos_y, float a_screen_width, float a_screen_height, Ray& a_ray_out);
	private:
		/**
		*  Recursively raycasts against all entities in each scene.
		*  @param a_ray is the ray being cast.
		*  @param a_distance_sqr is the square distance of the entity hit.
		*  @param a_scene is a shared pointer to the scene being traversed.
		*  @return A weak pointer to the iga::Entity hit, will be invalid if no hit.
		*  @see iga::Scene.
		*  @see iga::Entity.
		*  @see iga::Transform.
		*/
		std::weak_ptr<Entity> GetEntityHitByRayInTree(Ray a_ray, float *a_distance_sqr, std::shared_ptr<Scene> a_scene, std::shared_ptr<Entity> a_current_camera_entity);
	};
}
