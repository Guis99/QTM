#include "..\include\QTM.hpp"

using namespace QTM;

// Don't wrap 'this' pointer inside of smart pointer in a class method if SP gets ref-counted after function call ends 

int main() {
    std::unordered_map<bool, std::string> score;
    score[true] = "pass"; score[false] = "fail";

    std::cout<<"test1"<<std::endl;
    int nx = 1; int ny = 1;
    QuadTreeMesh mesh(5, nx, ny, 1, 1);
    
    mesh.topCells[0]->subdivide();
    mesh.topCells[0]->children[1]->subdivide();
    mesh.topCells[0]->children[2]->subdivide();

    auto leaves = mesh.GetAllCells();
    mesh.leaves = leaves;
    mesh.assignNodes();

    std::cout<<score[leaves.size() == 10]<<std::endl;

    std::cout<<mesh.leaves.size()<<std::endl;
    for (auto leaf : mesh.leaves) {
        std::cout<<leaf->CID<<std::endl;
    }

    std::vector<std::shared_ptr<Cell>> toRefine = {leaves[5], leaves[6]};

    mesh.Refine(toRefine);
    std::cout<<mesh.leaves.size()<<std::endl;
    std::cout<<mesh.topCells[0]->children[3]->isLeaf()<<std::endl;
    std::cout<<score[mesh.leaves.size() == 19];

    return 0;
}