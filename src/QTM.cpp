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
    topNeighbors.reserve(nx*ny);
    for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
            auto nextCell = std::make_shared<Cell>(nullptr, 0, dx/2, dx/2 + x*dx, dx/2 + y*dx);
            nextCell->CID = CID; nextCell->topIdx = CID; CID++;

            topCells.push_back(nextCell);
        }
    }

    for (int y=0; y<ny; y++) {
        for (int x=0; x<nx; x++) {
            topNeighbors.push_back(GetTopNeighborCells(x, y));
        }
    }

    leaves = topCells;
    numLeaves = nx*ny;
    assignNodes();
    gaussPoints.reserve(deg+1);

    for (int i=0; i < deg+1; i++) {
        gaussPoints.push_back(-cos(i*PI/deg));
    }
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

std::vector<std::array<double,2>> QuadTreeMesh::AllNodePos() {
    std::vector<std::array<double,2>> out; out.reserve(numLeaves*numElemNodes);
    auto leaves = GetAllCells();
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

void QuadTreeMesh::Refine(std::vector<std::shared_ptr<Cell>> cells) {
    std::array<Direction,4> dirs = {Direction::N, Direction::E, Direction::S, Direction::W};

    std::vector<std::shared_ptr<Cell>> neighbors;
    std::vector<std::shared_ptr<Cell>> initNeighbors; initNeighbors.reserve(cells.size() * 4);

    // loop over cells to subdivide
    for (auto leaf : cells) {
        std::cout<<leaf<<std::endl;
        for (auto dir : dirs) {
            neighbors = GetCellNeighbors(dir, leaf->CID);
            std::cout<<"nn: "<<neighbors.size()<<std::endl;
            for (auto neighbor : neighbors) {
                std::cout<<leaf->CID<<", "<<neighbor->CID<<std::endl;
                if (leaf->level - neighbor->level > 0) { // add neighbor if subdividing unbalances
                    std::cout<<"neighbor added with CID: "<<neighbor->CID<<std::endl;
                    initNeighbors.push_back(neighbor);
                }
            }
        }
        std::cout<<"---------"<<std::endl;
        // leaf->subdivide();
    }
    for (auto leaf : cells) {
        leaf->subdivide();
    }

    // balancing
    std::stack<std::shared_ptr<Cell>> toRefine;

    std::cout<<"pushing to stack"<<std::endl;
    // fill stack with cells that need balancing
    for (auto cell : initNeighbors) {
        if (cell->CID != -1) {
            std::cout<<cell<<", "<<cell->level<<", "<<cell->center[0]<<", "<<cell->center[1]<<std::endl;
            toRefine.push(cell);
        }
    }

    // iterate over cells to refine
    std::cout<<"recursively subdividing neighbors"<<std::endl;
    std::shared_ptr<Cell> currCell;
    while (toRefine.size() > 0) {
        currCell = toRefine.top();
        toRefine.pop();
        for (auto dir : dirs) {
            neighbors = GetCellNeighbors(dir, currCell->CID);
            for (auto neighbor : neighbors) {
                if (currCell->level - neighbor->level > 0)  {
                    std::cout<<"neighbor added with CID: "<<neighbor->CID<<std::endl;
                    toRefine.push(neighbor);
                }
            }
        }
        if (currCell->isLeaf()) {
            currCell->subdivide();
        }
    }

    leaves = GetAllCells();
    assignNodes();
}

std::vector<std::shared_ptr<Cell>> QuadTreeMesh::GetCellNeighbors(Direction direction, int CID) {
    std::vector<std::shared_ptr<Cell>> out;
    auto cell = leaves[CID];
    auto neighbor = geqNeighbor(direction, cell);
    std::cout<<cell->isLeaf()<<", geqn: "<<neighbor;
    if (neighbor != nullptr) {
        std::cout<<", "<<neighbor->CID;
        std::cout<<std::endl;
        out = cell->subneighbors(neighbor, direction);
        return out;
    } else {
        std::cout<<std::endl;
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

// std::shared_ptr<Cell> QuadTreeMesh::geqNeighbor(Direction direction, std::shared_ptr<Cell> cell) {
//     if (cell->level == 0) {
//         return topNeighbors[cell->topIdx][direction];
//     }
//     switch (direction) {
//         case Direction::N : {
//             // base case
//             std::cout<<cell->level<<std::endl;
//             if (cell->parent == nullptr && cell->level > 0) {
//                 return nullptr;
//             } else if (cell->level > 0 && cell->parent->children[Child::SW] == cell) {
//                 return cell->parent->children[Child::NW];
//             } else if (cell->level > 0 && cell->parent->children[Child::SE] == cell) {
//                 return cell->parent->children[Child::NE];
//             }

//             auto node = geqNeighbor(direction, cell->parent);
//             if (node == nullptr || node->isLeaf()) {
//                 return node;
//             } else if (cell->parent->children[Child::NW] == cell) {
//                 return node->children[Child::SW];
//             } else {
//                 return node->children[Child::SE];
//             }
//             break;
//         }

//         case Direction::E : {
//             // base case
//             std::cout<<cell->level<<std::endl;
//             if (cell->level > 0 && cell->parent == nullptr) {
//                 return nullptr;
//             } else if (cell->level > 0 && cell->parent->children[Child::NW] == cell) {
//                 return cell->parent->children[Child::NE];
//             } else if (cell->level > 0 && cell->parent->children[Child::SW] == cell) {
//                 return cell->parent->children[Child::SE];
//             }
            
//             auto node = geqNeighbor(direction, cell->parent);
//             if (node == nullptr || node->isLeaf()) {
//                 return node;
//             } else if (cell->parent->children[Child::NE] == cell) {
//                 return node->children[Child::NW];
//             } else {
//                 return node->children[Child::SW];
//             }
//             break;
//         }

//         case Direction::S : {
//             // base case
//             if (cell->level > 0 && cell->parent == nullptr) {
//                 return nullptr;
//             } else if (cell->level > 0 && cell->parent->children[Child::NW] == cell) {
//                 return cell->parent->children[Child::SW];
//             } else if (cell->level > 0 && cell->parent->children[Child::NE] == cell) {
//                 return cell->parent->children[Child::SE];
//             }
            
//             auto node = geqNeighbor(direction, cell->parent);
//             if (node == nullptr || node->isLeaf()) {
//                 return node;
//             } else if (cell->parent->children[Child::SW] == cell) {
//                 return node->children[Child::NW];
//             } else {
//                 return node->children[Child::NE];
//             }

//             break;
//         }

//         case Direction::W : {
//             // base case
//             if (cell->level > 0 && cell->parent == nullptr) {
//                 return nullptr;
//             } else if (cell->level > 0 && cell->parent->children[Child::NE] == cell) {
//                 return cell->parent->children[Child::NW];
//             } else if (cell->level > 0 && cell->parent->children[Child::SE] == cell) {
//                 return cell->parent->children[Child::SW];
//             }

            
//             auto node = geqNeighbor(direction, cell->parent);
                
//             if (node == nullptr || node->isLeaf()) {
//                 return node;
//             } else if (cell->parent->children[Child::NW] == cell) {
//                 return node->children[Child::NE];
//             } else {
//                 return node->children[Child::SE];
//             }
//             break;
//         }
//     }
// }