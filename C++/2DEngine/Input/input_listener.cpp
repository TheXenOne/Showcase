#include <Input/input_listener.h>

#include <ig_art_engine.h>

iga::InputListener::InputListener() {
	GetInput()->Subscribe(this);

	gamepad_ = &GetInput()->GetGamepad();
	kb_m_ = &GetInput()->GetKBM();
}

iga::InputListener::~InputListener() {
	GetInput()->Unsubscribe(this);
}
