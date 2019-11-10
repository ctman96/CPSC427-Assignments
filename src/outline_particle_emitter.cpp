//
// Created by Cody on 10/30/2019.
//

#include <iostream>
#include "outline_particle_emitter.hpp"

static const float START_LIFE = 0.5;

bool outline_particle_emitter::init(std::vector<Vertex> vertices, std::vector<uint16_t> indices) {
    icount = indices.size();

    for (auto& vertex : vertices) {
        ++vertex.position.z;
    }

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

    glGenVertexArrays(1, &mesh.vao);

    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("outline.vs.glsl"), shader_path("outline.fs.glsl")))
        return false;

    return true;
}

void outline_particle_emitter::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &m_instance_vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteVertexArrays(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);

    m_outlines.clear();
}

void outline_particle_emitter::update(float ms) {
    auto it = m_outlines.begin();
    while (it != m_outlines.end()) {
        Outline &outline = (*it);
        // Delete dead pebbles
        outline.life -= ms / 1000;
        if (outline.life <= 0.f) {
            it = m_outlines.erase(it);
            continue;
        }
        outline.color.w = outline.life / START_LIFE;

        it++;
    }
}

void outline_particle_emitter::draw(const mat3 &projection) {
// Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    // Getting uniform locations
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Draw the screen texture on the geometry
    // Setting vertices
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

    // Mesh vertex positions
    // Bind to attribute 0 (in_position) as in the vertex shader
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(0, 0);

    // Load up pebbles into buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_outlines.size() * sizeof(Outline), m_outlines.data(), GL_DYNAMIC_DRAW);

    // translations
    // Bind to attribute 1 (in_translate) as in vertex shader
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Outline), (GLvoid*)offsetof(Outline, position));
    glVertexAttribDivisor(1, 1);

    // scale
    // Bind to attribute 2 (in_scale) as in vertex shader
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Outline), (GLvoid*)offsetof(Outline, scale));
    glVertexAttribDivisor(2, 1);

    // rotation
    // Bind to attribute 3 (in_rotation) as in vertex shader
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Outline), (GLvoid*)offsetof(Outline, rotation));
    glVertexAttribDivisor(3, 1);

    // color
    // Bind to attribute 4 (in_color) as in vertex shader
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Outline), (GLvoid*)offsetof(Outline, color));
    glVertexAttribDivisor(4, 1);

    // Draw using instancing
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDrawArraysInstanced.xhtml
    glDrawArraysInstanced(GL_LINE_LOOP, 0, icount, m_outlines.size());

    // Reset divisor
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 0);
}

void outline_particle_emitter::spawn_outline(vec2 position, vec2 scale, float rotation) {
    Outline outline;
    outline.position = position;
    outline.scale = scale;
    outline.rotation = rotation;
    outline.color = { 1.f, 1.f, 1.f, 1.f};
    outline.life = START_LIFE;
    m_outlines.emplace_back(outline);
}

