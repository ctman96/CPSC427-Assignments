//
// Created by Cody on 10/22/2019.
//

#include <cmath>
#include <iostream>
#include <algorithm>
#include <set>
#include <limits>
#include "EntityGrid.hpp"

// Initialization, generate grid
bool EntityGrid::init(int width, int height, int size) {
    m_vertices.clear();
    m_indices.clear();

    // Calculate grid size
    gridW = (int)std::ceil((float)width / (float)size);
    gridH = (int)std::ceil((float)height / (float)size);
    this->size = size;

    // Generate EType grid
    for (int i = 0; i < gridW; ++i) {
        std::vector<EType> row;
        for (int j = 0; j < gridH; ++j) {
            row.emplace_back(EType::empty);
        }
        grid.emplace_back(row);
    }

    // Generate grid vertices
    for (int i=0; i < gridW+1; ++i) {
        for (int j = 0; j < gridH+1; ++j) {
            Vertex vertex;
            vertex.position = {(float)(i*size), (float)(j*size), 1.f};
            m_vertices.emplace_back(vertex);
        }
    }


    // Generate Grid indices
    //uint16_t indices[((gridW+1) * gridH * 2) + ((gridH+1) * gridW * 2)];
    //int ind = 0;

    // Vertical grid lines
    for(int x = 0; x < gridW + 1; x++) {
        for(int y = 0; y < gridH; y++) {
            m_indices.emplace_back(x * (gridH+1) + y);
            m_indices.emplace_back(x * (gridH+1) + y+1);
        }
    }
    // Horizontal grid lines
    for(int y = 0; y < gridH+1; y++) {
        for(int x = 0; x < gridW; x++) {
            m_indices.emplace_back(x * (gridH+1) + y);
            m_indices.emplace_back((x + 1) * (gridH+1) + y);
        }
    }

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);


    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

    // Vertex Array (Container for Vertex + Index buffer)
    //glGenVertexArrays(1, &mesh.vao);

    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("coloured.vs.glsl"), shader_path("coloured.fs.glsl")))
        return false;

    return square.init((float)size);
}

void EntityGrid::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    //glDeleteBuffers(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);

    square.destroy();
}

// Set all squares as emtpy
void EntityGrid::clear() {
    for (int x = 0; x < gridW; x++) {
        grid[x].clear();
        for (int y = 0; y < gridH; y++) {
            grid[x].emplace_back(EType::empty);
        }
    }
}

void EntityGrid::addToGrid(const Fish &fish) {
    vec2 box = fish.get_bounding_box();
    vec2 pos = fish.get_position();

    float w = box.x/2;
    float h = box.y/2;
    vec2 tl = {pos.x-w, pos.y-h};
    vec2 br = {pos.x+w, pos.y+h};


    addBoxToGrid(tl, br, EType::goal);
}

void EntityGrid::addToGrid(const Salmon &salmon) {
    vec2 box = salmon.get_bounding_box();
    vec2 pos = salmon.get_position();

    float w = box.x/2;
    float h = box.y/2;
    vec2 tl = {pos.x-w, pos.y-h};
    vec2 br = {pos.x+w, pos.y+h};


    addBoxToGrid(tl, br, EType::player);
}

void EntityGrid::addToGrid(const Turtle &turtle) {
    vec2 box = turtle.get_bounding_box();
    vec2 pos = turtle.get_position();

    float w = box.x/2;
    float h = box.y/2;
    vec2 tl = {pos.x-w, pos.y-h};
    vec2 br = {pos.x+w, pos.y+h};


    addBoxToGrid(tl, br, EType::enemy);
}

int bound(int val, int low, int hi) {
    return std::max(low, std::min(val, hi));
}

// Calculates which squares the box(tl,br) occupies, and sets as type
void EntityGrid::addBoxToGrid(vec2 tl, vec2 br, EType type) {
    auto l = bound((int)std::floor(tl.x / (float)size), 0, gridW);
    auto r = bound((int)std::ceil(br.x / (float)size), l, gridW);
    auto t = bound((int)std::floor(tl.y / (float)size), 0, gridH);
    auto b = bound((int)std::ceil(br.y / (float)size), t, gridH);


    for (int x = l; x < r; x++) {
        for (int y = t; y < b; y++) {
            grid[x][y] = type;
        }
    }
}


void EntityGrid::draw(const mat3 &projection) {
    transform.begin();
    transform.end();

    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "color");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

    // Setting vertices and indices
    //glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.out);
    float color[] = { 1.f, 1.f, 1.f };
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Get number of infices from buffer,
    // we know our vbo contains both colour and position information, so...
    GLint size = 0;
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    GLsizei num_indices = size / sizeof(uint16_t);

    // Drawing!
    glDrawElements(GL_LINES, num_indices, GL_UNSIGNED_SHORT, nullptr); // TODO crashing here, segfault???

    // Draw grid square colors
    for (int i=0; i < gridW; ++i) {
        for (int j = 0; j < gridH; ++j) {
            Vertex v = m_vertices[i*(gridH+1)+j];
            square.draw(projection, {v.position.x, v.position.y}, grid[i][j]);
        }
    }
}


bool isDestFish(const std::vector<std::vector<EType>>& egrid, pair pos, pair dest) {
    return pos.x == 0; // Fish destination is anywhere on left edge of screen
}
float fishH(const std::vector<std::vector<EType>>& grid, pair pos, pair dest) {
    return std::abs(pos.x); // Heuristic = distance to left edge of screen - Admissible
}
std::vector<vec2> EntityGrid::getPath(const Fish& fish) {
    vec2 fpos = fish.get_position();
    vec2 fbox = fish.get_bounding_box();

    return search(fpos, fbox, {EType::player}, &isDestFish, &fishH);
}


bool isDestTurtle(const std::vector<std::vector<EType>>& grid, pair pos, pair dest) {
    return pos.x == dest.x && pos.y == dest.y; // Turtle destination is the specific dest point (player);
}
float turtleH(const std::vector<std::vector<EType>>& grid, pair pos, pair dest) {
    // Direct distance to player - admissible
    return (float)(std::sqrt(std::pow((pos.x - dest.x), 2) + std::pow((pos.y - dest.y), 2) ));
}

std::vector<vec2> EntityGrid::getPath(const Turtle &turtle, const Salmon &salmon) {
    vec2 tpos = turtle.get_position();
    vec2 tbox = turtle.get_bounding_box();

    vec2 spos = salmon.get_position();
    auto sx = (int)std::floor((spos.x) / (float)size);
    auto sy = (int)std::floor((spos.y) / (float)size);
    pair s = { sx, sy };

    return search(tpos, tbox, {EType::goal}, &isDestTurtle, &turtleH, s);
}


bool isDestSalmon(const std::vector<std::vector<EType>>& grid, pair pos, pair dest) {
    return grid[pos.x][pos.y] == EType::goal;
}
float salmonH(const std::vector<std::vector<EType>>& grid, pair pos, pair dest) {
    // Direct distance to closest fish - admissible?
    return (float)(std::sqrt(std::pow((pos.x - dest.x), 2) + std::pow((pos.y - dest.y), 2) ));
}

std::vector<vec2> EntityGrid::getPath(const Salmon &salmon, const std::vector<Fish> &fishes) {
    vec2 spos = salmon.get_position();
    vec2 sbox = salmon.get_bounding_box();

    // Find closest fish for heuristic
    if (fishes.empty()) return std::vector<vec2>();
    vec2 fpos = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
    float mindist = std::numeric_limits<float>::max();
    for (const auto &fish : fishes) {
        float dist = (float)sqrt(pow(fish.get_position().x - spos.x, 2) + pow(fish.get_position().y - spos.y, 2));
        if (dist < mindist) {
            fpos = fish.get_position();
            mindist = dist;
        }
    }
    auto fx = (int)std::floor((fpos.x) / (float)size);
    auto fy = (int)std::floor((fpos.y) / (float)size);
    pair f = { fx, fy };

    return search(spos, sbox, {EType::enemy}, &isDestSalmon, &salmonH, f);
}


// Debug printing node details
void EntityGrid::printNode(Node node) {
    std::cout << "Node { " << std::endl;
    std::cout <<"\tposition: (" << node.position.x << "," << node.position.y << ")," << std::endl;
    std::cout <<"\tparent: (" << node.parent.x << "," << node.parent.y << ")," << std::endl;
    std::cout <<"\tg=" << node.g << ", h=" << node.h << ", f=" << node.f << std::endl;
    std::cout << "}" << std::endl;
}

// Check if x,y within grid
bool EntityGrid::isValid(int x, int y) {
    return !(x < 0 || x >= gridW || y < 0 || y >= gridH);
}

// A* Search
std::vector<vec2> EntityGrid::search(vec2 position, vec2 bbox, const std::vector<EType> avoid, isDestinationFn isDest, heuristicFn hfn, pair dest, bool DEBUG_LOG) {
    // Caclulate size of the object searching
    float wr = bbox.x/2;
    float hr = bbox.y/2;
    vec2 tl = {position.x-wr, position.y-hr};
    vec2 br = {position.x+wr, position.y+hr};

    auto l = bound((int)std::floor(tl.x / (float)size), 0, gridW);
    auto r = bound((int)std::ceil(br.x / (float)size), l, gridW);
    auto t = bound((int)std::floor(tl.y / (float)size), 0, gridH);
    auto b = bound((int)std::ceil(br.y / (float)size), t, gridH);

    auto w = r - l;
    auto h = b - t;

    // Determine centeral square to search from
    auto fx = (int)std::floor((position.x) / (float)size);
    auto fy = (int)std::floor((position.y) / (float)size);

    // Keep offset for path generation
    auto offsetX = position.x - (size * fx);
    auto offsetY = position.y - (size * fy);

    const float MAX = std::numeric_limits<float>::max();

    // Track which nodes have been closed
    bool closed[gridW][gridH];

    // Generate node map with initial values
    Node map[gridW][gridH];
    for (int x = 0; x < gridW; x++){
        for (int y = 0; y < gridH; y++){
            map[x][y].position = {x, y};
            map[x][y].parent = {-1,-1};
            map[x][y].f = MAX;
            map[x][y].g = map[x][y].f;
            map[x][y].h = map[x][y].g;
            closed[x][y] = false;
        }
    }

    // If starting position is not valid, or if we're already at the destination, return empty
    if (!isValid(fx, fy) || isDest(grid, {fx, fy}, dest)) {
        return std::vector<vec2>();
    }

    // Set starting node
    int x = fx;
    int y = fy;
    map[x][y].f = 0.0;
    map[x][y].g = 0.0;
    map[x][y].h = 0.0;
    map[x][y].parent = {x,y};

    // Add the starting node to the list of open nodes
    std::vector<Node> open;
    open.emplace_back(map[x][y]);
    int debugopencount = 1;

    if (DEBUG_LOG)std::cout << std::endl << "=========================" << std::endl << "startSearch" << std::endl << std::endl;

    int iterN = 0;
    do {
        if (DEBUG_LOG)std::cout << std::endl << "ITERATION START: " << iterN << std::endl;
        if (DEBUG_LOG)std::cout << std::endl << "open (" << open.size() << "):" << debugopencount << std::endl;

        // Get node with lowest f value from open
        Node node;
        float tmp = MAX;
        std::vector<Node>::iterator nodeIt;
        for (auto it = open.begin(); it != open.end(); it++) {
            if ((*it).f < tmp) {
                tmp = (*it).f;
                nodeIt = it;
            }
        }
        node = (*nodeIt);
        int curX = node.position.x;
        int curY = node.position.y;

        if (DEBUG_LOG) {for (auto n : open) printNode(n);}
        if (DEBUG_LOG)std::cout << std::endl << "getLowest:" << std::endl;

        // Remove the node from open
        open.erase(nodeIt);
        debugopencount--;

        // Add to closed
        closed[node.position.x][node.position.y] = true;

        // Check the node's four adjacent squares (W,N,E,S)
        pair adjacent[4] = {{curX-1,curY}, {curX,curY-1}, {curX+1, curY}, {curX, curY+1}};
        for (auto n : adjacent) {
            int adjX = n.x;
            int adjY = n.y;

            // Ignore if position is invalid
            if (!isValid(adjX, adjY)) {
                if (DEBUG_LOG)std::cout << "Node (" << adjX <<","<< adjY << ") not valid, ignoring" << std::endl;
                continue;
            }

            // Check for clearance
            bool clear = true;
            int clearBoundL = std::max((adjX - (int)std::floor(w/2)), 0);
            int clearBoundR = std::min((adjX + (int)std::ceil(w/2) + 1), gridW);
            int clearBoundT = std::max((adjY - (int)std::floor(h/2)), 0);
            int clearBoundB = std::min((adjY + (int)std::ceil(h/2) + 1), gridH);
            for (int clearX = clearBoundL; clearX < clearBoundR; clearX++) {
                for (int clearY = clearBoundT; clearY < clearBoundB; clearY++) {
                    for (auto type : avoid) {
                        if (grid[clearX][clearY] == type) {
                            if (DEBUG_LOG) std::cout << "Node (" << clearX << "," << clearY << ") is avoided type" << std::endl;
                            clear = false;
                        }
                    }
                }
            }
            if (!clear){
                if (DEBUG_LOG) std::cout << "Ignoring" << std::endl;
                continue;
            }

            // Check for destination
            if (isDest(grid, {adjX, adjY}, dest)) {
                if (DEBUG_LOG)std::cout << "Node (" << adjX <<","<< adjY << ") is destination" << std::endl;
                map[adjX][adjY].parent = node.position;
                std::vector<vec2> path;

                int px = adjX;
                int py = adjY;

                // Create path by following nodes' parents back to the start node
                if (DEBUG_LOG) std::cout << "Generating path: " << std::endl;
                while (!(map[px][py].parent.x == px && map[px][py].parent.y == py)
                       && map[px][py].position.x != -1 && map[px][py].position.y != -1) {
                    vec2 pos = {
                            // Return path offset to the entity's actual position
                            (map[px][py].position.x * size + offsetX),
                            (map[px][py].position.y * size + offsetY),
                    };
                    path.emplace_back(pos);
                    if (DEBUG_LOG)std::cout << "(" << pos.x <<","<< pos.y<<")" << ", ";

                    pair temp = map[px][py].parent;
                    px = temp.x;
                    py = temp.y;
                }
                // Note: Ignoring starting node

                if (DEBUG_LOG)std::cout << std::endl << "Successful Search" << std::endl << std::endl<< "=========================" << std::endl;
                return path;
            }

            // Check if not already closed
            if (!closed[adjX][adjY]){
                if (DEBUG_LOG)std::cout << "Node (" << adjX <<","<< adjY << ") is not closed, checking path" << std::endl;

                // Calculate new F value
                float newG = node.g + 1.f; // Since we're only moving in cardinal directions, just increment G
                float newH = hfn(grid, {adjX, adjY}, {dest.x,dest.y});
                float newF = newG + newH;


                if (DEBUG_LOG)std::cout << "new(f="<<newF<<",g="<<newG<<",h="<<newH<<"), old(f="<<map[adjX][adjY].f<<",g="<<map[adjX][adjY].g<<",h="<<map[adjX][adjY].h<<")"<< std::endl;

                // If it is a new node or has a better f value, update values and add to frontier
                if (map[adjX][adjY].f == MAX || map[adjX][adjY].f > newF) {
                    map[adjX][adjY].parent = node.position;
                    map[adjX][adjY].f = newF;
                    map[adjX][adjY].g = newG;
                    map[adjX][adjY].h = newH;
                    open.emplace_back(map[adjX][adjY]);
                    debugopencount++;
                    if (DEBUG_LOG)std::cout << "Added node to open: " << std::endl;
                    if (DEBUG_LOG) printNode(map[adjX][adjY]);
                } else {
                    if (DEBUG_LOG)std::cout << "Path not better" << std::endl;
                }
            } else {
                if (DEBUG_LOG)std::cout << "Node (" << adjX <<","<< adjY << ") is closed" << std::endl;
            }
        }
        iterN++;
    } while (!open.empty());

    if (DEBUG_LOG)std::cout << std::endl<< "Failed Search" << std::endl << std::endl << "=========================" << std::endl;
    // Unable to find path, return empty
    return std::vector<vec2>();
}

bool EntityGridSquare::init(float size) {
    Vertex vertices[4];
    vertices[0].position = { 0, size, -0.01f };
    vertices[1].position = { size, size, -0.01f };
    vertices[2].position = { size, 0, -0.01f };
    vertices[3].position = { 0, 0, -0.01f };

    // Counterclockwise as it's the default opengl front winding direction.
    uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STATIC_DRAW);

    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

    // Vertex Array (Container for Vertex + Index buffer)
    glGenVertexArrays(1, &mesh.vao);
    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("coloured.vs.glsl"), shader_path("coloured.fs.glsl")))
        return false;

    return true;
}

void EntityGridSquare::destroy() {
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.ibo);
    glDeleteBuffers(1, &mesh.vao);

    glDeleteShader(effect.vertex);
    glDeleteShader(effect.fragment);
    glDeleteShader(effect.program);
}

void EntityGridSquare::draw(const mat3 &projection) {}

void EntityGridSquare::draw(const mat3 &projection, vec2 pos, EType type) {
    if (type == EType::empty) return;
    transform.begin();
    transform.translate(pos);
    transform.rotate(0);
    transform.scale({1.f,1.f});
    transform.end();

    // Setting shaders
    glUseProgram(effect.program);

    // Enabling alpha channel for textures
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Getting uniform locations for glUniform* calls
    GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
    GLint color_uloc = glGetUniformLocation(effect.program, "color");
    GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

    // Setting vertices and indices
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

    // Input data location as in the vertex buffer
    GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
    glEnableVertexAttribArray(in_position_loc);
    glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Setting uniform values to the currently bound program
    glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)&transform.out);
    float color[] = { 0.f, 0.f, 0.f };
    switch(type) {
        case EType::player: {
            color[1] = 1.f;
            break;
        } case EType::enemy: {
            color[0] = 1.f;
            break;
        } case EType::goal: {
            color[0] = 1.f;
            color[1] = 1.f;
            break;
        } default: {
            color[0] = 1.f;
            color[1] = 1.f;
            color[2] = 1.f;
        }
    }
    glUniform3fv(color_uloc, 1, color);
    glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)&projection);

    // Drawing!
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}
