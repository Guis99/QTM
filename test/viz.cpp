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
    std::shared_ptr<Cell> initptr(nullptr);
    auto tree1ptr = std::make_shared<Cell>(initptr, 0, 1, 0, 0);
    tree1ptr->subdivide();
    tree1ptr->children[0]->subdivide();
    tree1ptr->children[1]->subdivide();
    // Initialize your quadtree here

    std::ofstream outFile("quadtree.json");
    exportToJson(tree1ptr, outFile);
}