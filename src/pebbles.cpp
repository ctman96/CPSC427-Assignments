#define _USE_MATH_DEFINES

// Header
#include "pebbles.hpp"

#include <cmath>
#include <iostream>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

static const int MAX_PEBBLES = 25;
constexpr int NUM_SEGMENTS = 12;

bool Pebbles::init() 
{
	std::vector<GLfloat> screen_vertex_buffer_data;
	constexpr float z = -0.1;

	for (int i = 0; i < NUM_SEGMENTS; i++) {
		screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(z);
	}

	// Clearing errors
	gl_flush_errors();

	// Vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, screen_vertex_buffer_data.size()*sizeof(GLfloat), screen_vertex_buffer_data.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

	if (gl_has_errors())
		return false;

	// Loading shaders
	if (!effect.load_from_file(shader_path("pebble.vs.glsl"), shader_path("pebble.fs.glsl")))
		return false;

	return true;
}

// Releases all graphics resources
void Pebbles::destroy() {
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &m_instance_vbo);

	glDeleteShader(effect.vertex);
	glDeleteShader(effect.fragment);
	glDeleteShader(effect.program);

	m_pebbles.clear();
}

void Pebbles::update(float ms) {
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE PEBBLE UPDATES HERE
	// You will need to handle both the motion of pebbles 
	// and the removal of dead pebbles.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	auto it = m_pebbles.begin();
	while (it != m_pebbles.end()) {
		Pebble &pebble = (*it);
		// Delete dead pebbles
		pebble.life--;
		if (pebble.life <= 0.f) {
			it = m_pebbles.erase(it);
			continue;
		}

		// Add gravity acceleration
		if (pebble.acceleration.y < 8){
			float gravity = 0.000001f * pebble.radius;
			pebble.acceleration.y += gravity;
		}

		// Accelerate
		pebble.velocity.x += pebble.acceleration.x * ms;
		pebble.velocity.y += pebble.acceleration.y * ms;


		// Move
		pebble.position = add(pebble.position, pebble.velocity);

		it++;
	}
}

void Pebbles::spawn_pebble(vec2 position, float dir)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE PEBBLE SPAWNING HERE
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Pebble pebble;
	// place bullet n away from center of salmon
	pebble.position.x = position.x + 100.f*std::sin(dir);
	pebble.position.y = position.y + 100.f*std::cos(dir);

	// Rand angle should be maybe 180 degrees, around mouth, based on dir
	auto randangle = (float)(((rand()%(180)) * M_PI / 180.f) + dir - M_PI);
	auto randX = (float)(rand()%3+1);
	auto randY = (float)(rand()%3+1);
	pebble.velocity.x = randX * (float)cos(randangle);
    pebble.velocity.y = randY * (float)sin(randangle);

	pebble.radius = rand()%(25-5 + 1) + 10.f;
	pebble.life = 300;
	m_pebbles.emplace_back(pebble);
}

void Pebbles::collides_with()
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE PEBBLE COLLISIONS HERE
	// You will need to write additional functions from scratch.
	// Make sure to handle both collisions between pebbles
	// and collisions between pebbles and salmon/fish/turtles.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	auto it = m_pebbles.begin();
	while (it != m_pebbles.end()) {
		Pebble &a = (*it);
		// Check pebbles
		auto otherIt = m_pebbles.begin();
		while (otherIt != m_pebbles.end()) {
			Pebble &b = (*otherIt);
			// Don't collide with self
			if (otherIt == it)
				continue;
			if (a.collides_with(b)) {
				// TODO: fix overlaps?
				//float dist = sqrt((*it))
			}
		}
	}

	// TODO salmon, fish turtles

	/*
	// Flip x velocity
	if (flipX) {
		m_velocity.x = -m_velocity.x;
	}
	// Flip Y velocity
	if (flipY) {
		m_velocity.y = -m_velocity.y;
	}
	// Wait to adjust rotation until next frame
	if (flipX || flipY) {
		auto preangle = (float) atan2(pre.x, pre.y);
		auto postangle = (float) atan2(m_velocity.x, m_velocity.y);
		m_update_rotation = motion.radians + postangle - preangle;
	}
 	*/
}

// Draw pebbles using instancing
void Pebbles::draw(const mat3& projection) 
{
	// Setting shaders
	glUseProgram(effect.program);

  	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Getting uniform locations
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint color_uloc = glGetUniformLocation(effect.program, "color");

	// Pebble color
	float color[] = { 0.4f, 0.4f, 0.4f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

	// Draw the screen texture on the geometry
	// Setting vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// Mesh vertex positions
	// Bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(0, 0);

	// Load up pebbles into buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_pebbles.size() * sizeof(Pebble), m_pebbles.data(), GL_DYNAMIC_DRAW);

	// Pebble translations
	// Bind to attribute 1 (in_translate) as in vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Pebble), (GLvoid*)offsetof(Pebble, position));
	glVertexAttribDivisor(1, 1);

	// Pebble radii
	// Bind to attribute 2 (in_scale) as in vertex shader
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Pebble), (GLvoid*)offsetof(Pebble, radius));
	glVertexAttribDivisor(2, 1);

	// Draw using instancing
	// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArraysInstanced.xhtml
	glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_SEGMENTS*3, m_pebbles.size());

  	// Reset divisor
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
}

bool Pebbles::Pebble::collides_with(Pebbles::Pebble other) {
	float dx = position.x - other.position.x;
	float dy = position.y - other.position.y;
	float d_sq = dx * dx + dy * dy;
	float dr = radius + other.radius;
	float r_sq = dr * dr;
	return d_sq < r_sq;
}
