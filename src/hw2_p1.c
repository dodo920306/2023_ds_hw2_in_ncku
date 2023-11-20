#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

struct Node {
    int key; /* Added when inserting */
    int val; /* Added when inserting */
    int degree; /* Added when consolidating */
    bool mark; /* Added when decreasing key */
    struct Node *left; /* Added when inserting */
    struct Node *right; /* Added when inserting */
    struct Node *child; /* Added when extracting */
    struct Node *parent; /* Added when decreasing key */
};

void insertion(int key, int val);
void extract();
void link(struct Node *small, struct Node *big);
void consolidate();

struct Node *mini = NULL;

int no_of_nodes = 0;

void insert(int key, int val)
{
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    new_node->key = key;
    new_node->val = val;
    new_node->degree = 0;
    new_node->mark = false;
    new_node->left = new_node;
    new_node->right = new_node;    /* Form a single node doubly linked list first. */
    new_node->child = NULL;
    new_node->parent = NULL;
    if (mini) {
        (mini->left)->right = new_node;    /* Insert the new node as the left of the mini node. */
        new_node->right = mini;
        new_node->left = mini->left;
        mini->left = new_node;
        if (new_node->key < mini->key)
            mini = new_node; /* Update the minimum. */
    }
    else {
        mini = new_node;
    }

    no_of_nodes++;
}

void link(struct Node *small, struct Node *big)
{
    /* delete big */
    big->left->right = big->right;
    big->right->left = big->left;

    if (!small->child) {
        small->child = big;
        big->right = big;
        big->left = big;
    }
    else {
        /* Insert big as small's children. */
        big->right = small->child;
        big->left = small->child->left;
        small->child->left->right = big;
        small->child->left = big;
    }
    big->parent = small;
    small->degree++;
}

void consolidate()
{
    int length = 0, x = no_of_nodes;
    /* log2 */
    while (x > 1) {
        x /= 2;
        length++;
    }
    length++; /* Just for sure. */
    struct Node *pntr = mini, *temp = mini;
    struct Node ** degrees = (struct Node **)malloc(length * sizeof(struct Node *));
    for (int i = 0; i < length; i++)
        degrees[i] = NULL;
    do {
        pntr = temp->right;
        while (degrees[temp->degree]) {
            int degree = temp->degree;
            if (temp->key > degrees[temp->degree]->key) {
                link(degrees[temp->degree], temp);
                temp = degrees[temp->degree];
            }
            else
                link(temp, degrees[temp->degree]);
            degrees[degree] = NULL;
        }
        temp->left = temp;
        temp->right = temp;
        degrees[temp->degree] = temp;
        temp = pntr;
    } while (pntr != mini);

    /* Find new mini */
    int i = 0;
    while (!degrees[i]) i++;
    mini = degrees[i];
    for (; i < length; i++) {
        /* rebuild the connection */
        if (degrees[i]) {
            /* Insert */
            degrees[i]->right = mini;
            degrees[i]->left = mini->left;
            mini->left->right = degrees[i];
            mini->left = degrees[i];
            mini = degrees[i]->key < mini->key ? degrees[i] : mini;
        }
    }
    free(degrees);
}

void extract()
{
    if (no_of_nodes) {
        printf("(%d)%d\n", mini->key, mini->val);
        if (no_of_nodes == 1) {
            /* Only one node in the list and it's mini, and it has no child. */
            free(mini);
            mini = NULL;
            /* Back to no node. */
        }
        else {
            struct Node *pntr = NULL, *temp = mini;
            if (mini->child) {
                /* temp is children iterator of mini's children. */
                temp = mini->child;
                do {
                    /* pntr is the position of the next child to be deleted. */
                    pntr = temp->right;
                    /* insert temp */
                    (mini->left)->right = temp;    /* Insert the child as the left of the mini node. */
                    temp->right = mini;
                    temp->left = mini->left;
                    mini->left = temp;
                    /* Update the temp. */
                    temp = pntr;
                } while (pntr != mini->child); /* Back to the beginning (mini->child). */
            }
            /* mini won't alter by the insertion since it's a min heap. */
            /* Delete mini */
            (mini->left)->right = mini->right;
            (mini->right)->left = mini->left;
            free(mini);
            /* Move mini to right to be the start point of consolidation. */
            mini = temp->right;
            consolidate();
        }
        no_of_nodes--;
    }
}

struct Node *find(struct Node *mini, int key, int val)
{
    struct Node *pntr = mini, *temp = NULL;
    do {
        if (pntr->key == key && pntr->val == val) {
            return pntr;
        }
        else if (pntr->key <= key && pntr->child) {
            temp = find(pntr->child, key, val);
            if (temp)   break;
        }
        pntr = pntr->right;
    } while (pntr != mini);
    return temp;
}

void cut(struct Node *pntr)
{
    if (pntr->parent->degree-- == 1)
        pntr->parent->child = NULL;
    else {
        pntr->right->left = pntr->left;
        pntr->left->right = pntr->right;
        pntr->parent->child = pntr->right;
    }
    pntr->left = mini->left;
    pntr->right = mini;
    mini->left->right = pntr;
    mini->left = pntr;
    pntr->mark = false;
    pntr->parent = NULL;
}

void cascading_cut(struct Node *pntr)
{
    while (pntr->parent) {
        if (!pntr->mark) {
            pntr->mark = true;
            break;
        }
        else {
            cut(pntr);
        }
        pntr = pntr->parent;
    }
}

void decrease_key(struct Node *pntr, int delta)
{
    struct Node *temp = pntr->parent;
    pntr->key -= delta;
    if (temp && pntr->key < temp->key) {
        cut(pntr);
        cascading_cut(temp);
    }
    if (pntr->key < mini->key)
        mini = pntr;
}

void delete(struct Node *pntr)
{
    struct Node *temp = pntr->parent;
    if (temp) {
        cut(pntr);
        cascading_cut(temp);
    }
    (pntr->left)->right = pntr->right;
    (pntr->right)->left = pntr->left;
    no_of_nodes--;
    if (mini == pntr) {
        mini = pntr->right;
        struct Node *temp = mini->right;
        while  (temp != mini) {
            mini = temp->key < mini->key ? temp : mini;
            temp = temp->right;
        }
    }
    free(pntr);
}

int main(int argc, char* argv[])
{
    char *op;
    int key, val, delta;
    op = malloc(8 * sizeof(char));
    while (true) {
        if (scanf("%s", op) != 1) return 1;
        if (!strncmp(op, "quit", 4)) {
            free(op);
            break;
        }
        else if (!strncmp(op, "insert", 6)) {
            if (scanf("%d %d", &key, &val) != 2) return 2;
            insert(key, val);
        }
        else if (!strncmp(op, "delete", 6)) {
            if (scanf("%d %d", &key, &val) != 2) return 2;
            struct Node *temp = find(mini, key, val);
            if (temp)
                delete(temp);
        }
        else if (!strncmp(op, "decrease", 8)) {
            if (scanf("%d %d %d", &key, &val, &delta) != 3) return 3;
            struct Node *temp = find(mini, key, val);
            if (temp)
                decrease_key(temp, delta);
        }
        else if (!strncmp(op, "extract", 7)) {
            extract();
        }
    }
    return 0;
}
