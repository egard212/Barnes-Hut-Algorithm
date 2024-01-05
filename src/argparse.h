#ifndef _ARGPARSE_H
#define _ARGPARSE_H

#include <getopt.h>
#include <stdlib.h>
#include <iostream>

struct options_t {
    int n_bodies;
    char *in_file;
    char *out_file;
    int steps;
    double theta;
    double dt;
    bool visualize;
};

void get_opts(int argc, char **argv, struct options_t *opts);
#endif
