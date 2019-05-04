#pragma once

#include <DirectXMath.h>

namespace Rebellion
{
	class SteeringBehaviours
	{
	public:
		SteeringBehaviours();

		static DirectX::XMVECTOR Seek(const DirectX::XMVECTOR& a_target, const DirectX::XMVECTOR& a_position, const DirectX::XMVECTOR& a_velocity,
										float a_maxSpeed, float a_maxForce); //Returns steering force towards target from given position

		static DirectX::XMVECTOR Arrive(const DirectX::XMVECTOR& a_target, const DirectX::XMVECTOR& a_position, const DirectX::XMVECTOR& a_velocity,
			float a_maxSpeed, float a_maxForce); //Returns steering force towards target from given position

		static DirectX::XMVECTOR ArriveOnPath(const DirectX::XMVECTOR& a_finalDestination, const DirectX::XMVECTOR& a_target, const DirectX::XMVECTOR& a_position, const DirectX::XMVECTOR& a_velocity,
			float a_maxSpeed, float a_maxForce); //Returns steering force towards target from given position

		static DirectX::XMVECTOR Truncate(DirectX::XMVECTOR a_vector, float a_value); //Truncates vector to value

	private:

	};
}