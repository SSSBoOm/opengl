#pragma once
#include <glm/glm.hpp>

class GroundMesh {
public:
    unsigned int VAO = 0, VBO = 0;

    void build();
    void draw() const;
};
