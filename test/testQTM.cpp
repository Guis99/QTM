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

    for (auto baseCell : mesh.topCells) {
        auto leaves = baseCell->traverse();
        for (auto leaf : leaves) {
            std::cout<<leaf->nodes[0]<<" to "<<leaf->nodes[1]<<std::endl;
        }
    }
    
    

    return 0;
}