#pragma once

#include <chrono>

/**
* @file time.h
* @author Ben Landor
* @date 02 mar 2018
* @brief iga::Time class header.
*
* iga::Time is class that keeps track of delta time and elapsed time for the engine and game.
* The game delta time is simply the engine time multiplied by a float.
*/

/**
*  IGArt namespace
*/
namespace iga {

	/**
	*  iga::Time is keeps track of both delta time and elapsed time for the game and engine.
	*/
	class Time {
	public:
		/**
		*  Default constructor.
		*/
		Time();
		/**
		*  Destructor.
		*/
		~Time();

		/**
		*  Initializes the delta time, starts timer for elapsed time.
		*/
		void InitDeltaTime();
		/**
		*  Updates the delta time for this frame.
		*/
		void UpdateDeltaTime();

		/**
		*  Gets the delta time to be used in game.
		*  @return A const float containing the delta time.
		*/
		const float GetGameDeltaTime() const;
		/**
		*  Gets the time elapsed in the game.
		*  @return A const float containing the elapsed time.
		*/
		const float GetGameTimeElapsed() const;
		/**
		*  Gets the real delta time for this frame in the engine loop.
		*  @return A const float containing the delta time.
		*/
		const float GetEngineDeltaTime() const;
		/**
		*  Gets the real time elapsed in the engine.
		*  @return A const float containing the elapsed time.
		*/
		const float GetRealTimeElapsed() const;

		/**
		*  Gets the current time scale scalar.
		*  @return A const float containing the game time scale time.
		*/
		const float GetGameTimeMultiplier() const;
		/**
		*  Sets the time scale scalar.
		*  @param a_time_multiplier A const float containing the new game time scale time.
		*/
		void SetGameTimeMultiplier(const float a_time_multiplier);

	private:
		float game_delta_time_; /**< The delta time to be used in the game. */
		float game_time_elapsed_; /**< The total time elapsed in the game. */
		float game_time_multiplier_; /**< The multiplier for the game delta time. */
		float engine_delta_time_; /**< The delta time of the engine in real time. */
		float real_time_elapsed_; /**< The total time elapsed since engine start. */
		const float max_delta_time_ = 1.f / 10.f; /**< The maximum delta time for the engine. Calculated as 1 / minimum fps. */

		std::chrono::high_resolution_clock clock_; /**< The high resolution clock used for calculating frame times */
		std::chrono::high_resolution_clock::time_point current_frame_; /**< The time point for the current frame. */
		std::chrono::high_resolution_clock::time_point last_frame_; /**< The time point for the last frame. */
	};

}