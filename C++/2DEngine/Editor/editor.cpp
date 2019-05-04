#include <Editor/editor.h>

#ifdef EDITOR

#include <Editor/BTEditor/behaviour_tree_editor.h>
#include <Editor/menu_bar.h>
#include <Editor/inspector.h>
#include <Editor/hierarchy.h>
#include <Editor/profiler.h>
#include <Editor/play_state_window.h>
#include <Editor/asset_dropin_handler.h>
#include <Editor/graphics_editor_tools.h>
#include <Editor/collision_renderer.h>
#include <Editor/editor_options.h>
#include <ig_art_engine.h>

#include <imgui.h>

#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/binary.hpp>

iga::Editor::Editor() {
	editor_enabled_ = true;
	renderer_enabled_ = false;
	show_demo_window_ = false;
	sub_window_focused_ = false;
	gizmo_focused_ = false;
	custom_font_ = nullptr;
	duplicate_timer_ = 0.f;
}

iga::Editor::~Editor() {

	for (size_t i = 0, vector_size = editor_windows_.size(); i < vector_size; i++) {
		editor_windows_.pop_back();
	}
}

void iga::Editor::Init() {
	// Create editor windows
	behaviour_tree_ = std::make_shared<bte::BehaviourTreeEditor>();
	menu_bar_ = std::make_shared<MenuBar>();
	inspector_ = std::make_shared<Inspector>();
	hierarchy_ = std::make_shared<Hierarchy>();
	profiler_ = std::make_shared<Profiler>();
	play_state_window_ = std::make_shared<PlayStateWindow>();
	asset_drop_in_handler_ = std::make_shared<AssetDropInHandler>();
	graphics_editor_tools_ = std::make_shared<GraphicsEditorTools>();
	editor_options_window_ = std::make_shared<EditorOptions>();

	// Add editor windows to vector
	editor_windows_.push_back(std::weak_ptr<EditorWindow>(behaviour_tree_));
	editor_windows_.push_back(std::weak_ptr<EditorWindow>(menu_bar_));
	editor_windows_.push_back(std::weak_ptr<EditorWindow>(inspector_));
	editor_windows_.push_back(std::weak_ptr<EditorWindow>(hierarchy_));
	editor_windows_.push_back(std::weak_ptr<EditorWindow>(profiler_));

	// Init Editor Options.
	editor_options_window_->OnCreate();

	// Call OnCreate of each editor window
	behaviour_tree_->OnCreate();
	menu_bar_->OnCreate();
	inspector_->OnCreate();
	hierarchy_->OnCreate();
	profiler_->OnCreate();
	play_state_window_->OnCreate();

	// Create collision renderer
	collision_renderer_ = std::make_shared<CollisionRenderer>();

	// Set Style
	SetImGuiStyle();

	std::shared_ptr<Entity> camera_entity = std::shared_ptr<Entity>(new Entity(std::weak_ptr<Scene>()));
	camera_entity->self_ = camera_entity;
	camera_entity->OnCreate();

	camera_entity->SetName("EditorCamera");
	camera_entity->NewComponent<CameraComponent>();
	if (auto transform = camera_entity->GetTransform().lock()) {
		transform->SetPosition(Vector3(0.f, 0.f, 10.f));
	}
	if (auto camera_comp = camera_entity->GetComponent<CameraComponent>().lock()) {
		camera_comp->SetForward(Vector3(0.f, 0.f, -1.f));
        camera_comp->SetEditorCameraBool(true);
	}

	mouse_previous_pos_ = GetInput()->GetKBM().GetMousePos();
	editor_camera_ = camera_entity;

	LoadSettings();
}

void iga::Editor::Update() {
	if (!GetGame()->IsPlaying() && editor_enabled_) {
		if (editor_camera_) {
			ProcessEditorCamera();
			editor_camera_->Update();
			editor_camera_->PostUpdate();
		}

		for (auto it = editor_windows_.begin(); it != editor_windows_.end(); ++it) {
			if (std::shared_ptr<EditorWindow> editor_window = it->lock()) {
				if (editor_window->IsOpen()) {
					editor_window->Update();
				}
			}
		}

		ShowRendererWindow();
		editor_options_window_->Update();

		const KBM& kbm = GetInput()->GetKBM();
		if (kbm.IsCtrlDown() && kbm.IsKeyDown(Key::D) && duplicate_timer_ <= 0.0f)
		{
			duplicate_timer_ = 1.0f;
			if (std::shared_ptr<iga::Scene> scene = GetGame()->GetCurrentScene().lock())
			{
				scene->DuplicateEntity(inspector_->GetSelectedEntity());
			}
		}

		{
			duplicate_timer_ -= GetTime()->GetEngineDeltaTime();
		}
	}

	// Always render play/stop button when editor exists.
	if (play_state_window_) {
		play_state_window_->Update();
	}

	collision_renderer_->Update();

	sub_window_focused_ = false;
	gizmo_focused_ = false;

	graphics_editor_tools_->SetSelectedBox2D(inspector_->GetSelectedEntity());
}

void iga::Editor::EnableEditor() {
	editor_enabled_ = true;
}

void iga::Editor::DisableEditor() {
	editor_enabled_ = false;
}

bool iga::Editor::IsEditorEnabled() {
    return editor_enabled_;
}

std::weak_ptr <iga::bte::BehaviourTreeEditor > iga::Editor::GetBehaviourTreeEditor() {
	return behaviour_tree_;
}

std::weak_ptr<iga::MenuBar> iga::Editor::GetMenuBar() {
	return menu_bar_;
}

std::weak_ptr<iga::Inspector> iga::Editor::GetInspector() {
	return inspector_;
}

std::weak_ptr<iga::Hierarchy> iga::Editor::GetHierarchy() {
	return hierarchy_;
}

std::weak_ptr<iga::Profiler> iga::Editor::GetProfiler() {
	return profiler_;
}

std::weak_ptr<iga::AssetDropInHandler> iga::Editor::GetAssetDropInHandler() {
	return asset_drop_in_handler_;
}

std::weak_ptr<iga::CollisionRenderer> iga::Editor::GetCollisionRenderer() {
	return collision_renderer_;
}

std::weak_ptr<iga::EditorOptions> iga::Editor::GetEditorOptions()
{
	return editor_options_window_;
}

bool iga::Editor::IsAnyWindowFocused() {
	for (auto window : editor_windows_) {
		if (auto current_window = window.lock()) {
			if ((current_window->IsFocused() && (current_window->GetName() != "Menu Bar")) ||
				sub_window_focused_ ||
				gizmo_focused_) {
				return true;
			}
		}
	}

	return false;
}

bool iga::Editor::IsAnyWindowHovered() {
	for (auto window : editor_windows_) {
		if (auto current_window = window.lock()) {
			if ((current_window->IsHovered() && (current_window->GetName() != "Menu Bar")) ||
				sub_window_focused_ ||
				gizmo_focused_) {
				return true;
			}
		}
	}

	return false;
}

void iga::Editor::SubWindowFocused() {
	sub_window_focused_ = true;
}

void iga::Editor::GizmoFocused() {
	gizmo_focused_ = true;
}

void iga::Editor::ShowRendererWindow() {
#ifdef _WIN32
	if (renderer_enabled_) {
		if (RVX::RendererD3D12 *renderer = GetTestRenderer()) {
			renderer->Inspect();
		}
	}
#endif
}

void iga::Editor::ShowDemoWindow() {
	if (show_demo_window_) {
		ImGui::ShowTestWindow(&show_demo_window_); // ImGui 1.50 Code
	}
}

void iga::Editor::SetImGuiStyle() {

	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.ChildWindowRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.FramePadding = ImVec2(4.5f, 1.8f);
	style.GrabMinSize = 15.0f;
	style.GrabRounding = 0.0f;
	style.IndentSpacing = 10.f;
	style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
	style.ItemSpacing = ImVec2(17.5f, 4.5f);
	style.WindowPadding = ImVec2(7.5f, 5.0f);
	style.WindowRounding = 0.0f;
	style.ScrollbarSize = 15.0f;
	style.ScrollbarRounding = 0.0f;

	ImVec4 white_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	ImVec4 window_bg_color = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	ImVec4 frame_bg_color = ImVec4(0.10f, 0.63f, 0.29f, 0.95f);
	ImVec4 frame_hover_color = ImVec4(0.20f, 0.59f, 0.24f, 0.80f);
	ImVec4 frame_active_color = ImVec4(0.20f, 0.65f, 0.24f, 1.00f);

	style.Colors[ImGuiCol_Text]									= ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled]							= ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg]								= window_bg_color;
	style.Colors[ImGuiCol_ChildWindowBg]						= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg]								= ImVec4(0.05f, 0.05f, 0.10f, 0.94f);
	style.Colors[ImGuiCol_Border]								= ImVec4(0.11f, 0.73f, 0.33f, 0.29f);
	style.Colors[ImGuiCol_BorderShadow]							= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg]								= frame_bg_color;
	style.Colors[ImGuiCol_FrameBgHovered]						= frame_hover_color;
	style.Colors[ImGuiCol_FrameBgActive]						= frame_active_color;
	style.Colors[ImGuiCol_TitleBg]								= ImVec4(frame_bg_color.x, frame_bg_color.y, frame_bg_color.z, 0.825f);
	style.Colors[ImGuiCol_TitleBgCollapsed]						= frame_hover_color;
	style.Colors[ImGuiCol_TitleBgActive]						= ImVec4(frame_active_color.x, frame_active_color.y, frame_active_color.z, 0.825f);
	style.Colors[ImGuiCol_MenuBarBg]							= ImVec4(window_bg_color.x, window_bg_color.y, window_bg_color.z, 0.80f);
	style.Colors[ImGuiCol_ScrollbarBg]							= ImVec4(window_bg_color.x, window_bg_color.y, window_bg_color.z, 0.80f);
	style.Colors[ImGuiCol_ScrollbarGrab]						= frame_bg_color;
	style.Colors[ImGuiCol_ScrollbarGrabHovered]					= frame_hover_color;
	style.Colors[ImGuiCol_ScrollbarGrabActive]					= frame_active_color;
	style.Colors[ImGuiCol_ComboBg]								= ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_CheckMark]							= white_color;
	style.Colors[ImGuiCol_SliderGrab]							= white_color;
	style.Colors[ImGuiCol_SliderGrabActive]						= ImVec4(white_color.x, white_color.y, white_color.z, 0.78f);
	style.Colors[ImGuiCol_Button]								= frame_bg_color;
	style.Colors[ImGuiCol_ButtonHovered]						= frame_hover_color;
	style.Colors[ImGuiCol_ButtonActive]							= frame_active_color;
	style.Colors[ImGuiCol_Header]								= ImVec4(frame_bg_color.x, 0.83f, 0.39f, 0.5f);
	style.Colors[ImGuiCol_HeaderHovered]						= ImVec4(0.20f, 0.92f, 0.38f, 0.50f);
	style.Colors[ImGuiCol_HeaderActive]							= ImVec4(frame_active_color.x, frame_active_color.y, frame_active_color.z, 0.00f);
	style.Colors[ImGuiCol_Column]								= ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered]						= ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_ColumnActive]							= ImVec4(white_color.x, white_color.y, white_color.z, 0.82f);
	style.Colors[ImGuiCol_ResizeGrip]							= white_color;
	style.Colors[ImGuiCol_ResizeGripHovered]					= ImVec4(white_color.x, white_color.y, white_color.z, 0.69f);
	style.Colors[ImGuiCol_ResizeGripActive]						= ImVec4(white_color.x, white_color.y, white_color.z, 0.82f);
	style.Colors[ImGuiCol_CloseButton]							= ImVec4(0.80f, 0.10f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonHovered]					= ImVec4(0.93f, 0.12f, 0.19f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive]					= ImVec4(0.80f, 0.06f, 0.10f, 1.00f);
	style.Colors[ImGuiCol_PlotLines]							= white_color;
	style.Colors[ImGuiCol_PlotLinesHovered]						= ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram]						= ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered]					= ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg]						= ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening]					= ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void iga::Editor::ProcessEditorCamera() {
	const KBM &kbm = GetInput()->GetKBM();
	Vector2 current_mouse_pos = kbm.GetMousePos();

	auto transform = editor_camera_->GetTransform().lock();
	if (transform && !IsAnyWindowHovered() && !ImGui::GetIO().WantCaptureKeyboard) {
		static float camera_normal_speed = 5.f;
		static float camera_shift_speed = camera_normal_speed * 3.f;
		static float camera_speed = 0.f;
		camera_speed = camera_normal_speed;

		bool set = false;
		Vector3 direction(0.f);
		Vector3 move(0.f);
		move.setZ(-kbm.GetMouseWheel());

		if (kbm.IsMouseDown(iga::MouseButton::RIGHT)) {
			float mouseDeltaFactor = 0.5f;
			Vector2 delta = Vector2(current_mouse_pos - mouse_previous_pos_);
			move = Vector3(delta.getX(), delta.getY(), 0) * mouseDeltaFactor;

			move.setX(-move.getX() * GetTime()->GetEngineDeltaTime() * camera_normal_speed);
			move.setY(move.getY() * GetTime()->GetEngineDeltaTime() * camera_normal_speed);

			transform->Move(move);
			mouse_previous_pos_ = current_mouse_pos;
			return;
		}

		if (kbm.IsKeyDown(Key::A)) {
			direction.setX(-1.f);
			set = true;
		}
		if (kbm.IsKeyDown(Key::D)) {
			direction.setX(1.f);
			set = true;
		}
		if (kbm.IsKeyDown(Key::W)) {
			direction.setY(1.f);
			set = true;
		}
		if (kbm.IsKeyDown(Key::S)) {
			direction.setY(-1.f);
			set = true;
		}
		if (kbm.IsKeyDown(Key::SPACE)) {
			camera_speed += camera_shift_speed;
		}

		if (set) {
			direction = normalize(direction);
		}

		move.setX(direction.getX() * GetTime()->GetEngineDeltaTime() * camera_speed);
		move.setY(direction.getY() * GetTime()->GetEngineDeltaTime() * camera_speed);

		transform->Move(move);
	}

	mouse_previous_pos_ = current_mouse_pos;
}

void iga::Editor::LoadSettings() {
	bool found_file = false;
	if(iga::CreateNewDirectory(SETTINGS_PATH)) {
		if(iga::DoesFileExistInDir(SETTINGS_PATH, EDITOR_SETTINGS_FN)) {
			found_file = true;

			{
				std::istringstream* settings_ss = iga::GetFileData(SETTINGS_PATH DIR_SLASH EDITOR_SETTINGS_FN);

				cereal::BinaryInputArchive entity_input_archive(*settings_ss);
				entity_input_archive(settings_);

				delete settings_ss;
			}

			// Get boot scene, check if it's empty.
			if(settings_.boot_scene.empty()) {
				// Set an empty scene as default scene
				GetGame()->CreateScene();
			} else {
				std::shared_ptr<iga::Scene> loaded_scene = iga::GetGame()->LoadRootScene(settings_.boot_scene);
				iga::GetGame()->SetRootScene(loaded_scene);
			}

		}
	}

	if(!found_file) {
		// If no settings where found,
		// Do some default actions to initialize the game

		// Set an empty scene as default scene
		GetGame()->CreateScene();

	}
}

void iga::Editor::OnGamePlay() {
}

void iga::Editor::OnGamePause() {
	std::weak_ptr<CameraComponent> camera_comp = editor_camera_->GetComponent<CameraComponent>();

	if (camera_comp.lock()) {
		GetRenderer()->SetActiveCamera(camera_comp);
	}
}

std::shared_ptr<iga::Entity> iga::Editor::GetEditorCamera() {
	return editor_camera_;
}

void iga::Editor::SetCustomFont(ImFont * a_font) {
	if (a_font != nullptr && custom_font_ == nullptr) {
		custom_font_ = a_font;
	}
}

ImFont * iga::Editor::GetCustomFont() {
	return custom_font_;
}

#endif