#include <Time/time.h>

#include <iostream>

iga::Time::Time() {
	engine_delta_time_ = 0.f;
	real_time_elapsed_ = 0.f;
	game_delta_time_ = 0.f;
	game_time_elapsed_ = 0.f;
	game_time_multiplier_ = 1.f;

	clock_ = std::chrono::high_resolution_clock();
}

iga::Time::~Time() {
}

void iga::Time::InitDeltaTime() {
	last_frame_ = clock_.now();
}

void iga::Time::UpdateDeltaTime() {
	// Calculate the delta time using high resolution clock
	current_frame_ = clock_.now();
	engine_delta_time_ = std::chrono::duration_cast<std::chrono::duration<float>>(current_frame_ - last_frame_).count();

	if (engine_delta_time_ > max_delta_time_) {
		engine_delta_time_ = max_delta_time_;
	}

	last_frame_ = current_frame_;
	game_delta_time_ = engine_delta_time_ * game_time_multiplier_;

	// Calculate the real time elapsed
	real_time_elapsed_ += engine_delta_time_;
	// Calculate the game time elapsed based on multiplier
	game_time_elapsed_ += engine_delta_time_ * game_time_multiplier_;
}

const float iga::Time::GetGameDeltaTime() const {
	return game_delta_time_;
}

const float iga::Time::GetGameTimeElapsed() const {
	return game_time_elapsed_;
}

const float iga::Time::GetEngineDeltaTime() const {
	return engine_delta_time_;
}

const float iga::Time::GetRealTimeElapsed() const {
	return real_time_elapsed_;
}

const float iga::Time::GetGameTimeMultiplier() const {
	return game_time_multiplier_;
}

void iga::Time::SetGameTimeMultiplier(const float a_time_multiplier) {
	if(game_time_multiplier_ < 0.f) {
		game_time_multiplier_ = 0.f;
		return;
	}
	game_time_multiplier_ = a_time_multiplier;
}