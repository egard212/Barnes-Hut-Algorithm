#include "argparse.h"

void get_opts(int argc,
              char **argv,
              struct options_t *opts)
{
    if (argc == 1)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "\t--in or -i <file_path>" << std::endl;
        std::cout << "\t--out or -o <file_path>" << std::endl;
        std::cout << "\t--steps or -s <steps>" << std::endl;
        std::cout << "\t--theta or -t <theta>" << std::endl;
        std::cout << "\t--dt or -d <timestep>" << std::endl;
        std::cout << "\t[Optional] --visualize or -V" << std::endl;
        exit(0);
    }

    opts->visualize = false;

    struct option l_opts[] = {
        {"in", required_argument, NULL, 'i'},
        {"out", required_argument, NULL, 'o'},
        {"steps", required_argument, NULL, 's'},
        {"theta", required_argument, NULL, 't'},
        {"dt", required_argument, NULL, 'd'},
        {"visualize", no_argument, NULL, 'V'}
    };

    int ind, c;
    while ((c = getopt_long(argc, argv, "i:o:s:t:d:V", l_opts, &ind)) != -1)
    {
        switch (c)
        {
        case 0:
            break;
        case 'i':
            opts->in_file = (char *)optarg;
            break;
        case 'o':
            opts->out_file = (char *)optarg;
            break;
        case 's':
            opts->steps = atoi((char *)optarg);
            break;
        case 'V':
            opts->visualize = true;
            break;
        case 't':
            opts->theta = atof((char *)optarg);
            break;
        case 'd':
            opts->dt = atof((char *)optarg);
            break;
        case ':':
            std::cerr << argv[0] << ": option -" << (char)optopt << "requires an argument." << std::endl;
            exit(1);
        }
    }
}
