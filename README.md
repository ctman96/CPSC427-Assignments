# 427 2019 Winter 1, Assignment 3

Cody Newman
35833145
k2a0b

Citations:


## Controls:
Changed to be in line with assignment specifications,
Left    :   Rotates CCW
Right   :   Rotates CW
Up      :   Move forwards along facing
Down    :   Move backwards along facing

L/R Ctrl  :   Shoot bullet along facing

A       :   Switch to advanced/debug visualization
B       :   Switch to basic

D       :   Toggle salmon AI control

Minus   :   Decrease pathfinding update frequency
Equals  :   Increase pathfinding update frequency

L/R Shift  :  Fire pebbles ahead of you

## Added features:

### Particle Animation
Implemented pebble particle system that fires pebbles periodically from the salmon's mouth while holding SHIFT.
- Randomized initial directions
- Randomized initial velocity
- Randomized Radii/Mass
- Pebbles velocities/acceleration calculations affected by both gravity and water resistance.

### Bouncing Pebbles
- When pebbles collide with each other, they bounce using physcially correct bounce,  speed, and acceleration, by
using the equation for two dimensional collision between two moving circles 
(https://en.wikipedia.org/wiki/Elastic_collision#Two-dimensional_collision_with_two_moving_objects)
    - Mass is calculated relative to the pebble's radius

- When pebbles collide with other entities they perform a similar collision, 
approximating the other assets as spheres as well. Mass is similarly relative to the radius of the entities.

## Creative portion:
- Pebble collisions trigger change in the motion of collided entities. 
Salmon/Turtles/Fish were adjusted also be affected by pebble collisions, 
receiving changes to velocity upon collision with a pebble according to the same formula.

- I believe all of my physics calculations take elapsed time into consideration in their calculations.

- Trippy Salmon after-image particle effect?
