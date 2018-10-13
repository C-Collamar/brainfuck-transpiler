/**
 * Enumeration of all tokens that are recognized in the source
 * language.
 */
typedef enum Token {
    TOKEN_MOVE_LEFT,
    TOKEN_MOVE_RIGHT,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,
    TOKEN_OUTPUT,
    TOKEN_INPUT,
    TOKEN_JUMP_FORWARD,
    TOKEN_JUMP_BACKWARD
} Token;

//maps tokens from `enum Token` to their lexicon for printing purposes.
char *tokenString[] = { "<", ">", "+", "-", ".", ",", "[", "]" };

/**
 * Linked list of tokens.
 */
struct TokenList {
    Token token;
    struct {
        unsigned int row;
        unsigned int col;
    } position;
    struct TokenList *next;
};

/**
 * Frees allocated memory in `tokenStream` and points it to NULL.
 */
void freeTokenStream() {
    TokenList *currNode = NULL;
    while(tokenStream) {
        currNode = tokenStream;
        tokenStream = tokenStream->next;
        free(currNode);
    }
}

/**
 * Determine lexemes in the stream of characters from the source file
 * to create a list of tokens.
 */
TokenList* tokenize(void) {
    TokenList *head = NULL, *tail = NULL, *newToken = NULL;
    char c, ignore_char;
    unsigned int rowCount = 1, colCount = 1;

    while(fscanf(inputFile, "%c", &c) != EOF) {
        ignore_char = 0;
        //create new token if it matches any of the language's lexemes
        switch(c) {
            case '<':
                newToken = malloc(sizeof(TokenList));
                newToken->token = TOKEN_MOVE_LEFT;
                newToken->position.row = rowCount;
                newToken->position.col = colCount;
                break;
            case '>':
                newToken = malloc(sizeof(TokenList));
                newToken->token = TOKEN_MOVE_RIGHT;
                newToken->position.row = rowCount;
                newToken->position.col = colCount;
                break;
            case '+':
                newToken = malloc(sizeof(TokenList));
                newToken->token = TOKEN_INCREMENT;
                newToken->position.row = rowCount;
                newToken->position.col = colCount;
                break;
            case '-':
                newToken = malloc(sizeof(TokenList));
                newToken->token = TOKEN_DECREMENT;
                newToken->position.row = rowCount;
                newToken->position.col = colCount;
                break;
            case '.':
                newToken = malloc(sizeof(TokenList));
                newToken->token = TOKEN_OUTPUT;
                newToken->position.row = rowCount;
                newToken->position.col = colCount;
                break;
            case ',':
                newToken = malloc(sizeof(TokenList));
                newToken->token = TOKEN_INPUT;
                newToken->position.row = rowCount;
                newToken->position.col = colCount;
                break;
            case '[':
                newToken = malloc(sizeof(TokenList));
                newToken->token = TOKEN_JUMP_FORWARD;
                newToken->position.row = rowCount;
                newToken->position.col = colCount;
                break;
            case ']':
                newToken = malloc(sizeof(TokenList));
                newToken->token = TOKEN_JUMP_BACKWARD;
                newToken->position.row = rowCount;
                newToken->position.col = colCount;
                break;
            case '\n':
                ++rowCount;
                colCount = 1;
                break;
            default:
                ignore_char = 1;
        }

        newToken->next = NULL;
        ++colCount;

        if(!ignore_char) {
            if(newToken == NULL) {
                fprintf(stderr, "Error: Memory allocation failed.\n");
                freeTokenStream();
                fclose(inputFile);
                exit(EXIT_FAILURE);
            }

            //insert new token to the list
            if(tail == NULL) {
                head = tail = newToken;
            }
            else {
                tail = tail->next = newToken;
            }
        }
    }

    return head;
}