#include <fstream>
#include <iostream>
#include "../include/QTM.hpp"

using namespace QTM;

void exportToJson(QuadTreeMesh mesh, std::ostream& out) {
    std::vector<std::shared_ptr<Cell>> allLeaves;
    for (auto cell : mesh.topCells) {
        if (cell->isLeaf()) {
            allLeaves.push_back(cell);
        } else {
            auto leaves = cell->traverse();
            allLeaves.insert(allLeaves.end(), leaves.begin(), leaves.end());
        }
    }
    out << "{";
    out << "\"children\": [\n";

    out << "{";
    out << "\"x\": " << allLeaves[0]->center[0] << ", ";
    out << "\"y\": " << allLeaves[0]->center[1] << ", ";
    out << "\"width\": " << 2*allLeaves[0]->width << ", ";
    out << "\"level\": " << allLeaves[0]->level << ", ";
    out << "\"CID\": " << allLeaves[0]->CID << "}";

    for (int i=1; i<allLeaves.size(); i++) {
        auto leaf = allLeaves[i];
        out <<",\n{";
        out << "\"x\": " << leaf->center[0] << ", ";
        out << "\"y\": " << leaf->center[1] << ", ";
        out << "\"width\": " << 2*leaf->width << ", ";
        out << "\"level\": " << leaf->level << ", ";
        out << "\"CID\": " << leaf->CID << "} ";
    }
    out << "]}";
}

int main() {
    // Assume root is the root of your quadtree
    // Initialize your quadtree here


    int nx = 1; int ny = 1;
    QuadTreeMesh mesh(2, nx, ny, nx, ny);

    // std::vector<std::shared_ptr<Cell>> toRefine = {mesh.leaves[0], mesh.leaves[2]};
    // mesh.Refine(toRefine);

    // auto boundaryNodes = mesh.boundaryNodes;
    // auto freeNodes = mesh.freeNodes;


    // std::cout<<"free nodes: "<<std::endl;
    // for (int node : freeNodes) {
    //     std::cout<<node<<std::endl;
    // }

    // int nb = 0;
    // std::cout<<"boundary nodes: "<<std::endl;
    // for (auto vec : boundaryNodes) {
    //     std::cout<<"bound:"<<std::endl;
    //     for (auto node : vec) {
    //         std::cout<<node<<std::endl;
    //     }
    //     nb+=vec.size();
    // }

    // std::cout<<"num boundary: "<<nb<<std::endl;
    // std::cout<<"num free: "<<freeNodes.size()<<std::endl;

    std::vector<std::shared_ptr<Cell>> toRefine;
    toRefine = {mesh.leaves[0]};
    mesh.Refine(toRefine);

    toRefine = {mesh.leaves[0],mesh.leaves[2]};
    mesh.Refine(toRefine);
    // std::cout<<"hi1"<<std::endl;
    // toRefine = {mesh.leaves[5],mesh.leaves[6],mesh.leaves[7],mesh.leaves[8],
    //             mesh.leaves[9],mesh.leaves[10],mesh.leaves[11],mesh.leaves[12]};
    // std::cout<<"hi2"<<std::endl;
    // mesh.Refine(toRefine);
    // std::cout<<"hi3"<<std::endl;

    // toRefine = {mesh.leaves[14],mesh.leaves[15],mesh.leaves[16],mesh.leaves[17],
    //             mesh.leaves[18],mesh.leaves[19],mesh.leaves[20],mesh.leaves[21],
    //             mesh.leaves[22],mesh.leaves[23],mesh.leaves[24],mesh.leaves[25],
    //             mesh.leaves[26],mesh.leaves[27],mesh.leaves[28],mesh.leaves[29]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[33]};
    // mesh.Refine(toRefine);

    // std::cout<<mesh.leaves.size()<<std::endl;
    
    // toRefine = {mesh.leaves[15]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[2], mesh.leaves[29]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[25], mesh.leaves[35]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[50]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[37]};
    // mesh.Refine(toRefine);

    // toRefine = {mesh.leaves[49]};
    // mesh.Refine(toRefine);

    // int nx = 1; int ny = 1;
    // QuadTreeMesh mesh(5, nx, ny, nx, ny);
    
    // mesh.leaves = mesh.GetAllCells();
    // mesh.assignNodes();

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
    exportToJson(mesh, outFile);

    std::cout<<"North boundaries:"<<std::endl;
    for (const auto& cell : mesh.boundaryCells[0]) {
        std::cout<<cell->CID<<std::endl;
    }

    std::cout<<"East boundaries:"<<std::endl;
    for (const auto& cell : mesh.boundaryCells[1]) {
        std::cout<<cell->CID<<std::endl;
    }

    std::cout<<"South boundaries:"<<std::endl;
    for (const auto& cell : mesh.boundaryCells[2]) {
        std::cout<<cell->CID<<std::endl;
    }

    std::cout<<"West boundaries:"<<std::endl;
    for (const auto& cell : mesh.boundaryCells[3]) {
        std::cout<<cell->CID<<std::endl;
    }
    // outFile << "{";
    // outFile << "\"x\": " << 0 << ",\n ";
    // outFile << "\"y\": " << 0 << ",\n ";
    // outFile << "\"width\": " << 0 << ",\n ";
    // outFile << "\"level\": " << 0 << ",\n ";
    // outFile << "\"isLeaf\": " << "false" << ",\n ";
    // outFile << "\"CID\": " << 0 << ",\n ";
    // outFile << "\"children\": [\n";
    // for (auto cell : mesh.topCells) {
    //     exportToJson(cell, outFile, 1);
    //     if (cell != mesh.topCells.back()) {
    //         outFile << ", ";
    //     }
    // }
    // outFile << "]}";
}