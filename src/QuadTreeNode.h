#ifndef QUADTREENODE_H
#define QUADTREENODE_H
#include "Body.h"
#include <memory>
#include <vector>
#include <string>

#define NUM_CHILDREN 4
#define GRAVITY 0.0001

enum QUADRANT {TOP_LEFT, TOP_RIGHT, BOT_LEFT, BOT_RIGHT};

// Square to define the area the node inhabits
struct Square {

    double x,y,length;

    Square() : x(0.0), y(0.0), length(0.0) {}

    Square(double x, double y, double length) : x(x), y(y), length(length) {}

    bool contains(double new_x, double new_y) {
        return (new_x >= x) && (new_x <= (x + length)) && 
                (new_y >= y) && (new_y <= (y + length));
    }
};

class QuadTreeNode {

    private:
        // Center of Mass Coordinates and Total Mass
        double com_x, com_y, total_mass;
        // Area Node Inhabits
        Square square;
        // Pointer to body
        Body *body;
        // Vector containing the 4 child nodes
        std::vector<std::shared_ptr<QuadTreeNode>> children;
        // Which level of the tree the Node is located in
        int level;
        // Number of Bodies located below the node
        int body_count;
        // Is the Node a leaf or not?
        bool leaf;

    public:
        QuadTreeNode(const Square &new_square, int new_level);
        ~QuadTreeNode();
        QUADRANT findQuad(double body_x, double body_y);
        void quadDivide();
        void insert(Body *new_body);
        void childInsert(Body *new_body);
        void updateForce(Body *new_body, double theta);
        // Getters to retrieve private data
        Square getSquare();
        std::vector<std::shared_ptr<QuadTreeNode>>* getChildren();
        bool getLeaf();

};

#endif 
