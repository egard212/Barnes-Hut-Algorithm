#include "QuadTreeNode.h"
#include <cstddef>
#include <cstdio>
#include <memory>


// Constructor
QuadTreeNode::QuadTreeNode(const Square &new_square, int new_level ) {
    // Set the members that were passed in
    square = new_square;
    level = new_level;
    // Default the rest
    total_mass = 0.0;
    com_x = 0.0;
    com_y = 0.0;
    leaf = true;
    body_count = 0;
    body = nullptr;
    children.resize(0);
}

// Destructor
QuadTreeNode::~QuadTreeNode() {
    // Free The Body Pointer 
    body = nullptr;
    free(body);
}

// Locate the Quadrant the body is located in
QUADRANT QuadTreeNode::findQuad(double body_x, double body_y) {
    double updatedLength = square.length/2;
    // Left Side
    if (body_x <= (square.x + updatedLength)) {
        return body_y <= (square.y + updatedLength) ? BOT_LEFT : TOP_LEFT; 
    }
    // Right Side
    return body_y <= (square.y + updatedLength) ? BOT_RIGHT : TOP_RIGHT;
}

// Split the Tree Node into 4 Quadrants
void QuadTreeNode::quadDivide() {
    double updatedWidth = square.length/2;
    children.resize(NUM_CHILDREN);
    children[TOP_LEFT] = std::make_shared<QuadTreeNode>(QuadTreeNode(Square(square.x, square.y + updatedWidth, updatedWidth), level ++)); 
    children[TOP_RIGHT] = std::make_shared<QuadTreeNode>(QuadTreeNode(Square(square.x + updatedWidth, square.y + updatedWidth, updatedWidth), level ++)); 
    children[BOT_LEFT] = std::make_shared<QuadTreeNode>(QuadTreeNode(Square(square.x, square.y, updatedWidth), level ++)); 
    children[BOT_RIGHT] = std::make_shared<QuadTreeNode>(QuadTreeNode(Square(square.x + updatedWidth, square.y, updatedWidth), level ++)); 
}

void QuadTreeNode::childInsert(Body *new_body) {
    // Find Quadrant body is located in 
    QUADRANT body_quad = findQuad(new_body->x_pos, new_body->y_pos);
    children[body_quad]->insert(new_body);

}

void QuadTreeNode::insert(Body *new_body) {
    // Check if body is valid/in-range
    if (new_body->mass != -1) {
        // Node is a leaf
        if (leaf) {
            // Node is a leaf with no body in it yet
            if (body == nullptr) {
                body = new_body;
            } else {
                leaf = false;
                this->quadDivide();
            }
        }

        if (!leaf) {
            if (body != nullptr) {
                com_x += body->x_pos * body->mass;
                com_y += body->y_pos *body->mass;
                total_mass += body->mass;
                com_x /= total_mass;
                com_y /= total_mass;
                childInsert(body);
                body = nullptr;
            }
            
            com_x += new_body->x_pos * new_body->mass;
            com_y += new_body->y_pos * new_body->mass;
            total_mass += new_body->mass;
            com_x /= total_mass;
            com_y /= total_mass;
            childInsert(new_body);

        }
    }
}


void QuadTreeNode::updateForce(Body *new_body, double theta) {
    // Create Temporary Body for Force Calculation
    Body temp_body;
    temp_body.x_pos = new_body->x_pos;
    temp_body.y_pos = new_body->y_pos;
    temp_body.mass = new_body->mass;

    // Creat Temporary Node Coordinates for Force Calculation
    double temp_node_x = 0.0;
    double temp_node_y = 0.0;
    double temp_node_mass = 0.0;
    double ratio = 0.0;

    // Flag to check if force needs to be updated
    bool needs_update = false;

    // Check if the node is a leaf
    if (!leaf) {
        temp_node_x = com_x;
        temp_node_y = com_y;
        temp_node_mass = total_mass;
        needs_update = true;
    } else {
        // Check if node has a body
        if (leaf && body != nullptr) {
            // Check for 
            if (body->index != new_body->index) {
                if (body->mass != -1) {
                    temp_node_x = body->x_pos;
                    temp_node_y = body->y_pos;
                    temp_node_mass = body->mass;
                    needs_update = true;
                }
            }
        }
    }

    // Check if Node needs and update
    if (needs_update) {
        // Calculate dist between body and node
        double dist_to_body = new_body->calcDistFromNode(temp_node_x, temp_node_y);
        if (!leaf) {
            ratio = square.length / dist_to_body;
            if (ratio < theta) {

                // Calculate the force on the X Axis
                double Force_x = ( temp_node_x - temp_body.x_pos );
                Force_x = Force_x * temp_body.mass * temp_node_mass * GRAVITY;
                Force_x /= (dist_to_body * dist_to_body * dist_to_body);
                new_body->x_force = Force_x;

                // Calculate the force on the Y Axis
                double Force_y = (temp_body.y_pos - temp_node_y);
                Force_y = Force_y * temp_body.mass * temp_node_mass * GRAVITY;
                Force_y /= (dist_to_body * dist_to_body * dist_to_body);
                new_body->y_force = Force_y;
            } else {
                // Update Force on Child Nodes
                for (std::size_t i = 0; i < children.size(); i ++) {
                    children[i]->updateForce(new_body, theta);
                }
            }
        } else {
            if (body != nullptr) {
                if (new_body->index != body->index) {

                    // Calculate the force on the X Axis
                    double Force_x = (temp_body.x_pos - temp_node_x);
                    Force_x = Force_x * temp_body.mass * temp_node_mass * GRAVITY;
                    Force_x /= (dist_to_body * dist_to_body * dist_to_body);
                    new_body->x_force = Force_x;

                    // Calculate the force on the Y Axis
                    double Force_y = (temp_body.y_pos - temp_node_y);
                    Force_y = Force_y * temp_body.mass * temp_node_mass * GRAVITY;
                    Force_y /= (dist_to_body * dist_to_body * dist_to_body);
                    new_body->y_force = Force_y;
                }
            }
        }

    }
    
}

// Getters of private data members
Square QuadTreeNode::getSquare() {
  return square;
}

std::vector<std::shared_ptr<QuadTreeNode>>* QuadTreeNode::getChildren() {
  return &children;
}

bool QuadTreeNode::getLeaf() {
  return leaf;
}

