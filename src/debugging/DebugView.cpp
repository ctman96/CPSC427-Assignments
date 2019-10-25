//
// Created by Cody on 10/22/2019.
//

#include <fish.hpp>
#include "DebugView.hpp"

bool DebugView::init(vec2 screen) {
    bool ret = true;
    ret &= debugDot.init();
    ret &= debugBoundary.init(screen);
    return ret;
}

void DebugView::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

void DebugView::draw(const mat3 &projection) {}

void DebugView::draw(const mat3 &projection, Salmon *salmon, const std::vector<Fish> * fishes, const std::vector<Turtle> * turtles) {
    // Salmon vertices
    for(auto dot : salmon->getM_debug_vertices()) {
        debugDot.draw(projection, { 1.f, 1.f, 1.f }, dot, 0.785f, {0.6f,0.6f});
    }
    // Salmon Collision vertices
    for(auto dot : salmon->getM_debug_collision_points()) {
        debugDot.draw(projection, { 1.f, 0.2f, 0.2f }, dot, 0.785f, {0.6f,0.6f});
    }
    // Fish dots
    for (const auto &fish : *fishes) {
        for(auto dot : fish.getM_path()){
            debugDot.draw(projection, { 0.5f, 0.f, 0.f }, dot);
        }
    }
    // Turtle dots
    for (const auto &turtle : *turtles) {
        for(auto dot : turtle.getM_path()){
            debugDot.draw(projection, { 0.5f, 0.8f, 0.f }, dot);
        }
    }
    debugBoundary.draw(projection);
}


