#ifndef MANAJER_PELANGGAN_H
#define MANAJER_PELANGGAN_H
 
#include "Customer.h"
#include "DoublyLinkedList.hpp" 
#include "PenyimpanFile.hpp" 
#include "RandomGenerator.hpp"
#include "Tampilan.hpp"

#include <string>
#include <iostream>
#include <limits> 

using namespace std; 
using namespace RandomUtils;

/**
 * @class ManajerPelanggan
 * @brief Mengelola semua operasi yang berkaitan dengan data pelanggan.
 */
class ManajerPelanggan {
private:
    DoublyLinkedList<Pelanggan> daftarPelanggan; 
    string fileDataPelanggan;                   

    /**
     * @brief Memeriksa apakah sebuah ID pelanggan sudah ada dalam daftar.
     */
    bool apakahIDAda(const string& id) const {
        for (auto it = daftarPelanggan.begin(); it != daftarPelanggan.end(); ++it) {
            if (it->getId() == id) {
                return true; 
            }
        }
        return false; 
    }

public:
    /**
     * @brief Menyimpan seluruh daftar pelanggan ke dalam file.
     */
    void simpanData() {
        PenyimpanFile::simpanKeFile(fileDataPelanggan, daftarPelanggan, Pelanggan::serialisasi);
    }

    /**
     * @brief Konstruktor untuk ManajerPelanggan.
     */
    ManajerPelanggan(const string& namaFile) : fileDataPelanggan(namaFile) {
        PenyimpanFile::muatDariFile(fileDataPelanggan, daftarPelanggan, Pelanggan::deserialisasi);
    }

    /**
     * @brief Menangani proses registrasi pelanggan baru.
     */
    void tambahPelanggan() {
        Tampilan::printHeader("Registrasi Pelanggan Baru");
        string id, nama, telepon, email, password;
        
        while (true) {
            id = Tampilan::getString("Masukkan ID Pelanggan");
            if (apakahIDAda(id)) {
                Tampilan::printError("ID Pelanggan sudah digunakan! Silakan pilih ID lain.");
            } else {
                break;
            }
        }

        nama = Tampilan::getString("Masukkan Nama Lengkap");
        telepon = Tampilan::getString("Masukkan No. Telepon");
        email = Tampilan::getString("Masukkan Email");
        password = Tampilan::getString("Masukkan Password");

        Pelanggan pelangganBaru(id, nama, telepon, email, password);
        daftarPelanggan.push_back(pelangganBaru);

        simpanData();
        
        Tampilan::printMessage("Pelanggan '" + nama + "' berhasil didaftarkan!", Tampilan::GREEN);
        Tampilan::pause();
    }

    /**
     * @brief Mencari pelanggan berdasarkan ID mereka.
     */
    Pelanggan* cariPelanggan(const string& id) {
        for (auto& pelanggan : daftarPelanggan) {
            if (pelanggan.getId() == id) {
                return &pelanggan;
            }
        }
        return nullptr;
    }

    /**
     * @brief Menampilkan informasi dari semua pelanggan yang terdaftar.
     */
    void tampilkanSemuaPelanggan() const {
        Tampilan::printHeader("Daftar Semua Pelanggan");
        if (daftarPelanggan.empty()) {
            Tampilan::printMessage("Belum ada pelanggan terdaftar.");
            return;
        }

        for (auto it = daftarPelanggan.begin(); it != daftarPelanggan.end(); ++it) {
            it->tampilkanInfo();
            cout << Tampilan::DIM << "----------------------------------------\n" << Tampilan::RESET;
        }
    }

    void catatJumlahPelanggan() const {
        // Placeholder
    }
};

#endif