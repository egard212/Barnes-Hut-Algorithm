#ifndef IO_H
#define IO_H
#include "Body.h"
#include "argparse.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>  

#define NUM_INPUTS 6 

void read_file(struct options_t *opts, std::vector<Body> &bodies);

void output_file(struct options_t *opts, std::vector<Body> &bodies);


#endif
