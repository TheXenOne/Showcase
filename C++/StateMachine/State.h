#pragma once

template <class entity_type>
class State
{
public:
	virtual ~State() {};

	virtual void Enter(entity_type *) = 0;

	virtual void Execute(entity_type *, float a_deltaTime) = 0;

	virtual void Exit(entity_type *) = 0;
};