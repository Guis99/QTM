#include "..\include\QTM.hpp"

using namespace QTM;

Cell::Cell(std::shared_ptr<Cell> parent, int level, double width, double xPos, double yPos) {
    this->parent = parent;
    this->level = level;
    this->center = { xPos, yPos };
    this->width = width;
}

Cell::Cell(int CID, std::shared_ptr<Cell> parent, int level, double width, double xPos, double yPos) {
    this->CID = CID;
    this->parent = parent;
    this->level = level;
    this->center = { xPos, yPos };
    this->width = width;
}

std::vector<std::shared_ptr<Cell>> Cell::subneighbors(std::shared_ptr<Cell> neighbor, Direction direction) {
    std::vector<std::shared_ptr<Cell>> candidates = { neighbor };
    std::vector<std::shared_ptr<Cell>> neighbors;

    switch (direction) {
        case Direction::N : {
            while (candidates.size() > 0) {
                if (candidates[0]->isLeaf()) {
                    neighbors.push_back(candidates[0]);
                } else {
                    candidates.push_back(candidates[0]->children[Child::SW]);
                    candidates.push_back(candidates[0]->children[Child::SE]);
                }
                candidates.erase(candidates.begin());
            }

            return neighbors;
        }

        case Direction::E : {
            while (candidates.size() > 0) {
                if (candidates[0]->isLeaf()) {
                    neighbors.push_back(candidates[0]);
                } else {
                    candidates.push_back(candidates[0]->children[Child::NW]);
                    candidates.push_back(candidates[0]->children[Child::SW]);
                }
                candidates.erase(candidates.begin());
            }

            return neighbors;
        }

        case Direction::S : {
            while (candidates.size() > 0) {
                if (candidates[0]->isLeaf()) {
                    neighbors.push_back(candidates[0]);
                } else {
                    candidates.push_back(candidates[0]->children[Child::NW]);
                    candidates.push_back(candidates[0]->children[Child::NE]);
                }
                candidates.erase(candidates.begin());
            }

            return neighbors;
        }

        case Direction::W : {
            while (candidates.size() > 0) {
                if (candidates[0]->isLeaf()) {
                    neighbors.push_back(candidates[0]);
                } else {
                    candidates.push_back(candidates[0]->children[Child::NE]);
                    candidates.push_back(candidates[0]->children[Child::SE]);
                }
                candidates.erase(candidates.begin());
            }

            return neighbors;
        }

        return neighbors;
    }
}

void Cell::subdivide() {
    this->isaLeaf = false;
    CID = -1;

    // Adding child cells
    double subWidth = this->width/2;
    double x = this->center[0];
    double y = this->center[1];
    auto parentPtr = getptr();
    // Starting from upper-right quadrant going CW
    this->children[0] = std::make_shared<Cell>(parentPtr, level+1, subWidth, x+subWidth, y+subWidth);
    this->children[1] = std::make_shared<Cell>(parentPtr, level+1, subWidth, x+subWidth, y-subWidth);
    this->children[2] = std::make_shared<Cell>(parentPtr, level+1, subWidth, x-subWidth, y-subWidth);
    this->children[3] = std::make_shared<Cell>(parentPtr, level+1, subWidth, x-subWidth, y+subWidth);
};

void Cell::undivide() {
    // Call via consensus - if all four children are below target residual, undivide
    this->isaLeaf = true;
    this->children = { nullptr, nullptr, nullptr, nullptr };
}

std::vector<std::shared_ptr<Cell>> Cell::traverse() {
    std::vector<std::shared_ptr<Cell>> out;
    std::stack<std::shared_ptr<Cell>> toProcess;

    if (isLeaf()) {
        std::vector<std::shared_ptr<Cell>> out = { getptr() };
        return out;
    }
     
    for (int i=3; i>=0; i--) {
        toProcess.push(children[i]);
    }

    while (!toProcess.empty()) { // iterative
        auto CurrCell = toProcess.top();
        toProcess.pop();
        if (CurrCell->isLeaf()) {
            out.push_back(CurrCell);
        } else {
            for (int i=3; i>=0; i--) {
                toProcess.push(CurrCell->children[i]);
            }
        }
    }

    return out;
}
    