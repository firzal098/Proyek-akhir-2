#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

#include <iostream>
#include <stdexcept>

using namespace std;

// Struktur Node (Simpul) untuk menampung data
template <typename T>
struct Node {
    T data;
    Node* next;
    Node* prev;

    Node(T value) : data(value), next(nullptr), prev(nullptr) {}
};

template <typename T>
class DoublyLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    size_t size;
    bool isCircular; // Penanda apakah list ini Circular atau tidak

    // Fungsi pembantu untuk menghubungkan ujung-ujung jika Circular
    void perbaruiLinkCircular() {
        if (isCircular && head != nullptr && tail != nullptr) {
            tail->next = head;
            head->prev = tail;
        } else if (!isCircular && head != nullptr && tail != nullptr) {
            tail->next = nullptr;
            head->prev = nullptr;
        }
    }

public:
    // --- Konstruktor & Destruktor ---

    // Konstruktor: Parameter 'circular' menentukan mode (default: false/Linear)
    explicit DoublyLinkedList(bool circular = false) {
        head = nullptr;
        tail = nullptr;
        size = 0;
        isCircular = circular;
    }

    // Destruktor
    ~DoublyLinkedList() {
        clear();
    }

    // --- Operasi Penambahan (Insertion) ---

    // Menambah data di belakang (Ekor)
    void push_back(const T& value) {
        Node<T>* newNode = new Node<T>(value);

        if (empty()) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }

        // Jika circular, sambungkan kembali ekor ke kepala
        perbaruiLinkCircular();
        size++;
    }

    // Menambah data di depan (Kepala)
    void push_front(const T& value) {
        Node<T>* newNode = new Node<T>(value);

        if (empty()) {
            head = tail = newNode;
        } else {
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
        }

        // Jika circular, sambungkan kembali ekor ke kepala
        perbaruiLinkCircular();
        size++;
    }

    // --- Operasi Penghapusan (Deletion) ---

    // Menghapus data paling belakang
    void pop_back() {
        if (empty()) return; // Atau lempar exception

        Node<T>* temp = tail;

        if (head == tail) { // Hanya ada 1 elemen
            head = tail = nullptr;
        } else {
            tail = tail->prev;
            // Jika BUKAN circular, tail->next harus nullptr. 
            // Jika circular, akan diurus oleh perbaruiLinkCircular()
            if (!isCircular) tail->next = nullptr;
        }

        delete temp;
        perbaruiLinkCircular(); // Pastikan link benar setelah penghapusan
        size--;
    }

    // Menghapus data paling depan
    void pop_front() {
        if (empty()) return;

        Node<T>* temp = head;

        if (head == tail) { // Hanya ada 1 elemen
            head = tail = nullptr;
        } else {
            head = head->next;
            // Jika BUKAN circular, head->prev harus nullptr
            if (!isCircular) head->prev = nullptr;
        }

        delete temp;
        perbaruiLinkCircular();
        size--;
    }

    // --- Utilitas ---

    // Menghapus semua elemen
    void clear() {
        if (empty()) return;

        // Jika Circular, putuskan dulu lingkarannya agar tidak infinite loop saat delete
        if (isCircular && tail != nullptr) {
            tail->next = nullptr;
        }

        Node<T>* current = head;
        while (current != nullptr) {
            Node<T>* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    // Menampilkan isi list
    void print() const {
        if (empty()) {
            cout << "[ Kosong ]" << endl;
            return;
        }

        Node<T>* current = head;
        cout << "[ ";
        
        // Logika loop khusus untuk menangani circular dan non-circular sekaligus
        if (isCircular) {
            do {
                cout << current->data << " ";
                current = current->next;
            } while (current != head); // Berhenti jika kembali ke kepala
        } else {
            while (current != nullptr) {
                cout << current->data << " ";
                current = current->next;
            }
        }
        
        cout << "]" << endl;
    }

    // Menampilkan list secara terbalik (dari tail ke head)
    // Berguna untuk mengecek integritas pointer 'prev'
    void printReverse() const {
        if (empty()) {
            cout << "[ Kosong ]" << endl;
            return;
        }

        Node<T>* current = tail;
        cout << "[ (Mundur) ";

        if (isCircular) {
            do {
                cout << current->data << " ";
                current = current->prev;
            } while (current != tail); // Berhenti jika kembali ke ekor
        } else {
            while (current != nullptr) {
                cout << current->data << " ";
                current = current->prev;
            }
        }

        cout << "]" << endl;
    }

    // Akses Data
    T& front() {
        if (empty()) throw out_of_range("List kosong");
        return head->data;
    }

    T& back() {
        if (empty()) throw out_of_range("List kosong");
        return tail->data;
    }

    bool empty() const { return size == 0; }
    size_t getSize() const { return size; }
};

#endif // DOUBLY_LINKED_LIST_HPP