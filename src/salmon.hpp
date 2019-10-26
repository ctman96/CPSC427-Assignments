#pragma once

#include <map>
#include "common.hpp"
#include <vector>

class Turtle;
class Fish;

class Salmon : public Entity
{
public:
	// Creates all the associated render resources and default transform
	bool init();

	// Releases all associated resources
	void destroy();
	
	// Update salmon position based on direction
	// ms represents the number of milliseconds elapsed from the previous update() call
	void update(float ms, std::map<int, bool> &keyMap, vec2 mouse_position, vec2 screen);
	
	// Renders the salmon
	void draw(const mat3& projection)override;

	// Collision routines for turtles and fish
	bool collides_with(const Turtle& turtle);
	bool collides_with(const Fish& fish);

	// Returns the current salmon position
	vec2 get_position() const;

	// Returns the current salmon rotation
	float get_rotation()const;

	// Moves the salmon's position by the specified offset
	void move(vec2 off);

	// Set salmon rotation in radians
	void set_rotation(float radians);

	// Change salmon velocity
	void accelerate(float x, float y);

	// True if the salmon is alive
	bool is_alive()const;

	// Kills the salmon, changing its alive state and triggering on death events
	void kill();

	// Called when the salmon collides with a fish, starts lighting up the salmon
	void light_up();

	void clear_debug_collision();

	const std::vector<vec2> &getM_debug_collision_points() const;

	vec2 get_bounding_box() const;

	const std::vector<vec2> &getM_debug_vertices() const;

private:
	float m_light_up_countdown_ms; // Used to keep track for how long the salmon should be lit up
	bool m_is_alive; // True if the salmon is alive
	vec2 m_velocity; // Velocity
	float m_update_rotation;
	vec2 m_bbox;

	mat3 world_projection;

  	std::vector<Vertex> m_vertices;
	std::vector<uint16_t> m_indices;

	std::vector<vec2> m_debug_collision_points;
	std::vector<vec2> m_debug_vertices;

    bool collides_with_exact(int left, int right, int top, int bottom);
    bool collides_with_aabb(vec2 pos, vec2 box);
	bool check_wall_collisions(vec2 screen);
};
