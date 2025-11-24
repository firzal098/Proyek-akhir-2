#ifndef STACK_HPP
#define STACK_HPP

#include "DynamicArray.hpp"
// Kita asumsikan DynamicArray.hpp ada di direktori yang sama
// dan memiliki "using namespace std;" di dalamnya berdasarkan permintaan sebelumnya.

template <typename T>
class Stack {
private:
    // Wadah (container) yang mendasarinya
    DynamicArray<T> m_container;

public:
    // --- Konstruktor ---
    Stack() {} // Konstruktor default bergantung pada default milik DynamicArray

    // --- Operasi Inti ---

    // Push: Menambahkan elemen ke atas (akhir array)
    void push(const T& value) {
        m_container.push_back(value);
    }

    // Pop: Menghapus elemen teratas (akhir array)
    void pop() {
        if (!empty()) {
            m_container.pop_back();
        } else {
            // Tergantung desain, kita bisa melempar exception atau mengabaikannya
            // Untuk kesederhanaan/keamanan, kita lempar exception
             throw out_of_range("Stack underflow: Tidak bisa pop dari stack kosong");
        }
    }

    // Top: Mengembalikan referensi ke elemen teratas
    T& top() {
        if (empty()) {
            throw out_of_range("Stack kosong: Tidak bisa mengakses top");
        }
        // Bagian "top" (atas) adalah elemen terakhir yang ditambahkan, yaitu di size - 1
        return m_container[m_container.size() - 1];
    }

    // Versi const dari Top
    const T& top() const {
        if (empty()) {
            throw out_of_range("Stack kosong: Tidak bisa mengakses top");
        }
        return m_container[m_container.size() - 1];
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