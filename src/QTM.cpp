#include "..\include\QTM.hpp"

using namespace QTM;

QuadTreeMesh::QuadTreeMesh(int deg, int nx, int ny, double Lx, double Ly) {
    this->deg = deg;
    this->numElemNodes = (deg+1)*(deg+1);
    this->nx = nx;
    this->ny = ny;
    double dx = Lx / nx; 
    double dy = Ly / ny;

    // Loop to populate initial mesh with cells
    int CID = 0;
    topCells.reserve(nx*ny);
    for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
            topCells.push_back(std::make_shared<Cell>(nullptr, 0, dx/2, dx/2 + x*dx, dx/2 + y*dx));
        }
    }
    numLeaves = nx*ny;
    assignNodes();
}

std::vector<std::shared_ptr<Cell>> QuadTreeMesh::GetNeighborCells(int x, int y) {
    std::vector<std::shared_ptr<Cell>> out; out.reserve(4);

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
    out.reserve(numLeaves);

    for (auto baseCell : topCells) {
        auto leaves = baseCell->traverse();
        out.insert(out.end(), leaves.begin(), leaves.end());
    }

    return out;
}

std::vector<int> QuadTreeMesh::GetBoundaryNodes(Direction direction, int CID) {
    std::vector<int> out; out.reserve(deg+1);
    int start = CID * numElemNodes;

    switch (direction) {
        case Direction::N : {
            start += (deg+1)*deg;
            break;
        }

        case Direction::E : {
            start += deg;
        }

        case Direction::S : {
            break;
        }

        case Direction::W : {
            break;
        }
    }

    int increment = (direction == Direction::N || direction == Direction::S) ? 1 : deg+1;

    for (int i=0; i<deg+1; i++) {
        out.push_back(start+increment*i);
    }

    return out;
}