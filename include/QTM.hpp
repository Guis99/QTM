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

    class Cell : public std::enable_shared_from_this<Cell> {
        public:
            std::shared_ptr<Cell> parent = nullptr;
            std::array<std::shared_ptr<Cell>, 4> children = {nullptr, nullptr, nullptr, nullptr};
            std::array<std::vector<std::shared_ptr<Cell>>, 4> neighbors; // each vector holds neighbors to North, East, South, West in that order

            int CID;
            int deg;
            double width;
            std::array<double, 2> center;

            int level;

            std::array<int,2> nodes;

            Cell(std::shared_ptr<Cell> parent, int level, double width, double xPos, double yPos);
            Cell(int CID, std::shared_ptr<Cell> parent, int level, double width, double xPos, double yPos);

            std::shared_ptr<Cell> getptr () { return shared_from_this(); }

            void subdivide();
            void undivide();
            std::shared_ptr<Cell> geqNeighbor(Direction direction);
            std::vector<std::shared_ptr<Cell>> subneighbors(std::shared_ptr<Cell> neighbor, Direction direction);
            std::vector<std::shared_ptr<Cell>> getNeighbors();
            std::vector<std::shared_ptr<Cell>> traverse();
            void setNodes(std::array<int,2> &&nodes) { this->nodes = nodes; }
            int selfdelete();
            bool isLeaf() { return isaLeaf; }
        private:
            bool isaLeaf = true;
    };

    class QuadTreeMesh {
        public:
            std::vector<std::shared_ptr<Cell>> topCells;
            std::vector<std::shared_ptr<Node>> allNodes;

            int nx; int ny;
            int deg;
            int numElemNodes;
            int numLeaves;

            QuadTreeMesh(int deg, int nx, int ny, double Lx, double Ly);

            std::vector<std::shared_ptr<Cell>> GetNeighborCells(int x, int y);
            std::vector<std::shared_ptr<Cell>> GetAllCells();
            std::vector<int> GetBoundaryNodes(Direction direction);
            void assignNodes();
    };
}

#endif 