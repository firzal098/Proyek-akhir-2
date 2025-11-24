#ifndef STATIC_ARRAY_HPP
#define STATIC_ARRAY_HPP

#include <stdexcept> // Untuk pengecekan (checks)
#include <iostream>

using namespace std;

/**
 * Implementasi array statis yang disederhanakan mirip dengan std::array.
 * Ukuran ditetapkan saat waktu kompilasi (compile time).
 * Alokasi terjadi di Stack.
 */
template <typename T, size_t Size>
class StaticArray {
private:
    // Array aktual disimpan langsung di dalam kelas (pada Stack)
    T m_data[Size]; 

public:
    // --- Konstruktor ---
    
    // Konstruktor default: Elemen diinisialisasi ke default (bisa berisi nilai sampah/garbage untuk int jika tidak hati-hati)
    StaticArray() {
        // Kita bisa secara opsional melakukan zero-init di sini jika ingin aman
        // tetapi untuk kecepatan/kesederhanaan mentah, kita biarkan seperti perilaku array gaya C.
    }

    // --- Akses Elemen ---

    // Akses tanpa pengecekan batas
    T& operator[](size_t index) {
        return m_data[index];
    }

    const T& operator[](size_t index) const {
        return m_data[index];
    }

    // Akses dengan pengecekan batas
    T& at(size_t index) {
        if (index >= Size) {
            throw out_of_range("Indeks StaticArray di luar jangkauan");
        }
        return m_data[index];
    }

    const T& at(size_t index) const {
        if (index >= Size) {
            throw out_of_range("Indeks StaticArray di luar jangkauan");
        }
        return m_data[index];
    }

    // Ambil elemen pertama
    T& front() {
        if (Size == 0) throw out_of_range("Array kosong");
        return m_data[0];
    }

    // Ambil elemen terakhir
    T& back() {
        if (Size == 0) throw out_of_range("Array kosong");
        return m_data[Size - 1];
    }

    // --- Kapasitas ---

    // Ukuran diketahui saat waktu kompilasi, jadi ini adalah ekspresi konstan
    constexpr size_t size() const {
        return Size;
    }

    bool empty() const {
        return Size == 0;
    }

    // --- Pengubah (Modifiers) ---

    // Isi array dengan nilai tertentu
    void fill(const T& value) {
        for (size_t i = 0; i < Size; ++i) {
            m_data[i] = value;
        }
    }

    // --- Iterator ---
    // Memungkinkan penggunaan dalam: for(auto x : arr)

    T* begin() { return m_data; }
    const T* begin() const { return m_data; }
    
    T* end() { return m_data + Size; }
    const T* end() const { return m_data + Size; }
};

#endif // STATIC_ARRAY_HPP