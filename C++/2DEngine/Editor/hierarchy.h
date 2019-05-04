#pragma once

#include <defines.h>

#ifdef EDITOR

#include <Editor/editor_window.h>

#include <memory>

/**
* @file hierarchy.h
* @author Ben Landor
* @date 08 mar 2018
* @brief iga::Hierarchy class header.
*
* iga::Hierarchy extends EditorWindow and contains the logic for showing the Hierarchy Window.
*/

/**
*  IGArt namespace
*/
namespace iga {

	class Scene;

	/**
	* iga::Inspector extends EditorWindow and contains the logic for showing the Hierarchy Window.
	*/
	class Hierarchy : public EditorWindow {
	public:
		/**
		*  Public constructor
		*/
		Hierarchy();

		/**
		*  Initializes iga::Hierarchy.
		*/
		virtual void OnCreate();
		/**
		*  Updates the iga::Hierarchy and shows the Window.
		*  @see iga::Scene.
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
		/**
		*  Sets  whether the window should show regardless of focus.
		*  @param a_ignore_focused A bool specifying whether the window should show regardless of focus.
		*/
		void SetIgnoreFocused(bool a_ignore_focused);
	protected:

	private:
		/**
		*  Displays the iga::Editor camera in the hierarchy so it can be selected and viewed.
		*/
		void ShowEditorCamera();

		bool ignore_focused_; /**< Bool specifying whether the window should show regardless of focus */
		const int32 hierarchy_bottom_spacing_; /**< const integer specifying how much empty spacing there should be at the bottom of the hierarchy. */

		std::string search_value_; /**< value used to filter entities in the hierarchy*/
		char search_value_buffer_[256] = ""; /**< value used for imgui textfield*/
	};
}

#endif