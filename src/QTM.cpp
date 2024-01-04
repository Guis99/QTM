#include "..\include\QTM.hpp"

using namespace QTM;

QuadTreeMesh::QuadTreeMesh(int deg, int nx, int ny, double Lx, double Ly) {
    double dx = Lx / nx; 
    double dy = Ly / ny;

    // Loop to populate initial mesh with cells
    topCells.reserve(nx*ny);
    for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
            topCells.emplace_back(nullptr, dx/2, dx/2 + x*dx, dx/2 + y*dx, deg);
        }
    }
}