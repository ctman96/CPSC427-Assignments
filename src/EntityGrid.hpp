//
// Created by Cody on 10/22/2019.
//

#ifndef SALMON_ENTITYGRID_HPP
#define SALMON_ENTITYGRID_HPP

#include <vector>
#include "salmon.hpp"
#include "common.hpp"

enum EType {
    empty,
    player,
    enemy,
    goal
};

class EntityGrid : public Entity{
public:
    bool init(int width, int height, int size);
    void clear();
    void addToGrid(const Fish& fish);
    void addToGrid(const Salmon& salmon);
    void addToGrid(const Turtle& turtle);
    void draw(const mat3 &projection);
private:
    std::vector<std::vector<EType>> grid;
    int gridW;
    int gridH;
    int size;
};


#endif //SALMON_ENTITYGRID_HPP
