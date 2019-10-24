//
// Created by Cody on 10/22/2019.
//

#ifndef SALMON_DEBUGCOLLISION_HPP
#define SALMON_DEBUGCOLLISION_HPP

#include <common.hpp>

class DebugCollision : public Entity {
    static Texture collision_texture;
public:
    bool init();
    void destroy();
    void draw(const mat3 &projection) override;
    void draw(const mat3& projection, vec2 pos, bool col);
};

#endif //SALMON_DEBUGCOLLISION_HPP
