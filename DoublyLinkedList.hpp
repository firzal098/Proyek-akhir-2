#ifndef DOUBLY_LINKED_LIST_HPP
#define DOUBLY_LINKED_LIST_HPP

#include <iostream>
#include <stdexcept>

using namespace std;

/**
 * @file DoublyLinkedList.hpp
 * @brief Implementasi struktur data Doubly Linked List generik.
 *
 * File ini berisi implementasi lengkap dari Doubly Linked List dengan fungsionalitas
 * untuk mode linear dan circular. Mendukung operasi dasar seperti penambahan,
 * penghapusan, akses, dan iterasi.
 */

#include <iostream>
#include <stdexcept>

using namespace std;

/**
 * @struct Node
 * @brief Template untuk simpul (node) dalam Doubly Linked List.
 * @tparam T Tipe data yang akan disimpan dalam node.
 *
 * Setiap node menyimpan sebuah data, serta pointer ke node berikutnya (`next`)
 * dan node sebelumnya (`prev`).
 */
template <typename T>
struct Node {
    T data;      // Data yang disimpan
    Node* next;  // Pointer ke node selanjutnya
    Node* prev;  // Pointer ke node sebelumnya

    /**
     * @brief Konstruktor untuk Node.
     * @param value Nilai data yang akan disimpan.
     */
    Node(T value) : data(value), next(nullptr), prev(nullptr) {}
};

/**
 * @class DoublyLinkedList
 * @brief Kelas implementasi Doubly Linked List.
 * @tparam T Tipe data dari elemen yang akan disimpan dalam list.
 *
 * Kelas ini menyediakan fungsionalitas untuk mengelola kumpulan data
 * dalam bentuk Doubly Linked List, baik linear maupun circular.
 */
template <typename T>
class DoublyLinkedList {
private:
    Node<T>* head;      // Pointer ke node pertama (kepala)
    Node<T>* tail;      // Pointer ke node terakhir (ekor)
    size_t m_size;      // Jumlah elemen dalam list
    bool isCircular;    // Flag untuk menandai apakah list bersifat circular

    /**
     * @brief Fungsi helper untuk memperbarui koneksi pada list circular.
     * Jika list diatur sebagai circular, fungsi ini akan menyambungkan
     * ekor (tail) ke kepala (head) dan sebaliknya.
     */
    void perbaruiLinkCircular() {
        if (isCircular && head != nullptr && tail != nullptr) {
            tail->next = head;
            head->prev = tail;
        } else if (!isCircular && head != nullptr && tail != nullptr) {
            // Memastikan list linear tidak terhubung secara circular
            tail->next = nullptr;
            head->prev = nullptr;
        }
    }

public:
    // --- Konstruktor & Destruktor ---

    /**
     * @brief Konstruktor utama DoublyLinkedList.
     * @param circular Flag untuk menentukan apakah list akan bersifat circular (true) atau linear (false). Defaultnya adalah linear.
     */
    explicit DoublyLinkedList(bool circular = false) {
        head = nullptr;
        tail = nullptr;
        m_size = 0;
        isCircular = circular;
    }

    /**
     * @brief Destruktor.
     * Akan memanggil fungsi `clear()` untuk membebaskan semua memori yang dialokasikan
     * untuk node-node dalam list.
     */
    ~DoublyLinkedList() {
        clear();
    }

    // --- Aturan Tiga (Rule of Three) untuk Manajemen Memori ---

    /**
     * @brief Copy Constructor.
     * Membuat salinan (deep copy) dari DoublyLinkedList lain.
     * @param other List lain yang akan disalin.
     */
    DoublyLinkedList(const DoublyLinkedList& other) {
        // Inisialisasi state awal sama seperti konstruktor default
        head = nullptr;
        tail = nullptr;
        m_size = 0;
        isCircular = other.isCircular;

        // Lakukan deep copy dengan menyalin setiap elemen dari 'other'
        for (ConstIterator it = other.begin(); it != other.end(); ++it) {
            push_back(*it);
        }
    }

    /**
     * @brief Copy Assignment Operator.
     * Menyalin konten dari DoublyLinkedList lain ke list yang sudah ada.
     * @param other List lain yang kontennya akan disalin.
     * @return Referensi ke list saat ini setelah penyalinan.
     */
    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
        // 1. Lindungi dari self-assignment (misal: list1 = list1)
        if (this != &other) {
            // 2. Hapus semua data yang ada di list saat ini untuk menghindari memory leak
            clear();

            // 3. Inisialisasi ulang state (jika diperlukan)
            isCircular = other.isCircular;
            head = tail = nullptr; // `clear()` sudah melakukannya, tapi ini untuk kejelasan
            m_size = 0;
            
            // 4. Lakukan deep copy elemen dari 'other'
            for (ConstIterator it = other.begin(); it != other.end(); ++it) {
                push_back(*it);
            }
        }
        // 5. Kembalikan referensi ke objek saat ini untuk memungkinkan chaining (misal: a = b = c)
        return *this;
    }

    // --- Operasi Penambahan (Insertion) ---

    /**
     * @brief Menambah elemen baru di akhir (ekor) list.
     * @param value Nilai yang akan ditambahkan.
     */
    void push_back(const T& value) {
        Node<T>* newNode = new Node<T>(value);

        if (empty()) {
            // Jika list kosong, node baru menjadi kepala dan ekor
            head = tail = newNode;
        } else {
            // Jika tidak, sambungkan node baru setelah ekor lama
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode; // Node baru menjadi ekor baru
        }

        // Jika circular, sambungkan kembali ekor ke kepala
        perbaruiLinkCircular();
        m_size++;
    }

    /**
     * @brief Menambah elemen baru di awal (kepala) list.
     * @param value Nilai yang akan ditambahkan.
     */
    void push_front(const T& value) {
        Node<T>* newNode = new Node<T>(value);

        if (empty()) {
            // Jika list kosong, node baru menjadi kepala dan ekor
            head = tail = newNode;
        } else {
            // Jika tidak, sambungkan node baru sebelum kepala lama
            newNode->next = head;
            head->prev = newNode;
            head = newNode; // Node baru menjadi kepala baru
        }

        // Jika circular, sambungkan kembali ekor ke kepala
        perbaruiLinkCircular();
        m_size++;
    }

    // --- Operasi Penghapusan (Deletion) ---

    /**
     * @brief Menghapus elemen terakhir (di ekor) dari list.
     * Operasi ini tidak melakukan apa-apa jika list kosong.
     */
    void pop_back() {
        if (empty()) return; // Aman jika list kosong

        Node<T>* temp = tail; // Simpan node yang akan dihapus

        if (head == tail) { // Kasus: Hanya ada 1 elemen
            head = tail = nullptr;
        } else {
            tail = tail->prev; // Pindahkan ekor ke node sebelumnya
            // Jika BUKAN circular, pastikan `next` dari ekor baru adalah null
            if (!isCircular) tail->next = nullptr;
        }

        delete temp; // Hapus node lama
        perbaruiLinkCircular(); // Pastikan link circular tetap benar
        m_size--;
    }

    /**
     * @brief Menghapus elemen pertama (di kepala) dari list.
     * Operasi ini tidak melakukan apa-apa jika list kosong.
     */
    void pop_front() {
        if (empty()) return;

        Node<T>* temp = head; // Simpan node yang akan dihapus

        if (head == tail) { // Kasus: Hanya ada 1 elemen
            head = tail = nullptr;
        } else {
            head = head->next; // Pindahkan kepala ke node selanjutnya
            // Jika BUKAN circular, pastikan `prev` dari kepala baru adalah null
            if (!isCircular) head->prev = nullptr;
        }

        delete temp; // Hapus node lama
        perbaruiLinkCircular(); // Pastikan link circular tetap benar
        m_size--;
    }

    // --- Utilitas ---

    /**
     * @brief Menghapus semua elemen dari list.
     * Membebaskan memori dari semua node dan mengatur ulang list ke keadaan kosong.
     */
    void clear() {
        if (empty()) return;

        // Jika Circular, putuskan dulu lingkarannya agar tidak terjadi infinite loop saat menghapus.
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

    /**
     * @brief Mencetak semua elemen dalam list ke konsol, dari kepala ke ekor.
     */
    void print() const {
        if (empty()) {
            cout << "[ Kosong ]" << endl;
            return;
        }

        Node<T>* current = head;
        cout << "[ ";
        
        // Logika loop ini menangani kasus circular dan non-circular.
        if (isCircular) {
            do {
                cout << current->data << " ";
                current = current->next;
            } while (current != head); // Berhenti jika sudah kembali ke kepala
        } else {
            while (current != nullptr) {
                cout << current->data << " ";
                current = current->next;
            }
        }
        
        cout << "]" << endl;
    }

    /**
     * @brief Mencetak semua elemen dalam list secara terbalik, dari ekor ke kepala.
     * Berguna untuk debugging dan memastikan pointer `prev` berfungsi dengan benar.
     */
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
            } while (current != tail); // Berhenti jika sudah kembali ke ekor
        } else {
            while (current != nullptr) {
                cout << current->data << " ";
                current = current->prev;
            }
        }

        cout << "]" << endl;
    }

    // --- Dukungan Iterator (STL-compliant) ---
    /**
     * @class Iterator
     * @brief Iterator non-konstan untuk DoublyLinkedList.
     * Memungkinkan perulangan dan modifikasi elemen list.
     */
    class Iterator {
    public:
        // Tag yang dibutuhkan untuk kompatibilitas dengan algoritma STL
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Iterator(Node<T>* ptr) : m_ptr(ptr) {}

        // Dereference operator
        reference operator*() const { return m_ptr->data; }
        pointer operator->() { return &(m_ptr->data); }

        // Prefix increment
        Iterator& operator++() { m_ptr = m_ptr->next; return *this; }
        // Postfix increment
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

        // Perbandingan
        friend bool operator==(const Iterator& a, const Iterator& b) { return a.m_ptr == b.m_ptr; };
        friend bool operator!=(const Iterator& a, const Iterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        Node<T>* m_ptr;
    };
    
    /**
     * @class ConstIterator
     * @brief Iterator konstan untuk DoublyLinkedList.
     * Memungkinkan perulangan dan pembacaan elemen list tanpa bisa memodifikasinya.
     */
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

    // --- Metode untuk mendapatkan Iterator ---
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); } // `end` adalah posisi setelah elemen terakhir

    ConstIterator begin() const { return ConstIterator(head); }
    ConstIterator end() const { return ConstIterator(nullptr); }
    ConstIterator cbegin() const { return ConstIterator(head); }
    ConstIterator cend() const { return ConstIterator(nullptr); }


    // --- Akses Data ---
    
    /**
     * @brief Mengakses elemen pertama (kepala) list.
     * @return Referensi ke data elemen pertama.
     * @throws std::out_of_range jika list kosong.
     */
    T& front() {
        if (empty()) throw out_of_range("List kosong saat memanggil front()");
        return head->data;
    }

    /**
     * @brief Mengakses elemen terakhir (ekor) list.
     * @return Referensi ke data elemen terakhir.
     * @throws std::out_of_range jika list kosong.
     */
    T& back() {
        if (empty()) throw out_of_range("List kosong saat memanggil back()");
        return tail->data;
    }
    
    /**
     * @brief Mengakses elemen pada indeks tertentu menggunakan operator [].
     * @param index Indeks dari elemen yang ingin diakses.
     * @return Referensi ke data pada indeks tersebut.
     * @throws std::out_of_range jika indeks di luar batas.
     */
    T& operator[](size_t index) {
        if (index >= m_size) {
            throw out_of_range("Indeks di luar jangkauan");
        }
        Node<T>* current = head;
        for (size_t i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    }

    /**
     * @brief Memeriksa apakah list kosong.
     * @return true jika list tidak memiliki elemen, false jika sebaliknya.
     */
    bool empty() const { return m_size == 0; }

    /**
     * @brief Mendapatkan jumlah elemen dalam list.
     * @return Jumlah elemen saat ini.
     */
    size_t size() const { return m_size; }
};

#endif // DOUBLY_LINKED_LIST_HPP