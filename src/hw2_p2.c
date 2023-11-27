#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

enum Color { RED, BLACK };

struct Node {
    int key;
    enum Color color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
};

struct RBTree {
    struct Node *root;
    struct Node *nil;
};

void insertFixup(struct RBTree *tree, struct Node *z);
void insert(struct RBTree *tree, int key);

void insertFixup(struct RBTree *tree, struct Node *z)
{
    ; /* TBD */
}

void insert(struct RBTree *tree, int key)
{
    struct Node *z = (struct Node *)malloc(sizeof(struct Node)), *y = tree->nil, *x = tree->root;
    z->key = key;
    z->color = RED;
    z->parent = NULL;
    z->left = tree->nil;
    z->right = tree->nil;

    while (x != tree->nil) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    /* x will end up in the position z should be, and y will be its parent. */

    z->parent = y;
    if (y == tree->nil) {
        tree->root = z; /* Tree was empty */
    }
    else if (z->key < y->key) {
        y->left = z;
    }
    else {
        y->right = z;
    }

    insertFixup(tree, z);
}


int main(int argc, char* argv[]) {
    struct Node nil = {0, BLACK, NULL, NULL, NULL};
    struct RBTree rbtree = {&nil, &nil};
    char *op;
    int key;
    op = malloc(8 * sizeof(char));
    while (true) {
        if (scanf("%s", op) != 1) return 1;
        if (!strncmp(op, "quit", 4)) {
            free(op);
            break;
        }
        else if (!strncmp(op, "insert", 6)) {
            if (scanf("%d", &key) != 1) return 1;
            insert(&rbtree, key);
        }
        else if (!strncmp(op, "delete", 6)) {
            printf("%s\n", op);
        }
        else if (!strncmp(op, "search", 6)) {
            printf("%s\n", op);
        }
        else {
            printf("Invalid instruction. Please try again, or enter 'quit' to exit.\n");
        }
    }
    return 0;
}
