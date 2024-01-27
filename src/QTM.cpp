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
    std::cout<<x<<", "<<y<<std::endl;
    std::cout<<CID<<std::endl;
    auto cell = topCells[CID];

    std::vector<int> idxs = {cell->CID + nx, cell->CID + 1, cell->CID - nx, cell->CID - 1};
    for (auto idx : idxs) {
        std::cout<<idx<<std::endl;
    }
    std::vector<int> xs = {0, 1, 0, -1};
    std::vector<int> ys = {1, 0, -1, 0};

    int xx; int yy;
    for (int i=0; i<4; i++) {
        std::cout<<"indices: "<<x+xs[i] <<", "<< y+ys[i]<<std::endl;
        if (x+xs[i] < 0 || x+xs[i] >= nx || y+ys[i] < 0 || y+ys[i] >= ny) {
            std::cout<<"nullpush"<<std::endl;
            out.push_back(nullptr);
        } else {
            std::cout<<"cell: "<<topCells[idxs[i]]<<std::endl;
            out.push_back(topCells[idxs[i]]);
            

        }
    }
    if (x == 1 && y == 1) {
        std::cout<<out.size()<<std::endl;
        std::cout<<out[0]<<std::endl;
    }
    std::cout<<"---------"<<std::endl;
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
    for (auto leaf : cells) {
        leaf->subdivide();
    }
    leaves = GetAllCells();
    assignNodes();
}

std::vector<std::shared_ptr<Cell>> QuadTreeMesh::GetCellNeighbors(Direction direction, int CID) {
    std::vector<std::shared_ptr<Cell>> out;
    auto cell = leaves[CID];
    auto neighbor = geqNeighbor(direction, cell);
    if (neighbor != nullptr) {
        out = cell->subneighbors(neighbor, direction);
        return out;
    } else {
        return out;
    }
}

std::shared_ptr<Cell> QuadTreeMesh::geqNeighbor(Direction direction, std::shared_ptr<Cell> cell) {
    switch (direction) {
        case Direction::N : {
            // base case
            std::cout<<cell->level<<std::endl;
            if (cell->parent == nullptr && cell->level > 0) {
                return nullptr;
            } else if (cell->level > 0 && cell->parent->children[Child::SW] == cell) {
                return cell->parent->children[Child::NW];
            } else if (cell->level > 0 && cell->parent->children[Child::SE] == cell) {
                return cell->parent->children[Child::NE];
            }

            if (cell->level == 0) {
                return topNeighbors[cell->topIdx][Direction::N];
            } 

            auto node = geqNeighbor(direction, cell->parent);
            if (node == nullptr || node->isLeaf()) {
                return node;
            } else if (cell->parent->children[Child::NW] == cell) {
                return node->children[Child::SW];
            } else {
                return node->children[Child::SE];
            }
            break;
        }

        case Direction::E : {
            // base case
            std::cout<<cell->level<<std::endl;
            if (cell->level > 0 && cell->parent == nullptr) {
                return nullptr;
            } else if (cell->level > 0 && cell->parent->children[Child::NW] == cell) {
                return cell->parent->children[Child::NE];
            } else if (cell->level > 0 && cell->parent->children[Child::SW] == cell) {
                return cell->parent->children[Child::SE];
            }

            if (cell->level == 0) {
                return topNeighbors[cell->topIdx][Direction::E];
            } 
            
            auto node = geqNeighbor(direction, cell->parent);
            if (node == nullptr || node->isLeaf()) {
                return node;
            } else if (cell->parent->children[Child::NE] == cell) {
                return node->children[Child::NW];
            } else {
                return node->children[Child::SW];
            }
            break;
        }

        case Direction::S : {
            // base case
            if (cell->level > 0 && cell->parent == nullptr) {
                return nullptr;
            } else if (cell->level > 0 && cell->parent->children[Child::NW] == cell) {
                return cell->parent->children[Child::SW];
            } else if (cell->level > 0 && cell->parent->children[Child::NE] == cell) {
                return cell->parent->children[Child::SE];
            }

            if (cell->level == 0) {
                return topNeighbors[cell->topIdx][Direction::S];
            } 
            
            auto node = geqNeighbor(direction, cell->parent);
            if (node == nullptr || node->isLeaf()) {
                return node;
            } else if (cell->parent->children[Child::SW] == cell) {
                return node->children[Child::NW];
            } else {
                return node->children[Child::NE];
            }

            break;
        }

        case Direction::W : {
            // base case
            if (cell->level > 0 && cell->parent == nullptr) {
                return nullptr;
            } else if (cell->level > 0 && cell->parent->children[Child::NE] == cell) {
                return cell->parent->children[Child::NW];
            } else if (cell->level > 0 && cell->parent->children[Child::SE] == cell) {
                return cell->parent->children[Child::SW];
            }

            if (cell->level == 0) {
                return topNeighbors[cell->topIdx][Direction::W];
            } 
            
            auto node = geqNeighbor(direction, cell->parent);
                
            if (node == nullptr || node->isLeaf()) {
                return node;
            } else if (cell->parent->children[Child::NW] == cell) {
                return node->children[Child::NE];
            } else {
                return node->children[Child::SE];
            }
            break;
        }
    }
}
