#pragma once

#include <defines.h>
#include <Input/gamepad.h>
#include <Input/kb_m.h>

#include <vector>

/**
* @file input.h
* @author Ben Landor
* @author Meine Zeinstra
* @date 27 feb 2018
* @brief iga::Input class header.
*
* iga::Input is an object that holds the current values of input data.
* iga::Input will return the value of keys, or a given axis, when queried.
* iga::Input can also be subscribed to by an iga::InputListener.
*/

/**
*  IGArt namespace
*/
namespace iga {

	class InputListener;

	/**
	*  iga::Input class. This provides input for controllers.
	*/
	class Input final {

		friend class Application;

	public:

		/**
		*  Subscribes an iga::InputListener to the Input class to be notified on button changes.
		*  This adds it to a vector of pointers to iga::InputListeners.
		*  @param a_listener is a pointer to the iga::InputListener that is to be subscribed to the Input class.
		*/
		void Subscribe(InputListener *a_listener);

		/**
		*  Unsubscribes an iga::InputListener from the Input class to no longer be notified on button changes.
		*  This removes it from a vector of pointers to iga::InputListeners.
		*  @param a_listener is a pointer to the iga::InputListener that is to be unsubscribed from the Input class.
		*/
		void Unsubscribe(InputListener *a_listener);

		/**
		*  Gets the gamepad.
		*  @return A const Gamepad refrence to the gamepad object from Input.
		*  @see iga::Gamepad
		*/
		const Gamepad& GetGamepad() const;

		/**
		*  Gets the keyboard and mouse input object.
		*  @return A const KBM refrence to the keyboard and mouse object from Input.
		*  @see iga::KBM
		*/
		const KBM& GetKBM() const;

	private:
		/**
		*  Constructor.
		*/
		Input();
		/**
		*  Destructor.
		*/
		~Input();

		/**
		*  Initializes the iga::Input class.
		*/
		void Init();
		/**
		*  Updates the iga::Input class.
		*/
		void Update();

		Gamepad gamepad_; /**< Gamepad that holds and updates state of buttons and axes of the controller. @see iga::Gamepad */
		KBM kb_m_; /**< Keyboard and mouse input object. @see iga::KBM */
		std::vector<InputListener*> listener_vector_; /**< Vector of input listeners. */
	};

}
