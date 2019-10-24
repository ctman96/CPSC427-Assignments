//
// Created by Cody on 10/22/2019.
//

#ifndef SALMON_DEBUGVIEW_HPP
#define SALMON_DEBUGVIEW_HPP


#include "common.hpp"
#include "salmon.hpp"
#include "DebugDot.hpp"
#include "DebugCollision.hpp"
#include "DebugScreenBoundary.hpp"

class DebugView : public Entity {
public:
    bool init(vec2 screen);
    void destroy();
    void draw(const mat3& projection) override;
    void draw(const mat3& projection, Salmon* salmon, const std::vector<Fish> * fish);
private:
    DebugDot debugDot;
    DebugCollision debugCollision;
    DebugScreenBoundary debugBoundary;
};


#endif //SALMON_DEBUGVIEW_HPP
