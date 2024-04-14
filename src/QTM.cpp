#include "..\include\QTM.hpp"

using namespace QTM;

QuadTreeMesh::QuadTreeMesh(int deg, int nx, int ny, double Lx, double Ly) {
    this->deg = deg;
    this->numElemNodes = (deg+1)*(deg+1);
    this->nx = nx;
    this->ny = ny;
    double dx = Lx / nx; 
    double dy = Ly / ny;

    // declare vectors of boundary cells
    std::vector<std::shared_ptr<Cell>> NB;
    std::vector<std::shared_ptr<Cell>> EB;
    std::vector<std::shared_ptr<Cell>> SB;
    std::vector<std::shared_ptr<Cell>> WB;
    
    // Loop to populate initial mesh with cells
    int CID = 0;
    topCells.reserve(nx*ny);
    topNeighbors.reserve(nx*ny);
    for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
            auto nextCell = std::make_shared<Cell>(nullptr, 0, dx/2, dx/2 + x*dx, dx/2 + y*dx);
            nextCell->CID = CID; nextCell->topIdx = CID; CID++;
            topCells.push_back(nextCell);
            // if (x==0 || x==nx-1 || y==0 || y==ny-1) {
            //     nextCell->isBoundary = true;
            // }
            if (y==0) {
                SB.push_back(nextCell);
            }
            if (y==ny-1) {
                NB.push_back(nextCell);
            }
            if (x==0) {
                WB.push_back(nextCell);
            }
            if (x==nx-1) {
                EB.push_back(nextCell);
            }
        }
    }

    boundaryCells = {NB, EB, SB, WB};

    for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
            topNeighbors.push_back(GetTopNeighborCells(x, y));
        }
    }
    numLeaves = nx*ny;
    gaussPoints.reserve(deg+1);
    halfGaussPoints.reserve(2*deg+1);

    for (int i=0; i < deg+1; i++) {
        gaussPoints.push_back(-cos(i*PI/deg));
    }
    for (double gp : gaussPoints) {
        halfGaussPoints.push_back((gp+1)/2 - 1);
    }
    for (int i=1; i<deg+1; i++) {
        double gp = gaussPoints[i];
        halfGaussPoints.push_back((gp+1)/2);
    }
    leaves = topCells;
    assignNodes();
    ClassifyNodes();
    nodePositions = AllNodePos();
}

std::vector<std::shared_ptr<Cell>> QuadTreeMesh::GetTopNeighborCells(int x, int y) {
    std::vector<std::shared_ptr<Cell>> out; out.reserve(4);

    int CID = nx*y + x;
    auto cell = topCells[CID];

    std::vector<int> idxs = {cell->CID + nx, cell->CID + 1, cell->CID - nx, cell->CID - 1};

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
    // assign nodes to new cells and label cells with CID
    int currNode = 0;
    int CID = 0;
    int numElementNodes = (deg+1)*(deg+1);

    for (auto leaf : leaves) {
        leaf->setNodes(std::array<int,2>{currNode, currNode + numElementNodes - 1});
        leaf->CID = CID;
        CID++;
        currNode += numElementNodes;
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

std::vector<int> QuadTreeMesh::GetLocalBoundaryNodes(Direction direction) {
    std::vector<int> out; out.reserve(deg+1);
    int start = 0;

    switch (direction) {
        case Direction::N : {
            start += (deg+1)*deg;
            break;
        }

        case Direction::E : {
            start += deg;
        }

        default : {
            break;
        }
    }

    int increment = (direction == Direction::N || direction == Direction::S) ? 1 : deg+1;

    for (int i=0; i<deg+1; i++) {
        out.push_back(start+increment*i);
    }

    return out;
}

std::vector<int> QuadTreeMesh::GetGlobalBoundaryNodes(Direction direction, int CID) {
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

        default : {
            break;
        }
    }

    int increment = (direction == Direction::N || direction == Direction::S) ? 1 : deg+1;

    for (int i=0; i<deg+1; i++) {
        out.push_back(start+increment*i);
    }

    return out;
}

std::vector<int> QuadTreeMesh::GetGlobalElemNodes(int CID) {
    std::vector<int> out; out.reserve(numElemNodes);
    int start = CID*numElemNodes;
    for (int i=start; i<start+numElemNodes; i++) {
        // if (nodeClass[i]) {
        //     out.push_back(i);
        // }
        out.push_back(i);
    }
    return out;
}

std::vector<int> QuadTreeMesh::GetTrimmedLocalNodes(int CID, std::vector<int>& globalNodes) {
    std::vector<int> out; out.reserve(globalNodes.size());
    int offset = CID*numElemNodes;
    for (int i : globalNodes) {
        out.push_back(i - offset);
    }
    return out;
}

std::vector<std::array<double,2>> QuadTreeMesh::AllNodePos() {
    std::vector<std::array<double,2>> out; out.reserve(numLeaves*numElemNodes);
    double x; double y;
    std::array<double,2> center;
    double ax; double bx;
    double ay; double by;
    double width;
    for (auto leaf : leaves) {
        width = leaf->width;
        center = leaf->center;
        ax = center[0] - width; bx = center[0] + width;
        ay = center[1] - width; by = center[1] + width;
        for (auto yy : gaussPoints) {
            for (auto xx : gaussPoints) {
                x = ax + ((bx - ax) / 2.0) * (xx + 1.0);
                y = ay + ((by - ay) / 2.0) * (yy + 1.0);
                out.emplace_back(std::array<double,2>{x,y});
            }
        }
    }

    return out;
}

std::vector<std::array<double,2>> QuadTreeMesh::GetNodePos(std::vector<int> nodes) {
    std::vector<std::array<double,2>> out; out.reserve(nodes.size());
    for (int node : nodes) {
        out.push_back(nodePositions[node]);
    }
    return out;
}

void QuadTreeMesh::Refine(std::vector<std::shared_ptr<Cell>> cells) {
    std::array<Direction,4> dirs = {Direction::N, Direction::E, Direction::S, Direction::W};

    std::vector<std::shared_ptr<Cell>> neighbors;
    std::vector<std::shared_ptr<Cell>> initNeighbors; initNeighbors.reserve(cells.size() * 4);

    // loop over cells to subdivide
    for (auto leaf : cells) {
        for (auto dir : dirs) {
            neighbors = GetCellNeighbors(dir, leaf->CID);
            for (auto neighbor : neighbors) {
                if (neighbor && neighbor->isLeaf() && leaf->level - neighbor->level > 0) { // add neighbor if subdividing unbalances
                    initNeighbors.push_back(neighbor);
                }
            }
        }
        leaf->subdivide();
    }

    // balancing
    std::stack<std::shared_ptr<Cell>> toRefine;

    std::cout<<"pushing to stack"<<std::endl;
    // fill stack with cells that need balancing
    for (auto cell : initNeighbors) {
        toRefine.push(cell);
    }

    // iterate over cells to refine
    std::cout<<"recursively subdividing neighbors"<<std::endl;
    std::shared_ptr<Cell> currCell;
    while (toRefine.size() > 0) {
        currCell = toRefine.top();
        toRefine.pop();
        if (currCell->isLeaf()) {
            for (auto dir : dirs) {
                neighbors = GetCellNeighbors(dir, currCell->CID);
                for (auto neighbor : neighbors) {
                    if (neighbor && currCell->level - neighbor->level > 0)  {
                        toRefine.push(neighbor);
                    }
                }
            }
            currCell->subdivide();
        }
    }

    // Find all new boundary cells
    std::vector<std::shared_ptr<Cell>> newNB;
    std::vector<std::shared_ptr<Cell>> newEB;
    std::vector<std::shared_ptr<Cell>> newSB;
    std::vector<std::shared_ptr<Cell>> newWB;

    for (const auto& cell : boundaryCells[0]) {
        if (!cell->isLeaf()) {
            newNB.push_back(cell->children[3]);
            newNB.push_back(cell->children[0]);
        } else {
            newNB.push_back(cell);
        }
    }

    for (const auto& cell : boundaryCells[1]) {
        if (!cell->isLeaf()) {
            newEB.push_back(cell->children[1]);
            newEB.push_back(cell->children[0]);
        } else {
            newEB.push_back(cell);
        }
    }

    for (const auto& cell : boundaryCells[2]) {
        if (!cell->isLeaf()) {
            newSB.push_back(cell->children[2]);
            newSB.push_back(cell->children[1]);
        } else {
            newSB.push_back(cell);
        }
    }

    for (const auto& cell : boundaryCells[3]) {
        if (!cell->isLeaf()) {
            newWB.push_back(cell->children[2]);
            newWB.push_back(cell->children[3]); 
        } else {
            newWB.push_back(cell);
        }
    }

    boundaryCells = {newNB, newEB, newSB, newWB};

    // cleanup + bookkeeping
    leaves = GetAllCells();
    assignNodes();
    ClassifyNodes();
    nodePositions = AllNodePos();

    numLeaves = leaves.size();
}

std::vector<std::shared_ptr<Cell>> QuadTreeMesh::GetCellNeighbors(Direction direction, int CID) {
    std::vector<std::shared_ptr<Cell>> out;
    auto cell = leaves[CID];
    auto neighbor = geqNeighbor(direction, cell);
    if (neighbor != nullptr) {
        out = cell->subneighbors(neighbor, direction);
        return out;
    } else {
        out = {nullptr};
        return out;
    }
}

std::shared_ptr<Cell> QuadTreeMesh::geqNeighbor(Direction direction, std::shared_ptr<Cell> cell) {
    Child c1; Child c2; Child c3; Child c4;

    switch (direction) {
        case Direction::N : {
            c1 = Child::SW; c2 = Child::SE; c3 = Child::NW; c4 = Child::NE;
            break;
        }

        case Direction::E : {
            c1 = Child::NW; c2 = Child::SW; c3 = Child::NE; c4 = Child::SE;
            break;
        }

        case Direction::S : {
            c1 = Child::NW; c2 = Child::NE; c3 = Child::SW; c4 = Child::SE;
            break;
        }

        case Direction::W : {
            c1 = Child::NE; c2 = Child::SE; c3 = Child::NW; c4 = Child::SW;
            break;
        }
    }

    if (cell->level == 0) {
        return topNeighbors[cell->topIdx][direction];
    } else if (cell->parent == nullptr) {
        return nullptr;
    } else if (cell->parent->children[c1] == cell) {
        return cell->parent->children[c3];
    } else if (cell->parent->children[c2] == cell) {
        return cell->parent->children[c4];
    }

    auto node = geqNeighbor(direction, cell->parent);
    if (node == nullptr || node->isLeaf()) {
        return node;
    } else if (cell->parent->children[c3] == cell) {
        return node->children[c1];
    } else {
        return node->children[c2];
    }
}

void QuadTreeMesh::ClassifyNodes() {
    nodeClass.clear(); nodeClass.resize(numElemNodes * leaves.size());
    freeNodes.clear();
    boundaryNodes.clear(); boundaryNodes.reserve(4); // std::vector<std::vector<int>>

    std::vector<int> nBound;
    std::vector<int> eBound;
    std::vector<int> sBound;
    std::vector<int> wBound;

    std::array<Direction,4> dirs = {Direction::N, Direction::E, Direction::S, Direction::W};
    for (auto leaf : leaves) {
        int start = leaf->CID*numElemNodes;
        bool isBound = false || (geqNeighbor(Direction::N, leaf) == nullptr && geqNeighbor(Direction::W, leaf) == nullptr);
        int offsets[4] = {0,0,0,0};

        // for (auto dir : dirs) {
        //     auto neighbor = geqNeighbor(dir, leaf);
        //     if (neighbor == nullptr) {
        //         std::vector<int> boundaryN = GetGlobalBoundaryNodes(dir, leaf->CID);
        //         if (isBound) {
        //             boundPtrs[dir]->insert(boundPtrs[dir]->end(), boundaryN.begin(), boundaryN.end()-1);
        //         } else {
        //             boundPtrs[dir]->insert(boundPtrs[dir]->end(), boundaryN.begin(), boundaryN.end());
        //         }
        //         isBound = true;
        //         offsets[dir] = 1;
        //     }
        // }

        auto neighborN = geqNeighbor(Direction::N, leaf);
        if (neighborN == nullptr) {
            std::vector<int> boundaryN = GetGlobalBoundaryNodes(Direction::N, leaf->CID);
            if (isBound) {
                nBound.insert(nBound.end(), boundaryN.begin()+1, boundaryN.end());
            } else {
                nBound.insert(nBound.end(), boundaryN.begin(), boundaryN.end());
            }
            isBound = true;
            offsets[Direction::N] = 1;
        }

        auto neighborE = geqNeighbor(Direction::E, leaf);
        if (neighborE == nullptr) {
            std::vector<int> boundaryE = GetGlobalBoundaryNodes(Direction::E, leaf->CID);
            if (isBound) {
                eBound.insert(eBound.end(), boundaryE.begin(), boundaryE.end()-1);
            } else {
                eBound.insert(eBound.end(), boundaryE.begin(), boundaryE.end());
            }
            isBound = true;
            offsets[Direction::E] = 1;
        }

        auto neighborS = geqNeighbor(Direction::S, leaf);
        if (neighborS == nullptr) {
            std::vector<int> boundaryS = GetGlobalBoundaryNodes(Direction::S, leaf->CID);
            if (isBound) {
                sBound.insert(sBound.end(), boundaryS.begin(), boundaryS.end()-1);
            } else {
                sBound.insert(sBound.end(), boundaryS.begin(), boundaryS.end());
            }
            isBound = true;
            offsets[Direction::S] = 1;
        }

        auto neighborW = geqNeighbor(Direction::W, leaf);
        if (neighborW == nullptr) {
            std::vector<int> boundaryW = GetGlobalBoundaryNodes(Direction::W, leaf->CID);
            if (isBound && geqNeighbor(Direction::S, leaf) == nullptr) {
                wBound.insert(wBound.end(), boundaryW.begin()+1, boundaryW.end());
            } else {
                wBound.insert(wBound.end(), boundaryW.begin(), boundaryW.end());
            }
            isBound = true;
            offsets[Direction::W] = 1;
        }

        for (int j=offsets[2]; j<deg+1-offsets[0]; j++) {
            for (int i=offsets[3]; i<deg+1-offsets[1]; i++) {
                int nodeNum = start+j*(deg+1)+i;
                freeNodes.push_back(nodeNum);
                nodeClass[nodeNum] = true;
            } 
        }
    }

    boundaryNodes.push_back(nBound);
    boundaryNodes.push_back(eBound);
    boundaryNodes.push_back(sBound);
    boundaryNodes.push_back(wBound);
    for (auto nodes : boundaryNodes) {
        for (auto node : nodes) {
            nodeClass[node] = false;
        }
    }
}