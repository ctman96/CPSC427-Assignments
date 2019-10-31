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
	pebble.position.x = position.x + 90.f*std::sin(dir);
	pebble.position.y = position.y + 90.f*std::cos(dir);

	// Rand angle should be maybe 180 degrees, around mouth, based on dir
	auto randangle = dir + (float)((((rand()%(90))-45) * M_PI / 180.f));
	auto randX = (float)(rand()%3+2);
	auto randY = (float)(rand()%3+2);
	pebble.velocity.x = randX * (float)sin(randangle);
    pebble.velocity.y = randY * (float)cos(randangle);

	pebble.radius = rand()%(20 + 1) + 10.f;
	pebble.life = 300;
	m_pebbles.emplace_back(pebble);
}

void Pebbles::collides_with(const Salmon& salmon, const std::vector<Fish> &fishes,  const std::vector<Turtle> &turtles)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// HANDLE PEBBLE COLLISIONS HERE
	// You will need to write additional functions from scratch.
	// Make sure to handle both collisions between pebbles
	// and collisions between pebbles and salmon/fish/turtles.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	auto it = m_pebbles.begin();
	while (it != m_pebbles.end()) {
		Pebble &pebble = (*it);

		
		// Check pebbles
		auto otherIt = m_pebbles.begin();
		while (otherIt != m_pebbles.end()) {
			Pebble &other = (*otherIt);
			// Don't collide with self
			if (otherIt != it && pebble.collides_with(other.position, other.radius)) {
				// Fix overlaps
				auto dist = std::sqrt((float)pow(pebble.position.x - other.position.x,2) + (float)pow(pebble.position.y - other.position.y, 2));
				float overlap = (dist - pebble.radius - other.radius) / 2.f;

				float dx = (pebble.position.x - other.position.x)/dist;
				float dy = (pebble.position.y - other.position.y)/dist;
				pebble.position.x -= overlap * dx;
				pebble.position.y -= overlap * dy;
				other.position.x += overlap * dx;
				other.position.y += overlap * dy;



				// https://en.wikipedia.org/wiki/Elastic_collision
				float amass = pebble.radius*100;
				float bmass = other.radius*100;

				float vap1 = 2*bmass/(amass+bmass);
				float vap2 = dot(sub(pebble.velocity, other.velocity), sub(pebble.position, other.position)) / (float)pow(len(sub(pebble.position, other.position)), 2);
				vec2 va = sub(pebble.velocity, mul(sub(pebble.position, other.position), vap1 * vap2));

				float vbp1 = 2*amass/(amass+bmass);
				float vbp2 = dot(sub(other.velocity, pebble.velocity), sub(other.position, pebble.position)) / (float)pow(len(sub(other.position, pebble.position)), 2);
				vec2 vb = sub(other.velocity, mul(sub(other.position, pebble.position), vbp1*vbp2));

				//vec2 va = mul(add(mul(a.velocity,amass - bmass), mul(b.velocity, 2.f*bmass)), 1/(amass+bmass));
				//vec2 vb = mul(add(mul(b.velocity, bmass - amass), mul(a.velocity, 2.f*amass)), 1/(amass+bmass));
				std::cout << pebble.velocity.x <<","<< pebble.velocity.y << std::endl;
				std::cout << va.x << "," << va.y << std::endl;
				pebble.velocity = va;
				other.velocity = vb;
			}
			otherIt++;
		}


		for (auto& fish : fishes) {
			vec2 fishpos = fish.get_position();
			float fishr = std::max(fish.get_bounding_box().x, fish.get_bounding_box().y) * 0.6f;
			if (pebble.collides_with(fishpos, fishr)) {
				auto dist = std::sqrt((float)pow(pebble.position.x - fishpos.x,2) + (float)pow(pebble.position.y - fishpos.y, 2));
				float overlap = (dist - pebble.radius - fishr);
				float dx = (pebble.position.x - fishpos.x)/dist;
				float dy = (pebble.position.y - fishpos.y)/dist;
				pebble.position.x -= overlap * dx;
				pebble.position.y -= overlap * dy;


				float pmass = pebble.radius*100;
				float fmass = fishr*100;

				vec2 fvel = {-190.f/1000, 0.f}; // TODO

				float vap1 = 2*fmass/(pmass+fmass);
				float vap2 = dot(sub(pebble.velocity, fvel), sub(pebble.position, fishpos)) / (float)pow(len(sub(pebble.position, fishpos)), 2);
				vec2 va = sub(pebble.velocity, mul(sub(pebble.position, fishpos), vap1 * vap2));
				pebble.velocity = va;
			}
		}


		// TODO could pull out fish/turtle collision code into reusable function
		for (auto& turtle : turtles) {
			vec2 turtlepos = turtle.get_position();
			float turtler = std::max(turtle.get_bounding_box().x, turtle.get_bounding_box().y) * 0.6f;
			if (pebble.collides_with(turtlepos, turtler)) {
				auto dist = std::sqrt((float)pow(pebble.position.x - turtlepos.x,2) + (float)pow(pebble.position.y - turtlepos.y, 2));
				float overlap = (dist - pebble.radius - turtler);
				float dx = (pebble.position.x - turtlepos.x)/dist;
				float dy = (pebble.position.y - turtlepos.y)/dist;
				pebble.position.x -= overlap * dx;
				pebble.position.y -= overlap * dy;


				float pmass = pebble.radius*100;
				float tmass = turtler*100;

				vec2 tvel = turtle.getVel(); // TODO

				float vap1 = 2*tmass/(pmass+tmass);
				float vap2 = dot(sub(pebble.velocity, tvel), sub(pebble.position, turtlepos)) / (float)pow(len(sub(pebble.position, turtlepos)), 2);
				vec2 va = sub(pebble.velocity, mul(sub(pebble.position, turtlepos), vap1 * vap2));
				pebble.velocity = va;
			}
		}

		// TODO salmon - bounding box then vertices? or can just do radius-based?

		it++;
	}

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

bool Pebbles::Pebble::collides_with(vec2 pos, float rad) {
	float dx = position.x - pos.x;
	float dy = position.y - pos.y;
	float d_sq = dx * dx + dy * dy;
	float dr = radius + rad;
	float r_sq = dr * dr;
	return d_sq < r_sq;
}
