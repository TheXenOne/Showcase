#pragma once

#include <defines.h>
#include <Input/gamepad.h>

/**
* @file input_listener.h
* @author Ben Landor
* @date 27 feb 2018
* @brief iga::InputListener class header.
*
* iga::InputListener is a listener for the input class, which it must subscribe to.
* A class can inherit from this to be notified of button changes.
*/

/**
*  IGArt namespace
*/
namespace iga {

	class Gamepad;
	class KBM;

	/**
	*  iga::InputListener class. This is notified of button changes from the Input class.
	*/
	class InputListener {
	public:
		/**
		* Constructor. Subscribes itself to iga::Input.
		*/
		InputListener();
		/**
		* Destructor. Unsubscribes itself from iga::Input.
		*/
		virtual ~InputListener();

		/**
		*  This is called when a button changes in the Input class.
		*  @param a_key is a uin16 containing the id of the key that was changed
		*/
		virtual void OnGamepadConnectionStateChanged(ConnectionState a_connection_state) = 0;

	protected:

		const Gamepad* gamepad_;
		const KBM* kb_m_;

	};

}