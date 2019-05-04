#include <Graphics/animation_component.h>
#include <ig_art_engine.h>

#ifdef EDITOR
#include <imgui.h>
#include <imgui_internal.h>
#endif

iga::AnimationData::AnimationData()
	: texture_path("")
	, animation_interval(0.f)
	, animation_fps(20.f)
	, animation_fps_default(20.f)
	, num_columns(1)
	, num_rows(1)
	, num_frames(1)
	, num_empty_frames(0)
	, loops(true)
	, has_atlas(false)
	, uses_custom_scale_(false)
	, uses_custom_offset_(false)
	, custom_scale_(Vector2(1.f))
	, custom_offset_(Vector2(0.f)) {
	frame_scales = std::vector<Vector2>();
	frame_scales.push_back(Vector2(1.f, 1.f));
	frame_offsets = std::vector<Vector2>();
	frame_offsets.push_back(Vector2(1.f, 1.f));
	frame_indices.push_back(0);
	frame_aspect_ratios = std::vector<float>();
	frame_aspect_ratios.push_back(1.f);
}

iga::AnimationComponent::AnimationComponent()
	: BaseRenderingComponent(std::weak_ptr<Entity>()) {
	Init();
}

iga::AnimationComponent::AnimationComponent(std::weak_ptr<Entity> a_entity)
	: BaseRenderingComponent(a_entity) {
	Init();
}

iga::AnimationComponent::~AnimationComponent() {
}

void iga::AnimationComponent::OnCreate(bool a_on_load) {
	// Load sprite/quad mesh
	mesh_resource_ = GetResourceManager()->Load<Mesh>("Sprite");

	if (a_on_load) {
		current_frame_ = 0;

		for (uint32 i = 0; i < animations_.size(); ++i) {
			animations_[i].texture = GetResourceManager()->Load<Texture>(animations_[i].texture_path);
			SetAnimationPathBuffer(i);
		}

		if (animations_.size() == 0) {
			AddAnimation("Default");
		}
	}
	else {
		// Add default texture
		AddAnimation("Default");
	}
}

void iga::AnimationComponent::Update() {
	// Count time
	elapsed_ += GetTime()->GetGameDeltaTime();

	// Get reference to the current animation for convenience
	const AnimationData& anim = animations_[current_animation_];

	// Calculate the amount of animation frames passed since the last game frame
	uint32 frames_to_add = 0;
	while (elapsed_ > anim.animation_interval) {
		elapsed_ -= anim.animation_interval;
		++frames_to_add;
	}

	// If the animation has reached the end
	if (current_frame_ + frames_to_add >= anim.num_frames - anim.num_empty_frames) {
		// If there are animations queued, change to them
		if (queued_animations_.size() > 0) {
			ChangeAnimation(queued_animations_[0], true, true, false);
			queued_animations_.erase(queued_animations_.begin());
		}
		// If we should revert to the previous animation
		else if (revert_to_previous_animation_on_complete_ && (previous_animation_ < ((uint32)-1))) {
			revert_to_previous_animation_on_complete_ = false;
			ChangeAnimation(previous_animation_);
			frames_to_add = static_cast<uint32>(0);
		}
		// If the animation is not set to loop
		else if (!anim.loops) {
			// Pause on the last frame
			current_frame_ = anim.num_frames - anim.num_empty_frames - 1;
			pause_animation_ = true;
		}
	}

	if (!pause_animation_) {
		// Otherwise make sure if we have reached the end of the animation that we loop back around
		while (current_frame_ + frames_to_add >= anim.num_frames - anim.num_empty_frames) {
			frames_to_add -= anim.num_frames - anim.num_empty_frames;
		}

		// Advance the current frame
		current_frame_ += frames_to_add;
	}

	// Update the aspect ratio for the current frame of the current animation
	if (auto owner = owner_.lock()) {
		if (auto transform = owner->GetTransform().lock()) {
			transform->SetImageAspectRatio(anim.frame_aspect_ratios[current_frame_]);
		}
	}
}

void iga::AnimationComponent::PostUpdate() {
}

void iga::AnimationComponent::ChangeAnimation(const std::string & a_path, bool a_ignore_same_anim, bool a_ignore_queued, bool a_remove_queued_animations, bool a_revert_to_previous_on_complete, uint32 a_frame) {
	// Find the animation with the given path and change to it
	for (uint32 i = 0; i < animations_.size(); ++i) {
		if (animations_[i].texture_path == a_path) {
			ChangeAnimation(i, a_ignore_same_anim, a_ignore_queued, a_remove_queued_animations, a_revert_to_previous_on_complete, a_frame);
			break;
		}
	}
}

void iga::AnimationComponent::ChangeAnimation(uint32 a_animation_index, bool a_ignore_same_anim, bool a_ignore_queued, bool a_remove_queued_animations, bool a_revert_to_previous_on_complete, uint32 a_frame) {
	if (a_animation_index >= animations_.size()) {
		return;
	}

	// Set the current animation to the given one passed in
	if (a_ignore_same_anim && a_animation_index == current_animation_) {
		return;
	}

	if (a_remove_queued_animations) {
		queued_animations_.clear();
	}

	if (!a_ignore_queued && (queued_animations_.size() > 0)) {
		return;
	}

	if (current_animation_ != a_animation_index) {
		previous_animation_ = current_animation_;
	}

	current_animation_ = a_animation_index;
	current_frame_ = a_frame;
	pause_animation_ = false;

	revert_to_previous_animation_on_complete_ = a_revert_to_previous_on_complete;
}

void iga::AnimationComponent::RestartAnimation() {
	// Set the current frame to the first in this animation
	current_frame_ = 0;
}

bool iga::AnimationComponent::AddAnimation(const std::string & a_path) {
	// Construct a new animation
	AnimationData anim = AnimationData();

	// If there is a texture, and it is the default sprite
	if (animations_.size() > 0) {
		if (animations_[0].texture->GetPath() == "Default") {
			// Replace the default texture
			animations_[0].texture = GetResourceManager()->Load<Texture>(a_path);

			// Check valid path
			if (a_path != "Default" && animations_[0].texture->GetPath() == "Default") {
#ifdef EDITOR
				show_invalid_path = true;
#endif // EDITOR
				return false;
			}

			// If could not set modifiers 
			if (!SetAnimationData(a_path, 0)) {
				// Revert to default sprite
				animations_[0].texture = GetResourceManager()->Load<Texture>("Default");
			}


			// Set the file path buffer for ImGui if EDITOR is defined
			SetAnimationPathBuffer(uint32(animations_.size() - 1));

			return true;
		}
	}

	for (int i = 0; i < animations_.size(); ++i) {
		if (animations_[i].texture->GetPath() == a_path) {
			// Already loaded
			return true;
		}
	}

	// Load the texture
	if (anim.texture = GetResourceManager()->Load<Texture>(a_path)) {
		// Check valid path
		if (a_path != "Default" && anim.texture->GetPath() == "Default") {
#ifdef EDITOR
			show_invalid_path = true;
#endif // EDITOR
			return false;
		}

		// Add the animation
		animations_.push_back(anim);

		// If could not set modifiers 
		if (!SetAnimationData(a_path, uint32(animations_.size() - 1))) {
			// Remove animation
			animations_.erase(animations_.begin() + (animations_.size() - 1));
			return false;
		}

		if (auto owner = owner_.lock()) {
			if (auto transform = owner->GetTransform().lock()) {
				auto texture_resource_ptr = std::dynamic_pointer_cast<Texture>(anim.texture);
				transform->SetImageAspectRatio(float(texture_resource_ptr->width_) / float(texture_resource_ptr->height_));
			}
		}

		// Set the file path buffer for ImGui if EDITOR is defined
		SetAnimationPathBuffer(uint32(animations_.size() - 1));
	}

	return true;
}

void iga::AnimationComponent::RemoveAnimation(const std::string& a_path) {
	// Find the animation with the given file path and remove it
	for (int i = 0; i < animations_.size(); ++i) {
		if (a_path == animations_[i].texture_path) {
			RemoveAnimation(i);
			break;
		}
	}
}

void iga::AnimationComponent::RemoveAnimation(uint32 a_animation_index) {
	// If the animation at the given index is the current one
	if (a_animation_index == current_animation_) {
		// Default to the first animation
		current_animation_ = 0;
		current_frame_ = 0;
	}
	else {
		// If the current animation is before the given index in the animation vector
		if (current_animation_ > a_animation_index) {
			// Carry on playing the current animation
			// This will be in the previous index so reduce the animation index
			--current_animation_;
		}
	}

	// If the given animation index is a correct value
	if (a_animation_index < animations_.size() && a_animation_index >= 0) {
		// Erase it
		animations_.erase(animations_.begin() + a_animation_index);
	}

	// If there are no animations
	if (animations_.size() == 0) {
		// Add a default one
		AddAnimation("Default");
		// Make sure it will play
		current_animation_ = 0;
	}
}

void iga::AnimationComponent::LoopCurrentAnimation() {
	animations_[current_animation_].loops = true;
}

void iga::AnimationComponent::LoopAnimation(const std::string & a_animation_path) {
	// Find the animation with the given file path and set it to loop
	for (int i = 0; i < animations_.size(); ++i) {
		if (a_animation_path == animations_[i].texture_path) {
			animations_[i].loops = true;
			break;
		}
	}
}

void iga::AnimationComponent::Init() {
	current_animation_ = 0;
	current_frame_ = 0;
	previous_animation_ = (uint32)-1;
	elapsed_ = 0.f;
#ifdef EDITOR
	show_invalid_path = false;
#endif // EDITOR
	component_type_ = "AnimationComponent";
	animations_ = std::vector<AnimationData>();
	color_modifier_ = Vector4(1.f, 1.f, 1.f, 1.f);
	queued_animations_ = std::vector<uint32>();
	pause_animation_ = false;
	revert_to_previous_animation_on_complete_ = false;
}

void iga::AnimationComponent::ResizeAnimationFrames(uint32 a_empty_frames) {
	AnimationData& anim = animations_[current_animation_];
	anim.num_empty_frames = a_empty_frames;

	if (current_frame_ > anim.num_frames - anim.num_empty_frames) {
		current_frame_ = anim.num_frames - anim.num_empty_frames - 1;
	}
}

bool iga::AnimationComponent::SetAnimationData(const std::string & a_path, uint32 a_animation_index) {
	// Get a reference to the animation at the specified index
	AnimationData& anim = animations_[a_animation_index];
	// If the path has a . 
	if (a_path.find(".") != std::string::npos) {
		// Get the last 8 chars before the .
		std::string str = a_path.substr(a_path.find_last_of(".") - 8, 8);
		if (str[2] == 'x' && str[5 == '@']) {
			// Set the values based on the file name
			anim.num_columns = std::stoi(str.substr(0, 2));
			anim.num_rows = std::stoi(str.substr(3, 2));
			anim.animation_fps_default = std::stof(str.substr(6, 2));
			anim.animation_fps = anim.animation_fps_default;
			anim.animation_interval = 1.f / anim.animation_fps;
			anim.frame_scales[0].setX(1.f / (float)animations_[a_animation_index].num_columns);
			anim.frame_scales[0].setY(1.f / (float)animations_[a_animation_index].num_rows);
			anim.frame_offsets[0].setX(1.f / (float)animations_[a_animation_index].num_columns);
			anim.frame_offsets[0].setY(1.f / (float)animations_[a_animation_index].num_rows);
			anim.num_frames = animations_[a_animation_index].num_columns * animations_[a_animation_index].num_rows;
			anim.loops = true;
			anim.texture_path = a_path;
		}
		else {
#ifdef EDITOR
			// Open the atlas file to read the data
			std::string atlas_filename = FILEPATHPREFIX + a_path.substr(0, a_path.find_last_of(".")) + ".atlas";
			std::ifstream input_filestream(atlas_filename.c_str());
			std::string line;

			if (input_filestream.good()) {
				input_filestream >> line; // Texture name
				input_filestream >> line; // Size text
				input_filestream >> line; // Image size

				uint32 image_width, image_height;
				uint32 first_number = (uint32)(line.find_last_of(",")); // Number of chars before comma
				uint32 frame_count = 0;

				image_width = std::stoi(line.substr(0, (size_t)first_number));
				image_height = std::stoi(line.substr(line.find_last_of(",") + 1, line.length() - (size_t)first_number));

				// Ignore next few lines
				for (size_t i = 0; i < 6; i++) {
					input_filestream >> line;
				}

				// Read first frame
				input_filestream >> line;
				anim.frame_indices[frame_count] = GetFrameIndexFromName(line);
				ReadFrame(input_filestream, anim, image_width, image_height, 0);
				++frame_count;

				// Ignore second frame as it is a copy of the first, and simply signifies the "last" frame
				

				// Read all subsequent frames
				while (input_filestream >> line) {
					anim.frame_indices.push_back(GetFrameIndexFromName(line));
					ReadFrame(input_filestream, anim, image_width, image_height, frame_count);
					++frame_count;
				}

				input_filestream.close();

				// Store indices for each frame so they can be drawn in order
				std::vector<uint32> animation_indices(frame_count);
				for (size_t i = 0; i < frame_count; i++) {
					for (size_t j = 0; j < frame_count; ++j) {
						if ((uint32)i == anim.frame_indices[j]) {
							animation_indices[i] = (uint32)j;
							break;
						}
					}
				}

				anim.frame_indices = animation_indices;

				anim.has_atlas = true;
				anim.num_frames = frame_count;
				anim.animation_interval = 1.f / anim.animation_fps;
				anim.loops = true;
				anim.texture_path = a_path;
				anim.animation_fps = anim.animation_fps_default;
			}
			else {
				return false;
			}

#endif // EDITOR
		}

	}
	else {
		// Otherwise set default values
		anim.num_columns = 1;
		anim.num_rows = 1;
		anim.frame_scales[0].setX(1.f);
		anim.frame_scales[0].setY(1.f);
		anim.frame_offsets[0].setX(0.f);
		anim.frame_offsets[0].setY(0.f);
		anim.num_frames = 1;
		anim.animation_fps = 1.f;
		anim.animation_interval = 1.f;
		anim.loops = true;
		anim.texture_path = a_path;
	}

	return true;
}

void iga::AnimationComponent::SetAnimationPathBuffer(uint32 a_animation_index) {
#ifndef EDITOR
	UNUSED(a_animation_index); // Unused variable for RELEASE Build.
#else
	// Empty char array to correctly set buffer
	static char buffer[256] = "";
	// Get name of animation at given index
	const std::string& name = animations_[a_animation_index].texture_path;
	// Get length
	size_t name_length = name.length();

	// Loop through char array
	for (size_t i = 0; i < 256; ++i) {
		// Set the buffer value to the name
		if (i < name_length) {
			animations_[a_animation_index].animation_path_buffer_[i] = name.c_str()[i];
		}
		else {
			// Otherwise make it empty
			animations_[a_animation_index].animation_path_buffer_[i] = buffer[i];
		}
	}
#endif
}

#ifdef EDITOR
uint32 iga::AnimationComponent::GetFrameIndexFromName(const std::string & a_frame_title) {
	uint32 index_char_start = (uint32)a_frame_title.length() - 1;
	for (size_t i = 0; i < a_frame_title.length(); i++) {
		try {
			std::stoi(a_frame_title.substr(index_char_start, 1));
			index_char_start--;
		}
		catch (std::invalid_argument& e) {
			// Can't be cast to a number any more
			UNUSED(e);
			++index_char_start;
			break;
		}
	}

	uint32 index_char_length = (uint32)a_frame_title.length() - index_char_start;
	return (uint32)std::stoi(a_frame_title.substr(index_char_start, index_char_length));
}

void iga::AnimationComponent::ReadFrame(std::ifstream& a_input_file, AnimationData& a_anim_data, uint32 a_image_width, uint32 a_image_height, uint32 a_frame_number) {
	std::string line;

	// Discard
	for (size_t i = 0; i < 3; i++) {
		a_input_file >> line;
	}

	// Get UV coords
	a_input_file >> line;
	uint32 u_coord = (uint32)std::stoi(line.substr(0, line.find_last_of(",")));
	a_input_file >> line;
	uint32 v_coord = (uint32)std::stoi(line);

	// Get size
	a_input_file >> line;
	a_input_file >> line;
	uint32 x_size = (uint32)std::stoi(line.substr(0, line.find_last_of(",")));
	a_input_file >> line;
	uint32 y_size = (uint32)std::stoi(line);

	// Discard
	for (size_t i = 0; i < 8; i++) {
		a_input_file >> line;
	}

	if (a_frame_number > 0) {
		a_anim_data.frame_scales.push_back(Vector2());
		a_anim_data.frame_offsets.push_back(Vector2());
		a_anim_data.frame_aspect_ratios.push_back(1.f);
	}

	a_anim_data.frame_scales[a_frame_number].setX((float)x_size / a_image_width);
	a_anim_data.frame_scales[a_frame_number].setY((float)y_size / a_image_height);
	a_anim_data.frame_offsets[a_frame_number].setX((float)u_coord / a_image_width);
	a_anim_data.frame_offsets[a_frame_number].setY((float)v_coord / a_image_height);
	a_anim_data.frame_aspect_ratios[a_frame_number] = (float)x_size / (float)y_size;
}
#endif // EDITOR

iga::RenderDataStruct* iga::AnimationComponent::OnBuildRenderData() {
	if (visible_) {
#ifdef EDITOR
		if (!visible_editor_only_ || (visible_editor_only_ && !GetGame()->IsPlaying())) {
#endif
			// Create empty modifier struct
			ModifierStruct modifier = ModifierStruct();
			// Get a const reference to the current animation
			const AnimationData& anim = animations_[current_animation_];

			if (anim.has_atlas) {
				// Set the modifier using the index of the current frame to scale and offset the sprite
				modifier.sprite_uv_offset_and_scale.setX(anim.frame_offsets[anim.frame_indices[current_frame_]].getX());
				modifier.sprite_uv_offset_and_scale.setY(anim.frame_offsets[anim.frame_indices[current_frame_]].getY());
				modifier.sprite_uv_offset_and_scale.setZ(anim.frame_scales[anim.frame_indices[current_frame_]].getX());
				modifier.sprite_uv_offset_and_scale.setW(anim.frame_scales[anim.frame_indices[current_frame_]].getY());
			}
			else {
				// Calculate which column and row in the sprite sheet the current frame resides in
				uint32 column = current_frame_;
				uint32 row = 0;
				while (column >= anim.num_columns) {
					column -= anim.num_columns;
					++row;
				}

				// Set the uv offset and scale
				modifier.sprite_uv_offset_and_scale.setX((float)column * anim.frame_offsets[0].getX());
				modifier.sprite_uv_offset_and_scale.setY((float)row * anim.frame_offsets[0].getY());
				modifier.sprite_uv_offset_and_scale.setZ(anim.frame_scales[0].getX());
				modifier.sprite_uv_offset_and_scale.setW(anim.frame_scales[0].getY());
			}

			// Set color
			modifier.color_ = color_modifier_;

			// Build render data
			RenderDataStruct *render_data = new RenderDataStruct();
			render_data->transforms_.push_back(RenderDataTransformStruct());
			render_data->scales_.push_back(Vector3());

			if (auto owner = owner_.lock()) {
				if (auto transform = owner->GetTransform().lock()) {
					Vector2 custom_scale_modifier = Vector2(1.f);
					if (anim.uses_custom_scale_) {
						custom_scale_modifier = anim.custom_scale_;
					}

					Matrix4 modifier_matrix = Matrix4::scale(Vector3(transform->GetImageScaleCorrection().getX() * custom_scale_modifier.getX(), transform->GetImageScaleCorrection().getY() * custom_scale_modifier.getY(), 1.0f));
					
					if (anim.uses_custom_offset_) {
						modifier_matrix.setTranslation(Vector3(anim.custom_offset_.getX(), anim.custom_offset_.getY(), 1.f));
					}

					render_data->transforms_[0].model_matrix_ = transform->GetTransform() * modifier_matrix;
				}
			}
			render_data->mesh_ = GetResourceManager()->Get<Mesh>(mesh_resource_);
			render_data->transforms_[0].modifier_ = modifier;
			render_data->texture_ = GetResourceManager()->Get<Texture>(anim.texture);

			if (flip_u_) {
				render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.setX(render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.getX() + render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.getZ());
				render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.setZ(render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.getZ() * -1.f);
			}
			if (flip_v_) {
				render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.setY(render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.getY() + render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.getW());
				render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.setW(render_data->transforms_[0].modifier_.sprite_uv_offset_and_scale.getW() * -1.f);
			}


#ifdef EDITOR
			CheckSetSelectedModifiers(GetOwner(), render_data);
#endif

			return render_data;
#ifdef EDITOR
		}
#endif
	}
	return nullptr;
}

void iga::AnimationComponent::QueueAnimation(const std::string & a_animation_path) {
	for (size_t i = 0; i < animations_.size(); ++i) {
		if (a_animation_path == animations_[i].texture_path) {
			queued_animations_.push_back(static_cast<uint32>(i));
			break;
		}
	}
}

void iga::AnimationComponent::QueueAnimation(uint32 a_animation_index) {
	if (a_animation_index > 0 && a_animation_index < animations_.size()) {
		queued_animations_.push_back(a_animation_index);
	}
}

const std::string& iga::AnimationComponent::GetCurrentAnimation() const {
	return animations_[current_animation_].texture_path;
}

const uint32 iga::AnimationComponent::GetCurrentAnimationIndex() const {
	return current_animation_;
}

// normalized color channels.
void iga::AnimationComponent::SetSpriteColor(float a_r, float a_g, float a_b, float a_a) {
	color_modifier_.setX(a_r);
	color_modifier_.setY(a_g);
	color_modifier_.setZ(a_b);
	color_modifier_.setW(a_a);
}

void iga::AnimationComponent::SetSpriteAlpha(float a_a) {
	color_modifier_.setW(a_a);
}

void iga::AnimationComponent::SetAnimationSpeed(float a_animation_fps) {
	animations_[current_animation_].animation_fps = a_animation_fps;
	animations_[current_animation_].animation_interval = 1.f / a_animation_fps;
}

void iga::AnimationComponent::SetCurrentAnimationLoops(bool a_loops) {
	animations_[current_animation_].loops = a_loops;
}

void iga::AnimationComponent::SetAnimationLoops(const std::string & a_animation_path, bool a_loops) {
	// Find the animation with the given file path and set it to a_loops
	for (int i = 0; i < animations_.size(); ++i) {
		if (a_animation_path == animations_[i].texture_path) {
			animations_[i].loops = a_loops;
			break;
		}
	}
}

void iga::AnimationComponent::SetPaused(const bool a_paused) {
	pause_animation_ = a_paused;
}

bool iga::AnimationComponent::IsPaused() {
	return pause_animation_;
}

void iga::AnimationComponent::SetCurrentFrame(uint32 a_frame_index) {
	current_frame_ = a_frame_index;
}

uint32 iga::AnimationComponent::GetCurrentFrame() {
	return current_frame_;
}

void iga::AnimationComponent::SetUsesCustomOffset(bool a_uses_custom_offset) {
	animations_[current_animation_].uses_custom_offset_ = a_uses_custom_offset;
}

void iga::AnimationComponent::SetCustomOffset(Vector2 a_custom_offset) {
	animations_[current_animation_].custom_offset_ = a_custom_offset;
}

void iga::AnimationComponent::Inspect() {
#ifdef EDITOR
	// Make sure true is only returned on enter pressed
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	// Dummy int32 for storing animations value
	static int32 e = 0;

	// Show list of animations
	if (ImGui::TreeNode("Animations")) {
		// For each animation in array
		for (uint32 i = 0; i < animations_.size(); ++i) {
			// Push a unique id
			ImGui::PushID(i);
			// Show the radio button for currently selected animation
			if (ImGui::RadioButton("", &e, i)) {
				// If selected, change to this animation
				ChangeAnimation(i);
			}
			ImGui::PopID();
			ImGui::SameLine();
			// Push unique id for animation name
			ImGui::PushID(i * 100);
			// Show editable animation name
			if (ImGui::InputText("   ", animations_[i].animation_path_buffer_, IM_ARRAYSIZE(animations_[i].animation_path_buffer_), input_text_flags)) {
				// Remove animation
				std::string temp = animations_[i].animation_path_buffer_;
				RemoveAnimation(i);
				// If not blank, add new animation
				if (!temp.empty()) {
					AddAnimation(temp);
				}
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Current Animation Settings")) {
		// Slider for setting the fps of the current animation
		ImGui::PushID(window->GetID("Animation_FPS_Slider"));
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.6f);
		// Custom scale
		ImGui::Checkbox("Uses Custom Scale", &animations_[current_animation_].uses_custom_scale_);
		if (animations_[current_animation_].uses_custom_scale_) {
			float scale[2];
			scale[0] = animations_[current_animation_].custom_scale_.getX();
			scale[1] = animations_[current_animation_].custom_scale_.getY();

			ImGui::PushID(window->GetID("CustomScale##TreeNode"));
			if (ImGui::TreeNode("Custom Scale")) {
				ImGui::PushID(window->GetID("CustomScale##Slider"));
				if (ImGui::SliderFloat2("Custom Scale", scale, 0.001f, 100.f)) {
					animations_[current_animation_].custom_scale_.setX(scale[0]);
					animations_[current_animation_].custom_scale_.setY(scale[1]);
				}
				ImGui::PopID();
				ImGui::PushID(window->GetID("CustomScale##Input"));
				if (ImGui::InputFloat2("Custom Scale", &scale[0])) {
					animations_[current_animation_].custom_scale_.setX(scale[0]);
					animations_[current_animation_].custom_scale_.setY(scale[1]);
				}
				ImGui::PopID();
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		// Custom offset
		ImGui::Checkbox("Uses Custom Offset", &animations_[current_animation_].uses_custom_offset_);
		if (animations_[current_animation_].uses_custom_offset_) {
			float offset[2];
			offset[0] = animations_[current_animation_].custom_offset_.getX();
			offset[1] = animations_[current_animation_].custom_offset_.getY();

			ImGui::PushID(window->GetID("CustomOffset##TreeNode"));
			if (ImGui::TreeNode("Custom Offset")) {
				ImGui::PushID(window->GetID("CustomOffset##Slider"));
				if (ImGui::SliderFloat2("Custom Offset", offset, -10.f, 10.f)) {
					animations_[current_animation_].custom_offset_.setX(offset[0]);
					animations_[current_animation_].custom_offset_.setY(offset[1]);
				}
				ImGui::PopID();
				ImGui::PushID(window->GetID("CustomOffset##Input"));
				if (ImGui::InputFloat2("Custom Offset", &offset[0])) {
					animations_[current_animation_].custom_offset_.setX(offset[0]);
					animations_[current_animation_].custom_offset_.setY(offset[1]);
				}
				ImGui::PopID();
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		// Play speed in fps
		if (ImGui::SliderFloat("", &animations_[current_animation_].animation_fps, 1.f, 100.f, "FPS: %.3f")) {
			animations_[current_animation_].animation_interval = 1.f / animations_[current_animation_].animation_fps;
		}
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
			ImGui::SetTooltip("The speed of the current animation in frames per second.");
		}
		ImGui::PopItemWidth();
		ImGui::PopID();
		// Input text for setting the fps of the current animation
		ImGui::PushID(window->GetID("Animation_FPS_Input"));
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.3f);
		ImGui::SameLine();
		if (ImGui::InputFloat("", &animations_[current_animation_].animation_fps)) {
			animations_[current_animation_].animation_interval = 1.f / animations_[current_animation_].animation_fps;
		}
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
			ImGui::SetTooltip("The speed of the current animation in frames per second.");
		}
		ImGui::PopItemWidth();
		ImGui::PopID();

		// Show the number of frames to be trimmed if using old format
		if (!animations_[current_animation_].has_atlas) {
			int empty_frames = animations_[current_animation_].num_empty_frames;
			ImGui::PushItemWidth(100);
			ImGui::DragInt("Trim Frames", &empty_frames, 1, 100);
			if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
				ImGui::SetTooltip("Removes frames from the end of the animation.");
			}
			ImGui::PopItemWidth();
			if (empty_frames != int(animations_[current_animation_].num_empty_frames) && empty_frames < static_cast<int>(animations_[current_animation_].num_frames)) {
				animations_[current_animation_].num_empty_frames = empty_frames;
			}
		}

		// Show animation colour modifier
		ImGui::PushID(window->GetID("Animation_Colour_Input"));
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.9f);
		float colorRGB[4] = { color_modifier_.getX(), color_modifier_.getY(), color_modifier_.getZ(), color_modifier_.getW() };
		if (ImGui::ColorEdit4("", colorRGB)) {
			color_modifier_.setX(colorRGB[0]);
			color_modifier_.setY(colorRGB[1]);
			color_modifier_.setZ(colorRGB[2]);
			color_modifier_.setW(colorRGB[3]);
		}
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Modifies the colour of the sprite.");
		}
		ImGui::PopItemWidth();
		ImGui::PopID();

		// Show loops checkbox
		ImGui::Checkbox("Loops", &animations_[current_animation_].loops);
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
			ImGui::SetTooltip("When the animation reaches the last frame it will loop and continue playing.");
		}

		// Show flip UV checkboxes
		ImGui::Checkbox("Flip U", &flip_u_);
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Flips the sprite horizontally.");
		}
		ImGui::Checkbox("Flip V", &flip_v_);
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Flips the sprite vertically.");
		}

		ImGui::TreePop();
	}

	// Show the add animation section
	if (ImGui::TreeNode("Add New Animation")) {
		// Input text for adding new animation path
		ImGui::Indent();
		ImGui::PushID(window->GetID("Animation_Add_New"));
		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.9f);
		ImGui::InputText("    ", new_animation_path_, IM_ARRAYSIZE(new_animation_path_), input_text_flags);
		ImGui::Indent(ImGui::GetWindowWidth() * 0.22f);
		// Show the button to add an animation from the previous path entered
		if (ImGui::Button("Add Animation", ImVec2(100, 20))) {
			AddAnimation(new_animation_path_);
			strcpy_s(&new_animation_path_[0], 26, "Your animation path here.");
		}
		if (ImGui::IsItemActive() || ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Adds the animation at the file path specified above.");
		}
		ImGui::Unindent(ImGui::GetWindowWidth() * 0.22f);
		ImGui::Unindent();
		ImGui::TreePop();
		ImGui::PopID();
	}
	else {
		// Otherwise set input text to default text
		strcpy_s(&new_animation_path_[0], 26, "Your animation path here.");
	}

	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Allows the user to add a new animation at a specified filepath.");
	}

	// Popup to tell user they entered an invalid file path
	if (show_invalid_path) {
		ImGui::OpenPopup("Invalid Animation File");
		show_invalid_path = false;
	}
	if (ImGui::BeginPopupModal("Invalid Animation File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("You entered an invalid animation file path!\n\n");
		ImGui::Separator();
		float content_region = ImGui::GetContentRegionAvailWidth();
		float indent_width = content_region * 0.3f;
		ImGui::Indent(indent_width);
		if (ImGui::Button("OK", ImVec2(content_region * 0.4f, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::Unindent(indent_width);
		ImGui::EndPopup();
	}

#endif
}
