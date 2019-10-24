//
// Created by Cody on 10/22/2019.
//

#include <fish.hpp>
#include "DebugView.hpp"

bool DebugView::init(vec2 screen) {
    return debugDot.init() && debugCollision.init() && debugBoundary.init(screen);
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

void DebugView::draw(const mat3 &projection, Salmon *salmon, const std::vector<Fish> * fishes) {
    // Salmon vertices
    for(auto dot : salmon->getM_debug_vertices()) {
        debugCollision.draw(projection, dot, false);
    }
    // Salmon Collision vertices
    for(auto dot : salmon->getM_debug_collision_points()) {
        debugCollision.draw(projection, dot, true);
    }
    // Fish dots
    for (const auto &fish : *fishes) {
        for(auto dot : fish.getM_path()){
            debugDot.draw(projection, dot);
        }
    }
    debugBoundary.draw(projection);
}


