#include "io.h"
#include <fstream>
 
void read_file(struct options_t *opts, std::vector<Body> &bodies) {
    // Open File
    std::ifstream in;
    in.open(opts->in_file);

    in >> opts->n_bodies;

    // Read in each body line by line
    for (int i = 0; i < opts->n_bodies; i++) {
        // temp variables
        int idx;
        double x,y,m,x_v,y_v;
        for (int j = 0; j < NUM_INPUTS; j++) {
            switch (j) {
                case 0:
                    in >> idx;
                    break;
                case 1:
                    in >> x;
                    break;
                case 2:
                    in >> y;
                    break;
                case 3: 
                    in >> m;
                    break;
                case 4:
                    in >> x_v;
                    break;
                case 5:
                    in >> y_v;
                    break;
            }
        }
        bodies.push_back(Body(idx,x,y,m,x_v,y_v));
    }
    
}

void output_file(struct options_t *opts, std::vector<Body> &bodies) {
    // Open File
    std::ofstream out;
    out.open(opts->out_file, std::ofstream::trunc);

    // Write the bodies to a file
    out.precision(6);
    out << std::setprecision(6) << std::fixed << std::scientific;
    out << (int)bodies.size() << std::endl;
    for (std::size_t i = 0; i < bodies.size(); i++) {
        out << bodies[i].index << " " << bodies[i].x_pos << " " << bodies[i].y_pos << " " <<
                bodies[i].mass << " " << bodies[i].x_vel << " " << bodies[i].y_vel << std::endl;
    }
    out.flush();
    out.close();
}
