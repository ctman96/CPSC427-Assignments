//
// Created by Cody on 10/22/2019.
//

#include <cmath>
#include <iostream>
#include "EntityGrid.hpp"

bool EntityGrid::init(int width, int height, int size) {
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

    Vertex vertices[(gridW+1)][(gridH+1)];
    int vind = 0;
    for (int i=0; i < gridW+1; ++i) {
        //std::cout << "row " << i << ": ";
        for (int j = 0; j < gridH+1; ++j) {
            vertices[i][j].position = {i*size, j*size, 1.f};
            //std::cout << j << "[" << vertices[vind].position.x << "," << vertices[vind].position.y <<"] ";
            vind++;
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

    // Clearing errors
    gl_flush_errors();

    // Vertex Buffer creation
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vind, vertices, GL_STATIC_DRAW);


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
}

void EntityGrid::clear() {
    for (auto it = grid.begin() ; it != grid.end(); ++it)
        (*it).clear();
}

void EntityGrid::addToGrid(const Fish &fish) {
    // TODO
}

void EntityGrid::addToGrid(const Salmon &salmon) {
    // TODO
}

void EntityGrid::addToGrid(const Turtle &turtle) {
    // TODO
}
