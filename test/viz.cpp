#include <fstream>
#include <iostream>
#include "..\include\QTM.hpp"

using namespace QTM;

void exportToJson(std::shared_ptr<Cell> root, std::ostream& out) {
    out << "{";
    out << "\"x\": " << root->center[0] << ", ";
    out << "\"y\": " << root->center[1] << ", ";
    out << "\"width\": " << 2*root->width << ", ";
    out << "\"level\": " << root->level << ", ";
    out << "\"isLeaf\": " << (root->isLeaf() ? "true" : "false") << ", ";
    out << "\"CID\": " << root->CID << ", ";
    out << "\"children\": [";
    for (int i = 0; i < 4; ++i) {
        if (root->children[i] != nullptr) {
            if (i > 0) out << ", ";
            exportToJson(root->children[i], out);
        }
    }
    out << "]}";
}

int main() {
    // Assume root is the root of your quadtree
    // Initialize your quadtree here


    int nx = 4; int ny = 3;
    QuadTreeMesh mesh(5, nx, ny, nx, ny);
    
    mesh.leaves = mesh.GetAllCells();
    mesh.assignNodes();

    std::vector<std::shared_ptr<Cell>> toRefine = {mesh.leaves[5], mesh.leaves[6]};
    mesh.Refine(toRefine);

    toRefine = {mesh.leaves[5],mesh.leaves[6],mesh.leaves[7],mesh.leaves[8],
                mesh.leaves[9],mesh.leaves[10],mesh.leaves[11],mesh.leaves[12]};
    mesh.Refine(toRefine);

    toRefine = {mesh.leaves[30]};
    mesh.Refine(toRefine);

    toRefine = {mesh.leaves[30],mesh.leaves[36]};
    mesh.Refine(toRefine);

    std::cout<<mesh.leaves.size()<<std::endl;
    
    // toRefine = {mesh.leaves[15]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[25], mesh.leaves[35]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[50]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[37]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[49]};
    // mesh.Refine(toRefine);

    // mesh.topCells[0]->subdivide();
    // mesh.topCells[0]->children[1]->subdivide();
    // mesh.topCells[0]->children[2]->subdivide();

    // auto leaves = mesh.GetAllCells();
    // mesh.leaves = leaves;
    // mesh.assignNodes();

    // std::vector<std::shared_ptr<Cell>> toRefine = {leaves[5], leaves[6]};

    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[4]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[11],mesh.leaves[12],mesh.leaves[7]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[13],mesh.leaves[3]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[22]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[31]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[40]};
    // mesh.Refine(toRefine);


    std::ofstream outFile("quadtree.json");
    outFile << "{";
    outFile << "\"x\": " << 0 << ", ";
    outFile << "\"y\": " << 0 << ", ";
    outFile << "\"width\": " << 0 << ", ";
    outFile << "\"level\": " << 0 << ", ";
    outFile << "\"isLeaf\": " << "false" << ", ";
    outFile << "\"CID\": " << 0 << ", ";
    outFile << "\"children\": [";
    for (auto cell : mesh.topCells) {
        exportToJson(cell, outFile);
        if (cell != mesh.topCells.back()) {
            outFile << ", ";
        }
    }
    outFile << "]}";
}