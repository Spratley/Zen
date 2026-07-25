# Zen ECS
Achieve Data Oriented Enlightenment

Zen is a C++ ECS system, designed from the ground up by me, as part of my ongoing game engine project, YakuEn. I've isolated Zen into a standalone repo for anyone to use in their own projects!


## ⚠️ WIP Notice ⚠️
Zen is under development. It is not feature complete, but IS technically functional.

## How to Use
After bringing Zen's source into your project, include Zen_Garden.h wherever the root of your entity storage is needed.

Create a Zen::Garden instance to serve as the master storage for all the ECS entities in your program. In order to be constructed, a Garden must be provided two Zen::TypeList<> objects. The first should contain all the Component types that you want the Garden to be aware of, and the second all the System types you want to have ticked during the Garden's update.

## Components
Components need no special treatement for setting up. Each component should be a POD type, so no member functions. As of right now, Zen requires that components are trivially relocatable, as the ECS buffer is allowed to relocate portions during allocation. This is NOT enforced yet, so please do be careful.

## Systems
A System type is a struct, derived from Zen::SystemBase<>, that has a static function called Tick. SystemBase's template parameters should be the System type you're declaring (For use with CRTP calls), followed by the set of components that an entity must have in order to be processed by the system.  

The Tick function must accept a ComponentView const&, and return a void. ComponentView is a view helper that is built to allow ranged for loop processing of each entity that matches the system's requested component layout. Each iteration returns a Tuple containing a reference to each component on the current entity iteration. I prefer using a structured binding for access, but that's personal preference.

# The Future
Zen is missing several features before I can consider it done.

- Despawning Entities (Right now, entities just exist indefinitely once spawned)
- Asynchronous Spawn/Despawn Queue
- Runtime Addition + Removal of Components
- Better Diagnostics + Dumping Memory Layout