#include "..\include\QTM.hpp"

using namespace QTM;

// Don't wrap 'this' pointer inside of smart pointer in a class method if SP gets ref-counted after function call ends 

int main() {
    std::unordered_map<bool, std::string> score;
    score[true] = "pass"; score[false] = "fail";

    std::shared_ptr<Cell> initptr(nullptr);

    // auto tree1ptr = std::make_shared<Cell>(initptr, 0, 1, 0, 0);
    QuadTreeMesh mesh(1,1,1,1,1);

    auto leave = mesh.GetAllCells();
    auto tree1ptr = leave[0];

    tree1ptr->subdivide();
    tree1ptr->children[0]->subdivide();
    tree1ptr->children[0]->children[2]->subdivide();
    tree1ptr->children[3]->subdivide();
    tree1ptr->children[3]->children[1]->subdivide();
    tree1ptr->children[3]->children[1]->children[1]->subdivide();


    int CID = 0;
    auto leaves = tree1ptr->traverse();
    for (auto leaf : leaves) {
        leaf->CID = CID;
        CID++;
    }

    auto neighborS = tree1ptr->children[0]->children[2]->children[2]->geqNeighbor(Direction::S);
    auto neighborN = tree1ptr->children[0]->children[2]->children[2]->geqNeighbor(Direction::N);
    auto neighborE = tree1ptr->children[0]->children[2]->children[2]->geqNeighbor(Direction::E);
    auto neighborW = tree1ptr->children[0]->children[2]->children[2]->geqNeighbor(Direction::W);

    std::cout<<neighborS->CID<<", "<<score[neighborS->CID == 7]<<std::endl;
    std::cout<<neighborN->CID<<", "<<score[neighborN->CID == 5]<<std::endl;
    std::cout<<neighborE->CID<<", "<<score[neighborE->CID == 3]<<std::endl;
    std::cout<<score[neighborW == tree1ptr->children[3]->children[1]->children[1]]<<std::endl;

    std::cout<<tree1ptr->children[0]->children[2]->children[2]->CID<<", "<<neighborN->CID<<", "<<neighborE->CID<<std::endl;

    std::cout<<"test2"<<std::endl;
    mesh = QuadTreeMesh(1,3,3,1,1);
    std::cout<<"mesh success"<<std::endl;   

    mesh.topCells[3]->subdivide();

    mesh.topCells[4]->subdivide();

    mesh.topCells[5]->subdivide();
    mesh.topCells[5]->children[3]->subdivide();
    mesh.topCells[5]->children[3]->children[3]->subdivide();

    auto allcells = mesh.GetAllCells();
    mesh.leaves = allcells;
    mesh.assignNodes();

    auto testcell1 = allcells[7];
    auto testcell2 = mesh.topCells[0]; 

    neighborN = mesh.geqNeighbor(Direction::N, testcell1);
    neighborE = mesh.geqNeighbor(Direction::E, testcell1);
    neighborS = mesh.geqNeighbor(Direction::S, testcell1);
    neighborW = mesh.geqNeighbor(Direction::W, testcell1);
    std::cout<<score[neighborN == mesh.topCells[7]]<<std::endl;
    std::cout<<score[neighborE == mesh.topCells[5]->children[3]]<<std::endl;
    std::cout<<score[neighborS == allcells[8]]<<std::endl;
    std::cout<<score[neighborW == allcells[10]]<<std::endl;

    leaves = mesh.GetAllCells();

    neighborN = mesh.geqNeighbor(Direction::N, testcell2);
    neighborE = mesh.geqNeighbor(Direction::E, testcell2);
    neighborS = mesh.geqNeighbor(Direction::S, testcell2);
    neighborW = mesh.geqNeighbor(Direction::W, testcell2);

    std::cout<<score[neighborN == mesh.topCells[3]]<<std::endl;
    std::cout<<score[neighborE == mesh.topCells[1]]<<std::endl;
    std::cout<<score[neighborS == nullptr]<<std::endl;
    std::cout<<score[neighborW == nullptr]<<std::endl;


    std::cout<<score[leaves.size() == 24]<<std::endl;
    

    
    return 0;
}