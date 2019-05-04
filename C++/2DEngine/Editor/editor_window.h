#pragma once

#include <defines.h>

#ifdef EDITOR

#include <vectormath/vectormath.hpp>

#include <imgui/include/imgui.h>

#include <string>

/**
* @file editor_window.h
* @author Ben Landor
* @date 08 mar 2018
* @brief iga::EditorWindow class header.
*
* iga::EditorWindow is the abstract base class of all Editor Windows.
* It is responsible for initializing and calling the Window update functions.
*/

/**
*  IGArt namespace
*/
namespace iga {

	class Editor;

	/**
	* iga::EditorWindow is the abstract base class of all Editor Windows.
	*/
	class EditorWindow {
		friend class MenuBar;
	public:
		/**
		*  Default Constructor - sets iga::EditorWindow default values.
		*/
		EditorWindow();
		/**
		*  Virtual Destructor
		*/
		virtual ~EditorWindow();

		/**
		*  Pure virtual function called when editor is initialized.
		*  @see iga::Editor::Init().
		*/
		virtual void OnCreate() = 0;
		/**
		*  Pure virtual function called when editor is updated.
		*  @see iga::Editor::Update().
		*/
		virtual void Update() = 0;

		/**
		*  Sets is_open_ to true and calls virtual function OnOpen().
		*/
		void Open();
		/**
		*  Sets is_open_ to false and calls virtual function OnClose().
		*/
		void Close();

		/**
		*  Pure virtual function called when the window is about to open.
		*/
		virtual void OnOpen() = 0;
		/**
		*  Pure virtual function called when the window is about to close.
		*/
		virtual void OnClose() = 0;

		/**
		*  Updates whether the window is focused or not.
		*  @see Inspector::Update().
		*/
		void UpdateFocused();

		/**
		*  Adds one ImGui flag to the EditorWindow.
		*  @param a_window_flag The ImGuiWindowFlags to be added.
		*/
		void AddWindowFlag(ImGuiWindowFlags a_window_flag);

		/**
		*  @return A bool specifying if the window is open or not.
		*/
		bool IsOpen();
		/**
		*  @return A bool specifying if the window is focused or not.
		*/
		bool IsFocused();
		/**
		*  @return A bool specifying if the window is hovered or not.
		*/
		bool IsHovered();

		/**
		*  Gets the window name.
		*  @return A constant string reference with the window name.
		*/
		const std::string& GetName();
		/**
		*  Sets the window name.
		*  @param a_name is a const std::string reference specifying the window name.
		*/
		void SetName(const std::string& a_name);
		/**
		*  Sets the window size.
		*  @param a_window_size is a const Vector2 reference specifying the window size.
		*/
		void SetWindowSize(const Vector2& a_window_size);
	protected:
		Editor *editor_; /**< Raw pointer to the editor */
		bool is_open_; /**< Bool specifying if the window is open */
		std::string name_; /**< String containing window name */
		Vector2 window_size_; /**< Vector2 containing window size */
		ImGuiWindowFlags window_flags_; /**< Int typedef containing window flags */
		float window_alpha_; /**< Float containing window alpha override */
		bool focused_; /**< A bool specifying if the window is focused */
		bool set_focused_; /**< A bool specifying if the window should be focused next frame */
		bool hovered_; /**< A bool specifying if the window is hovered  */
	};
}

#endif