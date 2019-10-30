#pragma once

// internal
#include "common.hpp"
#include "salmon.hpp"
#include "turtle.hpp"
#include "fish.hpp"
#include "water.hpp"
#include "bullet.hpp"
#include "pebbles.hpp"
#include "debugging/DebugView.hpp"
#include "EntityGrid.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>
#include <map>

class DebugView;

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class World
{
	friend class DebugView;
	friend class EntityGrid;
public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;

private:
	// Generates a new turtle
	bool spawn_turtle();

	// Generates a new fish
	bool spawn_fish();

	bool spawn_bullet();

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);
    void on_mouse_button (GLFWwindow* window, int button, int action, int mods);

private:
	// Window handle
	GLFWwindow* m_window;
	float m_screen_scale; // Screen to pixel coordinates scale factor
	int m_frame;

	// Screen texture
	// The draw loop first renders to this texture, then it is used for the water shader
	GLuint m_frame_buffer;
	Texture m_screen_tex;

	// Key map
	std::map<int, bool> keyMap;

	// Mouse position
	vec2 mouse_position;

	// Water effect
	Water m_water;

	// Number of fish eaten by the salmon, displayed in the window title
	unsigned int m_points;

	// Game entities
	Salmon m_salmon;
	std::vector<Turtle> m_turtles;
	std::vector<Fish> m_fish;
	std::vector<Bullet> m_bullets;
	Pebbles m_pebbles_emitter;

	float m_current_speed;
	float m_next_turtle_spawn;
	float m_next_fish_spawn;
    float m_bullet_cooldown;
    float m_pebble_cooldown;
	
	Mix_Music* m_background_music;
	Mix_Chunk* m_salmon_dead_sound;
	Mix_Chunk* m_salmon_eat_sound;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	EntityGrid aiGrid;
	bool m_debug;
    DebugView m_debug_view;
    int m_path_update_frame;

	float m_freeze_timer;
};
