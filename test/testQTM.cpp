#include "..\include\QTM.hpp"

using namespace QTM;

// Don't wrap 'this' pointer inside of smart pointer in a class method if SP gets ref-counted after function call ends 

int main() {
    std::unordered_map<bool, std::string> score;
    score[true] = "pass"; score[false] = "fail";

    int nx = 4; int ny = 10;
    QuadTreeMesh mesh(5, nx, ny, 3, 2);
    mesh.topCells[0]->subdivide();
    mesh.topCells[2]->subdivide();
    mesh.assignNodes();

    int deg = mesh.deg;

    int currNode = 0;
    int numElementNodes = (deg+1)*(deg+1);

    bool numsExp = true;

    for (auto baseCell : mesh.topCells) {
        auto leaves = baseCell->traverse();
        for (auto leaf : leaves) {
            numsExp &= leaf->nodes[0] == currNode && leaf->nodes[1] == currNode + numElementNodes - 1;
            currNode += numElementNodes;
        }
    } 

    mesh.numLeaves = 46;
    std::cout<<"test1:"<<std::endl;
    std::cout<<score[numsExp]<<std::endl;
    std::cout<<score[mesh.GetAllCells().size() == 46]<<std::endl;


    // test2
    nx = 20; ny = 20;
    mesh = QuadTreeMesh(5, nx, ny, 3, 2);
    mesh.assignNodes();

    deg = mesh.deg;

    currNode = 0;
    numElementNodes = (deg+1)*(deg+1);

    numsExp = true;

    for (auto baseCell : mesh.topCells) {
        auto leaves = baseCell->traverse();
        for (auto leaf : leaves) {
            numsExp &= leaf->nodes[0] == currNode && leaf->nodes[1] == currNode + numElementNodes - 1;
            currNode += numElementNodes;
        }
    } 

    mesh.numLeaves = 400;
    std::cout<<"test2:"<<std::endl;
    std::cout<<score[numsExp]<<std::endl;
    std::cout<<score[mesh.GetAllCells().size() == 400]<<std::endl;

    // test3
    nx = 2; ny = 2;
    mesh = QuadTreeMesh(5, nx, ny, 3, 2);
    mesh.topCells[0]->subdivide();
    mesh.topCells[0]->children[0]->subdivide();
    mesh.topCells[0]->children[1]->subdivide();
    mesh.topCells[0]->children[3]->subdivide();
    mesh.topCells[0]->children[3]->children[2]->subdivide();
    mesh.topCells[2]->subdivide();
    mesh.topCells[2]->children[0]->subdivide();
    mesh.assignNodes();

    deg = mesh.deg;

    currNode = 0;
    numElementNodes = (deg+1)*(deg+1);

    numsExp = true;

    for (auto baseCell : mesh.topCells) {
        auto leaves = baseCell->traverse();
        for (auto leaf : leaves) {
            numsExp &= leaf->nodes[0] == currNode && leaf->nodes[1] == currNode + numElementNodes - 1;
            currNode += numElementNodes;
            std::cout<<leaf->CID<<": "<<leaf->level<<std::endl;
        }
    } 

    mesh.numLeaves = 25;
    std::cout<<"test3:"<<std::endl;
    std::cout<<score[numsExp]<<std::endl;
    std::cout<<score[mesh.GetAllCells().size() == 25]<<std::endl;


    return 0;
}