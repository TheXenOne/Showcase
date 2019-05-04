#pragma once

/**
* @file animation_component.h
* @author Ben Landor
* @date 27 mar 2018
* @brief iga::AnimationComponent class header.
*
* This file contains a class that is used in entities with animations.
* @see iga::Renderer
* @see iga::RenderListener
* @see iga::BaseRenderingComponent
*/

#include <defines.h>
#include <Graphics\base_rendering_component.h>
#include <vectormath\vectormath.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include <memory>
#include <vector>
#include <string>
#ifdef EDITOR
#include <fstream>
#endif // EDITOR

/**
* IGArt namespace.
*/
namespace iga {

	class Resource;

	/**
	* iga::AnimationData is a struct containing all relevant information needed to parse animations from a sprite sheet.
	*/
	struct AnimationData {
		enum class Version {
			DEFAULT = 0, /**< Default normal version. */
			EMPTY_FRAMES,
			ATLAS,
			SCALE,
			LATEST /**< Latest version. */
		};

		std::shared_ptr<Resource> texture; /**< The texture containing the animation sprite sheet. */
		std::string texture_path; /**< File path to the texture for this animation. */
		float animation_interval; /**< The time between animation frames, in seconds. */
		float animation_fps; /**< The speed of the animation in frames per second. */
		float animation_fps_default; /**< The speed of the animation in frames per second. */
		uint32 num_columns; /**< The number of sprites in each row. */
		uint32 num_rows; /**< The number of sprites in each column. */
		uint32 num_frames; /**< The total number of frames in the sprite sheet. */
		uint32 num_empty_frames; /**< The total number of empty frames in the sprite sheet. */
		std::vector<Vector2> frame_scales; /**< The scale of each frame in UV co-ordinates. */
		std::vector<Vector2> frame_offsets; /**< The offset of each frame in UV co-ordinates. */
		std::vector<uint32> frame_indices; /**< The order of indices to draw. */
		std::vector<float> frame_aspect_ratios; /**< The aspect ratios for each from of the animation. */
		bool loops; /**< Specifies whether the animation should play continuously. */
		bool has_atlas; /**< Specifies whether the animation uses an atlas for the position of each frame. */
		bool uses_custom_scale_; /**< Specifies whether this animation uses a custom scale modifier. */
		bool uses_custom_offset_; /**< Specifies whether this animation uses a custom offset modifier. */
		Vector2 custom_scale_; /**< Custom scale modifier for drawing this animation. */
		Vector2 custom_offset_; /**< Custom offset modifier for drawing this animation. */
#ifdef EDITOR
		char animation_path_buffer_[256]; /**< A char array containing the file path. Necessary for ImGui. */
#endif

		/**
		*  Constructor.
		*/
		AnimationData();

		/**
		*  Used to save and load iga::AnimationData.
		*/
		template<class Archive>
		void serialize(Archive &a_archive, uint32 const a_version);
	};

	/**
	* iga::AnimationComponent. This is a component used by entities that are represented by animated sprites.
	* Inherits iga::BaseRenderingComponent.
	* @see iga::Component
	* @see iga::RenderListener
	* @see iga::BaseRenderingComponent
	*/
	class AnimationComponent final : public BaseRenderingComponent {

	public:
		/**
		*   Cereal versions of iga::AnimationComponent
		*/
		enum class Version {
			DEFAULT = 0, /**< Default normal version. */
			COLORMODIFIER,
			SAVE,
			EMPTY_FRAMES,
			ATLAS,
			LATEST /**< Latest version. */
		};

		/**
		*  Constructor.
		*/
		AnimationComponent();
		/**
		*  Custom constructor with iga::Entity that will own the component.
		*  @param a_entity is a weak pointer to an iga::Entity that is to be the owner of this component.
		*  @see iga::Entity
		*/
		AnimationComponent(std::weak_ptr<Entity> a_entity);

		/**
		*  Destructor.
		*/
		virtual ~AnimationComponent();

		/**
		*  Gets called when it gets created. Is used as an initialization function.
		*  @param a_on_load A bool specifying if the component is being loaded.
		*/
		void OnCreate(bool a_on_load = false) override;
		/**
		*  Updates the component.
		*/
		void Update() override;
		/**
		*  Gets called after the update has happened.
		*/
		virtual void PostUpdate() override;

		/**
		*  Changes the current animation to the given animation, starting at the frame specified (defaults to 0).
		*  @param a_path is a constant reference to a string containing the file path of the animation.
		*  @param a_ignore_same_anim is a bool specifying if the request should be ignored if the given animation is already the current one.
		*  @param a_ignore_queued is a bool specifying if the request should be ignored if there are animations queued. Defaults to false.
		*  @param a_remove_queued_animations is a bool specifying if the animation queue should be emptied. Defaults to true.
		*  @param a_revert_to_previous_on_complete is a bool specifying whether the animation should revert to the previous animation on completion.
		*  @param a_frame is a uint32 containing the frame number to start the animation at (defaults to 0).
		*/
		void ChangeAnimation(const std::string& a_path, bool a_ignore_same_anim = true, bool a_ignore_queued = true, bool a_remove_queued_animations = true, bool a_revert_to_previous_on_complete = false, uint32 a_frame = 0);
		/**
		*  Changes the current animation to the given animation, starting at the frame specified (defaults to 0).
		*  @param a_animation_index is a uint32 specifying the index of the animation.
		*  @param a_ignore_same_anim is a bool specifying if the request should be ignored if the given animation is already the current one.
		*  @param a_ignore_queued is a bool specifying if the request should be ignored if there are animations queued. Defaults to false.
		*  @param a_remove_queued_animations is a bool specifying if the animation queue should be emptied. Defaults to true.
		*  @param a_revert_to_previous_on_complete is a bool specifying whether the animation should revert to the previous animation on completion.
		*  @param a_frame is a uint32 containing the frame number to start the animation at (defaults to 0).
		*/
		void ChangeAnimation(uint32 a_animation_index, bool a_ignore_same_anim = true, bool a_ignore_queued = true, bool a_remove_queued_animations = true, bool a_revert_to_previous_on_complete = false, uint32 a_frame = 0);
		/**
		*  Restarts the current animation by setting the current frame to 0.
		*/
		void RestartAnimation();
		/**
		*  Loads an animation texture from a path.
		*  @param a_path A const std::string reference that contains the path to the animation texture.
		*/
		bool AddAnimation(const std::string& a_path);
		/**
		*  Removes an animation from the component.
		*  @param a_path A const std::string reference containing the animation file path.
		*/
		void RemoveAnimation(const std::string& a_path);
		/**
		*  Removes an animation from the component.
		*  @param a_path A uint32 specifying the index of the animation.
		*/
		void RemoveAnimation(uint32 a_animation_index);
		/**
		*  Sets the current animation to loop.
		*/
		void LoopCurrentAnimation();
		/**
		*  Sets the specified animation to loop.
		*  @param a_animation_path A const std::string reference to the animation file path to be looped.
		*/
		void LoopAnimation(const std::string& a_animation_path);

		/**
		*  Virtual function that gets render data when it wants to build render data.
		*/
		virtual RenderDataStruct* OnBuildRenderData() override;

		/**
		*  Sets the specified animation to put in the play queue.
		*  @param a_animation_path A const std::string reference to the animation file path to be looped.
		*/
		void QueueAnimation(const std::string& a_animation_path);
		/**
		*  Sets the specified animation to put in the play queue.
		*  @param a_path A uint32 specifying the index of the animation.
		*/
		void QueueAnimation(uint32 a_animation_index);

		/**
		*  Gets the current animation file path.
		*  @return A constant string reference with the animation file path.
		*/
		const std::string& GetCurrentAnimation() const;
		/**
		*  Gets the current animation index.
		*  @return A constant uint32 with the index of the current animation.
		*/
		const uint32 GetCurrentAnimationIndex() const;

		/**
		*  Function to set a color for this sprite.
		*  @param a_r A float to set the red color channel (normalized).
		*  @param a_g A float to set the green color channel (normalized).
		*  @param a_b A float to set the blue color channel (normalized).
		*  @param a_a A float to set the alpha color channel (normalized).
		*/
		void SetSpriteColor(float a_r, float a_g, float a_b, float a_a);
        	/**
        	*  Function to set the alpha color for this sprite.
        	*  @param a_a A float to set the alpha color channel (normalized).
        	*/
        	void SetSpriteAlpha(float a_a);

		/**
		*  Sets the speed of the current animation in frames per seconds.
		*  @param a_animation_fps A float specifying the number of frames per second for the current animation to play at.
		*/
		void SetAnimationSpeed(float a_animation_fps);
		/**
		*  Sets whether the current animation to loop.
		*  @param a_loops A bool specifying whether the animation should loop.
		*/
		void SetCurrentAnimationLoops(bool a_loops);
		/**
		*  Sets the specified animation to loop.
		*  @param a_animation_path A const std::string reference to the animation file path to be set.
		*  @param a_loops A bool specifying whether the animation should loop.
		*/
		void SetAnimationLoops(const std::string& a_animation_path, bool a_loops);

		/**
		*  Sets the paused state of the animation.
		*  @param a_paused Whether the animation is paused or not.
		*/
		void SetPaused(const bool a_paused);
		/**
		*  Checks the paused state of the animation.
		*  @return Whether the animation is paused or not.
		*/
		bool IsPaused();

		/**
		*  Sets the current frame of the animation.
		*  @param a_frame_index The new current frame index of the animation sequence.
		*/
		void SetCurrentFrame(uint32 a_frame_index);
		/**
		*  Gets the current frame of the animation.
		*  @return The current frame index of the animation sequence.
		*/
		uint32 GetCurrentFrame();

		/**
		*  Sets if this animation uses a custom offset.
		*  @param a_uses_custom_offset is a bool specifying if the animation uses a custom offset.
		*/
		void SetUsesCustomOffset(bool a_uses_custom_offset);
		/**
		*  Sets the custom offset for this animation.
		*  @param a_custom_offset is the custom offset for this animation.
		*/
		void SetCustomOffset(Vector2 a_custom_offset);

		/**
		*  Used to show information in the iga::Editor : iga::Inspector.
		*/
		virtual void Inspect() override;
		/**
		*  Used to save and load iga::AnimationComponent.
		*/
		template<class Archive>
		void serialize(Archive &a_archive, uint32 const a_version);
	protected:

	private:
		/**
		*  Initializes AnimationComponent default values.
		*/
		void Init();
		/**
		*  Reduces the amount of frames in an animation.
		*  @param a_empty_frames A uint32 specifying how many empty frames the current animation has.
		*/
		void ResizeAnimationFrames(uint32 a_empty_frames);
		/**
		*  Sets the animation data for a given animation.
		*  @param a_path A const std::string reference that contains the path to the animation texture.
		*  @param a_path A uint32 specifying the index of the animation.
		*  @return A bool specifying success.
		*/
		bool SetAnimationData(const std::string& a_path, uint32 a_animation_index);
		/**
		*  Sets the animation path buffer to the name of the animation file path for use in ImGui.
		*  @param a_path A uint32 specifying the index of the animation.
		*/
		void SetAnimationPathBuffer(uint32 a_animation_index);
		/**
		*  Gets the frame index at the end of the frame title (spine atlas file).
		*  @param a_frame_title A constant string reference with the title of the frame containing the frame index.
		*  @return A uint32 with the index.
		*/
		uint32 GetFrameIndexFromName(const std::string& a_frame_title);
#ifdef EDITOR
		/**
		*  Adds the data for another frame in the specified animation.
		*  Editor only.
		*  @param a_input_file A reference to the input stream from the opened file.
		*  @param a_anim_data A constant reference to the animation data to be read in to.
		*  @param a_image_width A uint32 containing the width of the texture for this animation.
		*  @param a_image_width A uint32 containing the height of the texture for this animation.
		*  @param a_frame_number A uint32 containing the frame number for this data.
		*/
		void ReadFrame(std::ifstream& a_input_file, AnimationData& a_anim_data, uint32 a_image_width, uint32 a_image_height, uint32 a_frame_number);
#endif // EDITOR

		std::vector<AnimationData> animations_; /**< The possible animations for this component. */
		std::vector<uint32> queued_animations_; /**< Vector of animations in the queue to be played. */
		std::shared_ptr<Resource> mesh_resource_; /**< The mesh resource of a sprite. */
		uint32 current_animation_; /**< The index to the current animation playing. */
		uint32 current_frame_; /**< The index to the current frame being displayed. */
		uint32 previous_animation_; /**< The index to the previous animation playing. */
		Vector4 color_modifier_; /**< Modifier for coloring the sprite. */
		float elapsed_; /**< The time elapsed since the last animation frame in the current animation. */
		bool pause_animation_; /**< Whether the animation should pause. */
		bool revert_to_previous_animation_on_complete_; /**< Whether the animation should change to the previous animation on completion. */

#ifdef EDITOR
		bool show_invalid_path; /**< Bool used to specify if the popup notifying the user that they entered an invalid file path should show. */
		char new_animation_path_[256] = "Your animation path here."; /**< Default text to be displayed in the iga::Inspector when adding a new animation. */
#endif
	};

	SUBSCRIBECOMPONENT(AnimationComponent)

	template<class Archive>
	inline void AnimationComponent::serialize(Archive &a_archive, uint32 const a_version) {
		switch(static_cast<Version>(a_version)) {
			case Version::LATEST:

			case Version::ATLAS:
				
			case Version::EMPTY_FRAMES:

			case Version::SAVE:
				a_archive(cereal::make_nvp("animations", animations_));
				a_archive(cereal::make_nvp("current_animation", current_animation_));
				a_archive(cereal::make_nvp("color_modifier", color_modifier_));
			case Version::COLORMODIFIER:

			case Version::DEFAULT:
				a_archive(cereal::base_class<iga::BaseRenderingComponent>(this));
				break;
		}
	}

	template<class Archive>
	inline void AnimationData::serialize(Archive & a_archive, uint32 const a_version) {
		switch (static_cast<Version>(a_version)) {
		case Version::LATEST:

		case Version::SCALE:
			a_archive(cereal::make_nvp("uses_custom_scale", uses_custom_scale_));
			a_archive(cereal::make_nvp("custom_scale", custom_scale_));
			a_archive(cereal::make_nvp("uses_custom_offset", uses_custom_offset_));
			a_archive(cereal::make_nvp("custom_offset", custom_offset_));
		case Version::ATLAS:
			a_archive(cereal::make_nvp("has_atlas", has_atlas));
			a_archive(cereal::make_nvp("frame_scales", frame_scales));
			a_archive(cereal::make_nvp("frame_offsets", frame_offsets));
			a_archive(cereal::make_nvp("frame_indices", frame_indices));
			a_archive(cereal::make_nvp("frame_ratios", frame_aspect_ratios));
		case Version::EMPTY_FRAMES:
			a_archive(cereal::make_nvp("num_empty_frames", num_empty_frames));
		case Version::DEFAULT:
			a_archive(cereal::make_nvp("texture", texture));
			a_archive(cereal::make_nvp("texture_path", texture_path));
			a_archive(cereal::make_nvp("animation_interval", animation_interval));
			a_archive(cereal::make_nvp("animation_fps", animation_fps));
			a_archive(cereal::make_nvp("animation_fps_default", animation_fps_default));
			a_archive(cereal::make_nvp("num_columns", num_columns));
			a_archive(cereal::make_nvp("num_rows", num_rows));
			a_archive(cereal::make_nvp("num_frames", num_frames));
			a_archive(cereal::make_nvp("loop", loops));
			break;
		}
	}

}

CEREAL_CLASS_VERSION(iga::AnimationComponent, (static_cast<iga::uint32>(iga::AnimationComponent::Version::LATEST) - 1));
CEREAL_CLASS_VERSION(iga::AnimationData, (static_cast<iga::uint32>(iga::AnimationData::Version::LATEST) - 1));
