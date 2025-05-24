#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct block {
    size_t size;
    struct block *l, *r;
} block_t;

block_t **find_free_tree(block_t **root, block_t *target)
{
    if (!root)
        return NULL;

    block_t **node = root;
    while (*node) {
        if ((*node)->size == target->size)
            return node;
        else if ((*node)->size < target->size)
            node = &(*node)->r;
        else
            node = &(*node)->l;
    }

    return NULL;
}

block_t *find_predecessor_free_tree(block_t **root, block_t *node)
{
    if (!root || !*root || node)
        return NULL;
    
    if (node->l)
    {
        block_t *pred = node->l;
        while (pred->r)
            pred = pred->r;
        return pred;
    }

    block_t *pred = NULL;
    block_t *cur = *root;

    while (cur)
    {
        if (cur->size < node->size)
        {
            pred = cur;
            cur = cur->r;
        }
        else
            cur = cur->l; // NULL
    }
    return pred;
}

/*
 * Structure representing a free memory block in the memory allocator.
 * The free tree is a binary search tree that organizes free blocks (of type block_t)
 * to efficiently locate a block of appropriate size during memory allocation.
 */
void remove_free_tree(block_t **root, block_t *target)
{
    /* Locate the pointer to the target node in the tree. */
    block_t **node_ptr = find_free_tree(root, target);

    /* If the target node has two children, we need to find a replacement. */
    if ((*node_ptr)->l && (*node_ptr)->r) {
        /* Find the in-order predecessor:
         * This is the rightmost node in the left subtree.
         */
        block_t **pred_ptr = &(*node_ptr)->l;
        while ((*pred_ptr)->r)
            pred_ptr = &(*pred_ptr)->r;

        /* Verify the found predecessor using a helper function (for debugging).
         */
        block_t *expected_pred = find_predecessor_free_tree(root, *node_ptr);
        assert(expected_pred == *pred_ptr);

        /* If the predecessor is the immediate left child. */
        if (*pred_ptr == (*node_ptr)->l) {
            block_t *old_right = (*node_ptr)->r;
            *node_ptr = *pred_ptr; /* Replace target with its left child. */
            (*node_ptr)->r = old_right; /* Attach the original right subtree. */
            assert(*node_ptr != (*node_ptr)->l);
            assert(*node_ptr != (*node_ptr)->r);
        } else {
            /* The predecessor is deeper in the left subtree. */
            block_t *old_left = (*node_ptr)->l;
            block_t *old_right = (*node_ptr)->r;
            block_t *pred_node = *pred_ptr;
            /* Remove the predecessor from its original location. */
            remove_free_tree(&old_left, *pred_ptr);
            /* Replace the target node with the predecessor. */
            *node_ptr = pred_node;
            (*node_ptr)->l = old_left;
            (*node_ptr)->r = old_right;
            assert(*node_ptr != (*node_ptr)->l);
            assert(*node_ptr != (*node_ptr)->r);
        }
    }
    /* If the target node has one child (or none), simply splice it out. */
    else if ((*node_ptr)->l || (*node_ptr)->r) {
        block_t *child = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
        *node_ptr = child;
    } else {
        /* No children: remove the node. */
        *node_ptr = NULL;
    }

    /* Clear the removed node's child pointers to avoid dangling references. */
    target->l = NULL;
    target->r = NULL;
}

block_t *createNode(int value) {
    block_t* newNode = (block_t*)malloc(sizeof(block_t));
    newNode->size = value;
    newNode->l = newNode->r = NULL;
    return newNode;
}

block_t *insert(block_t *root, block_t *node) {
    if (root == NULL) {
        return node;
    }

    if (node->size < root->size) {
        // Insert in left subtree
        root->l = insert(root->l, node);
    } else if (node->size > root->size) {
        // Insert in r subtree
        root->r = insert(root->r, node);
    }
    // Duplicate values are not inserted
    return root;
}

void print_tree_inorder(block_t *root)
{
    if (!root)
        return;
    print_tree_inorder(root->l);
    printf("%ld ", root->size);
    print_tree_inorder(root->r);

    return;
}

int main()
{
    block_t *root = NULL;
    block_t blocks[10];
    for (int i = 0; i < 10; i++) {
        blocks[i].size = (i + 1) * 10;
        blocks[i].l = NULL;
        blocks[i].r = NULL;
        printf("%d: %ld ", i, blocks[i].size);

        root = insert(root, &blocks[i]);
    }
    printf("\n");

    print_tree_inorder(root);
    printf("\n");

    for (int i = 0; i < 10; i++) {
        printf("remove free tree size: %ld\n", blocks[i].size);
        remove_free_tree(&root, &blocks[i]);

        print_tree_inorder(root);
        printf("\n");
    }

    return 0;
}
