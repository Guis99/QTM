#include "..\include\QTM.hpp"

using namespace QTM;

QuadTreeMesh::QuadTreeMesh(int deg, int nx, int ny, double Lx, double Ly) {
    this->deg = deg;
    this->nx = nx;
    this->ny = ny;
    double dx = Lx / nx; 
    double dy = Ly / ny;

    // Loop to populate initial mesh with cells
    int CID = 0;
    topCells.reserve(nx*ny);
    for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
            topCells.push_back(std::make_shared<Cell>(CID, nullptr, 0, dx/2, dx/2 + x*dx, dx/2 + y*dx));
            CID++;
        }
    }
    numLeaves = CID;
    assignNodes();
}

std::vector<std::shared_ptr<Cell>> QuadTreeMesh::GetNeighborCells(int x, int y) {
    std::vector<std::shared_ptr<Cell>> out; out.reserve(4);

    std::shared_ptr<Cell> initptr(nullptr);

    int CID = nx*y + x;
    auto cell = topCells[CID];

    std::vector<int> idxs = {cell->CID + nx, cell->CID + 1, cell->CID - nx, cell->CID -1};
    std::vector<int> xs = {0, 1, 0, -1};
    std::vector<int> ys = {1, 0, -1, 0};

    int xx; int yy;
    for (int i=0; i<4; i++) {
        if (x+xs[i] < 0 || x+xs[i] >= nx || y+ys[i] < 0 || y+ys[i] >= ny) {
            out.push_back(nullptr);
        } else {
            out.push_back(topCells[idxs[i]]);
        }
    }
    return out;
}

void QuadTreeMesh::assignNodes() {
    int currNode = 0;
    int CID = 0;
    int numElementNodes = (deg+1)*(deg+1);
    for (auto baseCell : topCells) {
        auto leaves = baseCell->traverse();
        for (auto leaf : leaves) {
            leaf->setNodes(std::array<int,2>{currNode, currNode + numElementNodes - 1});
            leaf->CID = CID;
            CID++;
            currNode += numElementNodes;
        }
    }
}

std::vector<std::shared_ptr<Cell>> QuadTreeMesh::GetAllCells() {
    std::vector<std::shared_ptr<Cell>> out;
    out.resize(numLeaves);
    int offset = 0;

    for (auto baseCell : topCells) {
        auto leaves = baseCell->traverse();
        std::copy(leaves.begin(), leaves.end(), out.begin()+offset);
        offset += leaves.size();
    }

    return out;
}