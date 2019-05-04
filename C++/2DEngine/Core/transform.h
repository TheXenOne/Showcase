#pragma once

#include <defines.h>
#include <vectormath\vectormath.hpp>

#include <memory>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

/**
* @file transform.h
* @author Ben Landor
* @author Meine Zeinstra
* @date 1 mar 2018
* @brief iga::Transform class header.
*
* This file defines the iga::Transform class.
*
*/

/**
*  IGArt namespace
*/
namespace iga {

	class Entity;
	namespace tt {
		class TerrainComponent;
		class TerrainNodeComponent;
	}

	/**
	*  iga::Transform class. Contains position, rotation, scale and transform matrix
	*  Only updates transform when position, rotation or scale has been editted.
	*/
	class Transform final {

		friend class Entity;
		friend class PhysicsBodyComponent;
		friend class Raycast;
		friend class tt::TerrainComponent;
		friend class tt::TerrainNodeComponent;
#ifdef EDITOR
		friend class Inspector;
#endif // EDITOR

	public:
		/**
		*   Cereal versions of iga::Transform
		*/
		enum class Version {
			DEFAULT = 0, /**< Default normal version. */
			X_ROTATION, /**< Added support for x rotation */
			ASPECT,
			LATEST /**< Latest version. */
		};
		enum class DirtyFlags
		{
			TRANSFORM = 1 << 0,
			PHYSICS_SIZE = 1 << 1,
			PHYSICS_TRANSFORM = 1 << 2,
			TERRAIN = 1 << 3
		};

		/**
		*  Constructor.
		*/
		Transform();
		/**
		*  Constructor
		*  @param a_entity The owner of the iga::Transform
		*  @see iga::Entity
		*/
		Transform(std::weak_ptr<Entity> a_entity);
		/**
		*  Destructor.
		*/
		~Transform();

		/**
		*  Gets position of the iga::Transform relative to the world
		*  @return The world position of the iga::Transform as a const Vector3
		*/
		const Vector3 GetWorldPosition() const;
		/**
		*  Gets position of the iga::Transform
		*  @return The position of the iga::Transform as a const Vector3 reference
		*/
		const Vector3& GetPosition() const;
        	/**
        	*  Gets 2D position of the iga::Transform relative to the world
        	*  @return The world position of the iga::Transform as a const Vector2
        	*/
        	const Vector2 Get2DWorldPosition() const;
        	/**
        	*  Gets 2D position of the iga::Transform
        	*  @return The position of the iga::Transform as a const Vector2
        	*/
        	const Vector2 Get2DPosition() const;
		/**
		*  Gets z rotation of the iga::Transform
		*  @return The rotation of the iga::Transform as a float
		*/
		float GetRotation() const;
		/**
		*  Gets x rotation of the iga::Transform
		*  @return The x rotation of the iga::Transform as a float
		*/
		float GetRotationX() const;
		/**
		*  Gets the world z rotation of the iga::Transform
		*  @return The world rotation of the iga::Transform as a float
		*/
		float GetWorldRotation() const;
		/**
		*  Gets scale of the iga::Transform relative to the world
		*  @return The world scale of the iga::Transform as a const Vector3 reference
		*/
		const Vector3& GetWorldScale() const;
		/**
		*  Gets scale of the iga::Transform
		*  @return The scale of the iga::Transform as a const Vector3 reference
		*/
		const Vector3& GetScale() const;
        	/**
        	*  Gets 2D scale of the iga::Transform relative to the world
        	*  @return The world scale of the iga::Transform as a const Vector2
        	*/
        	const Vector2 Get2DWorldScale() const;
        	/**
        	*  Gets 2D scale of the iga::Transform
        	*  @return The scale of the iga::Transform as a const Vector2
        	*/
        	const Vector2 Get2DScale() const;
		
		/**
		*  Sets position of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_position New position for the iga::Transform as a const Vector3 reference.
		*/
		void SetPosition(const Vector3& a_position);
        	/**
        	*  Sets 2D position of the iga::Transform.
        	*  Sets dirty flag to true.
        	*  @param a_position New position for the iga::Transform as a const Vector2 reference.
        	*/
        	void SetPosition(const Vector2& a_position);
		/**
		*  Sets the z rotation of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_rotation New z rotation for the iga::Transform as a float .
		*/
		void SetRotation(float a_rotation);
		/**
		*  Sets the x rotation of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_rotation New x rotation for the iga::Transform as a float .
		*/
		void SetRotationX(float a_rotation_x);
		/**
		*  Sets scale of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_scale New scale for the iga::Transform as a const Vector3 reference.
		*/
		void SetScale(const Vector3& a_scale);
        	/**
        	*  Sets 2D scale of the iga::Transform.
        	*  Sets dirty flag to true.
        	*  @param a_scale New scale for the iga::Transform as a const Vector2 reference.
        	*/
		void SetScale(const Vector2& a_scale);

		/**
		*  Sets the transform to dirty.
		*/
		void SetDirty();

		/**
		* Set image aspect ratio.
		* @param aspect ratio.
		*/
		void SetImageAspectRatio( float arg_aspect_ratio );

		/**
		*  Adds given distance to the position of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_distance Vector3 to add to the position of the iga::Transform.
		*/
		void Move(const Vector3& a_distance);
        	/**
        	*  Adds given distance to the 2D position of the iga::Transform.
        	*  Sets dirty flag to true.
        	*  @param a_distance Vector2 to add to the position of the iga::Transform.
        	*/
        	void Move(const Vector2& a_distance);
		/**
		*  Adds given distance to the x position of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_distance_x A float to add to the x position of the iga::Transform.
		*/
		void MoveX(float a_distance_x);
		/**
		*  Adds given distance to the y position of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_distance_y A float to add to the y position of the iga::Transform.
		*/
		void MoveY(float a_distance_y);
		/**
		*  Adds given distance to the z position of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_distance_z A float to add to the z position of the iga::Transform.
		*/
		void MoveZ(float a_distance_z);
		/**
		*  Adds given z rotation to the current z rotation of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_distance float to add to the z rotation of the iga::Transform.
		*/
		void Rotate(float a_distance);
		/**
		*  Adds given x rotation to the current x rotation of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_distance float to add to the x rotation of the iga::Transform.
		*/
		void RotateX(float a_distance_x);
		/**
		*  Adds given scale to the current scale of the iga::Transform.
		*  Sets dirty flag to true.
		*  @param a_distance Vector3 to add to the scale of the iga::Transform.
		*/
		void Scale(const Vector3& a_distance);
        	/**
        	*  Adds given 2D scale to the current scale of the iga::Transform.
        	*  Sets dirty flag to true.
        	*  @param a_distance Vector2 to add to the scale of the iga::Transform.
        	*/
        	void Scale(const Vector2& a_distance);

		/**
		*  Updates the transform matrix of the iga::Transform if the dirty flag has been set.
		*/
		void Update();

		/**
		*  Adds a child of this transform. Also sets the child's parent to this.
		*  @param a_child is the child Transform to be added.
		*/
		void AddChild(const std::weak_ptr<Transform> a_child);
		/**
		*  Removes a child from this Transform. Also sets the parent of the child to nullptr.
		*  @param a_child is the child Transform to be removed.
		*/
		void RemoveChild(std::weak_ptr<Transform> a_child);
		/**
		*  Returns a the children of this Transform.
		*  @return A vector of weak pointers to children of this Transform.
		*/
		const std::vector<std::weak_ptr<Transform>>& GetChildren() const;
		/**
		*  Returns a weak pointer to the parent transform
		*  @return A weak pointer to the parent transform
		*/
		const std::weak_ptr<Transform> GetParent() const;
		/**
		*  Returns a weak pointer to the owner entity
		*  @return A weak pointer to the owner entity
		*/
		const std::weak_ptr<Entity> GetOwner() const;

		/**
		*  Gets the transform matrix from iga::Transform.
		*  @return The transform as a const Matrix4 refrence.
		*/
		const Matrix4& GetTransform();

		/**
		* get Image aspect ratio.
		* @return aspect ratio. float
		*/
		float GetImageAspectRatio( );

		/**
		* get scale of transform corrected by aspect ratio.
		* @return vector 2 scale x and y.
		*/
		Vector2 GetImageScaleCorrection();
		/**
		*  Used to show information in the editor : Inspector
		*/
		void Inspect();
		/**
		*  Used to save and load iga::Transform
		*/
		template<class Archive>
		void serialize(Archive &a_archive, uint32 const a_version);
	protected:


	private:
		/**
		*  Gets called when loaded from file.
		*  Calls this for it's children as well.
		*  @param a_self A shared ptr of itself
		*  @see iga::Entity
		*/
		void OnLoad(std::shared_ptr<Transform> a_self);

		/**
		*  Gets the total local position from all parents, starting from the entity this is called uppon.
		*  @param a_position Vector3 to add the local positions to.
		*  @param a_only_physics bool whether the position should be added until the second last parent with physics.
		*/
		void GetTotalPosition(Vector3 &a_position, bool a_only_physics = false);

		/**
		*  Gets the total rotation from all parents, starting from the entity this is called uppon.
		*  @param a_rotation float to add the rotation to.
		*/
		void GetTotalRotation(float &a_rotation);

		/**
		*  Gets the total scale from all parents, starting from the entity this is called uppon.
		*  @param a_rotation Vector3 to multiplies the scale with.
		*/
		void GetTotalScale(Vector3 &a_scale);

		/**
		*  Gets the total local position and rotation from all parents, starting from the entity this is called uppon.
		*  @param a_position Vector3 to add the local positions to.
		*  @param a_rotation float to add the local rotations to.
		*  @param a_only_physics bool whether the position and rotation should be added until the second last parent with physics.
		*/
		void GetTotalProperties(Vector3 &a_position, Vector3 & a_scale, float & a_rotation, bool a_only_physics = false);

		/**
		*  Recursively calculates the world matrix for this Transform and all its children.
		*/
		void CalculateTransformTree();
		/**
		*  Calculates the world matrix for this transform relative to its parent.
		*/
		void CalculateTransform();

		Vector3 position_; /**< The position of the iga::Transform as a Vector3 */
		Vector3 scale_; /**< The scale of the iga::Transform as a Vector3 */
		Vector3 world_scale_; /**< The world scale of the iga::Transform as a Vector3 */
		Vector4 world_position_; /**< The world position of the iga::Transform as a Vector3 */
		Matrix4 transform_; /**< The transform matrix of the iga::Transform as a Matrix4 */

		Vector3 rotation_; /**< The x and z rotation of the iga::Transform as a Vector3. */
		float image_aspect_ratio_; /**< Scale modifier for none square textures*/
		bool aspect_scales_up_; /**< Whether an image will scale up or down with it's aspect ratio. False by default */
		float world_rotation_; /**< The world rotation of the iga::Transform as a float */

		std::vector<std::weak_ptr<Transform>> children_; /**< The children of the iga::Transform in a std::vector< std::weak_ptr<Entity>> */
		std::weak_ptr<Transform> parent_;  /**< The parent of the iga::Transform as a std::weak_ptr<Entity> */
		std::weak_ptr<Entity> owner_; /**< The owner of the iga::Transform as a std::weak_ptr<Entity> */

		 /**
		 * The dirty flag of the iga::Transform.
		 * @see iga::Transform::SetPosition
		 * @see iga::Transform::SetRotation
		 * @see iga::Transform::SetScale
		 * @see iga::Transform::Move
		 * @see iga::Transform::Rotate
		 * @see iga::Transform::Scale
		 */
		 uint8 dirty_mask_;

		 public:
			 

	};

	template<class Archive>
	inline void Transform::serialize(Archive &a_archive, uint32 const a_version) {
		switch(static_cast<Version>(a_version)) {
			case Version::LATEST:
			case Version::ASPECT:
				a_archive(cereal::make_nvp("aspect_scales_up", aspect_scales_up_));
			case Version::X_ROTATION:
				a_archive(cereal::make_nvp("rotation3", rotation_));
			case Version::DEFAULT:
				a_archive(cereal::make_nvp("owner", owner_));
				a_archive(cereal::make_nvp("position", position_));
				if(static_cast<Version>(a_version) == Version::DEFAULT) {
					float rot = rotation_.getZ();
					a_archive(cereal::make_nvp("rotation", rot));
					rotation_.setZ(rot);
				}
				a_archive(cereal::make_nvp("scale", scale_));
				a_archive(cereal::make_nvp("children", children_));
				break;
		}
	}

}

CEREAL_CLASS_VERSION(iga::Transform, (static_cast<iga::uint32>(iga::Transform::Version::LATEST) - 1));

