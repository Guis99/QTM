#include "../include/QTM.hpp"

using namespace QTM;

// Don't wrap 'this' pointer inside of smart pointer in a class method if SP gets ref-counted after function call ends 

int main() {
    std::unordered_map<bool, std::string> score;
    score[true] = "pass"; score[false] = "fail";

    std::cout<<"test 1"<<std::endl;
    int nx = 1; int ny = 1;
    QuadTreeMesh mesh(5, nx, ny, 1, 1);
    
    mesh.topCells[0]->subdivide();
    mesh.topCells[0]->children[1]->subdivide();
    mesh.topCells[0]->children[2]->subdivide();

    auto leaves = mesh.GetAllCells();
    mesh.leaves = leaves;
    mesh.assignNodes();

    std::cout<<score[leaves.size() == 10]<<std::endl;

    DEBUG_PRINT(mesh.leaves.size());
    for (auto leaf : mesh.leaves) {
        DEBUG_PRINT(leaf->CID);
    }

    std::vector<std::shared_ptr<Cell>> toRefine = {leaves[5], leaves[6]};

    mesh.Refine(toRefine);
    DEBUG_PRINT(mesh.leaves.size());
    DEBUG_PRINT(mesh.topCells[0]->children[3]->isLeaf());
    std::cout<<score[mesh.leaves.size() == 19]<<std::endl;

    return 0;
}