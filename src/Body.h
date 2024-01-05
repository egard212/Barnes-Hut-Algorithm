#ifndef BODY_H
#define BODY_H
#include <iostream>

#define RLIMIT 0.03

struct Body {
    // members
    int index;
    double x_pos;
    double y_pos;
    double mass;
    double x_vel;
    double y_vel;
    double x_force;
    double y_force;

    // methods
    Body();
    Body(int idx, double x, double y, double m, double x_v, double y_v);
    void printBody();
    double calcDistFromNode(double node_x, double node_y);
    void clearForce();
    void updatePositionVelocity(double dt);
};

#endif 
