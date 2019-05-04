#pragma once

#include "State.h"
#include <assert.h>

template <class entity_type>
class StateMachine
{
public:
	StateMachine(entity_type *a_owner) : m_Owner(a_owner),
		m_CurrentState(nullptr),
		m_PreviousState(nullptr),
		m_GlobalState(nullptr)
	{}

	virtual ~StateMachine() {}

	void Update(float a_deltaTime)
	{
		if (m_GlobalState)
		{
			m_GlobalState->Execute(m_Owner, a_deltaTime);
		}

		if (m_CurrentState)
		{
			m_CurrentState->Execute(m_Owner, a_deltaTime);
		}
	}

	void ChangeState(State<entity_type> *a_newState)
	{
		assert(a_newState && "<StateMachine::ChangeState>: trying to change to null state.");

		m_PreviousState = m_CurrentState;

		m_CurrentState->Exit(m_Owner);

		m_CurrentState = a_newState;

		m_CurrentState->Enter(m_Owner);
	}

	inline void	RevertToPreviousState()
	{
		ChangeState(m_PreviousState);
	}

	bool GetIsInState(const State<entity_type> &a_state)
	{
		if (m_CurrentState == a_state)
		{
			return true;
		}

		return false;
	}

	virtual State<entity_type>* GetState() { return m_CurrentState; }
	void SetCurrentState(State<entity_type> *a_state) { m_CurrentState = a_state; }
	void SetGlobalState(State<entity_type> *a_state) { m_GlobalState = a_state; }
	void SetPreviousState(State<entity_type> *a_state) { m_PreviousState = a_state; }
protected:
	StateMachine() {}

	entity_type	*m_Owner;
	State<entity_type>	*m_CurrentState;
	State<entity_type>	*m_PreviousState;
	State<entity_type>	*m_GlobalState;
};