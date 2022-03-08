#include <stdio.h>
#include "HEAP.h"

typedef struct Node Node;
struct Node{
    int data;
    Node *right;
    Node *left;
};

Node *createNewNode(int value) {
    Node *temp = heap_alloc(sizeof(Node));
    temp->data = value;
    temp->left = temp->right = NULL;
    return temp;
}

Node *insert_node(Node *root, int value)
{
    if (!root) {
        // Insert the first node, if root is NULL.
        return createNewNode(value);
    }

    // Insert data.
    if (value > root->data) {
        // Insert right node data, if the 'value'
        // to be inserted is greater than 'root' node data.

        // Process right nodes.
        root->right = insert_node(root->right, value);
    }
    else {
        // Insert left node data, if the 'value'
        // to be inserted is greater than 'root' node data.

        // Process left nodes.
        root->left = insert_node(root->left, value);
    }

    // Return 'root' node, after insertion.
    return root;
}

void inorder(Node *root) {
    // cout << "Inorder Traversal Started[+]" << endl;
    if (root == NULL) return;

    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

// { Driver Code Starts.

int main() {
    Node *root = createNewNode(10);
    int n;
    printf("Enter the number of Elements: ");
    scanf("%d", &n);
    int key;
    while(n--)
    {
        scanf("%d", &key);
        insert_node(root, key);
    }
    inorder(root);

    printf("\n ALLOCED_PTR \n ---------------------------------------------- \n");
    console_alloced();
    printf("\n DEALLOCED_PTR \n ---------------------------------------------- \n");
    console_dealloced();

    return 0;
}























