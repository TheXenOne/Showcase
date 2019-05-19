#include <Input/input.h>
#include <Input/input_listener.h>

namespace iga {
	Input::Input() {

	}

	Input::~Input() {
		gamepad_.Destroy();
	}

	void iga::Input::Init() {
		gamepad_.Init();
	}

	void iga::Input::Update() {
		kb_m_.Update();

		if(gamepad_.gamepad_state_.prev_connection_state_ != gamepad_.gamepad_state_.connection_state_) {
			for(auto listener : listener_vector_) {
				listener->OnGamepadConnectionStateChanged(gamepad_.gamepad_state_.connection_state_);
			}
		}

		gamepad_.Update();
	}

	const Gamepad & iga::Input::GetGamepad() const {
		return gamepad_;
	}

	const KBM & Input::GetKBM() const {
		return kb_m_;
	}

	void iga::Input::Subscribe(InputListener *a_listener) {
		for(auto it : listener_vector_) {
			if(it == a_listener) {
				return;
			}
		}

		listener_vector_.push_back(a_listener);
	}

	void iga::Input::Unsubscribe(InputListener *a_listener) {
		for(auto it = listener_vector_.begin(); it != listener_vector_.end(); ++it) {
			if(*it == a_listener) {
				listener_vector_.erase(it);
				break;
			}
		}
	}

}
