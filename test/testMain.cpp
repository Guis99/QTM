#include "..\include\QTM.hpp"

using namespace QTM;

// Don't wrap 'this' pointer inside of smart pointer in a class method if SP gets ref-counted after function call ends 

int main() {
    std::shared_ptr<Cell> initptr(nullptr);

    auto tree1ptr = std::make_shared<Cell>(initptr, 0, 1, 0, 0, 1);
    // auto tree1 = *tree1ptr;
    std::cout<<"here11"<<std::endl;
    tree1ptr->subdivide();
    std::cout<<"here12"<<std::endl;
    tree1ptr->children[0]->subdivide();
    tree1ptr->children[1]->subdivide();

    tree1ptr->children[0]->children[3]->subdivide();
    tree1ptr->children[0]->children[3]->children[0]->subdivide();
    tree1ptr->children[0]->children[3]->children[0]->children[2]->subdivide();
    
    std::vector<std::shared_ptr<Cell>> leaves = tree1ptr->traverse();
    std::cout<<leaves.size()<<std::endl;
    if (leaves.size() == 19) {
        std::cout<<"success"<<std::endl;
    } else {
        std::cout<<"no"<<std::endl;
    }

    

    return 0;
}