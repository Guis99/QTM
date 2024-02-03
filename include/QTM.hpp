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

    class Cell : public std::enable_shared_from_this<Cell> {
        public:
            std::shared_ptr<Cell> parent = nullptr;
            std::array<std::shared_ptr<Cell>, 4> children = {nullptr, nullptr, nullptr, nullptr};
            // std::array<std::vector<std::shared_ptr<Cell>>, 4> neighbors; // each vector holds neighbors to North, East, South, West in that order

            int CID; int topIdx;
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
            std::vector<std::vector<std::shared_ptr<Cell>>> topNeighbors;
            std::vector<std::shared_ptr<Cell>> leaves;
            std::vector<std::shared_ptr<Cell>> boundaryCells;

            int nx; int ny;
            int deg;
            int numElemNodes;
            int numLeaves;
            std::vector<double> gaussPoints;
            std::vector<double> halfGaussPoints;
            std::vector<std::vector<int>> boundaryNodes;
            std::vector<int> freeNodes;
            std::vector<std::array<double,2>> nodePositions;

            QuadTreeMesh(int deg, int nx, int ny, double Lx, double Ly);

            std::vector<std::shared_ptr<Cell>> GetTopNeighborCells(int x, int y);
            std::vector<std::shared_ptr<Cell>> GetCellNeighbors(Direction direction, int CID);
            std::shared_ptr<Cell> geqNeighbor(Direction direction, std::shared_ptr<Cell> cell);
            std::vector<std::shared_ptr<Cell>> GetAllCells();
            std::vector<int> GetLocalBoundaryNodes(Direction direction); // edge node indices within cell
            std::vector<int> GetGlobalBoundaryNodes(Direction direction, int CID); // global indices of edge nodes
            std::vector<std::array<double,2>> GetNodePos(std::vector<int> nodes);
            std::vector<std::array<double,2>> AllNodePos();
            void ClassifyNodes(); // determines nodes on domain boundary and interior
            void Refine(std::vector<std::shared_ptr<Cell>> cells);
            void assignNodes();
            int nNodes() {return numElemNodes*leaves.size();};
    };
}

#endif 