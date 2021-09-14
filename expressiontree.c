#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum OperatorType
{
    PLUS,
    MINUS,
    MULTIPLICATION,
    DIVISION
} OperatorType;

typedef struct NodeValue
{
    union
    {
        OperatorType operatorType;
        int number;
    };
    int isNumber;
} NodeValue;

typedef struct TreeNodeStruct
{
    NodeValue value;
    struct TreeNodeStruct* left;
    struct TreeNodeStruct* right;
} TreeNode;

TreeNode* newOpteratorNode(OperatorType operatortype, TreeNode *l, TreeNode *r)
{
    TreeNode* treeNode = (TreeNode*)(malloc(sizeof(TreeNode)));
    treeNode->left=l;
    treeNode->right=r;
    treeNode->value= (NodeValue){
        .isNumber=0,
        .operatorType=operatortype,
    };
    return treeNode;
}

TreeNode* newNumberNode(int number)
{
    TreeNode* res = (TreeNode*)(malloc(sizeof(TreeNode)));
    res->value=(NodeValue){
        .isNumber=1,
        .number=number,
    };
    res->left=0;
    res->right=0;
    return res;
}

typedef struct 
{
    TreeNode* root;
}Tree;

Tree* newTree(TreeNode *n)
{
    Tree *res = (Tree*)malloc(sizeof(Tree));
    res->root=n;
    return res;
}
int empty(Tree* t)
{
    return !(t->root);
}

int findHeight(TreeNode* n)
{
    if(!n) return -1;
    else
    {
        int lr = findHeight(n->left);
        int rr = findHeight(n->right);
        if(lr>=rr) return lr +1;
        else return rr +1;
    }
}


int calculate(TreeNode *n)
{
    int num;
    if(n->value.isNumber)
    {
    num=n->value.number;
    }
    else{
        int left = calculate(n->left);
        int right = calculate(n->right);
        switch (n->value.operatorType)
        {
        case PLUS:
            num = left + right;
            break;
        case MINUS:
            num = left - right;
            break;
        case MULTIPLICATION:
            num = left * right;
            break;
        case DIVISION:
            num = left / right;
            break;
        default:
            printf("### Operation Error ###");
            break;
        }
    }
    return num;
}

int traverse(TreeNode* n)
{
  if(!n) return 0;
    int result = calculate(n);
    traverse(n->left);
    traverse(n->right);
    return result;
}

char* createExpression(TreeNode *n)
{
    if(n->value.isNumber)
    {
        char* mem = (char*)(malloc(12));
        sprintf(mem, "%d", n->value.number);
        return mem;
    }
    else
    {
        char* left = createExpression(n->left);
        char* right = createExpression(n->right);
        int length = strlen(left)+strlen(right)+4;
        char* mem = (char*)(malloc(length));
        char op;
        switch (n->value.operatorType)
        {
        case PLUS:
            op = '+';
            break;
        case MINUS:
            op = '-';
            break;
        case MULTIPLICATION:
            op = '*';
            break;
        case DIVISION:
            op='/';
            break;
        default:
            printf("### printing fail ####");
            break;
        }

        sprintf(mem, "(%s%c%s)",left, op, right);
        free(left);
        free(right);
        return mem;
    }
}


int main()
{
    TreeNode *node_1=newOpteratorNode(MULTIPLICATION,newNumberNode(16),newNumberNode(32));
    TreeNode *node_2=newOpteratorNode(MINUS, node_1, newNumberNode(-5));
    TreeNode *node_3=newOpteratorNode(PLUS, newNumberNode(9), newNumberNode(12));
    TreeNode *node_4=newOpteratorNode(PLUS, node_3, node_2);
    TreeNode *rootNode=newOpteratorNode(MULTIPLICATION, newNumberNode(3), node_4);

    printf("Height: %d \n", findHeight(rootNode));
    printf("Expression: %s = %d\n",createExpression(rootNode), calculate(rootNode));
}