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
    tree1ptr->children[1]->subdivide();

    tree1ptr->children[0]->children[3]->subdivide();
    tree1ptr->children[0]->children[3]->children[0]->subdivide();
    tree1ptr->children[0]->children[3]->children[0]->children[2]->subdivide();
    
    std::vector<std::shared_ptr<Cell>> leaves = tree1ptr->traverse();
    for (auto ptr : leaves) {
        std::cout<<ptr<<", "<<ptr->level<<std::endl;
    }

    std::cout<<score[leaves.size() == 19]<<std::endl;

    auto tree2ptr = std::make_shared<Cell>(initptr, 0, 1, 0, 0, 1);
    tree2ptr->subdivide();
    tree2ptr->children[0]->subdivide();
    tree2ptr->children[1]->subdivide();
    tree2ptr->children[2]->subdivide();
    tree2ptr->children[3]->subdivide();

    bool inOrder = true;
    
    std::vector<std::shared_ptr<Cell>> leaves2 = tree2ptr->traverse();

    for (auto ptr : leaves2) {
        std::cout<<ptr<<", "<<ptr->level<<std::endl;
    }

    inOrder &= leaves2[0] == tree2ptr->children[0]->children[0];
    inOrder &= leaves2[1] == tree2ptr->children[0]->children[1];
    inOrder &= leaves2[2] == tree2ptr->children[0]->children[2];
    inOrder &= leaves2[3] == tree2ptr->children[0]->children[3];

    inOrder &= leaves2[4] == tree2ptr->children[1]->children[0];
    inOrder &= leaves2[5] == tree2ptr->children[1]->children[1];
    inOrder &= leaves2[6] == tree2ptr->children[1]->children[2];
    inOrder &= leaves2[7] == tree2ptr->children[1]->children[3];

    inOrder &= leaves2[8] == tree2ptr->children[2]->children[0];
    inOrder &= leaves2[9] == tree2ptr->children[2]->children[1];
    inOrder &= leaves2[10] == tree2ptr->children[2]->children[2];
    inOrder &= leaves2[11] == tree2ptr->children[2]->children[3];

    inOrder &= leaves2[12] == tree2ptr->children[3]->children[0];
    inOrder &= leaves2[13] == tree2ptr->children[3]->children[1];
    inOrder &= leaves2[14] == tree2ptr->children[3]->children[2];
    inOrder &= leaves2[15] == tree2ptr->children[3]->children[3];

    

    std::cout<<score[leaves2.size() == 16 && inOrder]<<std::endl;
    

    return 0;
}