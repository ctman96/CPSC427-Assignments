//
// Created by Cody on 10/22/2019.
//

#ifndef SALMON_DEBUGDOT_HPP
#define SALMON_DEBUGDOT_HPP


#include <common.hpp>

class DebugDot : public Entity {
public:
    bool init();
    void destroy();
    void draw(const mat3 &projection) override;
    void draw(const mat3& projection, vec3 color, vec2 pos, float rad = 0, vec2 scale = {1,1});
};


#endif //SALMON_DEBUGDOT_HPP
