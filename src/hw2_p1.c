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

struct FHeap {
    struct Node *mini;
    int no_of_nodes;
};

void insertion(struct FHeap *fheap, int key, int val);
void link(struct Node *small, struct Node *big);
void consolidate(struct FHeap *fheap);
void extract(struct FHeap *fheap);
struct Node *find(struct Node *mini, int key, int val);
void cut(struct FHeap *fheap, struct Node *pntr);
void cascading_cut(struct FHeap *fheap, struct Node *pntr);
void decrease_key(struct FHeap *fheap, struct Node *pntr, int delta);
void delete(struct FHeap *fheap, struct Node *pntr);


void insert(struct FHeap *fheap, int key, int val)
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
    if (fheap->mini) {
        (fheap->mini->left)->right = new_node;    /* Insert the new node as the left of the mini node. */
        new_node->right = fheap->mini;
        new_node->left = fheap->mini->left;
        fheap->mini->left = new_node;
        if (new_node->key < fheap->mini->key)
            fheap->mini = new_node; /* Update the minimum. */
    }
    else {
        fheap->mini = new_node;
    }

    fheap->no_of_nodes++;
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

void consolidate(struct FHeap *fheap)
{
    int length = 0, x = fheap->no_of_nodes;
    /* log2 */
    while (x > 1) {
        x /= 2;
        length++;
    }
    length++; /* Just for sure. */
    struct Node *pntr = fheap->mini, *temp = fheap->mini;
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
    } while (pntr != fheap->mini);

    /* Find new mini */
    int i = 0;
    while (!degrees[i]) i++;
    fheap->mini = degrees[i];
    for (; i < length; i++) {
        /* rebuild the connection */
        if (degrees[i]) {
            /* Insert */
            degrees[i]->right = fheap->mini;
            degrees[i]->left = fheap->mini->left;
            fheap->mini->left->right = degrees[i];
            fheap->mini->left = degrees[i];
            fheap->mini = degrees[i]->key < fheap->mini->key ? degrees[i] : fheap->mini;
        }
    }
    free(degrees);
}

void extract(struct FHeap *fheap)
{
    if (fheap->no_of_nodes) {
        printf("(%d)%d\n", fheap->mini->key, fheap->mini->val);
        if (fheap->no_of_nodes == 1) {
            /* Only one node in the list and it's mini, and it has no child. */
            free(fheap->mini);
            fheap->mini = NULL;
            /* Back to no node. */
        }
        else {
            struct Node *pntr = NULL, *temp = fheap->mini;
            if (fheap->mini->child) {
                /* temp is children iterator of mini's children. */
                temp = fheap->mini->child;
                do {
                    /* pntr is the position of the next child to be deleted. */
                    pntr = temp->right;
                    /* insert temp */
                    (fheap->mini->left)->right = temp;    /* Insert the child as the left of the mini node. */
                    temp->right = fheap->mini;
                    temp->left = fheap->mini->left;
                    fheap->mini->left = temp;
                    /* Update the temp. */
                    temp = pntr;
                } while (pntr != fheap->mini->child); /* Back to the beginning (mini->child). */
            }
            /* mini won't alter by the insertion since it's a min heap. */
            /* Delete mini */
            (fheap->mini->left)->right = fheap->mini->right;
            (fheap->mini->right)->left = fheap->mini->left;
            free(fheap->mini);
            /* Move mini to right to be the start point of consolidation. */
            fheap->mini = temp->right;
            consolidate(fheap);
        }
        fheap->no_of_nodes--;
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

void cut(struct FHeap *fheap, struct Node *pntr)
{
    if (pntr->parent->degree-- == 1)
        pntr->parent->child = NULL;
    else {
        pntr->right->left = pntr->left;
        pntr->left->right = pntr->right;
        pntr->parent->child = pntr->right;
    }
    pntr->left = fheap->mini->left;
    pntr->right = fheap->mini;
    fheap->mini->left->right = pntr;
    fheap->mini->left = pntr;
    pntr->mark = false;
    pntr->parent = NULL;
}

void cascading_cut(struct FHeap *fheap, struct Node *pntr)
{
    while (pntr->parent) {
        if (!pntr->mark) {
            pntr->mark = true;
            break;
        }
        else {
            cut(fheap, pntr);
        }
        pntr = pntr->parent;
    }
}

void decrease_key(struct FHeap *fheap, struct Node *pntr, int delta)
{
    struct Node *temp = pntr->parent;
    pntr->key -= delta;
    if (temp && pntr->key < temp->key) {
        cut(fheap, pntr);
        cascading_cut(fheap, temp);
    }
    if (pntr->key < fheap->mini->key)
        fheap->mini = pntr;
}

void delete(struct FHeap *fheap, struct Node *pntr)
{
    struct Node *temp = pntr->parent;
    if (temp) {
        cut(fheap, pntr);
        cascading_cut(fheap, temp);
    }
    (pntr->left)->right = pntr->right;
    (pntr->right)->left = pntr->left;
    fheap->no_of_nodes--;
    if (fheap->mini == pntr) {
        fheap->mini = pntr->right;
        struct Node *temp = fheap->mini->right;
        while  (temp != fheap->mini) {
            fheap->mini = temp->key < fheap->mini->key ? temp : fheap->mini;
            temp = temp->right;
        }
    }
    free(pntr);
}

int main(int argc, char* argv[])
{
    struct FHeap fheap = {NULL, 0};
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
            insert(&fheap, key, val);
        }
        else if (!strncmp(op, "delete", 6)) {
            if (scanf("%d %d", &key, &val) != 2) return 2;
            struct Node *temp = find(fheap.mini, key, val);
            if (temp)
                delete(&fheap, temp);
        }
        else if (!strncmp(op, "decrease", 8)) {
            if (scanf("%d %d %d", &key, &val, &delta) != 3) return 3;
            struct Node *temp = find(fheap.mini, key, val);
            if (temp)
                decrease_key(&fheap, temp, delta);
        }
        else if (!strncmp(op, "extract", 7)) {
            extract(&fheap);
        }
    }
    return 0;
}
