#ifndef MANAJER_PELANGGAN_H
#define MANAJER_PELANGGAN_H
 
#include "Customer.h"
#include "DoublyLinkedList.hpp" 
#include "PenyimpanFile.hpp" 
#include "RandomGenerator.hpp"

#include <string>
#include <iostream>
#include <limits> 
#include <conio.h>

using namespace std; 
using namespace RandomUtils;

/**
 * @class ManajerPelanggan
 * @brief Mengelola semua operasi yang berkaitan dengan data pelanggan.
 *
 * Kelas ini bertanggung jawab untuk memuat, menyimpan, menambah, mencari,
 * dan menampilkan data pelanggan. Ini bertindak sebagai lapisan abstraksi
 * antara data mentah pelanggan dan logika aplikasi utama.
 */
class ManajerPelanggan {
private:
    DoublyLinkedList<Pelanggan> daftarPelanggan; // Kontainer untuk menyimpan semua objek pelanggan.
    string fileDataPelanggan;                   // Nama file database untuk menyimpan data pelanggan.

    /**
     * @brief Memeriksa apakah sebuah ID pelanggan sudah ada dalam daftar.
     * @param id ID yang akan diperiksa.
     * @return true jika ID sudah ada, false jika belum.
     */
    bool apakahIDAda(const string& id) const {
        // Iterasi melalui daftar pelanggan untuk mencari ID yang cocok.
        for (auto it = daftarPelanggan.begin(); it != daftarPelanggan.end(); ++it) {
            if (it->getId() == id) {
                return true; // Ditemukan
            }
        }
        return false; // Tidak ditemukan
    }

public:
    /**
     * @brief Menyimpan seluruh daftar pelanggan ke dalam file.
     * Menggunakan kelas PenyimpanFile untuk menangani proses serialisasi dan penulisan.
     */
    void simpanData() {
        PenyimpanFile::simpanKeFile(fileDataPelanggan, daftarPelanggan, Pelanggan::serialisasi);
    }

    /**
     * @brief Konstruktor untuk ManajerPelanggan.
     * @param namaFile Nama file database yang akan digunakan.
     * Saat objek dibuat, secara otomatis memuat data pelanggan dari file.
     */
    ManajerPelanggan(const string& namaFile) : fileDataPelanggan(namaFile) {
        // Memuat data dari file saat program pertama kali dijalankan.
        PenyimpanFile::muatDariFile(fileDataPelanggan, daftarPelanggan, Pelanggan::deserialisasi);
    }

    /**
     * @brief Menangani proses registrasi pelanggan baru.
     * Meminta input dari pengguna untuk detail pelanggan, memvalidasi keunikan ID,
     * membuat objek Pelanggan baru, dan menyimpannya.
     */
    void tambahPelanggan() {
        string id, nama, telepon, email, password;

        cout << "\n--- Registrasi Pelanggan Baru ---" << endl;
        
        // Loop untuk memastikan ID yang dimasukkan unik.
        while (true) {
            cout << "Masukkan ID Pelanggan: ";
            getline(cin, id);
            if (apakahIDAda(id)) {
                cout << "Error: ID Pelanggan sudah digunakan! Silakan pilih ID lain." << endl;
            } else {
                break; // ID unik, lanjutkan proses.
            }
        }

        // Meminta input untuk data pelanggan lainnya.
        cout << "Masukkan Nama Lengkap: ";
        getline(cin, nama);

        cout << "Masukkan No. Telepon: ";
        getline(cin, telepon);

        cout << "Masukkan Email: ";
        getline(cin, email);

        cout << "Masukkan Password: ";
        getline(cin, password);

        // Membuat objek pelanggan baru dan menambahkannya ke daftar.
        Pelanggan pelangganBaru(id, nama, telepon, email, password);
        daftarPelanggan.push_back(pelangganBaru);

        // Menyimpan perubahan ke file database.
        simpanData();
        
        cout << "\n>> Pelanggan '" << nama << "' berhasil didaftarkan! <<" << endl;
        getch();
    }

    /**
     * @brief Mencari pelanggan berdasarkan ID mereka.
     * @param id ID pelanggan yang akan dicari.
     * @return Pointer ke objek Pelanggan jika ditemukan, jika tidak, mengembalikan nullptr.
     */
    Pelanggan* cariPelanggan(const string& id) {
        // Menggunakan auto& untuk mendapatkan referensi ke objek agar lebih efisien.
        for (auto& pelanggan : daftarPelanggan) {
            if (pelanggan.getId() == id) {
                return &pelanggan; // Mengembalikan alamat dari objek yang ditemukan.
            }
        }
        return nullptr; // Tidak ada pelanggan yang cocok.
    }

    /**
     * @brief Menampilkan informasi dari semua pelanggan yang terdaftar.
     * Berguna untuk keperluan administrasi.
     */
    void tampilkanSemuaPelanggan() const {
        cout << "\n--- Daftar Semua Pelanggan ---" << endl;
        if (daftarPelanggan.empty()) {
            cout << "Belum ada pelanggan terdaftar." << endl;
            return;
        }

        // Iterasi dan panggil metode tampilkanInfo() untuk setiap pelanggan.
        for (auto it = daftarPelanggan.begin(); it != daftarPelanggan.end(); ++it) {
            it->tampilkanInfo();
        }
    }

    /**
     * @brief (Placeholder) Mencatat jumlah total pelanggan.
     * Fungsi ini saat ini tidak memiliki implementasi aktif tetapi bisa
     * dikembangkan untuk menampilkan statistik.
     */
    void catatJumlahPelanggan() const {
        // Dihilangkan untuk keringkasan, logika tetap sama jika menggunakan list.size()
    }
};

#endif