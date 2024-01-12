#include "..\include\QTM.hpp"

using namespace QTM;

// Don't wrap 'this' pointer inside of smart pointer in a class method if SP gets ref-counted after function call ends 

int main() {
    std::unordered_map<bool, std::string> score;
    score[true] = "pass"; score[false] = "fail";

    std::shared_ptr<Cell> initptr(nullptr);

    auto tree1ptr = std::make_shared<Cell>(initptr, 0, 1, 0, 0, 1);

    tree1ptr->subdivide();
    tree1ptr->children[0]->subdivide();
    tree1ptr->children[0]->children[2]->subdivide();

    auto neighborS = tree1ptr->children[0]->children[2]->geqNeighbor(Direction::S);
    auto neighborN = tree1ptr->children[0]->children[2]->geqNeighbor(Direction::N);
    auto neighborE = tree1ptr->children[0]->children[2]->geqNeighbor(Direction::E);
    auto neighborW = tree1ptr->children[0]->children[2]->geqNeighbor(Direction::W);


    std::cout<<score[neighborS == tree1ptr->children[1] &&
                    neighborN == tree1ptr->children[0]->children[3] && 
                    neighborE == tree1ptr->children[0]->children[1] &&
                    neighborW == tree1ptr->children[3]]<<std::endl;
    
    

    return 0;
}