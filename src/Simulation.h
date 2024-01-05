#ifndef SIMULATION_H
#define SIMULATION_H

#include "argparse.h"
#include "io.h"
#include "Body.h"
#include "QuadTreeNode.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <mpi.h>

double coordTransform(double coord);
void drawParticle2D(double x_window, double y_window, double radius, float *colors);
void drawOctreeBounds2D(QuadTreeNode &node);
void createMPIData(MPI_Datatype &mpi_body);
void broadcastData(struct options_t &opts, std::vector<Body> &bodies, MPI_Datatype &mpi_body, int rank);
int startVisuals(GLFWwindow *window);
void output_and_finalize(struct options_t &opts, std::vector<Body> &bodies,MPI_Datatype &mpi_body, double &start_sim_time,
                          double &end_sim_time, GLFWwindow *window);
void simulate(struct options_t &opts, std::vector<Body> &bodies, 
				int &size, int &rank, QuadTreeNode &root, MPI_Datatype &mpi_body, GLFWwindow *window);

#endif