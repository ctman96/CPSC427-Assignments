# 427 2019 Winter 1, Assignment 2

Cody Newman
35833145
k2a0b

Citations:
https://www.geeksforgeeks.org/a-search-algorithm/


## Controls:
Changed to be in line with assignment specifications,
Left    :   Rotates CCW
Right   :   Rotates CW
Up      :   Move forwards along facing
Down    :   Move backwards along facing

R Ctrl  :   Shoot bullet along facing

A       :   Switch to advanced/debug visualization
B       :   Switch to basic

D       :   Toggle salmon AI control

Minus   :   Decrease pathfinding update frequency
Equals  :   Increase pathfinding update frequency

## Added features:
- Adjusted control scheme to match assignment

### Salmon Collisions
- AABB bounding box checking, and then exact vertex collision checking
- exact vertex collision checking (checking AABB first)
- Wall collision detection (AABB followed by exact vertex), Debug visualization of colliding vertices
- Wall bouncing - Flip direction of motion, rotate appropriately

### AI
- Entity grid which tracks entity types in a grid made up of 32px squares. Debug visualization of grid and square types
- Reusable A* pathfinding utilizing grid. 
- Smarter fish - Fish want to move to x=0, avoiding any Player squares. It will check for clearance to ensure its bounding box won't overlap with any Player squares. Debug visualization of paths
- User controlled pathfinding frequency (-= keys)
- Smarter turtles - Turtles will chase after the player. Debug visualization of paths
- Sorta-smart salmon AI - When toggled, salmon will attempt to avoid the turtle and get the fish, though can potentially be caught by the turtle when distracted by a fish. Does like to bang up against boundaries often, so is best observed in basic visualization

## Creative portion:
- Not sure if it counts but the full entity grid system and reusable A* search?
- Smart turtle which chases the player and avoids fish
- Salmon AI mode (Was originally going to implement a second salmon to fight against, but it caused a bunch of graphical bugs I had no idea how to fix).
