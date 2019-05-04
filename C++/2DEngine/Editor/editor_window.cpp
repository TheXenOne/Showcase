#include <Editor/editor_window.h>

#ifdef EDITOR

#include <ig_art_engine.h>

iga::EditorWindow::EditorWindow() {
	editor_ = GetEditor();
	is_open_ = false;
	focused_ = false;
	set_focused_ = false;
	hovered_ = false;
	name_ = "";
	window_size_ = Vector2(275.f, 275.f);
	window_flags_ = 0;
	window_alpha_ = -1.f;
}

iga::EditorWindow::~EditorWindow() {
	editor_ = nullptr;
}

void iga::EditorWindow::Open() {
	is_open_ = true;
	OnOpen();
}

void iga::EditorWindow::Close() {
	is_open_ = false;
	OnClose();
}

void iga::EditorWindow::AddWindowFlag(ImGuiWindowFlags a_window_flag) {
	window_flags_ |= a_window_flag;
}

bool iga::EditorWindow::IsOpen() {
	return is_open_;
}

bool iga::EditorWindow::IsFocused() {
	return focused_;
}

bool iga::EditorWindow::IsHovered() {
	return hovered_;
}

const std::string & iga::EditorWindow::GetName() {
	return name_;
}

void iga::EditorWindow::SetName(const std::string& a_name) {
	name_ = a_name;
}

void iga::EditorWindow::SetWindowSize(const Vector2& a_window_size) {
	window_size_ = a_window_size;
}

void iga::EditorWindow::UpdateFocused() {
	focused_ = ImGui::IsRootWindowOrAnyChildFocused();
	
	hovered_ = ImGui::IsRootWindowOrAnyChildHovered();
}

#endif