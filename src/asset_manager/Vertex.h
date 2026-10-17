#ifndef DL_TEMPLATE_POINT_H
#define DL_TEMPLATE_POINT_H
struct Vertex {
    // Position
    float x, y, z;
    // normal
    float nX, nY, nZ;
    // Texture coordinates and layer
    float u, v;
    int d;
};
/*
struct InstanceData {
    float x{.0f}, y{.0f}, z{.0f};
    int texIndex{0};
};*/

#endif