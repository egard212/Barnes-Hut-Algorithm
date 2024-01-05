#include "Simulation.h"

int main(int argc, char **argv) {
	
	// Initialize MPI
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	/* 
		Create Variables for Initialization:
		1) mpi_body -> Custom MPI Datatype for the bodies
		2) opts -> command line arguments
		3) start/end_sim_time -> Output Time for the Simulation
		4) bodies -> Vector containing all the bodies from the input file
		5) root -> QuadTreeNode (Data Structure for holding all the bodies)
		6) window -> Window for Visualization
	
	*/
	MPI_Datatype mpi_body;
	struct options_t opts;
	double start_sim_time, end_sim_time;
	std::vector<Body> bodies;
	QuadTreeNode root(Square(0.0,0.0,4), 0);
	GLFWwindow *window;
	
  createMPIData(mpi_body);
	// Read in Command Line Arguments
	get_opts(argc, argv, &opts);

	// Start The Simulation
  if (rank == 0) {
    
    if (opts.visualize) {
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
    }
    
    // Start Recording the time and read in the inputs
    start_sim_time = MPI_Wtime();
    read_file(&opts, bodies);

    if (size != 1) {
        broadcastData(opts, bodies, mpi_body, rank);
    }
  } else {
      broadcastData(opts, bodies, mpi_body, rank);
  }

  simulate(opts, bodies, size, rank, root, mpi_body, window);

  MPI_Barrier(MPI_COMM_WORLD);

  if (rank == 0) {
      output_and_finalize(opts, bodies, mpi_body, start_sim_time, end_sim_time, window);
  }
	
    MPI_Type_free(&mpi_body);

    MPI_Finalize();

  return 0;
}
