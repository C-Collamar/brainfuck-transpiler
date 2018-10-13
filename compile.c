#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct TokenList TokenList;
typedef struct NodeList NodeList;
typedef struct TreeNode TreeNode;

FILE      *inputFile;     //input file to read the source language from
FILE      *outputFile;    //destination file to dump the translated source code
char      *generatedCode; //string containing the translated source code
TokenList *tokenStream;   //head of the generated token list
TreeNode  *syntaxTree;    //root node of the generated abstract syntax tree

#include "tokenizer.h"
#include "parser.h"
#include "translator.h"
#include "misc.h"

int main(int argc, char **argv) {
    //open source file
    inputFile = readFile(argc, argv);

    //generate tokens from the source code
    tokenStream = tokenize();

    //close input file
    fclose(inputFile);

    //parse from the generated tokens
    syntaxTree = parse();

    //deallocate token list
    freeTokenStream();

    //translates source language to C given the AST
    generatedCode = translate();

    //free allocated memory to create the syntax tree
    freeAST();

    //dump compiled code to a .c file
    writeFile("output.c", generatedCode);

    printf("\"%s\" successfully compiled to \"output.c\".\n", argv[1]);

    free(generatedCode);
    return EXIT_SUCCESS;
}