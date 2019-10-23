//
// Created by Cody on 10/22/2019.
//

#include <cmath>
#include <iostream>
#include <algorithm>
#include <set>
#include <limits>
#include "EntityGrid.hpp"

bool EntityGrid::init(int width, int height, int size) {
    m_vertices.clear();

    // std::cout << width << "," << height << "," << size << std::endl;
    gridW = (int)std::ceil((float)width / (float)size);
    gridH = (int)std::ceil((float)height / (float)size);
    this->size = size;
    // std::cout << gridW << "," << gridH << std::endl;

    for (int i = 0; i < gridW; ++i) {
        //std::cout << "row " << i << ": ";
        std::vector<EType> row;
        for (int j = 0; j < gridH; ++j) {
            //std::cout << j << " ";
            row.emplace_back(EType::empty);
        }
        grid.emplace_back(row);
        //std::cout << std::endl;
    }

    // Vertex vertices[(gridW+1)][(gridH+1)];
    for (int i=0; i < gridW+1; ++i) {
        //std::cout << "row " << i << ": ";
        for (int j = 0; j < gridH+1; ++j) {
            Vertex vertex;
            vertex.position = {(float)(i*size), (float)(j*size), 1.f};
            m_vertices.emplace_back(vertex);
            //std::cout << j << "[" << vertices[vind].position.x << "," << vertices[vind].position.y <<"] ";
        }
        //std::cout << std::endl;
    }
    // std::cout << (gridW+1) * (gridH+1) << std::endl;


    uint16_t indices[((gridW+1) * gridH * 2) + ((gridH+1) * gridW * 2)];
    int ind = 0;
    // std::cout << "Vertical" << std::endl;
    // Vertical grid lines
    for(int x = 0; x < gridW + 1; x++) {
        for(int y = 0; y < gridH; y++) {
            indices[ind++] = x * (gridH+1) + y;
            indices[ind++] = x * (gridH+1) + y+1;
            // std::cout << "(" << x << "," << y << ") " << "["<< indices[ind-2]<< "]: " << vertices[indices[ind-2]].position.x << "," << vertices[indices[ind-2]].position.y << " | " << "["<< indices[ind-1] << "]: " << vertices[indices[ind-1]].position.x << "," << vertices[indices[ind-1]].position.y <<  std::endl;
        }
    }
    // std::cout << "Horizontal" << std::endl;
    // Horizontal grid lines
    for(int y = 0; y < gridH+1; y++) {
        for(int x = 0; x < gridW; x++) {
            indices[ind++] = x * (gridH+1) + y;
            indices[ind++] = (x + 1) * (gridH+1) + y;
            //std::cout << "(" << x << "," << y << ") " << "["<< indices[ind-2]<< "]: " << vertices[indices[ind-2]].position.x << "," << vertices[indices[ind-2]].position.y << " | " << "["<< indices[ind-1] << "]: " << vertices[indices[ind-1]].position.x << "," << vertices[indices[ind-1]].position.y <<  std::endl;
        }
    }

    /*
    uint16_t gridIndices
    for(int x = 0; x < gridW + 1; x++) {
        for(int y = 0; y < gridH + 1; y++) {

        }
    }
     */

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);


    // Index Buffer creation
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * ind, indices, GL_STATIC_DRAW);

    // Vertex Array (Container for Vertex + Index buffer)
    //glGenVertexArrays(1, &mesh.vao);

    if (gl_has_errors())
        return false;

    // Loading shaders
    if (!effect.load_from_file(shader_path("coloured.vs.glsl"), shader_path("coloured.fs.glsl")))
        return false;

    return true;
}

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

    // Drawing!
    glDrawElements(GL_LINES, ((gridW+1) * gridH * 2) + ((gridH+1) * gridW * 2), GL_UNSIGNED_SHORT, nullptr);

    for (int i=0; i < gridW; ++i) {
        for (int j = 0; j < gridH; ++j) {
            draw(projection, i*(gridH+1) + j, grid[i][j]);
        }
    }
}

void EntityGrid::draw(const mat3 &projection, int index, EType type) {
    // Could be optimized
    if (type == EType::empty) return;
    transform.begin();
    transform.end();

    Vertex vertices[4] = {
            m_vertices[index],
            m_vertices[index+1],
            m_vertices[index+gridH+2],
            m_vertices[index+gridH+1]
    };

    uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

    GLuint vbo;
    GLuint ibo;
    GLuint vao;

    // Vertex Buffer creation
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, vertices, GL_STATIC_DRAW);

    // Index Buffer creation
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

    // Vertex Array (Container for Vertex + Index buffer)
    glGenVertexArrays(1, &vao);

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
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

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

std::vector<vec2> EntityGrid::getPath(const Fish& fish) {
    //TODO pre-check if at destination;

    vec2 fbox = fish.get_bounding_box();
    vec2 fpos = fish.get_position();

    float wr = fbox.x/2;
    float hr = fbox.y/2;
    vec2 tl = {fpos.x-wr, fpos.y-hr};
    vec2 br = {fpos.x+wr, fpos.y+hr};

    auto l = bound((int)std::floor(tl.x / (float)size), 0, gridW);
    auto r = bound((int)std::ceil(br.x / (float)size), l, gridW);
    auto t = bound((int)std::floor(tl.y / (float)size), 0, gridH);
    auto b = bound((int)std::ceil(br.y / (float)size), t, gridH);

    auto w = r - l;
    auto h = b - t;

    const float MAX = std::numeric_limits<float>::max();

    // Generate node map with initial values
    Node map[gridW][gridH];
    for (int x = 0; x < gridW; x++){
        for (int y = 0; y < gridH; y++){
            map[x][y].position = {x, y};
            map[x][y].parent = {-1,-1};
            map[x][y].f = MAX;
            map[x][y].g = map[x][y].f;
            map[x][y].h = map[x][y].g;
        }
    }

    // Set starting node
    int x = (int)std::floor(l/2); // TODO??
    int y = (int)std::floor(t/2);
    map[x][y].f = 0.0;
    map[x][y].g = 0.0;
    map[x][y].h = 0.0;
    map[x][y].parent = {x,y};

    std::set<Node> open;
    open.emplace(map[x][y]);

    bool closed[gridW][gridH];

    do {
        // Get and remove lowest f score node from open
        auto it = open.begin();
        Node node = *it;
        open.erase(open.begin());
        // Add to closed
        closed[node.position.x][node.position.y] = true;

        // Get adjacent
        pair adjacent[4] = {{x-1,y}, {x,y-1}, {x+1, y}, {x, y+1}};
        for (auto n : adjacent) {
            // ignore invalid
            if (n.x < 0 || n.x >= gridW || n.y < 0 || n.y >= gridH) continue;

            // Check for destination (left edge of map)
            if (n.x == 0) {
                map[n.x][n.y].parent = node.position;
                std::vector<vec2> path;
                int px = n.x;
                int py = n.y;
                // Create path
                while (!(map[px][py].parent.x == px && map[px][py].parent.y == py)
                       && map[px][py].position.x == -1 && map[px][py].position.y == -1) {
                    vec2 pos = {
                            (float) (map[px][py].position.x * size) + (size / 2.f),
                            (float) (map[px][py].position.y * size) + (size / 2.f),
                    };
                    path.emplace_back(pos);

                    pair temp = map[px][py].parent;
                    px = temp.x;
                    py = temp.y;
                }
                // Add start node
                vec2 pos = {
                        (float) (map[px][py].position.x * size) + (size / 2.f),
                        (float) (map[px][py].position.y * size) + (size / 2.f),
                };
                path.emplace_back(pos);

                return path;
            }

            // check if not already closed
            if (!closed[node.position.x][node.position.y]){
                // calculate values
                float newG = node.g + 1.f;
                float newH = std::abs(n.x); // heuristic = straight line distance to the edge of map
                float newF = newG + newH;

                // TODO clearance checking?

                // Only update if better path
                if (map[n.x][n.y].f == MAX || map[n.x][n.y].f > newF) {
                    map[n.x][n.y].parent = node.position;
                    map[n.x][n.y].f = newF;
                    map[n.x][n.y].g = newG;
                    map[n.x][n.y].h = newH;
                    open.emplace(map[n.x][n.y]);
                }
            }
        }
    } while (!open.empty());


    return std::vector<vec2>();
}
