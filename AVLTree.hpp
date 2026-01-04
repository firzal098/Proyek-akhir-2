#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include <iostream>
#include "DoublyLinkedList.hpp" // Include file DoublyLinkedList

using namespace std;

// Struktur untuk Node Tree menggunakan Template
// T adalah tipe data untuk Value (bisa struct, class, string, dll)
template <typename T>
struct Node {
    int key;       // Key tetap integer untuk pengurutan
    T value;       // Value menyimpan objek/data
    Node<T>* left;
    Node<T>* right;
    int height;
};

template <typename T>
class AVLTree {
private:
    Node<T>* root;

    // Fungsi utilitas untuk mendapatkan tinggi node
    int getHeight(Node<T>* N) {
        if (N == NULL)
            return 0;
        return N->height;
    }

    // Fungsi utilitas manual untuk max
    int max(int a, int b) {
        return (a > b) ? a : b;
    }

    // Membuat node baru dengan Key dan Value
    Node<T>* newNode(int key, T value) {
        Node<T>* node = new Node<T>();
        node->key = key;
        node->value = value;
        node->left = NULL;
        node->right = NULL;
        node->height = 1;
        return node;
    }

    // Rotasi Kanan
    Node<T>* rightRotate(Node<T>* y) {
        Node<T>* x = y->left;
        Node<T>* T2 = x->right;

        x->right = y;
        y->left = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    // Rotasi Kiri
    Node<T>* leftRotate(Node<T>* x) {
        Node<T>* y = x->right;
        Node<T>* T2 = y->left;

        y->left = x;
        x->right = T2;

        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

        return y;
    }

    // Get Balance Factor
    int getBalance(Node<T>* N) {
        if (N == NULL)
            return 0;
        return getHeight(N->left) - getHeight(N->right);
    }

    // Insert dengan Key dan Value
    Node<T>* insertNode(Node<T>* node, int key, T value) {
        // 1. Standar BST insert
        if (node == NULL)
            return newNode(key, value);

        if (key < node->key)
            node->left = insertNode(node->left, key, value);
        else if (key > node->key)
            node->right = insertNode(node->right, key, value);
        else {
            // Jika key sama, update valuenya dengan data baru
            node->value = value; 
            return node;
        }

        // 2. Update tinggi
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        // 3. Cek balance
        int balance = getBalance(node);

        // 4. Rebalancing (Rotasi)
        
        // Left Left Case
        if (balance > 1 && key < node->left->key)
            return rightRotate(node);

        // Right Right Case
        if (balance < -1 && key > node->right->key)
            return leftRotate(node);

        // Left Right Case
        if (balance > 1 && key > node->left->key) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Left Case
        if (balance < -1 && key < node->right->key) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    // Cari node terkecil untuk keperluan delete
    Node<T>* minValueNode(Node<T>* node) {
        Node<T>* current = node;
        while (current->left != NULL)
            current = current->left;
        return current;
    }

    // Delete node
    Node<T>* deleteNode(Node<T>* root, int key) {
        if (root == NULL)
            return root;

        if (key < root->key)
            root->left = deleteNode(root->left, key);
        else if (key > root->key)
            root->right = deleteNode(root->right, key);
        else {
            // Node ditemukan
            if ((root->left == NULL) || (root->right == NULL)) {
                Node<T>* temp = root->left ? root->left : root->right;

                if (temp == NULL) {
                    temp = root;
                    root = NULL;
                }
                else
                    *root = *temp; 
                
                delete temp;
            }
            else {
                // Node dengan 2 anak
                Node<T>* temp = minValueNode(root->right);

                // Salin KEY dan VALUE dari successor
                root->key = temp->key;
                root->value = temp->value; 

                root->right = deleteNode(root->right, temp->key);
            }
        }

        if (root == NULL)
            return root;

        root->height = 1 + max(getHeight(root->left), getHeight(root->right));
        int balance = getBalance(root);

        // Rebalancing setelah delete
        if (balance > 1 && getBalance(root->left) >= 0)
            return rightRotate(root);

        if (balance > 1 && getBalance(root->left) < 0) {
            root->left = leftRotate(root->left);
            return rightRotate(root);
        }

        if (balance < -1 && getBalance(root->right) <= 0)
            return leftRotate(root);

        if (balance < -1 && getBalance(root->right) > 0) {
            root->right = rightRotate(root->right);
            return leftRotate(root);
        }

        return root;
    }

    // Fungsi helper rekursif untuk mengisi list
    void inorderToList(Node<T>* root, DoublyLinkedList<T>& list) {
        if (root != NULL) {
            inorderToList(root->left, list);
            list.push_back(root->value); // Masukkan value ke list
            inorderToList(root->right, list);
        }
    }

    void inorder(Node<T>* root) {
        if (root != NULL) {
            inorder(root->left);
            cout << root->key << " ";
            inorder(root->right);
        }
    }

    void preorder(Node<T>* root) {
        if (root != NULL) {
            cout << root->key << " ";
            preorder(root->left);
            preorder(root->right);
        }
    }

    // Helper internal untuk pencarian
    Node<T>* findNode(Node<T>* node, int key) {
        if (node == NULL || node->key == key)
            return node;
        
        if (key < node->key)
            return findNode(node->left, key);
            
        return findNode(node->right, key);
    }

public:
    AVLTree() {
        root = NULL;
    }

    // Insert sekarang menerima key (int) dan value (T)
    void insert(int key, T value) {
        root = insertNode(root, key, value);
    }

    void remove(int key) {
        root = deleteNode(root, key);
    }

    // Fungsi baru: Mencari value berdasarkan key
    bool search(int key, T& result) {
        Node<T>* node = findNode(root, key);
        if (node != NULL) {
            result = node->value;
            return true;
        }
        return false;
    }

    // Fungsi baru: Mengembalikan semua value dalam bentuk DoublyLinkedList terurut (Inorder)
    DoublyLinkedList<T> getInOrderList() {
        DoublyLinkedList<T> list;
        inorderToList(root, list);
        return list;
    }

    void displayInOrder() {
        cout << "Inorder (Keys): ";
        inorder(root);
        cout << endl;
    }

    void displayPreOrder() {
        cout << "Preorder (Keys): ";
        preorder(root);
        cout << endl;
    }
};

#endif // AVL_TREE_HPP