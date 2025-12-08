#ifndef STACK_HPP
#define STACK_HPP

#include "DoublyLinkedList.hpp"
#include <stdexcept> // For std::out_of_range

// Kita asumsikan DoublyLinkedList.hpp ada di direktori yang sama

template <typename T>
class Stack {
private:
    // Wadah (container) yang mendasarinya
    DoublyLinkedList<T> m_container;

public:
    // --- Konstruktor ---
    Stack() {} // Konstruktor default bergantung pada default milik DoublyLinkedList

    // --- Operasi Inti ---

    // Push: Menambahkan elemen ke atas (akhir list)
    void push(const T& value) {
        m_container.push_back(value);
    }

    // Pop: Menghapus elemen teratas (akhir list)
    void pop() {
        if (!empty()) {
            m_container.pop_back();
        } else {
            // Tergantung desain, kita bisa melempar exception atau mengabaikannya
            // Untuk kesederhanaan/keamanan, kita lempar exception
             throw std::out_of_range("Stack underflow: Tidak bisa pop dari stack kosong");
        }
    }

    // Top: Mengembalikan referensi ke elemen teratas
    T& top() {
        if (empty()) {
            throw std::out_of_range("Stack kosong: Tidak bisa mengakses top");
        }
        // Bagian "top" (atas) adalah elemen terakhir yang ditambahkan, yaitu di "back"
        return m_container.back();
    }

    // Versi const dari Top
    const T& top() const {
        if (empty()) {
            throw std::out_of_range("Stack kosong: Tidak bisa mengakses top");
        }
        return m_container.back();
    }

    // --- Utilitas ---

    bool empty() const {
        return m_container.empty();
    }

    size_t size() const {
        return m_container.size();
    }
};

#endif // STACK_HPP