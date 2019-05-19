# Showcase
## Overview
This repository is intended to show some of the code I have created in the past 3 years.

It is not provided in a state where it will run, as most of it is pulled from larger projects, however it should demonstrate some of my ability and what I have learned.

Code is divided by language and then project.

## 2D Custom Engine
[IGArt, a custom 2D Engine](https://benlandor.000webhostapp.com/igart), is the best and most extensive show of my skill. Built over a period of 3-4 months by 9 programmers, the engine runs on PC and PSVita. Code from IGArt uses Doxygen comment style.

My contribution to this included:
* Core Engine functionality
  * [Component](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Core/component.h) and [Entity](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Core/entity.h) Classes using Component Pattern
  * [Transform](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Core/transform.h), [Raycasting](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Core/ray.h) and [Time](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Time/time.h) Classes
* Entire Editor functionality
  * [Editor](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Editor/editor.h) [menubar](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Editor/menu_bar.h), [hierarchy](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Editor/hierarchy.h), and [inspector](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Editor/inspector.h) (details) panels
  * [3D Gizmos](https://github.com/TheXenOne/Showcase/blob/7df3b9e8b8f6f7dac0fa0f12cab4ff104c2c80fd/C%2B%2B/2DEngine/Editor/inspector.cpp#L189)
* [2D Animation Component](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/2DEngine/Graphics/animation_component.h) (using sprite-sheet Atlas)
* [Scene Graph using Dirty Flag pattern](https://github.com/TheXenOne/Showcase/blob/73a801c620370cdf6055511ecfeb239db05f6b20/C%2B%2B/2DEngine/Core/transform.h#L339)
* [Input](https://github.com/TheXenOne/Showcase/blob/7df3b9e8b8f6f7dac0fa0f12cab4ff104c2c80fd/C%2B%2B/2DEngine/Input/input.h#L37) and [InputListener](https://github.com/TheXenOne/Showcase/blob/7df3b9e8b8f6f7dac0fa0f12cab4ff104c2c80fd/C%2B%2B/2DEngine/Input/input_listener.h#L29) using Observer pattern

## Other Projects
State Machine - A [generic state machine](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/StateMachine/StateMachine.h) for many situations.

Arena Brawler - The [camera code](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/ArenaBrawler/MMCamera.h) currently in use for [Muscle Magic](https://benlandor.000webhostapp.com/muscle-magic)

Pathfinding - [A*](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/AI/Pathfinding/Graph_SearchAStar.h), [Flow Fields](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/AI/Pathfinding/Graph_FlowField.h), [Dijkstra](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/AI/Pathfinding/Graph_SearchDijkstra.h), [Breadth-First Search](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/AI/Pathfinding/Graph_SearchBFS.h), and [Depth-First Search](https://github.com/TheXenOne/Showcase/blob/master/C%2B%2B/AI/Pathfinding/Graph_SearchDFS.h) implementations.
