#include "common.hpp"

#ifndef QTMesh
#define QTMesh

namespace QTM {

    enum Child {
        NE = 0,
        SE = 1,
        SW = 2,
        NW = 3
    };

    enum Direction {
        N = 0,
        E = 1,
        S = 2,
        W = 3
    };

    class Node {
        public:
            int NID;
            std::array<double, 2> Position;
            int nClass;

            Node();
            Node(int NID, std::array<double, 2> Position, int nClass);
    };

    class Cell {
        public:
            std::shared_ptr<Cell> parent = nullptr;
            std::array<std::shared_ptr<Cell>, 4> children = {nullptr, nullptr, nullptr, nullptr};
            std::array<std::vector<std::shared_ptr<Cell>>, 4> neighbors; // each vector holds neighbors to East, South, West, North in that order

            int deg;
            double width;
            std::array<double, 2> center;

            int level;

            Cell(std::shared_ptr<Cell> parent, int level, double width, double xPos, double yPos, int deg);
            void subdivide();
            void undivide();
            std::shared_ptr<Cell> geqNeighbor(Direction direction);
            std::vector<std::shared_ptr<Cell>> subneighbors(std::shared_ptr<Cell> neighbor, Direction direction);
            std::vector<std::shared_ptr<Cell>> traverse();
            bool isLeaf() { return isaLeaf; }
        private:
            bool isaLeaf = true;
    };

    class QuadTreeMesh {
        public:
            std::vector<Cell> topCells;
            std::vector<std::shared_ptr<Node>> allNodes;

            QuadTreeMesh(int deg, int nx, int ny, double Lx, double Ly);
    };
}

#endif 