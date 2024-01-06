#include "..\include\QTM.hpp"

using namespace QTM;

int main() {
    std::cout<<"debug1"<<std::endl;
    std::shared_ptr<Cell> initptr(nullptr);
    std::cout<<"debug2"<<std::endl;
    auto tree1 = Cell(initptr, 0, 1, 0, 0, 1);
    std::cout<<"debug3"<<std::endl;
    tree1.subdivide();
    // std::cout<<"debug4"<<std::endl;
    tree1.children[3]->subdivide();
    // std::cout<<"debug5"<<std::endl;
    tree1.children[3]->children[0]->subdivide();
    tree1.children[3]->children[0]->children[0]->subdivide();
    tree1.children[0]->subdivide();
    tree1.children[1]->subdivide();
    tree1.children[1]->children[0]->subdivide();
    std::cout<<"debug6"<<std::endl;

    std::vector<std::shared_ptr<Cell>> leaves = tree1.traverse();
    std::cout<<"debug7"<<std::endl;
    std::cout<<leaves.size()<<std::endl;
    if (leaves.size() == 16) {
        std::cout<<"success"<<std::endl;
        std::cout<<"hi";
    } else {
        std::cout<<"no"<<std::endl;
    }

    return 0;
}