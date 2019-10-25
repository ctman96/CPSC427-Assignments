//
// Created by Cody on 10/22/2019.
//

#ifndef SALMON_DEBUGSCREENBOUNDARY_HPP
#define SALMON_DEBUGSCREENBOUNDARY_HPP


#include <common.hpp>

// Draws the screen boundary square
class DebugScreenBoundary : public Entity{
public:
    bool init(vec2 screen);
    void destroy();
    void draw(const mat3 &projection) override;
};


#endif //SALMON_DEBUGSCREENBOUNDARY_HPP
