#pragma once

#include <defines.h>

#ifdef EDITOR

#include <memory>
#include <string>
#include <vector>

#include <vectormath/vectormath.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

/**
* @file editor.h
* @author Ben Landor
* @date 06 mar 2018
* @brief iga::Editor class header.
*
* iga::Editor contains all of the Editor Window objects, and updates them every frame.
*/

struct ImFont;

/**
*  IGArt namespace
*/
namespace iga {

    /**
    *   Behaviour Tree Editor namspace
    */
    namespace bte {
        class BehaviourTreeEditor;
    }
	
	class Entity;
	class MenuBar;
	class Inspector;
	class Hierarchy;
	class Profiler;
	class EditorWindow;
	class PlayStateWindow;
	class AssetDropInHandler;
	class GraphicsEditorTools;
	class CollisionRenderer;
	class EditorOptions;

	/**
	* iga::Editor contains all of the Editor Window objects, and updates them every frame.
	*/
	class Editor final {
		// Give MenuBar access to Editor class so it can change renderer_enabled_
		friend MenuBar;
		friend EditorOptions;
		friend class Application;

	public:
		/**
		*  The data that Editor saves/loads.
		*/
		class Settings {

			friend Editor;
			friend EditorOptions;
		public:
			/**
			*   Cereal versions of iga::Settings
			*/
			enum class Version {
				DEFAULT = 0, /**< Default normal version. */
				LATEST /**< Latest version. */
			};

			/**
			*  Used to save and load iga::Game::Settings
			*/
			template<class Archive>
			void serialize(Archive &a_archive, uint32 const a_version);

		private:
			std::string boot_scene; /**< The scene that the application uses to boot. */

		};

		/**
		*  Default Constructor - sets iga::Editor default values.
		*/
		Editor();
		/**
		*  Default Destructor.
		*/
		~Editor();

		/**
		*  Initialize the editor by creating the window objects and calling their OnCreate function.
		*  @see iga::EditorWindow.
		*/
		void Init();
		/**
		*  Loops through all windows, first Opening, then Updating, and finally closing.
		*  @see iga::EditorWindow.
		*/
		void Update();

		/**
		*  Enables the editor.
		*/
		void EnableEditor();
		/**
		*  Disables the editor.
		*/
		void DisableEditor();
        	/**
        	*   Returns boolean whether the editor is enabled.
        	*   @return Is True ~  If the editor is enabled. Is False ~  If the editor is disabled.
        	*/
        	bool IsEditorEnabled();

		/**
		*  Gets the Behaviour Tree Editor window object.
		*  @return The Behaviour Tree Editor window object.
		*/
		std::weak_ptr<bte::BehaviourTreeEditor> GetBehaviourTreeEditor();
		/**
		*  Gets the Menu Bar window object.
		*  @return The Menu Bar window object.
		*/
		std::weak_ptr<MenuBar> GetMenuBar();
		/**
		*  Gets the Inspector window object.
		*  @return The Inspector window object.
		*/
		std::weak_ptr<Inspector> GetInspector();
		/**
		*  Gets the Hierarchy window object.
		*  @return The Hierarchy window object.
		*/
		std::weak_ptr<Hierarchy> GetHierarchy();
		/**
		*  Gets the Profiler window object.
		*  @return The Profiler window object.
		*/
		std::weak_ptr<Profiler> GetProfiler();

		/**
		*  Gets the AssetDropInHandler object.
		*  @return The AssetDropInHandler object.
		*/
		std::weak_ptr<AssetDropInHandler> GetAssetDropInHandler();

		/**
		*  Gets the CollisionRenderer object.
		*  @return The CollisionRenderer object.
		*  @see iga::CollisionRenderer
		*/
		std::weak_ptr<CollisionRenderer> GetCollisionRenderer();

		/**
		*  Gets the EditorOptions object.
		*  @return The EditorOptions object.
		*  @see iga::EditorOptions
		*/
		std::weak_ptr<EditorOptions> GetEditorOptions();

		/**
		*  Gets if any window is focused apart from the menu bar.
		*  @return A bool specifying if any window is focused apart from the menu bar.
		*  @see iga::EditorWindow.
		*/
		bool IsAnyWindowFocused();
		/**
		*  Gets if any window is hovered apart from the menu bar.
		*  @return A bool specifying if any window is hovered apart from the menu bar.
		*  @see iga::EditorWindow.
		*/
		bool IsAnyWindowHovered();

		/**
		*  Specifies that a sub-window has been focused. Reset every frame.
		*/
		void SubWindowFocused();
		/**
		*  Specifies that a Gizmo has been focused. Reset every frame.
		*/
		void GizmoFocused();
		/**
		*  Called when the game is about to resume playing.
		*/
		void OnGamePlay();
		/**
		*  Called when the game is about to be paused.
		*/
		void OnGamePause();
		/**
		*  Gets the iga::Entity containing the iga::Editors camera.
		*  @return A shared pointer to the camera iga::Entity.
		*/
		std::shared_ptr<Entity> GetEditorCamera();

		/**
		*  Sets the custom font.
		*  Can only be set/used once.
		*  @param a_font The new custom font of type ImFont*
		*/
		void SetCustomFont(ImFont *a_font);

		/**
		*  Gets the custom font.
		*  @return The current custom font of type ImFont*
		*/
		ImFont * GetCustomFont();

		/**
		*  Get the settings of the editor.
		*  @return A const reference to the settings of the game.
		*/
		const Settings& GetSettings() const;

	private:
		/**
		*  Shows the renderer window.
		*  @see RVX::RendererD3D12.
		*/
		void ShowRendererWindow();
		/**
		*  Shows the demo imgui window.
		*/
		void ShowDemoWindow();
        	/**
        	*  Sets the style of ImGui.
        	*/
        	void SetImGuiStyle();
		/**
		*  Processes Input for Editor Camera.
		*/
		void ProcessEditorCamera();

		/**
		*  Load and set the editor settings from the editor file.
		*/
		void LoadSettings();

		bool editor_enabled_; /**< Bool specifying if the editor is active */
		bool renderer_enabled_; /**< Bool specifying if the renderer window should be shown */
		bool show_demo_window_; /**< Bool specifying if the demo window should be shown */
		bool sub_window_focused_; /**< Bool specifying if a sub window is focused */
		bool gizmo_focused_; /**< Bool specifying if a gizmo is focused */

		float duplicate_timer_;  /**< timer keeps track of the duplicate state. */

		Vectormath::Vector2 mouse_previous_pos_; /**< Vector2 keeping track of the previous mouse position, (for delta purposes) */

		Settings settings_; /**< The settings of the editor. */

		std::shared_ptr<Entity> editor_camera_; /**< Shared pointer to the camera iga::Entity */
		std::vector<std::weak_ptr<EditorWindow>> editor_windows_; /**< Vector of weak pointers to the EditorWindows */
		std::shared_ptr<bte::BehaviourTreeEditor> behaviour_tree_; /**< Shared pointer to the Behaviour Tree Editor Window */
		std::shared_ptr<MenuBar> menu_bar_; /**< Shared pointer to the Menu Bar Window */
		std::shared_ptr<Inspector> inspector_; /**< Shared pointer to the Inspector Window */
		std::shared_ptr<Hierarchy> hierarchy_; /**< Shared pointer to the Hierarchy Window */
		std::shared_ptr<Profiler> profiler_; /**< Shared pointer to the Profiler Window */
		std::shared_ptr<PlayStateWindow> play_state_window_; /**< Shared pointer to the Play State Window */
		std::shared_ptr<AssetDropInHandler> asset_drop_in_handler_; /**< Shared pointer to the Asset drop in handler */
		std::shared_ptr<GraphicsEditorTools> graphics_editor_tools_; /**< Shared pointer to graphical_editor_tools (2D boxes)*/
		std::shared_ptr<CollisionRenderer> collision_renderer_; /** Shared pointer to the collision renderer */
		std::shared_ptr<EditorOptions> editor_options_window_; /** Shared pointer to the editor options Window */

		ImFont* custom_font_; /**< Will hold the custom font for ImGui */
	};

	template<class Archive>
	inline void Editor::Settings::serialize(Archive &a_archive, uint32 const a_version) {
		switch(static_cast<Version>(a_version)) {
			case Version::LATEST:
			case Version::DEFAULT:
				a_archive(cereal::make_nvp("boot_scene", boot_scene));
				break;
		}
	}

	inline const Editor::Settings & Editor::GetSettings() const {
		return settings_;
	}

}

CEREAL_CLASS_VERSION(iga::Editor::Settings, (static_cast<iga::uint32>(iga::Editor::Settings::Version::LATEST) - 1));

#endif
