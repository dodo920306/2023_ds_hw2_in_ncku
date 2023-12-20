#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


struct Node {
    int key;
    bool isRed;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
};

struct RBTree {
    struct Node *root;
    /* nil = All leaves of the tree. */
    struct Node *nil;
};

struct Node *search(struct RBTree *tree, int key);
void leftRotation(struct RBTree *tree, struct Node *x);
void rightRotation(struct RBTree *tree, struct Node *x);
void insertFixUp(struct RBTree *tree, struct Node *current);
void insert(struct RBTree *tree, int key);
void deleteFixUp(struct RBTree *tree, struct Node *current);
void delete(struct RBTree *tree, int key);

struct Node *search(struct RBTree *tree, int key)
{
    struct Node *current = tree->root;

    while (current != tree->nil && key != current->key) {
        if (key < current->key){                      
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return current;
}

void leftRotation(struct RBTree *tree, struct Node *x)
{
    /**    |                   |
     *     x                   y
     *    / \        =>       / \
     *       y               x
     *      /                 \
     */
    struct Node *y = x->right;
    x->right = y->left; /* Change x's right child to its right child's left child. */
    if (y->left != tree->nil)
        y->left->parent = x; /* If x's right child's left child exists, make its parent x. */
    y->parent = x->parent; /* Move y up. */
    if (x->parent == tree->nil)
        /* x is the root. */
        tree->root = y;
    else if (x == x->parent->left)  /* Maintain the left and right. */
        x->parent->left = y;
    else
        x->parent->right = y;
    /* Finally, change x and y. */
    y->left = x;
    x->parent = y;
}

void rightRotation(struct RBTree *tree, struct Node *x)
{
    /**    |                   |
     *     x                   y
     *    / \        =>       / \
     *   y                       x
     *    \                     /
     */
    struct Node *y = x->left;
    x->left = y->right;
    if (y->right != tree->nil)
        y->right->parent = x;
    y->parent = x->parent; /* Move y up. */
    if (x->parent == tree->nil)
        /* x is the root. */
        tree->root = y;
    else if (x == x->parent->left)  /* Maintain the left and right. */
        x->parent->left = y;
    else
        x->parent->right = y;
    /* Finally, change x and y. */
    y->right = x;
    x->parent = y;
}

void insertFixUp(struct RBTree *tree, struct Node *current)
{
    while (current->parent->isRed) {
        /* The problem must occur when parent is red because the current must be red. */
        /* Find the current's parent's brother i.e. uncle. */
        if (current->parent == current->parent->parent->left) {
            struct Node *uncle = current->parent->parent->right;
            if (uncle->isRed) {
                /* Both current's parent and its uncle are red, so turn them black. */
                current->parent->isRed = false;
                uncle->isRed = false;

                /* Then turn current's grandparent red. */
                current->parent->parent->isRed = true;
                /* Make it the new current */
                current = current->parent->parent;
            }
            else {
                /* The uncle is black. */
                if (current == current->parent->right) {
                    current = current->parent;
                    leftRotation(tree, current);
                }
                current->parent->isRed = false;
                current->parent->parent->isRed = true;
                rightRotation(tree, current->parent->parent);
            }
        }
        else {
            struct Node *uncle = current->parent->parent->left;
            if (uncle->isRed) {
                /* Both current's parent and its uncle are red, so turn them black. */
                current->parent->isRed = false;
                uncle->isRed = false;

                /* Then turn current's grandparent red. */
                current->parent->parent->isRed = true;
                /* Make it the new current */
                current = current->parent->parent;
            }
            else {
                /* The uncle is black. */
                if (current == current->parent->left) {
                    current = current->parent;
                    rightRotation(tree, current);
                }
                current->parent->isRed = false;
                current->parent->parent->isRed = true;
                leftRotation(tree, current->parent->parent);
            }
        }
    }
    /* In case, it accidently was changed. */
    tree->root->isRed = false;
}

void insert(struct RBTree *tree, int key)
{
    if (search(tree, key) == tree->nil) {
        struct Node *z = (struct Node *)malloc(sizeof(struct Node)), *y = tree->nil, *x = tree->root;
        z->key = key;
        z->isRed = true;
        z->parent = NULL;
        z->left = tree->nil;
        z->right = tree->nil;

        while (x != tree->nil) {
            /* The tree isn't empty. */
            /* Keep y as x's parent. */
            y = x;
            /* Use x to find the position for z. */
            if (z->key < x->key) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        /* Insert z. */
        z->parent = y;
        if (y == tree->nil) {
            /* Tree was empty */
            tree->root = z;
        }
        else if (z->key < y->key) {
            y->left = z;
        }
        else {
            y->right = z;
        }

        insertFixUp(tree, z);
    }
}

void deleteFixUp(struct RBTree *tree, struct Node *current)
{
    while (current != tree->root && !current->isRed) {
        if (current == current->parent->left) {
            struct Node *sibling = current->parent->right;
            if (sibling->isRed) {
                sibling->isRed = false;
                current->parent->isRed = true;
                leftRotation(tree, current->parent);
                sibling = current->parent->right;
            }
            if (!sibling->left->isRed && !sibling->right->isRed) {
                sibling->isRed = true;
                current = current->parent;
            }
            else {
                if (!sibling->right->isRed){
                    sibling->left->isRed = false;
                    sibling->isRed = true;
                    rightRotation(tree, sibling);
                    sibling = current->parent->right;
                }
                sibling->isRed = current->parent->isRed;
                current->parent->isRed = false;
                sibling->right->isRed = false;
                leftRotation(tree, current->parent);
                current = tree->root;
            }
        }
        else {
            struct Node *sibling = current->parent->left;
            if (sibling->isRed) {
                sibling->isRed = false;
                current->parent->isRed = true;
                rightRotation(tree, current->parent);
                sibling = current->parent->left;
            }
            if (!sibling->left->isRed && !sibling->right->isRed) {
                sibling->isRed = true;
                current = current->parent;
            }
            else {
                if (!sibling->left->isRed){
                    sibling->right->isRed = false;
                    sibling->isRed = true;
                    leftRotation(tree, sibling);
                    sibling = current->parent->left;
                }
                sibling->isRed = current->parent->isRed;
                current->parent->isRed = false;
                sibling->left->isRed = false;
                rightRotation(tree, current->parent);
                current = tree->root;
            }
        }
    }
    current->isRed = false;
}

void delete(struct RBTree *tree, int key)
{
    struct Node *x = search(tree, key);
    if (x != tree->nil) {
        struct Node *y = NULL, *z = NULL;
        /* Make the number of y's children smaller or equals to 1. */
        if (x->left == tree->nil || x->right == tree->nil){
            /* If x has only one child, just delete x and let its child replace itself. */
            y = x;
        }
        else{
            /* x->right != tree->nil, so y must be the leftmost of x's right subtree. */
            /* If x has two child, overwrite x with its leftmost node in its right subtree, and delete it. */
            y = x->right;
            while (y->left != tree->nil) {
                y = y->left;
            }
        }
        /* Make z y's child. */
        if (y->left != tree->nil) {
            z = y->left;
        }
        else {
            /* z could be tree->nil. */
            z = y->right;
        }

        /* Replace y with z. */
        z->parent = y->parent;
        if (y->parent == tree->nil){
            /* y is root */
            tree->root = z;
        }
        else if (y == y->parent->left) {
            y->parent->left = z;
        }
        else{
            y->parent->right = z;
        }

        if (y != x)
            /* y = successor(x); */
            /* move y to x. */
            x->key = y->key;

        if (!y->isRed) {
            /* Delete black node. */
            deleteFixUp(tree, z);
        }
        /* Now y can be saftly removed. */
        free(y);
    }
}

int main(int argc, char* argv[])
{
    struct Node nil = {0, false, NULL, NULL, NULL};
    struct RBTree rbtree = {&nil, &nil};
    char *op;
    int key;
    op = malloc(8 * sizeof(char));
    while (true) {
        if (scanf("%s", op) != 1) return 1;
        if (!strncmp(op, "quit", 4))
            break;
        else if (!strncmp(op, "insert", 6)) {
            if (scanf("%d", &key) != 1) return 1;
            insert(&rbtree, key);
        }
        else if (!strncmp(op, "delete", 6)) {
            if (scanf("%d", &key) != 1) return 1;
            delete(&rbtree, key);
        }
        else if (!strncmp(op, "search", 6)) {
            if (scanf("%d", &key) != 1) return 1;
            struct Node *result = search(&rbtree, key);
            if (result == &nil) printf("Not found\n");
            else {
                if (result->isRed)  printf("red\n");
                else  printf("black\n");
            }
        }
        else {
            printf("Invalid instruction. Please try again, or enter 'quit' to exit.\n");
        }
    }
    free(op);
    return 0;
}
