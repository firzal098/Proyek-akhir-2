#ifndef MANAJER_VENDOR_H
#define MANAJER_VENDOR_H

#include "vendor.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
#include <sstream>
#include "DoublyLinkedList.hpp"
#include "PenyimpanFile.hpp"
#include "Tampilan.hpp"


using namespace std;

class ManajerVendor {
private:
    DoublyLinkedList<Vendor> daftarVendor;
    string namaFile = "vendors.db";

    void tambahVendor() {
        Tampilan::printHeader("Tambah Vendor");
        
        cout << Tampilan::BOLD << Tampilan::YELLOW << "-> ID Vendor (Angka): " << Tampilan::RESET;
        int id;
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        for (auto& v : daftarVendor) {
            if (v.getId() == id) {
                Tampilan::printError("ID Vendor sudah digunakan!");
                Tampilan::pause();
                return;
            }
        }

        string nama = Tampilan::getString("Nama Vendor");
        string produk = Tampilan::getString("Produk Utama");
        string kat = Tampilan::getString("Kategori");

        daftarVendor.push_back(Vendor(id, nama, produk, kat));
        simpan();
        Tampilan::printMessage("Vendor berhasil disimpan!", Tampilan::GREEN);
        Tampilan::pause();
    }

    void cariVendor() {
        Tampilan::printHeader("Cari Vendor");
        string key = Tampilan::getString("Masukkan kata kunci (Nama/Kategori)");
        string lowerKey = keHurufKecil(key);
        
        cout << "\n" << Tampilan::CYAN << "--- Hasil Pencarian ---\n" << Tampilan::RESET;
        
        bool found = false;
        for (auto& v : daftarVendor) {
            if (keHurufKecil(v.getNama()).find(lowerKey) != string::npos || 
                keHurufKecil(v.getKategori()).find(lowerKey) != string::npos) {
                v.info();
                found = true;
            }
        }
        if (!found) {
            Tampilan::printMessage("Tidak ditemukan vendor dengan kata kunci tersebut.");
        }
    }

    void menuKelolaProduk() {
        Tampilan::printHeader("Kelola Produk");
        tampilkanSemua();
        cout << Tampilan::BOLD << Tampilan::YELLOW << "-> Masukkan ID Vendor yang ingin dikelola: " << Tampilan::RESET;
        int id;
        cin >> id;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        Vendor* target = nullptr;
        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            if (it->getId() == id) {
                target = &(*it);
                break;
            }
        }

        if (!target) {
            Tampilan::printError("Vendor tidak ditemukan.");
            Tampilan::pause();
            return;
        }

        int subPilih;
        do {
            Tampilan::printHeader("Kelola Produk: " + target->getNama());
            DoublyLinkedList<string> menuItems;
            menuItems.push_back("Tambah Produk");
            menuItems.push_back("Lihat Daftar Produk");
            menuItems.push_back("Kembali");
            Tampilan::printMenu(menuItems);
            subPilih = Tampilan::getChoice();

            if (subPilih == 1) {
                target->tambahProduk();
                simpan(); 
                Tampilan::printMessage("Produk berhasil ditambahkan.", Tampilan::GREEN);
                Tampilan::pause();
            } else if (subPilih == 2) {
                Tampilan::printHeader("Daftar Produk: " + target->getNama());
                target->tampilkanProduk();
                Tampilan::pause();
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

    Vendor* getVendorById(int id) {
        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            if (it->getId() == id) {
                return &(*it);
            }
        }
        return nullptr;
    }

    void tampilkanSemua() {
        cout << "\n" << Tampilan::CYAN << "--- Daftar Vendor ---\n" << Tampilan::RESET;
        if (daftarVendor.empty()) {
            Tampilan::printMessage("Data vendor kosong.");
            return;
        }
        for (auto& v : daftarVendor) {
            v.info();
        }
    }

    void jalankan() {
        int pilih;
        do {
            Tampilan::printHeader("Manajemen Vendor");
            DoublyLinkedList<string> menuItems;
            menuItems.push_back("Tambah Vendor Baru");
            menuItems.push_back("Tampilkan Semua Vendor");
            menuItems.push_back("Cari Vendor (Nama/Kategori)");
            menuItems.push_back("Kelola Produk Vendor");
            menuItems.push_back("Kembali ke Menu Utama");
            Tampilan::printMenu(menuItems);
            pilih = Tampilan::getChoice();

            switch (pilih) {
                case 1: tambahVendor(); break;
                case 2: 
                    Tampilan::printHeader("Semua Vendor");
                    tampilkanSemua(); 
                    Tampilan::pause(); 
                    break;
                case 3: 
                    cariVendor(); 
                    Tampilan::pause(); 
                    break;
                case 4: menuKelolaProduk(); break;
                case 0: break;
                default: 
                    Tampilan::printError("Pilihan tidak valid.");
                    Tampilan::pause();
            }
        } while (pilih != 0);
    }
};

#endif // MANAJER_VENDOR_H
