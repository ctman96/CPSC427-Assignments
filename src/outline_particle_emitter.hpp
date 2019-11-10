//
// Created by Cody on 10/30/2019.
//

#ifndef SALMON_OUTLINE_PARTICLE_EMITTER_HPP
#define SALMON_OUTLINE_PARTICLE_EMITTER_HPP


#include <vector>
#include "common.hpp"

class outline_particle_emitter : public Entity {
    struct Outline {
        vec2 position;
        vec2 scale;
        float rotation;
        vec4 color;
        float life = 0.0f; // remove pebble when its life reaches 0
    };
public:
    // Creates all the associated render resources
    bool init(std::vector<Vertex> vertices, std::vector<uint16_t> indices);

    // Releases all associated resources
    void destroy();

    // Updates all
    // ms represents the number of milliseconds elapsed from the previous update() call
    void update(float ms);

    // Renders
    // projection is the 2D orthographic projection matrix
    void draw(const mat3& projection) override;

    // Spawn new particle
    void spawn_outline(vec2 position, vec2 scale, float rotation);
private:
    GLuint m_instance_vbo; // vbo for instancing
    std::vector<Outline> m_outlines; // vector of outlines
    int icount;
};


#endif //SALMON_OUTLINE_PARTICLE_EMITTER_HPP
