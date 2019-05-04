#pragma once

#include <defines.h>

#ifdef EDITOR

#include <Editor/editor_window.h>

/**
* @file menu_bar.h
* @author Ben Landor
* @date 08 mar 2018
* @brief iga::MenuBar class header.
*
* iga::MenuBar extends EditorWindow and contains the logic for showing the Menu Bar.
*/

/**
*  IGArt namespace
*/
namespace iga {

	/**
	* iga::MenuBar extends EditorWindow and contains the logic for showing the Menu Bar.
	*/
	class MenuBar : public EditorWindow {
	public:
		/**
		*  Default Constructor - sets default values.
		*/
		MenuBar();
		/**
		*  Default Destructor.
		*/
		virtual ~MenuBar();

		/**
		*  Initializes iga::MenuBar.
		*/
		virtual void OnCreate();
		/**
		*  Updates the iga::MenuBar and shows the menu.
		*  @see iga::Editor.
		*  @see iga::EntityCreationWindow.
		*/
		virtual void Update();

		/**
		*  Virtual function called when the window is about to open.
		*/
		virtual void OnOpen();
		/**
		*  Virtual function called when the window is about to close.
		*/
		virtual void OnClose();
	private:
		/**
		*  Finds all scene names in the scenes asset folder.
		*/
		void LoadScenesNames();

		size_t scene_names_size_; /**< The amount of scene names found in the scenes folder. */

		std::string new_scene_note_; /**< Note to use for feedback when there's something wrong with the new scene name that has been filled in. */

		const char * const new_scene_warning_title_; /**< const c-string that contains the title of the new scene popup */
		const char * const load_scene_warning_title_; /**< const c-string that contains the title of the load scene popup */
		char new_scene_name_[256]; /**< Char array used to read/write scene name from ImGui */
		const char ** loadable_scene_name_list_; /**< array of c-strings that contains all found scene names in the scenes asset folder. Used for ImGui ListComboBox. */

		float averaged_delta_time_; /**< Bool containing the averaged delta time over the last x frames */

		int32 load_scene_current_item; /**< The current item of the the list box to load a scene. */

		bool create_new_scene_; /**< Whether to create a new scene. */
		bool open_new_scene_popup_; /**< Whether the popup for a new scene is opened. */
		bool open_load_scene_popup_; /**< Whether the popup to load a scene is opened. */
		bool fps_enabled_; /**< Bool specifying whether to show the fps */
	};
}

#endif