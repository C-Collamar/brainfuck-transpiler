/**
 * Helper function that determines whether file has a .b or .bf extension.
 */
int valid_filetype(const char *path) {
    size_t len = strlen(path);
    return
        (path[len - 2] == '.' && path[len - 1] == 'b') ||
        (path[len - 3] == '.' && path[len - 2] == 'b' && path[len - 1] == 'f');
}

/**
 * 
 */

/**
 * Attempts to open the source file; terminates upon failure.
 */
FILE* readFile(int argc, char **argv) {
    FILE *inFile = NULL;
    //check if invalid command-line arguments
    if(argc != 2) {
        //print program usage
        printf(
            "Compiles Brainf*** source code to standard C.\n"
            "Usage: %s path\n"
            "    path - path to a .b or .bf file\n",
            argv[0]);
        exit(EXIT_SUCCESS);
    }
    //if input file is not of type .b or .bf
    else if(!valid_filetype(argv[1])) {
        fprintf(stderr, "Error: Invalid file format. Compiler only accepts .b or .bf source files.");
        exit(EXIT_FAILURE);
    }

    //if file does not exist
    if(!(inFile = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error: Cannot read \"%s\".\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    return inFile;
}

void writeFile(char *filename, char *code) {
    FILE *outFile;

    if(!(outFile = fopen(filename, "w"))) {
        fprintf(stderr, "Error: Unable to write to \"%s\".", filename);
        free(generatedCode);
        exit(EXIT_FAILURE);
    }

    fprintf(outFile, "%s", code);
    fclose(outFile);
}