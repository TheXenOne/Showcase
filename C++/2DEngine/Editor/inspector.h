#pragma once

#include <defines.h>

#ifdef EDITOR

#include <Editor/editor_window.h>
#include <Core/scene.h>

#include <ImGuizmo/ImGuizmo.h>

#include <memory>

/**
* @file inspector.h
* @author Ben Landor
* @date 08 mar 2018
* @brief iga::Inspector class header.
*
* iga::Inspector extends EditorWindow and contains the logic for showing the Inspector Window.
*/

/**
*  IGArt namespace
*/
namespace iga {

	class Entity;

	/**
	* iga::Inspector extends EditorWindow and contains the logic for showing the Inspector Window.
	*/
	class Inspector : public EditorWindow {
	public:
		/**
		*  Default Constructor - sets default values.
		*/
		Inspector();
		/**
		*  Default Destructor.
		*/
		~Inspector();

		/**
		*  Initializes iga::Inspector.
		*/
		virtual void OnCreate();
		/**
		*  Updates the iga::Inspector and shows the Window.
		*  @see iga::Entity.
		*  @see iga::Component.
		*  @see iga::Input.
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
		*  Gets the iga::Entity to be inspected.
		*  @return A weak pointer to the selected iga::Entity.
		*/
		std::weak_ptr<Entity> GetSelectedEntity();
		/**
		*  Sets the iga::Entity to be inspected.
		*  @param a_entity A weak pointer to the selected iga::Entity.
		*  @see iga::Entity.
		*/
		void SetSelectedEntity(std::weak_ptr<Entity> a_entity);
	protected:

	private:
		/**
		*  Processes input to raycast and select an iga::Entity in the scene.
		*/
		void ProcessSelectEntity();
		/**
		*  Processes input to change Gizmo mode and displays/receives input from Gizmo.
		*/
		void ProcessGizmo();
		/**
		*  Loads an array of floats from a Matrix4
		*  @param a_matrix A Matrix4 to be loaded.
		*  @param a_first_matrix_element A pointer to the first element of the float array.
		*/
		void LoadFloatMatrixFromMath(const Matrix4& a_matrix, float *a_first_matrix_element);
		/**
		*  Loads a Matrix4 from an array of floats
		*  @param a_float_matrix A pointer to the float array.
		*  @param a_matrix A Matrix4 to be loaded.
		*/
		void LoadMatrixFromFloat(Matrix4 &a_matrix, float *a_float_matrix);

		ImGuizmo::OPERATION gizmo_operation_; /**< Operation mode for Gizmo of selected entity */
		std::weak_ptr<Entity> selected_entity_; /**< Weak pointer to the currently selected iga::Entity to inspect */
		char entity_name_buffer_[256] = ""; /**< Char array used to read/write entity name from ImGui */
	};
}

#endif