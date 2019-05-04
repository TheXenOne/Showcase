using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StateMachine<entity_type>
{
    private entity_type m_Owner;
    private State<entity_type> m_CurrentState;
    private State<entity_type> m_GlobalState;
    public Stack<State<entity_type>> m_StateStack;

    public bool m_HasGlobalState = true;

    public StateMachine(entity_type a_owner, State<entity_type> a_currentState, State<entity_type> a_globalState)
    {
        m_Owner = a_owner;
        m_CurrentState = a_currentState;
        m_GlobalState = a_globalState;
        m_StateStack = new Stack<State<entity_type>>();
        m_StateStack.Push(a_currentState);
    }

    public StateMachine(entity_type a_owner, State<entity_type> a_currentState)
    {
        m_Owner = a_owner;
        m_CurrentState = a_currentState;
        m_HasGlobalState = false;
        m_StateStack = new Stack<State<entity_type>>();
        m_StateStack.Push(a_currentState);
        a_currentState.Enter(a_owner);
    }

    public void UpdateStateMachine()
    {
        if (m_HasGlobalState)
        {
            m_GlobalState.Execute(m_Owner);
        }

        m_CurrentState.Execute(m_Owner);
    }

    public void ChangeState(State<entity_type> a_newState)
    {
        m_StateStack.Push(m_CurrentState);

        m_CurrentState.Exit(m_Owner);

        m_CurrentState = a_newState;

        m_CurrentState.Enter(m_Owner);
    }

    public void RevertToPreviousState()
    {
        ChangeState(m_StateStack.Pop());
    }

    public bool GetIsInState(State<entity_type> a_state)
    {
        if (m_CurrentState == a_state)
        {
            return true;
        }

        return false;
    }

    public State<entity_type> GetPreviousState()
    {
        return m_StateStack.Peek();
    }

    public bool GetHasBeenInState(State<entity_type> a_state)
    {
        State<entity_type>[] stateArray = m_StateStack.ToArray();

        foreach (State<entity_type> state in stateArray)
        {
            if (state == a_state)
            {
                return true;
            }
        }

        return false;
    }
};