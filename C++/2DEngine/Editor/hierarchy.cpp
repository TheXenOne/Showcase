#include <Editor/hierarchy.h>

#ifdef EDITOR

#include <Core/scene.h>
#include <Editor/inspector.h>
#include <ig_art_engine.h>

#include <imgui.h>
#include <imgui_internal.h>


iga::Hierarchy::Hierarchy() :
	hierarchy_bottom_spacing_(5) 
{

}

void iga::Hierarchy::OnCreate() {
	is_open_ = true;
	focused_ = false;
	ignore_focused_ = false;
	window_flags_ = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
	search_value_ = "";
	name_ = "Hierarchy";
}

void iga::Hierarchy::Update() {
	// Set window pos and size
	ImVec2 window_pos = ImVec2(0.f, 22.f);
	ImVec2 window_pos_pivot = ImVec2(0.f, 0.f);
    ImGui::SetNextWindowPos(window_pos, ImGuiSetCond_Always); // ImGui 1.50 Code
    ImGui::SetNextWindowSize(ImVec2(window_size_.getX(), ImGui::GetIO().DisplaySize.y -22.f), ImGuiSetCond_Always); // ImGui 1.50 Code

	// Set window colour
	if (focused_ || ignore_focused_) {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_WindowBg].x, ImGui::GetStyle().Colors[ImGuiCol_WindowBg].y, ImGui::GetStyle().Colors[ImGuiCol_WindowBg].z, 1.0f)); // Dark background
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_WindowBg].x, ImGui::GetStyle().Colors[ImGuiCol_WindowBg].y, ImGui::GetStyle().Colors[ImGuiCol_WindowBg].z, 0.4f)); // Transparent background
	}

	ImGui::Begin(name_.c_str(), &is_open_, window_flags_);

	UpdateFocused();

	ImGui::Text("Hierarchy View");
	ImGui::Separator();
	
	ImGui::PushID( "EntitySearchInput" );
	if( ImGui::InputText("", search_value_buffer_, IM_ARRAYSIZE(search_value_buffer_) ) )
	{
		search_value_ = search_value_buffer_;
		for( size_t i = 0; i < search_value_.length(); i++ )
		{
			search_value_[i] = std::tolower( search_value_[i], std::locale() );
		}
	}
	ImGui::PopID();

	ImGui::Separator();

	if (std::shared_ptr<Entity> editor_camera = editor_->GetEditorCamera()) {
		ShowEditorCamera();
		ImGui::Separator();
	}

	// Call scene inspect
	if (std::shared_ptr<Scene> scene = GetGame()->GetCurrentScene().lock()) {
		// Set first scene to be highlighted
		ImVec2 pos = ImGui::GetCursorScreenPos();
		ImU32 col = ImColor(ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive]);
		ImGui::RenderFrame(pos, ImVec2(pos.x + ImGui::GetContentRegionMax().x, pos.y + ImGui::GetTextLineHeight()), col, false);

		// Show scene tree node
		if (ImGui::TreeNodeEx(scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
			if(ImGui::BeginPopupContextItem("Scene Context Menu")) {
				if(ImGui::MenuItem("New entity")) {
					scene->open_add_entity_ = true;
					scene->entity_delete_ = std::shared_ptr<Entity>();
					scene->entity_add_scene_ = scene;
				}

				ImGui::EndPopup();
			}
			scene->Inspect(search_value_);
			ImGui::TreePop();
		}
	}

	// Add spacing to the bottom of the hierarchy.
	// To prevent that the last entity is at the very bottom of the hierarchy. (suggested by Channing)
	for(int i = 0; i < hierarchy_bottom_spacing_; ++i) {
		ImGui::Spacing();
	}

	ImGui::End();
	ImGui::PopStyleColor();
}

void iga::Hierarchy::OnOpen() {
}

void iga::Hierarchy::OnClose() {
}

void iga::Hierarchy::SetIgnoreFocused(bool a_ignore_focused) {
	ignore_focused_ = a_ignore_focused;
}

void iga::Hierarchy::ShowEditorCamera() {
	std::shared_ptr<Entity> camera = editor_->GetEditorCamera();

	if (camera) {
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		if (std::shared_ptr<Inspector> inspector = editor_->GetInspector().lock()) {
			if (std::shared_ptr<Entity> selected_entity = inspector->GetSelectedEntity().lock()) {
				if (camera == selected_entity) {
					node_flags |= ImGuiTreeNodeFlags_Selected;
				}
			}
		}

		// Create leaf node
		ImGui::TreeNodeEx(camera->GetName().c_str(), node_flags);

		if (ImGui::IsItemClicked()) {
			if (std::shared_ptr<Inspector> inspector = editor_->GetInspector().lock()) {
				inspector->SetSelectedEntity(camera);
			}
		}
	}
}

#endif