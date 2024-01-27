#include "..\include\QTM.hpp"

using namespace QTM;

// Don't wrap 'this' pointer inside of smart pointer in a class method if SP gets ref-counted after function call ends 

int main() {
    std::unordered_map<bool, std::string> score;
    score[true] = "pass"; score[false] = "fail";

    int nx = 3; int ny = 3;
    QuadTreeMesh mesh(3, nx, ny, 3, 2);

    for (int j=0; j<ny; j++) {
        for (int i=0; i<nx; i++) {
            auto neighbors = mesh.GetTopNeighborCells(i,j);
            for (auto neighbor : neighbors) {
                if (neighbor.get() == nullptr) {
                    std::cout<<"outOfBounds"<<", ";
                } else {
                    std::cout<<neighbor->CID<<", ";
                }
            }
            std::cout<<std::endl;
        }
        
    }
    
    

    return 0;
}