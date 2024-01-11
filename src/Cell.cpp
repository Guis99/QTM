#include "..\include\QTM.hpp"

using namespace QTM;

Cell::Cell(std::shared_ptr<Cell> parent, int level, double width, double xPos, double yPos, int deg) {
    this->parent = parent;
    this->level = level;
    this->center = { xPos, yPos };
    this->width = width;
    this->deg = deg;
}

std::shared_ptr<Cell> Cell::geqNeighbor(Direction direction) {
    switch (direction) {
        case Direction::N : {
            // base case
            if (this->parent == nullptr) {
                return nullptr;
            } else if (this->parent->children[Child::SW].get() == this) {
                return this->parent->children[Child::NW];
            } else if (this->parent->children[Child::SE].get() == this) {
                return this->parent->children[Child::NE];
            }

            auto node = this->parent->geqNeighbor(direction);
            if (node == nullptr || node->isLeaf()) {
                return node;
            }

            if (this->parent->children[Child::NW].get() == this) {
                return node->children[Child::SW];
            } else {
                return node->children[Child::SE];
            }

        }

        case Direction::E : {
            // base case
            if (this->parent == nullptr) {
                return nullptr;
            } else if (this->parent->children[Child::NW].get() == this) {
                return this->parent->children[Child::NE];
            } else if (this->parent->children[Child::SW].get() == this) {
                return this->parent->children[Child::SE];
            }

            auto node = this->parent->geqNeighbor(direction);
            if (node == nullptr || node->isLeaf()) {
                return node;
            }

            if (this->parent->children[Child::NE].get() == this) {
                return node->children[Child::NW];
            } else {
                return node->children[Child::SW];
            }

        }

        case Direction::S : {
            // base case
            if (this->parent == nullptr) {
                return nullptr;
            } else if (this->parent->children[Child::NW].get() == this) {
                return this->parent->children[Child::SW];
            } else if (this->parent->children[Child::NE].get() == this) {
                return this->parent->children[Child::SE];
            }

            auto node = this->parent->geqNeighbor(direction);
            if (node == nullptr || node->isLeaf()) {
                return node;
            }

            if (this->parent->children[Child::SW].get() == this) {
                return node->children[Child::NW];
            } else {
                return node->children[Child::NE];
            }

        }

        case Direction::W : {
            // base case
            if (this->parent == nullptr) {
                return nullptr;
            } else if (this->parent->children[Child::NE].get() == this) {
                return this->parent->children[Child::NW];
            } else if (this->parent->children[Child::SE].get() == this) {
                return this->parent->children[Child::SW];
            }

            auto node = this->parent->geqNeighbor(direction);
            if (node == nullptr || node->isLeaf()) {
                return node;
            }

            if (this->parent->children[Child::NW].get() == this) {
                return node->children[Child::NE];
            } else {
                return node->children[Child::SE];
            }

        }
    }
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

    // Adding child cells
    double subWidth = this->width/2;
    double x = this->center[0];
    double y = this->center[1];
    auto parentPtr = shared_from_this();
    // Starting from upper-right quadrant going CW
    this->children[0] = std::make_shared<Cell>(parentPtr, level+1, subWidth, x+subWidth, y+subWidth, this->deg);
    this->children[1] = std::make_shared<Cell>(parentPtr, level+1, subWidth, x+subWidth, y-subWidth, this->deg);
    this->children[2] = std::make_shared<Cell>(parentPtr, level+1, subWidth, x-subWidth, y-subWidth, this->deg);
    this->children[3] = std::make_shared<Cell>(parentPtr, level+1, subWidth, x-subWidth, y+subWidth, this->deg);

    // Updating nodes

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
    