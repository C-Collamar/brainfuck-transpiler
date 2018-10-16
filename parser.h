/**
 * Grammar Productions:
 * (0) start := expr
 * (1) expr  := op expr'
 * (2) expr' := op expr' | ε
 * (3) op    := + | - | . | , | < | > | loop
 * (4) loop  := [ expr ]
 */

TokenList *currTokenPtr; //iterates through the token stream
TokenList *prevTokenPtr; //points to previously consumed token

/**
 * Enumeration of all terminal and non-terminal symbols in the grammar.
 */
typedef enum Symbol {
    NODE_START,
    NODE_EXPR,
    NODE_EXPR_CONT,
    NODE_OP,
    NODE_LOOP,
    NODE_MOVE_LEFT,
    NODE_MOVE_RIGHT,
    NODE_INCREMENT,
    NODE_DECREMENT,
    NODE_OUTPUT,
    NODE_INPUT,
    NODE_JUMP_FORWARD,
    NODE_JUMP_BACKWARD
} Symbol;

/**
 * Node in the abstract syntax tree. Contains symbol information and
 * child nodes. Note that terminal symbols cannot have child nodes.
 */
struct TreeNode {
    Symbol type;
    unsigned int depth;
    NodeList *children;
};

/**
 * Data structure for containing a list of nodes. Mainly used for storing
 * child nodes.
 */
struct NodeList {
    TreeNode *node;
    NodeList *next;
};

/**
 * Frees allocated nodes and list containers in the abstract syntax tree.
 */
void freeAST(TreeNode *node) {
    if(node != NULL) {
        NodeList *nextChild;
        while(node->children != NULL) {
            nextChild = node->children->next;
            freeAST(node->children->node);
            free(node->children);
            node->children = nextChild;
        }

        free(node);
    }
}

TreeNode* production1(unsigned int);

TreeNode* parse(void) {
    TreeNode *rootNode = NULL;

    if(tokenStream) {
        currTokenPtr = tokenStream;

        rootNode = malloc(sizeof(TreeNode));
        rootNode->type = NODE_START;
        rootNode->depth = 0;
        rootNode->children = malloc(sizeof(NodeList));
        rootNode->children->node = production1(rootNode->depth + 1);
        rootNode->children->next = NULL;
    }
    else {
        fprintf(stderr, "Error: No instructions to compile.\n");
        freeTokenStream();
        fclose(inputFile);
        exit(EXIT_FAILURE);
    }

    return rootNode;
}

TreeNode* production4(unsigned int depth) {
    //if there are no available tokens to consume
    if(currTokenPtr == NULL) {
        fprintf(stderr,
            "Error: Unexpected end of instruction. Expected \"%s\" after \"%s\" at line %i, column %i.\n",
            tokenString[TOKEN_JUMP_FORWARD],
            tokenString[prevTokenPtr->token],
            prevTokenPtr->position.col,
            prevTokenPtr->position.row
        );
        freeAST(syntaxTree);
        freeTokenStream();
        exit(EXIT_FAILURE);
    }
    //if the token to consume is not in the FIRST set of producton 4
    else if(currTokenPtr->token != TOKEN_JUMP_FORWARD) {
        fprintf(stderr,
            "Error: Expected \"%s\" instead of \"%s\" at \"%s\" at line %i, column %i.\n",
            tokenString[TOKEN_JUMP_FORWARD],
            tokenString[prevTokenPtr->token],
            currTokenPtr->position.col,
            currTokenPtr->position.row
        );
        freeAST(syntaxTree);
        freeTokenStream();
        exit(EXIT_FAILURE);
    }

    //advance token pointer
    prevTokenPtr = currTokenPtr;
    currTokenPtr = currTokenPtr->next;

    //create node for LOOP non-terminal symbol
    TreeNode *node = malloc(sizeof(TreeNode));
    node->type = NODE_LOOP;
    node->depth = depth;
    node->children = malloc(sizeof(NodeList));

    //create child node for '[' terminal symbol
    node->children->node = malloc(sizeof(TreeNode));
    node->children->node->type = NODE_JUMP_FORWARD;
    node->children->node->depth = depth;
    node->children->node->children = NULL;

    //create child node for 'expr' non-terminal
    node->children->next = malloc(sizeof(NodeList));
    node->children->next->node = production1(depth + 1);

    if(currTokenPtr == NULL) {
        fprintf(stderr,
            "Error: Unexpected end of instruction. Expected \"%s\" at least after \"%s\" at line %i, column %i.\n",
            tokenString[TOKEN_JUMP_BACKWARD],
            tokenString[prevTokenPtr->token],
            prevTokenPtr->position.col,
            prevTokenPtr->position.row
        );
        freeAST(syntaxTree);
        freeTokenStream();
        exit(EXIT_FAILURE);
    }
    else if(currTokenPtr->token != TOKEN_JUMP_BACKWARD) {
        fprintf(stderr,
            "Error: Expected \"%s\" instead of \"%s\" at \"%s\" at line %i, column %i.\n",
            tokenString[TOKEN_JUMP_BACKWARD],
            tokenString[prevTokenPtr->token],
            currTokenPtr->position.col,
            currTokenPtr->position.row
        );
        freeAST(syntaxTree);
        freeTokenStream();
        exit(EXIT_FAILURE);
    }

    //advance token pointer
    prevTokenPtr = currTokenPtr;
    currTokenPtr = currTokenPtr->next;

    //create child node for ']' non-terminal
    node->children->next->next = malloc(sizeof(NodeList));
    node->children->next->next->next = NULL;
    node->children->next->next->node = malloc(sizeof(TreeNode));
    node->children->next->next->node->type = NODE_JUMP_BACKWARD;
    node->children->next->next->node->depth = depth;
    node->children->next->next->node->children = NULL;

    return node;
}

TreeNode* production3(unsigned int depth) {
    if(currTokenPtr == NULL) {
        fprintf(stderr,
            "Error: Unexpected end of instruction. Expected either \"%s\", \"%s\", \"%s\", "
            "\"%s\", \"%s\", \"%s\" or \"%s\" after \"%s\" at line %i, column %i.\n",
            tokenString[TOKEN_MOVE_LEFT],
            tokenString[TOKEN_MOVE_RIGHT],
            tokenString[TOKEN_INCREMENT],
            tokenString[TOKEN_DECREMENT],
            tokenString[TOKEN_OUTPUT],
            tokenString[TOKEN_INPUT],
            tokenString[TOKEN_JUMP_FORWARD],
            tokenString[prevTokenPtr->token],
            prevTokenPtr->position.row,
            prevTokenPtr->position.col
        );
        freeAST(syntaxTree);
        freeTokenStream();
        exit(EXIT_FAILURE);
    }
    else if(currTokenPtr->token == TOKEN_JUMP_BACKWARD) {
        fprintf(stderr,
            "Error: No matching \"%s\" before \"%s\" at line %i, column %i.\n",
            tokenString[TOKEN_JUMP_FORWARD],
            tokenString[TOKEN_JUMP_BACKWARD],
            currTokenPtr->position.row,
            currTokenPtr->position.col
        );
        freeAST(syntaxTree);
        freeTokenStream();
        exit(EXIT_FAILURE);
    }

    TreeNode *node = malloc(sizeof(TreeNode));
    node->type = NODE_OP;
    node->depth = depth;
    node->children = malloc(sizeof(NodeList));
    node->children->next = NULL;

    if(currTokenPtr->token == TOKEN_JUMP_FORWARD) {
        node->children->node = production4(depth);
    }
    else {
        node->children->node = malloc(sizeof(TreeNode));
        node->children->node->depth = depth;
        node->children->node->children = NULL;

        switch(currTokenPtr->token) {
            case TOKEN_MOVE_LEFT:
                node->children->node->type = NODE_MOVE_LEFT;
                break;
            case TOKEN_MOVE_RIGHT:
                node->children->node->type = NODE_MOVE_RIGHT;
                break;
            case TOKEN_INCREMENT:
                node->children->node->type = NODE_INCREMENT;
                break;
            case TOKEN_DECREMENT:
                node->children->node->type = NODE_DECREMENT;
                break;
            case TOKEN_OUTPUT:
                node->children->node->type = NODE_OUTPUT;
                break;
            case TOKEN_INPUT:
                node->children->node->type = NODE_INPUT;
                break;
            default:
                fprintf(stderr,
                    "Error: Unexpected \"%s\" at line %i, column %i. Expected either \"%s\", \"%s\", "
                    "\"%s\", \"%s\", \"%s\", \"%s\", \"%s\" or \"%s\" after \"%s\" at line %i, column %i.\n",
                    tokenString[currTokenPtr->token],
                    currTokenPtr->position.row,
                    currTokenPtr->position.col,
                    tokenString[TOKEN_MOVE_LEFT],
                    tokenString[TOKEN_MOVE_RIGHT],
                    tokenString[TOKEN_INCREMENT],
                    tokenString[TOKEN_DECREMENT],
                    tokenString[TOKEN_OUTPUT],
                    tokenString[TOKEN_INPUT],
                    tokenString[TOKEN_JUMP_FORWARD],
                    tokenString[TOKEN_JUMP_BACKWARD],
                    tokenString[prevTokenPtr->token],
                    prevTokenPtr->position.row,
                    prevTokenPtr->position.col
                );
                freeAST(syntaxTree);
                freeTokenStream();
                free(node->children->node);
                free(node->children);
                free(node);
                exit(EXIT_FAILURE);
        }

        //point to next token to consume
        prevTokenPtr = currTokenPtr;
        currTokenPtr = currTokenPtr->next;
    }

    return node;
}

TreeNode* production2(unsigned int depth) {
    TreeNode *node = NULL;

    if(currTokenPtr != NULL && currTokenPtr->token != TOKEN_JUMP_BACKWARD) {
        //create node for EXPR_CONT non-terminal symbol
        node = malloc(sizeof(TreeNode));
        node->type = NODE_EXPR_CONT;
        node->depth = depth;
        node->children = malloc(sizeof(NodeList));

        //create node for OP non-terminal symbol
        node->children->node = production3(depth);
        node->children->next = NULL;

        //create node for EXPR_CONT non-terminal symbol if there are remaining tokens
        if(currTokenPtr != NULL) {
            node->children->next = malloc(sizeof(NodeList));
            node->children->next->node = production2(depth);
            node->children->next->next = NULL;
        }
    }

    return node;
}

TreeNode* production1(unsigned int depth) {
    //if there are no available tokens to consume
    if(currTokenPtr == NULL) {
        fprintf(stderr,
            "Error: Unexpected end of instruction. Expected either \"%s\", \"%s\", \"%s\", "
            "\"%s\",\"%s\", \"%s\", \"%s\" or \"%s\" after \"%s\" at line %i, column %i.\n",
            tokenString[TOKEN_MOVE_LEFT],
            tokenString[TOKEN_MOVE_RIGHT],
            tokenString[TOKEN_INCREMENT],
            tokenString[TOKEN_DECREMENT],
            tokenString[TOKEN_OUTPUT],
            tokenString[TOKEN_INPUT],
            tokenString[TOKEN_JUMP_FORWARD],
            tokenString[TOKEN_JUMP_BACKWARD],
            tokenString[prevTokenPtr->token],
            prevTokenPtr->position.row,
            prevTokenPtr->position.col
        );
        freeAST(syntaxTree);
        freeTokenStream();
        exit(EXIT_FAILURE);
    }

    TreeNode *node = malloc(sizeof(TreeNode));
    node->type = NODE_EXPR;
    node->depth = depth;
    node->children = malloc(sizeof(NodeList));
    node->children->node = production3(depth);
    node->children->next = NULL;

    if(currTokenPtr) {
        TreeNode *prod2Node = production2(depth);

        if(prod2Node != NULL) {
            node->children->next = malloc(sizeof(NodeList));
            node->children->next->node = prod2Node;
            node->children->next->next = NULL;
        }
    }

    return node;
}
