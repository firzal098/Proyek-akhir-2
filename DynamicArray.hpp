#ifndef DYNAMIC_ARRAY_HPP
#define DYNAMIC_ARRAY_HPP

#include <stdexcept> // Masih diperlukan untuk exception out_of_range
#include <iostream>  // Berguna untuk debugging jika diperlukan, tetapi utamanya untuk namespace std di sini

using namespace std;

/**
 * Implementasi array dinamis yang disederhanakan.
 * Menghapus ketergantungan pada <algorithm>, <utility>, dan <initializer_list>.
 */
template <typename T>
class DynamicArray {
private:
    T* m_data;          // Pointer ke elemen pertama
    size_t m_size;      // Jumlah elemen yang sedang digunakan saat ini
    size_t m_capacity;  // Jumlah memori yang dialokasikan saat ini (kapasitas total)

    // Fungsi pembantu untuk mengubah ukuran buffer internal
    void reallocate(size_t new_capacity) {
        // 1. Alokasikan blok memori baru
        T* new_block = new T[new_capacity];

        // 2. Salin elemen yang ada secara manual ke blok baru
        // Karena kita tidak bisa menggunakan std::move atau std::copy, kita gunakan loop sederhana
        if (m_data) {
            for (size_t i = 0; i < m_size; ++i) {
                new_block[i] = m_data[i];
            }
            
            // 3. Hapus memori lama
            delete[] m_data;
        }

        // 4. Perbarui pointer dan kapasitas
        m_data = new_block;
        m_capacity = new_capacity;
    }

public:
    // --- Konstruktor & Destruktor ---

    // Konstruktor Default
    DynamicArray() {
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    // Konstruktor dengan kapasitas awal
    explicit DynamicArray(size_t initial_capacity) {
        m_data = new T[initial_capacity];
        m_size = 0;
        m_capacity = initial_capacity;
    }

    // Destruktor
    ~DynamicArray() {
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
    }

    // --- Semantik Penyalinan (Implementasi Manual) ---

    // Copy Constructor (Konstruktor Salin)
    DynamicArray(const DynamicArray& other) {
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        m_data = new T[m_capacity];

        // Loop penyalinan manual
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    }

    // Copy Assignment Operator (Operator Penugasan Salin)
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            // Bersihkan data yang ada
            if (m_data != nullptr) {
                delete[] m_data;
            }

            // Salin properti
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            
            // Alokasikan dan salin data
            m_data = new T[m_capacity];
            for (size_t i = 0; i < m_size; ++i) {
                m_data[i] = other.m_data[i];
            }
        }
        return *this;
    }

    // --- Akses Elemen ---

    // Menambahkan elemen ke akhir
    void push_back(const T& value) {
        if (m_size >= m_capacity) {
            // Jika kosong, mulai dengan 1, jika tidak, gandakan kapasitasnya
            size_t new_cap = (m_capacity == 0) ? 1 : m_capacity * 2;
            reallocate(new_cap);
        }
        m_data[m_size] = value;
        m_size++;
    }

    // Menghapus elemen terakhir
    void pop_back() {
        if (m_size > 0) {
            m_size--;
        }
    }

    // Akses tanpa pengecekan batas (bounds checking)
    T& operator[](size_t index) {
        return m_data[index];
    }

    const T& operator[](size_t index) const {
        return m_data[index];
    }

    // Akses dengan pengecekan batas
    T& at(size_t index) {
        if (index >= m_size) {
            throw out_of_range("Indeks DynamicArray di luar jangkauan");
        }
        return m_data[index];
    }

    const T& at(size_t index) const {
        if (index >= m_size) {
            throw out_of_range("Indeks DynamicArray di luar jangkauan");
        }
        return m_data[index];
    }

    // --- Kapasitas & Ukuran ---

    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }
    bool empty() const { return m_size == 0; }

    void reserve(size_t new_cap) {
        if (new_cap > m_capacity) {
            reallocate(new_cap);
        }
    }

    void clear() {
        m_size = 0;
    }

    // --- Iterator ---
    T* begin() { return m_data; }
    const T* begin() const { return m_data; }
    T* end() { return m_data + m_size; }
    const T* end() const { return m_data + m_size; }
};

#endif // DYNAMIC_ARRAY_HPP