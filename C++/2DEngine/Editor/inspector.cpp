#include <Editor/inspector.h>

#ifdef EDITOR

#include <Core/entity.h>
#include <Core/component.h>
#include <Core/ray.h>
#include <Input/kb_m.h>
#include <ig_art_engine.h>
#include <defines.h>

#include <vector>

iga::Inspector::Inspector() {
}

iga::Inspector::~Inspector() {
}

void iga::Inspector::OnCreate() {
	is_open_ = true;
	window_flags_ = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;
	window_size_ = Vector2(window_size_.getX(), ImGui::GetIO().DisplaySize.y - 10.f);
	name_ = "Inspector";
	gizmo_operation_ = ImGuizmo::TRANSLATE;
}

void iga::Inspector::Update() {
	ProcessGizmo();

	// Set window position
	ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - window_size_.getX(), 22.f);
	ImVec2 window_pos_pivot = ImVec2(0.f, 0.f);
	ImGui::SetNextWindowPos(window_pos, ImGuiSetCond_Always);  // ImGui 1.50 code
	ImGui::SetNextWindowSize(ImVec2(window_size_.getX(), ImGui::GetIO().DisplaySize.y - 10.f), ImGuiSetCond_Always); // ImGui 1.50 code

	// Set window colour
	if (focused_) {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_WindowBg].x, ImGui::GetStyle().Colors[ImGuiCol_WindowBg].y, ImGui::GetStyle().Colors[ImGuiCol_WindowBg].z, 1.0f)); // Dark background
	}
	else {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(ImGui::GetStyle().Colors[ImGuiCol_WindowBg].x, ImGui::GetStyle().Colors[ImGuiCol_WindowBg].y, ImGui::GetStyle().Colors[ImGuiCol_WindowBg].z, 0.4f)); // Transparent background
	}

	// Start window
	ImGui::Begin(name_.c_str(), &is_open_, window_flags_);

	UpdateFocused();

	// Show entity name next to inspector
	ImGui::Text("Inspector");
	if (std::shared_ptr<Entity> entity = selected_entity_.lock()) {
		ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue;

		ImGui::SameLine(0.f, 0.f);
		ImGui::Text(":");
		ImGui::SameLine(0.f, 10.f);

		// Input text for entity name
		if (ImGui::InputText("EntityNameInput", entity_name_buffer_, IM_ARRAYSIZE(entity_name_buffer_), input_text_flags)) {
			std::string name = entity_name_buffer_;
			entity->SetName(name);
		}
	}

	ImGui::Separator();

	// Show context menu
	if (ImGui::BeginPopupContextWindow()) {
		if (is_open_ && ImGui::MenuItem("Close")) is_open_ = false;
		ImGui::EndPopup();
	}

	// Inpsect entity
	if (std::shared_ptr<Entity> entity = selected_entity_.lock()) {
		entity->Inspect();

		// Add Component Create Button.
		ImGui::Indent(ImGui::GetContentRegionMax().x * 0.21f);
		if (ImGui::Button("Create Component", ImVec2(150, 30))) {
			ImGui::OpenPopup("Components");
		}
		ImGui::Unindent();
	}

	// Show component popup
	if (ImGui::BeginPopup("Components")) {
		// Make sure editor doesn't lose focus
		editor_->SubWindowFocused();
		// Show creatable components
		for (auto comp : *StaticComponentVector::component_vector_) {
			if (ImGui::Selectable(comp.first.c_str())) {
				if (std::shared_ptr<Entity> entity = selected_entity_.lock()) {
					Entity* ent_ptr = &*entity;
					comp.second(ent_ptr);
				}
			}
		}
		ImGui::EndPopup();
	}

	ImGui::End();
	ImGui::PopStyleColor();

	ProcessSelectEntity();
}

void iga::Inspector::OnOpen() {
}

void iga::Inspector::OnClose() {
}

std::weak_ptr<iga::Entity> iga::Inspector::GetSelectedEntity() {
	return selected_entity_;
}

void iga::Inspector::SetSelectedEntity(std::weak_ptr<Entity> a_entity) {
	// Set selected entity
	selected_entity_ = a_entity;
	if (std::shared_ptr<Entity> entity = a_entity.lock()) {
		ImGuizmo::Enable(true);
		// Check entity is valid
		char buffer[256] = "";
		const std::string& name = entity->GetName();
		size_t name_length = name.length();

		// Set entity name buffer to current name and remove any blank chars
		for (size_t i = 0; i < 256; ++i) {
			if (i < name_length) {
				entity_name_buffer_[i] = name.c_str()[i];
			}
			else {
				entity_name_buffer_[i] = buffer[i];
			}
		}
	}
	else {
		ImGuizmo::Enable(false);
	}
}

void iga::Inspector::ProcessSelectEntity() {
	const KBM& kbm = GetInput()->GetKBM();

	if (kbm.IsKeyPressed(Key::DEL) && !ImGui::GetIO().WantCaptureKeyboard) {
		if (std::shared_ptr<Entity> entity = selected_entity_.lock()) {
			if (entity != editor_->GetEditorCamera()) {
				if (auto scene = entity->GetScene().lock()) {
					scene->DestroyEntity(entity);
				}
			}
		}
	}

	if (kbm.IsKeyPressed(Key::F)) {
		if (std::shared_ptr<Entity> entity = selected_entity_.lock()) {
			if (auto transform = editor_->GetEditorCamera()->GetTransform().lock()) {
				if (auto entity_transform = entity->GetTransform().lock()) {
					Vector3 entity_pos = entity_transform->GetWorldPosition();
					Vector3 new_pos = Vector3(entity_pos.getX(), entity_pos.getY(), transform->GetWorldPosition().getZ());
					transform->SetPosition(new_pos);
				}
			}
		}
	}

	// If mouse is clicked and not on an open window
	if (kbm.IsMouseClicked(iga::MouseButton::LEFT) && !editor_->IsAnyWindowHovered()) {
		if (std::shared_ptr<CameraComponent> camera = GetRenderer()->GetActiveCamera().lock()) {
			// Raycast from mouse click position
			Ray ray;
			Vector2 mouse_pos = kbm.GetMousePos();
			if (GetRaycastSystem()->ScreenPointToRay(mouse_pos.getX(), mouse_pos.getY(), ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, ray)) {
				// If entity hit
				if (std::shared_ptr<Entity> entity_selected = GetRaycastSystem()->GetEntityHitByRay(ray).lock()) {
					// Set it to selected
					SetSelectedEntity(entity_selected);
				}
				else {
					// Otherwise select nothing
					SetSelectedEntity(std::weak_ptr<Entity>());
				}
			}
		}
	}
}

void iga::Inspector::ProcessGizmo() {
	// Change gizmo mode based on input
	const KBM& kbm = GetInput()->GetKBM();
	if (kbm.IsKeyPressed(iga::Key::E)) {
		gizmo_operation_ = ImGuizmo::OPERATION::TRANSLATE;
	}
	else if (kbm.IsKeyPressed(iga::Key::R)) {
		gizmo_operation_ = ImGuizmo::OPERATION::ROTATE;
	}
	else if (kbm.IsKeyPressed(iga::Key::T)) {
		gizmo_operation_ = ImGuizmo::OPERATION::SCALE;
	}

	// If we can get valid entity transform and camera
	if (std::shared_ptr<Entity> selected_entity = selected_entity_.lock()) {
		if (std::shared_ptr<Transform> entity_transform = selected_entity->GetTransform().lock()) {
			if (std::shared_ptr<CameraComponent> camera = GetRenderer()->GetActiveCamera().lock()) {
				// Create matrix in ImGuizmo format
				float view[16] = { 0.f };
				float projection[16] = { 0.f };
				float matrix[16] = { 0.f };

				LoadFloatMatrixFromMath(camera->GetView(), &view[0]);
				LoadFloatMatrixFromMath(camera->GetProjection(), &projection[0]);
				LoadFloatMatrixFromMath(entity_transform->GetTransform(), &matrix[0]);

				ImGuizmo::MODE mode = ImGuizmo::WORLD;

				// Set display size for ImGuizmo
				ImGuiIO& io = ImGui::GetIO();
				ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
				// Use gizmo
				ImGuizmo::Manipulate(&view[0], &projection[0], gizmo_operation_, mode, &matrix[0], NULL, NULL, NULL);

				// If it has been used, change transform
				if (ImGuizmo::IsUsing()) {
					editor_->GizmoFocused();
					float translation[3], rotation[3], scale_f[3];
					ImGuizmo::DecomposeMatrixToComponents(&matrix[0], &translation[0], &rotation[0], &scale_f[0]);

					if (gizmo_operation_ == ImGuizmo::TRANSLATE) {
						Vector3 worldpos = entity_transform->GetWorldPosition();
						Vector3 trans = Vector3(translation[0], translation[1], translation[2]);
						Vector3 difference = trans - worldpos;
						if (selected_entity->HasParent()) {
							difference.setX((difference.getX() / entity_transform->GetWorldScale().getX()));
							difference.setY((difference.getY() / entity_transform->GetWorldScale().getY()));
						}
						entity_transform->Move(difference);
					}

					if (gizmo_operation_ == ImGuizmo::ROTATE) {
						entity_transform->SetRotation(rotation[2]);
					}

					if (gizmo_operation_ == ImGuizmo::SCALE) {
						Vector3 difference = Vector3(scale_f[0], scale_f[1], scale_f[2]);
						Vector3 scale = entity_transform->GetWorldScale();
						Vector3 entity_scale = entity_transform->GetScale();
						difference.setX((difference.getX() / scale.getX() * entity_scale.getX()));
						difference.setY((difference.getY() / scale.getY() * entity_scale.getY()));
						difference.setZ((difference.getZ() / scale.getZ() * entity_scale.getZ()));
						entity_transform->SetScale(difference);
					}
				}

				// Make sure gizmo doesn't lose focus if hovered
				if (ImGuizmo::IsOver()) {
					editor_->GizmoFocused();
				}
			}
		}
	}
	else {
		// Disable Gizmos if no entity selected
		ImGuizmo::Enable(false);
	}
}

void iga::Inspector::LoadFloatMatrixFromMath(const Matrix4& a_matrix, float *a_first_matrix_element) {
	// Loop through matrix
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			// Set element at row i, column j
			a_first_matrix_element[(i * 4) + j] = a_matrix.getElem(i, j);
		}
	}
}

void iga::Inspector::LoadMatrixFromFloat(Matrix4 & a_matrix, float * a_float_matrix) {
	// Loop through float matrix
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			// Set element and column i, crow j
			a_matrix.setElem(i, j, a_float_matrix[(i * 4) + j]);
		}
	}
}

#endif