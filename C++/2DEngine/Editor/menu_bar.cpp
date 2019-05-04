#include <Editor/menu_bar.h>

#ifdef EDITOR

#include <Editor/hierarchy.h>
#include <Editor/inspector.h>
#include <Editor/hierarchy.h>
#include <Editor/profiler.h>
#include <Editor/BTEditor/behaviour_tree_editor.h>
#include <Editor/editor_options.h>
#include <TerrainTool/terrain_component.h>
#include <ig_art_engine.h>

#include <iga_util.h>

#include <vector>

iga::MenuBar::MenuBar() :
	open_new_scene_popup_(false),
	open_load_scene_popup_(false),
	new_scene_note_(""),
	new_scene_warning_title_("Create new scene"),
	load_scene_warning_title_("Load scene"),
	new_scene_name_(""),
	loadable_scene_name_list_(nullptr),
	load_scene_current_item(0)
{
	fps_enabled_ = true;
}

iga::MenuBar::~MenuBar() {
}

void iga::MenuBar::OnCreate() {
	is_open_ = true;
	name_ = "Menu Bar";
}

void iga::MenuBar::Update() {
	static std::vector<float> delta_time_vector = std::vector<float>(2);
	averaged_delta_time_ = 0.f;

	for (int i = 0; i < delta_time_vector.size() - 1; ++i) {
			delta_time_vector[i] = delta_time_vector[i + 1];
			averaged_delta_time_ += delta_time_vector[i];
	}

	delta_time_vector[delta_time_vector.size() - 1] = GetTime()->GetEngineDeltaTime();
	averaged_delta_time_ += delta_time_vector[delta_time_vector.size() - 1];

	averaged_delta_time_ /= delta_time_vector.size();
	delta_time_vector[delta_time_vector.size() - 1] = averaged_delta_time_;

	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File")) {
		if(ImGui::MenuItem("New Scene...")) {
			LoadScenesNames();

			// Create new scene
			new_scene_note_ = std::string();
			open_new_scene_popup_ = true;
			create_new_scene_ = false;
		}

		if(ImGui::MenuItem("Load Scene...")) {
			LoadScenesNames();

			// Open "Load scene" popup
			open_load_scene_popup_ = true;
		}

		if(ImGui::MenuItem("Save all")) {
			GetGame()->SaveScene();
		}

		if (ImGui::MenuItem("Open Asset folder")) {
			ShellExecute(NULL, "open", FILEPATHPREFIX, NULL, NULL, SW_SHOWDEFAULT);
		}

#ifdef DX12_RENDERER
		if (ImGui::MenuItem("Capture Screenshot")) {
			RVX::RendererD3D12* dxRenderer = dynamic_cast<RVX::RendererD3D12*>(Application::renderer_);
			dxRenderer->CaptureScreenShot();
		}
#endif
		if (ImGui::MenuItem("Options"))
		{
			if (std::shared_ptr<EditorOptions> editorOptions = editor_->GetEditorOptions().lock())
			{
				editorOptions->Open();
			}
		}
		if (ImGui::MenuItem("Close Editor")) {
			ShutdownGame();
		}
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Windows")) {
		if (std::shared_ptr<Hierarchy> hierarchy = editor_->GetHierarchy().lock()) {
			ImGui::Checkbox("Hierarchy", &hierarchy->is_open_);
		}
		if (std::shared_ptr<Inspector> inspector = editor_->GetInspector().lock()) {
			ImGui::Checkbox("Inspector", &inspector->is_open_);
		}
		if (std::shared_ptr<Profiler> profiler = editor_->GetProfiler().lock()) {
			ImGui::Checkbox("Profiler", &profiler->is_open_);
		}
        if (std::shared_ptr<bte::BehaviourTreeEditor> editor = editor_->GetBehaviourTreeEditor().lock()) {
            ImGui::Checkbox("Behaviour Tree Editor", &editor->is_open_);
        }
		//ImGui::Checkbox("Terrain Edge Draw", &tt::TerrainComponent::draw_edges); // Uncomment for option to toggle debug edge drawing in terrain component
		ImGui::Checkbox("Rendering (DX12)", &editor_->renderer_enabled_);
		ImGui::Checkbox("FPS", &fps_enabled_);
		ImGui::EndMenu();
	}

	if (fps_enabled_) {
		ImGui::Text("FPS: %3d       Delta Time: %3.1fms", (int)(1.f / averaged_delta_time_), (averaged_delta_time_ * 1000.f));
	}

	// Open new scene popup if it should be open
	if(open_new_scene_popup_) {
		ImGui::OpenPopup(new_scene_warning_title_);
	}

	// Create new scene
	if(ImGui::BeginPopupModal(new_scene_warning_title_)) {
		ImGui::Text("Are you sure you want to create a new scene?");
		ImGui::Text("The current scene will be saved and closed automatically.");

		if(new_scene_note_.size() > 0) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.0f, 0.0f, 1.f));
			ImGui::Text(new_scene_note_.c_str());
			ImGui::PopStyleColor();
		}

		ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.33f);
		ImGui::Text("Scene name:");
		ImGui::SameLine();
		// Input text for entity name
		if(ImGui::InputText("##NewSceneNameInput", new_scene_name_, IM_ARRAYSIZE(new_scene_name_), ImGuiInputTextFlags_EnterReturnsTrue)) {
			create_new_scene_ = true;
		}
		ImGui::PopItemWidth();

		ImGui::SameLine(0.f, 2.f);

		if(ImGui::Button("Create", ImVec2(50, 0))) {
			create_new_scene_ = true;
		}

		if(ImGui::Button("Cancel", ImVec2(50, 0))) {
			open_new_scene_popup_ = false;
			create_new_scene_ = false;
			ImGui::CloseCurrentPopup();
		}

		if(create_new_scene_) {
			if(strcmp(new_scene_name_, "") != 0) {
				bool found_name = false;
				for(size_t i = 0; i < scene_names_size_; ++i) {
					if(strcmp(loadable_scene_name_list_[i], new_scene_name_) == 0) {
						found_name = true;
					}
				}

				if(!found_name) {
					GetGame()->SaveScene();
					if(std::shared_ptr<iga::Scene> new_scene = GetGame()->CreateScene().lock()) {
						new_scene->SetName(new_scene_name_);
					}
					ImGui::CloseCurrentPopup();
					open_new_scene_popup_ = false;
					create_new_scene_ = false;
				} else {
					new_scene_note_ = "there's already a scene with this name";
				}
			} else {
				new_scene_note_ = "scene must have a name";
			}
		}

		create_new_scene_ = false;

		ImGui::EndPopup();
	}


	
	if(open_load_scene_popup_) {
		ImGui::OpenPopup(load_scene_warning_title_);
	}

	// Create new scene
	if(ImGui::BeginPopupModal(load_scene_warning_title_)) {
		ImGui::Text("Loading a scene will automatically save and close the current scene.");

		if(loadable_scene_name_list_ != nullptr) {
			ImGui::ListBox("", &load_scene_current_item, loadable_scene_name_list_, int32(scene_names_size_), 4);
		}

		ImGui::SameLine(0.f, 1.f);
		if(ImGui::Button("Load", ImVec2(50, 0))) {
			std::shared_ptr<iga::Scene> loaded_scene = iga::GetGame()->LoadRootScene(loadable_scene_name_list_[load_scene_current_item]);
			iga::GetGame()->SetRootScene(loaded_scene);

			iga::GetRenderer()->SetActiveCamera(iga::GetEditor()->editor_camera_->GetComponent<iga::CameraComponent>());

			open_load_scene_popup_ = false;
			ImGui::CloseCurrentPopup();
		}

		if(ImGui::Button("Cancel", ImVec2(50, 0))) {
			open_load_scene_popup_ = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


	ImGui::EndMainMenuBar();
}

void iga::MenuBar::OnOpen() {
}

void iga::MenuBar::OnClose() {
}

void iga::MenuBar::LoadScenesNames() {
	scene_names_size_ = 0;

	if(loadable_scene_name_list_ != nullptr) {
		delete[] loadable_scene_name_list_;
		loadable_scene_name_list_ = nullptr;
	}

	// Load loadable scene names
	std::vector<std::string> scene_names;
	std::vector<std::string> files_in_dir = iga::LoadFilesInDirectory(FILEPATHPREFIX "scenes" DIR_SLASH);
	for(std::string &folder : files_in_dir) {
		iga::RemoveLineEndings(folder);
		if(iga::GetFileExtension(folder).empty()) {
			// If file extension is empty, it's a folder
			scene_names.push_back(iga::GetFileName(folder));
		}
	}

	scene_names_size_ = scene_names.size();

	if(scene_names_size_ > 0) {
		size_t string_size = 0;
		loadable_scene_name_list_ = new const char*[scene_names_size_];
		for(size_t i = 0; i < scene_names_size_; ++i) {
			string_size = scene_names[i].size() + 1;
			char *scene_name = new char[string_size];
			memset(scene_name, 0, string_size);
			memcpy(scene_name, scene_names[i].c_str(), scene_names[i].size());
			loadable_scene_name_list_[i] = scene_name;
		}
	}
}

#endif