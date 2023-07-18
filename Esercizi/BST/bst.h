#ifndef BST_H
#define BST_H

// Definizione della struttura del nodo BST
typedef struct BST
{
    int data;
    struct BST *left;
    struct BST *right;
} BST;

// Funzione per creare un nuovo nodo
BST *createNode(int data)
{
    BST *newNode = (BST *)malloc(sizeof(BST));
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

// Funzione per l'inserimento di un nodo nell'albero BST
BST *insertNode(BST *root, int data)
{
    // Se l'albero è vuoto, crea un nuovo nodo e restituiscilo
    if (root == NULL)
    {
        return createNode(data);
    }

    // Altrimenti, inserisci il nodo nel sottoalbero corretto
    if (data < root->data)
    {
        root->left = insertNode(root->left, data);
    }
    else if (data > root->data)
    {
        root->right = insertNode(root->right, data);
    }

    return root;
}

// Funzione di utilità per stampare l'albero in ordine crescente
void printInorder(BST *node)
{
    if (node == NULL)
    {
        return;
    }

    printInorder(node->left);
    printf("%d ", node->data);
    printInorder(node->right);
}

#endif
