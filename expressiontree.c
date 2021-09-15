#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** 
 * OperatorType is the type of operator an operator node has. 
 */
typedef enum OperatorType
{
    PLUS,
    MINUS,
    MULTIPLICATION,
    DIVISION
} OperatorType;

/**
 * NodeValue is the value of a node.
 */
typedef struct NodeValue
{
    union
    {
        OperatorType operatorType;
        int number;
    };
    int isNumber;
} NodeValue;

/**
 * TreeNode represents a node in the tree.
 */
typedef struct TreeNode
{
    NodeValue value;
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

/**
 * newOperatorNode creates a new node with the specified operator type and children.
 */
TreeNode* newOperatorNode(OperatorType type, TreeNode *l, TreeNode *r)
{
    TreeNode* treeNode = (TreeNode*)(malloc(sizeof(TreeNode)));
    treeNode->left = l;
    treeNode->right = r;
    treeNode->value = (NodeValue) {
        .isNumber = 0,
        .operatorType = type,
    };
    return treeNode;
}

/**
 * newNumberNode creates a leaf node that contains a numeric value.
 */
TreeNode* newNumberNode(int number)
{
    TreeNode* res = (TreeNode*)(malloc(sizeof(TreeNode)));
    res->left = 0;
    res->right = 0;
    res->value = (NodeValue) {
        .isNumber = 1,
        .number = number,
    };
    return res;
}

/**
 * findHeight finds the height of a tree.
 */
int findHeight(TreeNode* n)
{
    if (!n) return -1;

    int lr = findHeight(n->left);
    int rr = findHeight(n->right);
    return lr >= rr ? lr + 1 : rr + 1;
}

/**
 * calculate traverses the expression tree and calculates the result.
 */
int calculate(TreeNode *n)
{
    int num;
    if(n->value.isNumber)
    {
        num=n->value.number;
    }
    else
    {
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

/**
 * createExpression creates a string that contains a mathematic expression for the given tree.
 *
 * NOTE: The string that is returned from createExpression must be freed.
 */
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
    TreeNode *node1 = newOperatorNode(MULTIPLICATION, newNumberNode(16), newNumberNode(32));
    TreeNode *node2 = newOperatorNode(MINUS, node1, newNumberNode(-5));
    TreeNode *node3 = newOperatorNode(PLUS, newNumberNode(9), newNumberNode(12));
    TreeNode *node4 = newOperatorNode(PLUS, node3, node2);
    TreeNode *rootNode = newOperatorNode(MULTIPLICATION, newNumberNode(3), node4);

    int height = findHeight(rootNode);
    char *expression = createExpression(rootNode);
    int result = calculate(rootNode);

    printf("Height: %d \n", height);
    printf("Expression: %s = %d\n", expression, result);

    free(expression);
}