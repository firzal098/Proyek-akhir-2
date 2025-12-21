#ifndef MANAJER_VENDOR_H
#define MANAJER_VENDOR_H

#include "vendor.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
#include <sstream>
#include <conio.h> 
#include "DoublyLinkedList.hpp"
#include "PenyimpanFile.hpp"


using namespace std;

class ManajerVendor {
private:
    DoublyLinkedList<Vendor> daftarVendor;
    string namaFile = "vendors.db";

    void tambahVendor() {
        int id;
        string nama, produk, kat;
        
        cout << "\n--- Tambah Vendor ---\n";
        cout << "ID Vendor (Angka): "; cin >> id; cin.ignore();
        
        // Cek duplicate ID
        for (auto& v : daftarVendor) {
            if (v.getId() == id) {
                cout << "Error: ID Vendor sudah digunakan!\n";
                getch();
                return;
            }
        }

        cout << "Nama Vendor : "; getline(cin, nama);
        cout << "Produk Utama: "; getline(cin, produk);
        cout << "Kategori    : "; getline(cin, kat);

        daftarVendor.push_back(Vendor(id, nama, produk, kat));
        simpan();
        cout << "Vendor berhasil disimpan!\n";
        getch();
    }

    void cariVendor() {
        string key;
        cout << "Masukkan kata kunci (Nama/Kategori): "; getline(cin, key);
        string lowerKey = keHurufKecil(key);
        
        bool found = false;
        for (auto& v : daftarVendor) {
            if (keHurufKecil(v.getNama()).find(lowerKey) != string::npos || 
                keHurufKecil(v.getKategori()).find(lowerKey) != string::npos) {
                v.info();
                found = true;
            }
        }
        if (!found) cout << "Tidak ditemukan vendor dengan kata kunci tersebut.\n";
    }

    void menuKelolaProduk() {
        int id;
        cout << "Masukkan ID Vendor yang ingin dikelola: "; cin >> id; cin.ignore();

        Vendor* target = nullptr;
        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            if (it->getId() == id) {
                target = &(*it);
                break;
            }
        }

        if (!target) {
            cout << "Vendor tidak ditemukan.\n";
            getch();
            return;
        }

        // Sub-menu for specific vendor
        int subPilih;
        do {
            system("cls");
            cout << "=== KELOLA PRODUK VENDOR: " << target->getNama() << " ===\n";
            cout << "1. Tambah Produk\n";
            cout << "2. Lihat Daftar Produk\n";
            cout << "0. Kembali\n";
            cout << "Pilihan: "; cin >> subPilih; cin.ignore();

            if (subPilih == 1) {
                target->tambahProduk();
                simpan(); // Auto save updates
                getch();
            } else if (subPilih == 2) {
                target->tampilkanProduk();
                getch();
            }
        } while (subPilih != 0);
    }

public:
    ManajerVendor() {
        PenyimpanFile::muatDariFile(namaFile, daftarVendor, Vendor::deserialisasi);
    }

    void simpan() {
        PenyimpanFile::simpanKeFile(namaFile, daftarVendor, Vendor::serialisasi);
    }

    DoublyLinkedList<Vendor>& getDaftarVendor() {
        return daftarVendor;
    }

    void tampilkanSemua() {
        cout << "\n--- Daftar Vendor ---\n";
        if (daftarVendor.empty()) {
            cout << "Data kosong.\n";
            return;
        }
        for (auto& v : daftarVendor) {
            v.info();
        }
    }

    void jalankan() {
        int pilih;
        do {
            system("cls");
            cout << "=== MENU VENDOR MANAGEMENT ===\n";
            cout << "1. Tambah Vendor Baru\n";
            cout << "2. Tampilkan Semua Vendor\n";
            cout << "3. Cari Vendor (Nama/Kategori)\n";
            cout << "4. Kelola Produk Vendor\n";
            cout << "0. Kembali ke Menu Utama\n";
            cout << "Pilihan: "; cin >> pilih; cin.ignore();

            switch (pilih) {
                case 1: tambahVendor(); break;
                case 2: tampilkanSemua(); getch(); break;
                case 3: cariVendor(); getch(); break;
                case 4: menuKelolaProduk(); break;
                case 0: break;
                default: cout << "Pilihan tidak valid.\n"; getch();
            }
        } while (pilih != 0);
    }
};

#endif // MANAJER_VENDOR_H
