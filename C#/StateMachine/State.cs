using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public abstract class State<entity_type>
{
    public abstract void Enter(entity_type a_entity);

    public abstract void Execute(entity_type a_entity);

    public abstract void Exit(entity_type a_entity);
};