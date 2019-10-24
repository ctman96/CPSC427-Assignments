//
// Created by Cody on 10/22/2019.
//

#ifndef SALMON_ENTITYGRID_HPP
#define SALMON_ENTITYGRID_HPP

#include <vector>
#include "salmon.hpp"
#include "fish.hpp"
#include "turtle.hpp"
#include "common.hpp"

enum EType {
    empty,
    player,
    enemy,
    goal
};

struct pair {
    int x;
    int y;
};

struct Node {
    pair position;
    pair parent;
    EType type;
    float f;
    float g;
    float h;
    bool operator < (const Node& r) const
    {
        return f < r.f && position.x == r.position.x && position.y == r.position.y;
    }
};

class EntityGrid : public Entity{
public:
    bool init(int width, int height, int size);
    void clear();
    void addToGrid(const Fish& fish);
    void addToGrid(const Salmon& salmon);
    void addToGrid(const Turtle& turtle);
    void addBoxToGrid(vec2 tl, vec2 br, EType type);
    void draw(const mat3 &projection) override;
    void draw(const mat3 &projection, int index, EType type);
    std::vector<vec2> getPath(const Fish& fish);
private:
    std::vector<std::vector<EType>> grid;
    int gridW;
    int gridH;
    int size;
    std::vector<Vertex> m_vertices;
    void printNode(Node node);
    bool isValid(int x, int y);
};


#endif //SALMON_ENTITYGRID_HPP
