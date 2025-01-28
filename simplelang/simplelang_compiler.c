#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_TOKEN_LEN 100;

typedef enum
{
    TOKEN_INT,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_IF,
    TOKEN_EQUAL,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMICOLON,
    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

// AST Node types
typedef enum
{
    NODE_VAR_DECL,
    NODE_ASSIGN,
    NODE_CONDITIONAL,
    NODE_EXPRESSION
} NodeType;

// Token structure
typedef struct
{
    TokenType type;
    char text[MAX_TOKEN_LEN];

} Token;

typedef struct
{
    NodeType type;
    char value[MAX_TOKEN_LEN];
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

Token currentToken;
FILE *inputFile;
// lexer function

void getNextToken()
{
    int c;
    while ((c = fgetc(inputFile)) != EOF)
    {
        if (isspace(c))
            continue;
        if (isalpha(c))
        {
            int len = 0;
            currentToken.text[len++] = c;
            while (isalnum(c = fgetc(inputFile)))
            {
                if (len < MAX_TOKEN_LEN - 1)
                    currentToken.text[len++] = c;
            }
            ungetc(c, inputFile);
            currentToken.text[len] = '\0' 
            if (strcmp(currentToken.text, "int") == 0) currentToken.type = TOKEN_INT;
            else if (strcmp(currentToken.text, "if") == 0) currentToken.type = TOKEN_IF;
            else currentToken.type= TOKEN_IDENTIFIER;
            return;
        }
        if (isdigit(c))
        {
            int len = 0;
            currentToken.text[len++] = c;
            while (isdigit(c = fgetc(inputFile)))
            {
                if (len < MAX_TOKEN_LEN - 1)
                    currentToken.text[len++] = c;
            }
            ungetc(c, inputFile);
            currentToken.text[len] = '\0' 
            currentToken.type=TOKEN_NUMBER;
            return;
        }
        switch(c){
            case '=': currentToken.type = TOKEN_ASSIGN; currentToken.text[0] = '='; currentToken.text[1] = '\0'; return;
            case '+': currentToken.type = TOKEN_PLUS; currentToken.text[0] = '+'; currentToken.text[1] = '\0'; return;
            case '-': currentToken.type = TOKEN_MINUS; currentToken.text[0] = '-'; currentToken.text[1] = '\0'; return;
            case '{': currentToken.type = TOKEN_LBRACE; currentToken.text[0] = '{'; currentToken.text[1] = '\0'; return;
            case '}': currentToken.type = TOKEN_RBRACE; currentToken.text[0] = '}'; currentToken.text[1] = '\0'; return;
            case '(': currentToken.type = TOKEN_LBRACE; currentToken.text[0] = '('; currentToken.text[1] = '\0'; return;
            case ')': currentToken.type = TOKEN_RBRACE; currentToken.text[0] = ')'; currentToken.text[1] = '\0'; return;
            case ';': currentToken.type = TOKEN_SEMICOLON; currentToken.text[0] = ';'; currentToken.text[1] = '\0'; return;
            default: currentToken.type = TOKEN_UNKNOWN; currentToken.text[0] = c; currentToken.text[1] = '\0'; return;
        }
    }
     currentToken.type= TOKEN_EOF;
      currentToken.text[0] = '\0';
}

//ASTNode Creation function
ASTNode *createNode(NodeType type,const char *value){
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type=type;
    strcpy(node->value ,value);
    node->left=node->right=NULL;
    return node;
}
//parsing function

ASTNode *parseExpression(){
    if(currentToken.type ==TOKEN_NUMBER || currentToken.type==TOKEN_IDENTIFIER){
        ASTNode *node=createNode(NODE_EXPRESSION,currentToken.text);
        getNextToken();
        return node;
    }
    return NULL;
}
ASTNode *parseAssignment(){
    if(currentToken.type==TOKEN_IDENTIFIER){
        ASTNode *node=createNode(NODE_ASSIGN,currentToken.text);
        if(currentToken.type==TOKEN_ASSIGN){   
            getNextToken();
            Node->left=parseExpression();
            if(currentToken.type==TOKEN_PLUS || currentToken.type ==TOKEN_MINUS){
                TokenType ps=currentToken.type;
                getNextToken();
                ASTNode *opNode=createNode(ps ==TOKEN_PLUS ? NODE_EXPRESSION:NODE_EXPRESSION,ps==TOKEN_PLUS?"+":"-");
                opNode->left=node->left;
                opNode->right=parseExpression();
                node->left=opNode;
            }
        }
        if(currentToken.type==TOKEN_SEMICOLON)getNextToken();
        return node;
    }
    return NULL;
}
ASTNode *parseStatement(){
    if(currentToken.type==TOKEN_INT){
        getNextToken();
        if(currentToken.type==TOKEN_IDENTIFIER){
            ASTNode *node=createNode(NODE_VAR_DECL,currentToken.text);
            getNextToken();
            if(currentToken.type==TOKEN_SEMICOLON)getNextToken();
            return node;
        }
    }else if(currentToken.type==TOKEN_IF){
        getNextToken();
        if(currentToken.type==TOKEN_LBRACE){
            getNextToken();
            ASTNode *node=createNode(NODE_CONDITIONAL,"IF");
            node->left=parseAssignment();
            if(currentToken.type==TOKEN_RBRACE)getNextToken();
            return node;
        }
    }else{
        return parseAssignment();
    }
    return NULL;
}
ASTNode *parseProgram(){
    ASTNode *root=NULL ,*current=NULL;
    while(currentToken.type!=TOKEN_EOF){
        ASTNode *stmt=parseAssignment();
        if(!root){
            root=stmt;
            current=root;
        }else{
            current->right=stmt;
            current=stmt;
        }
    }
    return root;
}


//Code Generation
void codegen(ASTNode *node){
    if(!node)return ;
    switch(node->type){
        case NODE_VAR_DECL:
          printf("Reserve %s\n",node->value);
          break;
        case NODE_ASSIGN:
          printf("Load %s\n",node->value);
          codegen(node->left);
          codegen(node->right);
          printf("Store %s\n",node->value);
          break;
        case NODE_CONDITIONAL:
          printf("CMP %s\n",node->left->value);
          printf("BEQ %s\n",node->left->value);
          break;
        case NODE_EXPRESSION:
          if(isdigit(node->value[0])){
            printf("LOAD %s\n",node->value);
          }else {
            printf("LOAD 5s\n",node->value);
          }
          break;
        default :
         break;
    }
    codegen(node->left);
    codegen(node->right);
}
int main(){
    inputFile =fopen("input.txt","r");
    if(!inputFile){
        perror("Failed to open file");
        return 1;
    }
    getNextToken();
    ASTNode *ast=parseProgram();
    codegen(ast);
    
    fclose(inputFile);
    return 0;
}
