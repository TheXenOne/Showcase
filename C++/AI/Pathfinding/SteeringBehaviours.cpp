#include <AI/Pathfinding/SteeringBehaviours.h>

namespace Rebellion
{
	SteeringBehaviours::SteeringBehaviours()
	{

	}

	DirectX::XMVECTOR SteeringBehaviours::Seek(const DirectX::XMVECTOR& a_target, const DirectX::XMVECTOR& a_position, const DirectX::XMVECTOR& a_velocity, float a_maxSpeed, float a_maxForce)
	{
		DirectX::XMVECTOR desiredVelocity = DirectX::XMVectorSubtract(a_target, a_position);
		desiredVelocity = DirectX::XMVector3Normalize(desiredVelocity);
		desiredVelocity = DirectX::XMVectorScale(desiredVelocity, a_maxSpeed);

		DirectX::XMVECTOR steeringForce = DirectX::XMVectorSubtract(desiredVelocity, a_velocity);

		return Truncate(steeringForce, a_maxForce);
	}

	DirectX::XMVECTOR SteeringBehaviours::Arrive(const DirectX::XMVECTOR & a_target, const DirectX::XMVECTOR & a_position, const DirectX::XMVECTOR & a_velocity, float a_maxSpeed, float a_maxForce)
	{
		DirectX::XMVECTOR desiredVelocity = DirectX::XMVectorSubtract(a_target, a_position);
		float desiredVelocityLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(desiredVelocity));
		desiredVelocity = DirectX::XMVector3Normalize(desiredVelocity);

		float slowingRadius = 25.f;

		if (desiredVelocityLength < slowingRadius)
		{
			desiredVelocity = DirectX::XMVectorScale(desiredVelocity, (a_maxSpeed * (desiredVelocityLength / slowingRadius)));
		}
		else
		{
			desiredVelocity = DirectX::XMVectorScale(desiredVelocity, a_maxSpeed);
		}

		DirectX::XMVECTOR steeringForce = DirectX::XMVectorSubtract(desiredVelocity, a_velocity);

		return Truncate(steeringForce, a_maxForce);
	}

	DirectX::XMVECTOR SteeringBehaviours::ArriveOnPath(const DirectX::XMVECTOR & a_finalDestination, const DirectX::XMVECTOR & a_target, const DirectX::XMVECTOR & a_position, const DirectX::XMVECTOR & a_velocity, float a_maxSpeed, float a_maxForce)
	{
		DirectX::XMVECTOR desiredVelocity = DirectX::XMVectorSubtract(a_target, a_position);
		desiredVelocity = DirectX::XMVector3Normalize(desiredVelocity);

		DirectX::XMVECTOR distanceToDestination = DirectX::XMVectorSubtract(a_finalDestination, a_position);
		float distanceToDestinationLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(distanceToDestination));

		float slowingRadius = 70.f;

		if (distanceToDestinationLength < slowingRadius)
		{
			desiredVelocity = DirectX::XMVectorScale(desiredVelocity, (a_maxSpeed * (distanceToDestinationLength / slowingRadius)));
		}
		else
		{
			desiredVelocity = DirectX::XMVectorScale(desiredVelocity, a_maxSpeed);
		}

		DirectX::XMVECTOR steeringForce = DirectX::XMVectorSubtract(desiredVelocity, a_velocity);

		return Truncate(steeringForce, a_maxForce);
	}

	DirectX::XMVECTOR SteeringBehaviours::Truncate(DirectX::XMVECTOR a_vector, float a_value)
	{
		float vectorLength = DirectX::XMVectorGetX(DirectX::XMVector3Length(a_vector));

		if (vectorLength > a_value)
		{
			a_vector = DirectX::XMVector3Normalize(a_vector);
			a_vector = DirectX::XMVectorScale(a_vector, a_value);
		}

		return a_vector;
	}
}