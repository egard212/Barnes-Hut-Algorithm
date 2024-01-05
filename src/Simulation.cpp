#include "Simulation.h"

double coordTransform(double coord) {
  return ((2 * coord / 4.0) - 1);
}

void drawParticle2D(double x_window, double y_window, double radius, float *colors) {
    int k = 0;
    float angle = 0.0f;
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(colors[0], colors[1], colors[2]);
    glVertex2f(x_window, y_window);
    for(k=0;k<20;k++){
        angle=(float)(k) /19*2*3.141592;
        glVertex2f(x_window+radius*cos(angle), y_window+radius*sin(angle));
    }
    glEnd();
}

void drawOctreeBounds2D(QuadTreeNode &node) {

    if(node.getLeaf())
        return;

    // Setup the points
    double full_width = node.getSquare().length / 2;
    double half_width = full_width/2;
    glBegin(GL_LINES);
    // set the color of lines to be white
    glColor3f(1.0f, 1.0f, 1.0f);
    // specify the start point's coordinates
    glVertex2f(coordTransform(node.getSquare().x),
                coordTransform(node.getSquare().y) + half_width);
    // specify the end point's coordinates
    glVertex2f(coordTransform(node.getSquare().x) + full_width, 
                coordTransform(node.getSquare().y) + half_width);
    // do the same for verticle line
    glVertex2f(coordTransform(node.getSquare().x) + half_width , 
                coordTransform(node.getSquare().y));
    glVertex2f(coordTransform(node.getSquare().x) + half_width, 
                coordTransform(node.getSquare().y) + full_width);
    glEnd();

    for (std::size_t i = 0; i < node.getChildren()->size(); i++ ) {
		drawOctreeBounds2D(*(*node.getChildren())[i]);
    }
        
}

// Function to create the custom MPI Body DataType
void createMPIData(MPI_Datatype &mpi_body) {
	MPI_Aint indices[8] = {offsetof(Body, index), offsetof(Body, x_pos), offsetof(Body, y_pos), offsetof(Body, mass), offsetof(Body, x_vel), offsetof(Body, y_vel),
            offsetof(Body, x_force), offsetof(Body, y_force)}; 
	// Create arrays to store blocklengths and types
	int blocks[8] = {1,1,1,1,1,1,1,1};
	MPI_Datatype type[8] = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
	MPI_Type_create_struct(8, blocks, indices, type, &mpi_body);
	MPI_Type_commit(&mpi_body);
}

// Broadcast Data to All Processes
void broadcastData(struct options_t &opts, std::vector<Body> &bodies, MPI_Datatype &mpi_body, int rank) {
	// Broadcast Number of Bodies to be evaluated
    MPI_Bcast(&opts.n_bodies, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (rank != 0) {
		bodies.resize(opts.n_bodies);
	}
	  
	// Broadcast the Vector Containg the Bodies
	MPI_Bcast(bodies.data(), opts.n_bodies, mpi_body, 0, MPI_COMM_WORLD);
	
}

// Initialize Window for Visuals
// TODO -> THis causes a segfault later on for some reason
int startVisuals(GLFWwindow *window) {
	// Setup Visuals
	// OpenGL window dims
	int width=600, height=600;
	if ( !glfwInit() ){
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}
	// Open a window and create its OpenGL context
	window = glfwCreateWindow( width, height, "Simulation", NULL, NULL);
	if ( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Initialize GLEW

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	return 0;
}

void output_and_finalize(struct options_t &opts, std::vector<Body> &bodies,MPI_Datatype &mpi_body, double &start_sim_time,double &end_sim_time, 
                        GLFWwindow *window) {
	// Output Bodies to Output File
    output_file(&opts, bodies);

    // Wait for Visual Window to close
    if (opts.visualize) {
      while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
      }
    }

	// Record End Time of Simulation
	end_sim_time = MPI_Wtime();
	printf("%06.0f\n", end_sim_time - start_sim_time);
}

void simulate(struct options_t &opts, std::vector<Body> &bodies, 
				int &size, int &rank, QuadTreeNode &root, MPI_Datatype &mpi_body, GLFWwindow *window) {
	// Repeat the sim for #steps iterations  
	for (int i = 0; i < opts.steps; i++) {
		// Create A Unique Pointer to the root of the Quad Tree
		std::unique_ptr<QuadTreeNode> root(new QuadTreeNode(Square(0.0,0.0,4), 0));

		// Insert Each Body into the root of the Tree
		for(std::size_t j = 0; j < bodies.size(); j ++) {
			root->insert(&bodies[j]);
		}

		// Only One Process Available (Sequential)
		if (size == 1) {
			// Calc force for each body
			for (std::size_t i = 0; i < bodies.size(); i ++) {
				bodies[i].clearForce();
				root->updateForce(&bodies[i], opts.theta);
			}

			// Compute the new position and velocity
			for (std::size_t j = 0; j < bodies.size(); j ++) {
				bodies[j].updatePositionVelocity(opts.dt);
			}
		} else {
			// Multiple Processes Available
			// Distribute Bodies to the processes (Load Balance)
			int process_body_count = bodies.size() / size;
			int process_remainder, final_rank;

			switch(process_body_count) {
			case 0:
				// # of Processes > # of Bodies
				process_body_count = 1;
				process_remainder = 1;
				final_rank = bodies.size() - 1;
			default:
				// # of Bodies > # of Processes
				final_rank = size - 1;
				process_remainder = (bodies.size() - (process_body_count * size)) + process_body_count;
			}

			// Create Array for the MPI Gather below 
			int body_count[final_rank + 1] = {0};
			int offset[final_rank + 1] = {0};
			for (int i = 0; i < final_rank + 1; i ++) {
				offset[i] = i * process_body_count;
				if (i != final_rank) {
					body_count[i] = process_body_count;
				} else {
					body_count[i] = process_remainder;
				}
			}

			if (rank <= final_rank) {
				for (int i = 0; i < final_rank + 1; i ++) {
					// Check if each process's id
					if (rank == i) {
						int startIdx = offset[i];
						int endIdx = body_count[i] + startIdx;

						// Clear and Calculate Force
						for (int j = startIdx; j < endIdx; j ++) {
							bodies[j].clearForce();
							root->updateForce(&bodies[j], opts.theta);
						}

						// Gather Data from all Tasks
						MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, bodies.data(), body_count, offset, mpi_body, MPI_COMM_WORLD);
					}
				}

				for (int i = 0; i < final_rank + 1; i ++) {
					if (rank == i) {
						int startIdx = offset[i];
						int endIdx = body_count[i] + startIdx;

						// Update the body's position and velocity
						for (int j = startIdx; j < endIdx; j ++) {
							bodies[j].updatePositionVelocity(opts.dt);
						}

						// Gather Data from all Tasks
						MPI_Allgatherv(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, bodies.data(), body_count, offset, mpi_body, MPI_COMM_WORLD);
					}
				}
			}
		}

		// Update Visuals
		if (opts.visualize && rank == 0) {

			glClear( GL_COLOR_BUFFER_BIT );
			drawOctreeBounds2D(*root);
			float colors[3] ={0.5f, 0.0f, 1.0f}; // Dark Purple
			for (std::size_t p = 0; p < bodies.size(); p++) {
				drawParticle2D(coordTransform(bodies[p].x_pos), coordTransform(bodies[p].y_pos) , 0.01 , colors);
			}
			// Swap buffers
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
}