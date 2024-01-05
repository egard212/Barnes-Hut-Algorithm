#include "Body.h"
#include <cmath>

Body::Body(int idx, double x, double y, double m, double x_v, double y_v) {
    index = idx;
    x_pos = x;
    y_pos = y;
    mass = m;
    x_vel = x_v;
    y_vel = y_v;
    x_force = 0.0;
    y_force = 0.0;
}

Body::Body() {
    index = 0;
    x_pos = 0.0;
    y_pos = 0.0;
    mass = 0.0;
    x_vel = 0.0;
    y_vel = 0.0;
    x_force = 0.0;
    y_force = 0.0;
}

// Calculate distance from this body to a passes in Node's coordinates
double Body::calcDistFromNode(double node_x, double node_y) {
    double x_diff = x_pos - node_x;
    double y_diff = y_pos - node_y;

    double dist = std::sqrt((x_diff * x_diff) + (y_diff * y_diff));

    return dist < RLIMIT ? RLIMIT : dist;
}

// Print Out members of Body
void Body::printBody() {
    std::cout << "Index = " << index << std::endl;
    std::cout << "X_Pos = " << x_pos << std::endl;
    std::cout << "Y_Pos = " << y_pos << std::endl;
    std::cout << "Mass  = " << mass  << std::endl;
    std::cout << "X_Vel = " << x_vel << std::endl;
    std::cout << "Y_Vel = " << y_vel << std::endl;
}

void Body::clearForce() {
  x_force = 0.0;
  y_force = 0.0;
}

void Body::updatePositionVelocity(double dt) {
  // Check For invalid Bodies
  if (mass == -1) {
    return;
  }

  // Calc Acceleration
  double ax = x_force/mass;
  double ay = y_force/mass;

  // Update Position
  x_pos += (x_vel * dt) + (0.5 * ax * (dt * dt));
  y_pos += (y_vel * dt) + (0.5 * ay * (dt * dt));

  // Update Velocity
  x_vel += (ax * dt);
  y_vel += (ay * dt);

  // Update Masses if the Body has gone out of range
  if (x_pos < 0 || x_pos > 4) {
    mass = -1;
  }
  if (y_pos < 0 || y_pos > 4) {
    mass = -1;
  }

}
