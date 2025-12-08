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
    size_t m_size;
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
        m_size = 0;
        isCircular = circular;
    }

    // Destruktor
    ~DoublyLinkedList() {
        clear();
    }

    // --- Rule of Three: Copy Constructor ---
    DoublyLinkedList(const DoublyLinkedList& other) {
        // Inisialisasi state awal sama seperti konstruktor default
        head = nullptr;
        tail = nullptr;
        m_size = 0;
        isCircular = other.isCircular;

        // Lakukan deep copy dengan menyalin setiap elemen
        for (ConstIterator it = other.begin(); it != other.end(); ++it) {
            push_back(*it);
        }
    }

    // --- Rule of Three: Copy Assignment Operator ---
    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
        // 1. Lindungi dari self-assignment (cth: list1 = list1)
        if (this != &other) {
            // 2. Hapus semua data yang ada di list saat ini
            clear();

            // 3. Inisialisasi ulang state (jika diperlukan)
            isCircular = other.isCircular;
            head = tail = nullptr; // clear() sudah melakukannya, tapi ini eksplisit
            m_size = 0;
            
            // 4. Lakukan deep copy elemen dari 'other'
            for (ConstIterator it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
        }
        // 5. Kembalikan reference ke objek saat ini untuk chaining (cth: a = b = c)
        return *this;
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
        m_size++;
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
        m_size++;
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
        m_size--;
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
        m_size--;
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
        m_size = 0;
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

    // --- Iterator Support ---
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(Node<T>* ptr) : m_ptr(ptr) {}

        reference operator*() const { return m_ptr->data; }
        pointer operator->() { return &(m_ptr->data); }

        Iterator& operator++() { m_ptr = m_ptr->next; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        Node<T>* m_ptr;
    };

    class ConstIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        ConstIterator(const Node<T>* ptr) : m_ptr(ptr) {}

        reference operator*() const { return m_ptr->data; }
        pointer operator->() const { return &(m_ptr->data); }

        ConstIterator& operator++() { m_ptr = m_ptr->next; return *this; }
        ConstIterator operator++(int) { ConstIterator tmp = *this; ++(*this); return tmp; }

        friend bool operator==(const ConstIterator& a, const ConstIterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!=(const ConstIterator& a, const ConstIterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        const Node<T>* m_ptr;
    };

    // Non-const iterators
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }

    // Const iterators
    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }
    ConstIterator cbegin() const { return ConstIterator(head); }
    ConstIterator cend() const { return ConstIterator(nullptr); }


    // --- Akses Data ---
    T& front() {
        if (empty()) throw out_of_range("List kosong");
        return head->data;
    }

    T& back() {
        if (empty()) throw out_of_range("List kosong");
        return tail->data;
    }
    
    T& operator[](size_t index) {
        if (index >= m_size) {
            throw out_of_range("Index di luar jangkauan");
        }
        Node<T>* current = head;
        for (size_t i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    }

    bool empty() const { return m_size == 0; }
    size_t size() const { return m_size; }
};

#endif // DOUBLY_LINKED_LIST_HPP