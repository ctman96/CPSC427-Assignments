// Header
#include "salmon.hpp"

// internal
#include "turtle.hpp"
#include "fish.hpp"

// stlib
#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>

bool Salmon::init()
{
	m_vertices.clear();
	m_indices.clear();

	// Reads the salmon mesh from a file, which contains a list of vertices and indices
	FILE* mesh_file = fopen(mesh_path("salmon.mesh"), "r");
	if (mesh_file == nullptr)
		return false;

	// Reading vertices and colors
	size_t num_vertices;
	fscanf(mesh_file, "%zu\n", &num_vertices);
	for (size_t i = 0; i < num_vertices; ++i)
	{
		float x, y, z;
		float _u[3]; // unused
		int r, g, b;
		fscanf(mesh_file, "%f %f %f %f %f %f %d %d %d\n", &x, &y, &z, _u, _u+1, _u+2, &r, &g, &b);
		Vertex vertex;
		vertex.position = { x, y, -z }; 
		vertex.color = { (float)r / 255, (float)g / 255, (float)b / 255 };
		m_vertices.push_back(vertex);
	}

	// Reading associated indices
	size_t num_indices;
	fscanf(mesh_file, "%zu\n", &num_indices);
	for (size_t i = 0; i < num_indices; ++i)
	{
		int idx[3];
		fscanf(mesh_file, "%d %d %d\n", idx, idx + 1, idx + 2);
		m_indices.push_back((uint16_t)idx[0]);
		m_indices.push_back((uint16_t)idx[1]);
		m_indices.push_back((uint16_t)idx[2]);
	}

	// Done reading
	fclose(mesh_file);

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

	// Index Buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

	// Vertex Array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("salmon.vs.glsl"), shader_path("salmon.fs.glsl")))
		return false;
	
	// Setting initial values
	motion.position = { 300.f, 500.f };
	motion.radians = 0.f;
	motion.speed = 100.f;

	physics.scale = { -35.f, 35.f };

	m_is_alive = true;
	m_light_up_countdown_ms = -1.f;

	return true;
}

// Releases all graphics resources
void Salmon::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteBuffers(1, &mesh.vao);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);
}

// Called on each frame by World::update()
void Salmon::update(float ms, std::map<int, bool> &keyMap, vec2 mouse_position, vec2 screen)
{
	float step = motion.speed * (ms / 1000);
	if (m_is_alive)
	{
	    // Delay changing rotation after a collision
	    if (m_update_rotation) {
            set_rotation(atan2(m_velocity.x, m_velocity.y));
            m_update_rotation = false;
	    }

		float accelX = 0.f;
		float accelY = 0.f;

		// Move along direction
		if (keyMap[GLFW_KEY_UP]) {
			accelX = 2 * sin(motion.radians);
			accelY = 2 * cos(motion.radians);
		}
		if (keyMap[GLFW_KEY_DOWN]) {
			accelX = -2 * sin(motion.radians);
			accelY = -2 * cos(motion.radians);
		}

		// Rotate
		if (keyMap [GLFW_KEY_LEFT]) {
			motion.radians -= step / 20;
		}
		if (keyMap [GLFW_KEY_RIGHT]) {
			motion.radians += step / 20;
		}

        accelerate(accelX,accelY);

		float bufX = screen.x * 0.95f;
		float bufY = screen.y * 0.95f;

		vec2 tl = { screen.x - bufX, screen.y - bufY };
		vec2 br = { bufX, bufY };

		transform.begin();
		transform.translate({motion.position.x, motion.position.y });
		transform.scale(physics.scale);
		transform.rotate(motion.radians - 3.14f/2);
		transform.end();

		bool flipX = false;
		bool flipY = false;
        for(auto vertex : m_vertices) {
            vec3 pos = mul(transform.out, vec3{vertex.position.x, vertex.position.y, 1.0});
            bool collision = false;
            if ((m_velocity.x < 0 && pos.x <= tl.x) || (m_velocity.x > 0 && pos.x >= br.x)) {
                flipX = true;
                collision = true;
            }
            if ((m_velocity.y < 0 && pos.y <= tl.y) || (m_velocity.y > 0 && pos.y >= br.y)) {
                flipY = true;
                collision = true;
            }
            if (collision)
                m_debug_collision_points.emplace_back(vec2{pos.x, pos.y});
        }
        if (flipX) {
            std::cout << "flipx" << std::endl;
            m_velocity.x = -m_velocity.x;
        }
        if (flipY) {
            std::cout << "flipy" <<std::endl;
            m_velocity.y = -m_velocity.y;
        }
        if (flipX || flipY){
			m_update_rotation = true;
        } else {
			// move based on velocity
			motion.position.x += m_velocity.x;
			motion.position.y += m_velocity.y;


			// Decay velocity
			float friction = 0.02;
			if (m_velocity.x > 0.f)
				m_velocity.x -= friction* m_velocity.x;
			else if (m_velocity.x < 0.f)
				m_velocity.x += -friction* m_velocity.x;

			if (m_velocity.y > 0.f)
				m_velocity.y -= friction*m_velocity.y;
			else if (m_velocity.y < 0.f)
				m_velocity.y += -friction*m_velocity.y;
        }
	}
	else
	{
		// If dead we make it face upwards and sink deep down
		set_rotation(3.1415f);
		move({ 0.f, step });
	}

	if (m_light_up_countdown_ms > 0.f)
		m_light_up_countdown_ms -= ms;
}

void Salmon::draw(const mat3& projection)
{
	m_projection = projection;
	transform.begin();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// SALMON TRANSFORMATION CODE HERE

	// see Transformations and Rendering in the specification pdf
	// the following functions are available:
	// translate()
	// rotate()
	// scale()


	transform.translate({motion.position.x, motion.position.y });
    transform.scale(physics.scale);
    transform.rotate(motion.radians - 3.14f/2);

	transform.end();

	// Setting shaders
	glUseProgram(effect.program);

	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint light_up_uloc = glGetUniformLocation(effect.program, "light_up");

	// Setting vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// Input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_color_loc = glGetAttribLocation(effect.program, "in_color");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_color_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec3));

	// Setting uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.out);

	// !!! Salmon Color
	float color[] = { 1.f, 1.f, 1.f };
	if (!is_alive()) {
	    color[1] = 0.2f;
	    color[2] = 0.2f;
	}
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HERE TO SET THE CORRECTLY LIGHT UP THE SALMON IF HE HAS EATEN RECENTLY
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	int light_up = (m_light_up_countdown_ms > 0 ? 1 : 0);
	glUniform1iv(light_up_uloc, 1, &light_up);

	// Get number of infices from buffer,
	// we know our vbo contains both colour and position information, so...
	GLint size = 0;
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	GLsizei num_indices = size / sizeof(uint16_t);

	// Drawing!
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
}

// Simple bounding box collision check
// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You don't
// need to try to use this technique.
bool Salmon::collides_with(const Turtle& turtle)
{
	// TODO salmon bounding box
	vec2 turtlepos =  turtle.get_position();
	vec2 turtlebox = turtle.get_bounding_box();
	float dx = motion.position.x - turtlepos.x;
	float dy = motion.position.y - turtlepos.y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(turtlebox.x, turtlebox.y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r)
		return collides_with_exact(turtlepos.x-(turtlebox.x/2), turtlepos.x+(turtlebox.x/2), turtlepos.y-(turtlebox.y/2), turtlepos.y+(turtlebox.y/2));
	return false;
}

bool Salmon::collides_with(const Fish& fish)
{
	// TODO salmon bounding box
	vec2 fishpos =  fish.get_position();
	vec2 fishbox = fish.get_bounding_box();
	float dx = motion.position.x - fishpos.x;
	float dy = motion.position.y - fishpos.y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(fishbox.x, fishbox.y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 0.6f;
	if (d_sq < r * r){
		return collides_with_exact(fishpos.x-(fishbox.x/2), fishpos.x+(fishbox.x/2), fishpos.y-(fishbox.y/2), fishpos.y+(fishbox.y/2));
	}
	return false;
}

// Check exact collision with a box
bool Salmon::collides_with_exact(int left, int right, int top, int bottom) {

	//std::cout << motion.position.x << "," << motion.position.y << std::endl;
	bool ret = false;

	for(auto vertex : m_vertices) {
		//std::cout << std::endl;
		//std::cout << vertex.position.x << "," << vertex.position.y << std::endl;
		vec3 pos = mul(transform.out, vec3{vertex.position.x, vertex.position.y, 1.0});
		//std::cout << pos.x << "," << pos.y << std::endl;
		if (pos.x >= left &&
		  	pos.x <= right &&
			pos.y >= top &&
		  	pos.y <= bottom) {
			ret = true;
			//m_debug_collision_points.emplace_back(vec2{pos.x, pos.y});
		}
	}
	return ret;
}

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// HANDLE SALMON - WALL COLLISIONS HERE
// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
// You will want to write new functions from scratch for checking/handling 
// salmon - wall collisions.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

vec2 Salmon::get_position() const
{
	return motion.position;
}

float Salmon::get_rotation() const {
    return motion.radians;
}

void Salmon::move(vec2 off)
{
	motion.position.x += off.x; 
	motion.position.y += off.y; 
}

void Salmon::set_rotation(float radians)
{
	motion.radians = radians;
}

void Salmon::accelerate(float x, float y) {
    float max = 4.f;

    float newX = m_velocity.x + x;
    if (newX > max) newX = max;
    if (newX < -max) newX = -max;

    float newY = m_velocity.y + y;
    if (newY > max) newY = max;
    if (newY < -max) newY = -max;

    m_velocity.x = newX;
    m_velocity.y = newY;
}

bool Salmon::is_alive() const
{
	return m_is_alive;
}

// Called when the salmon collides with a turtle
void Salmon::kill()
{
	m_is_alive = false;
}

// Called when the salmon collides with a fish
void Salmon::light_up()
{
	m_light_up_countdown_ms = 1500.f;
}

void Salmon::clear_debug_collision() {
	m_debug_collision_points.clear();
}

const std::vector<vec2> &Salmon::getM_debug_collision_points() const {
	return m_debug_collision_points;
}
