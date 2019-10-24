//
// Created by Cody on 10/22/2019.
//

#ifndef SALMON_DEBUGDOT_HPP
#define SALMON_DEBUGDOT_HPP


#include <common.hpp>

class DebugDot : public Entity {
    static Texture dot_texture;
public:
    bool init();
    void destroy();
    void draw(const mat3 &projection) override;
    void draw(const mat3& projection, vec2 pos);
};


#endif //SALMON_DEBUGDOT_HPP
