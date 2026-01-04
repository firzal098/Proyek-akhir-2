#ifndef MANAJER_PARKIR_H
#define MANAJER_PARKIR_H
 
#include <iostream>
#include <string>
#include <limits>
#include <conio.h>
#include <iomanip>
#include <fstream> 
#include "ParkingSystem.h"
#include "PenyimpanFile.hpp" 
#include "DoublyLinkedList.hpp"
#include "DataStructures.hpp" // IMPORT STRUKTUR DATA BARU

using namespace std;

// ==========================================
// KELAS UTAMA: MANAJER PARKIR
// ==========================================

class ManajerParkir {
private:
    SistemParkir sistemParkir; 
    
    // --- PENGGUNAAN STRUKTUR DATA DARI DataStructures.hpp ---
    DoublyLinkedList<DoublyLinkedList<string>> petaSlot; // Denah
    QueueParkir antrianMasuk;    // Queue (FIFO)
    StackParkir historyKeluar;   // Stack (LIFO)
    BSTLoyalitas databasePoin;   // BST

    const string fileLoyalitas = "poin_loyalitas.db";

    void inisialisasiPetaParkir() {
        petaSlot.clear(); 
        for (int i = 0; i < 5; ++i) {
            DoublyLinkedList<string> baris;
            for (int j = 0; j < 5; ++j) {
                string idSlot = string(1, 'A' + i) + to_string(j + 1);
                baris.push_back(idSlot);
            }
            petaSlot.push_back(baris);
        }
    }

    void muatPoinLoyalitas() {
        // Load manual file baris per baris ke BST
        ifstream file(fileLoyalitas);
        string baris;
        while (getline(file, baris)) {
            auto token = PenyimpanFile::pisah(baris, '|');
            auto it = token.begin();
            if (it != token.end()) {
                string p = *it;
                int val = 0;
                if (++it != token.end()) val = stoi(*it);
                databasePoin.insertDirect(p, val);
            }
        }
    }

    void simpanPoinLoyalitas() {
        databasePoin.simpanKeFile(fileLoyalitas);
    }

public:
    ManajerParkir() {
        inisialisasiPetaParkir();
        muatPoinLoyalitas();
    }
    
    void tampilkanPeta() {
        cout << "\n================ DENAH PARKIR (5x5) ================\n";
        cout << "Legenda: [ XX ] = Terisi, [ A1 ] = Kosong\n\n";
        for (auto itBaris = petaSlot.begin(); itBaris != petaSlot.end(); ++itBaris) {
            cout << "   ";
            for (auto itKolom = itBaris->begin(); itKolom != itBaris->end(); ++itKolom) {
                string slot = *itKolom;
                if (sistemParkir.apakahSlotTerisi(slot)) {
                    cout << "[ XX ] ";
                } else {
                    cout << "[ " << slot << " ] ";
                }
            }
            cout << endl;
        }
        cout << "====================================================\n";
    }

    // --- MENU 1: SUB-MENU ANTRIAN ---
    void menuAntrian() {
        int pil;
        while (true) {
            system("cls");
            cout << "=== MENU ANTRIAN PARKIR ===\n";
            cout << "1. Tambah Antrian Masuk Parkir (QUEUE)\n";
            cout << "2. Tampilkan Antrian\n";
            cout << "3. Tampilkan Kendaraan Aktif\n";
            cout << "4. Keluar\n";
            cout << "Pilihan: ";
            cin >> pil; cin.ignore();

            if (pil == 1) {
                string id, plat;
                cout << "Masukkan ID Pelanggan: "; getline(cin, id);
                cout << "Masukkan Plat Nomor: "; getline(cin, plat);
                antrianMasuk.enqueue(id, plat);
                getch();
            } else if (pil == 2) {
                antrianMasuk.tampilkan();
                getch();
            } else if (pil == 3) {
                sistemParkir.tampilkanTiketAktif();
                getch();
            } else if (pil == 4) {
                break;
            }
        }
    }

    // ===============================
// FUNGSI PENDUKUNG CHECK-OUT
// ===============================

TiketParkir* cariTiket(const string& idTiket) {
    auto& daftar = sistemParkir.getTiketAktif();
    for (auto it = daftar.begin(); it != daftar.end(); ++it) {
        if (it->idTiket == idTiket) {
            return &(*it);
        }
    }
    return nullptr;
}

double hitungBiaya(const TiketParkir& tiket) {
    long durasiJam = (time(nullptr) - tiket.waktuMasuk) / 3600;
    if (durasiJam < 1) durasiJam = 1;
    return durasiJam * 5000; // 5.000 per jam
}


void checkoutTiket(const string& idTiket) {
    TiketParkir tiket = sistemParkir.checkOut(idTiket);

    // Hitung poin: setiap 1000 biaya = 1 poin
    int poin = static_cast<int>(tiket.biaya / 1000);

    historyKeluar.push(tiket);
    databasePoin.tambahAtauUpdate(tiket.nomorPolisi, poin); // update BST dengan poin sesuai biaya
    simpanPoinLoyalitas();
}

    // --- MENU UTAMA ---
    void tampilkanMenu(Pelanggan* pelanggan, ManajerPelanggan& manajerPelanggan) {
        int pilihan;
        while (true) {
            system("cls"); 
            cout << "=== SISTEM MANAJEMEN PARKIR ===" << endl;
            cout << "1. Ambil Antrian Parkir" << endl;
            cout << "2. Masuk Parkir (Check-In)" << endl;
            cout << "3. Keluar Parkir (Check-Out)" << endl;
            cout << "4. Tampilkan Kendaraan Aktif" << endl;
            cout << "5. Cek Poin Loyalitas (BST)" << endl;
            cout << "6. Tampilkan Denah Parkir" << endl;
            cout << "7. Lihat Kendaraan Terakhir Keluar" << endl;
            cout << "8. Kembali" << endl;
            cout << "Pilihan: ";
            cin >> pilihan;
            cin.ignore(); 

            switch (pilihan) {
                case 1: 
                    menuAntrian();
                    break;

                case 2: { // Check-In dari Queue
                    cout << "\n--- Proses Check-In ---" << endl;
                    
                    if (antrianMasuk.isEmpty()) {
                        cout << "Gagal: Tidak ada kendaraan yang mengantri.\n";
                        cout << "Silakan ambil antrian terlebih dahulu (Menu 1).\n";
                        getch();
                        break;
                    }

                    // Ambil data dari Queue
                    string idCust, plat;
                    antrianMasuk.dequeue(idCust, plat);
                    cout << "Memproses Antrian Terdepan:\n";
                    cout << "ID: " << idCust << " | Plat: " << plat << endl;

                    // Pilih Slot
                    tampilkanPeta();
                    string slot;
                    cout << "Pilih Slot (misal A1): "; getline(cin, slot);

                    // Proses Checkin di Sistem
                    sistemParkir.checkIn(idCust, plat, slot);
                    getch();
                    break;
                }

                case 3: { // Checkout
                    string idTiket;
                    cout << "Masukkan ID Tiket: ";
                    getline(cin, idTiket);                   

                    TiketParkir* tiket = cariTiket(idTiket);
                    if (!tiket) {
                        cout << "Tiket tidak ditemukan!\n";
                        getch();
                        break;
                    }

                    double biaya = hitungBiaya(*tiket);

                    cout << "\n--- INFORMASI PEMBAYARAN ---\n";
                    cout << "Biaya Parkir : Rp" << biaya << endl;
                    cout << "Saldo Anda   : Rp" << pelanggan->getSaldo() << endl;

                    if (!pelanggan) {
                        cout << "Checkout hanya bisa dilakukan oleh pelanggan.\n";
                        getch();
                    break;
                    }

                    // 🔴 VALIDASI SALDO
                    if (pelanggan->getSaldo() < biaya) {
                        cout << "\n❌ Saldo tidak mencukupi!\n";
                        cout << "Silakan isi saldo terlebih dahulu.\n";
                        getch();
                        break;
                    }

                    // 🟢 POTONG SALDO
                    pelanggan->setSaldo(pelanggan->getSaldo() - biaya);
                    manajerPelanggan.simpanData();

                    // Proses checkout tiket
                    checkoutTiket(idTiket);

                    cout << "\n✅ Checkout berhasil!\n";
                    cout << "Sisa saldo: Rp" << pelanggan->getSaldo() << endl;
                    getch();
                    break;
                }

                case 4: 
                    sistemParkir.tampilkanTiketAktif();
                    getch();
                    break;
                case 5: { // Cek BST
                    string plat;
                    databasePoin.tampilkanTop5();
                    cout << "\n--- Cek Poin Loyalitas (BST) ---" << endl;
                    cout << "Masukkan Plat Nomor: "; getline(cin, plat);
                    databasePoin.cariPoin(plat);
                    getch();
                    break;
                }
                case 6:
                    tampilkanPeta();
                    getch();
                    break;
                case 7: // Cek Stack
                    sistemParkir.tampilkanRiwayat(); // fungsi baru di SistemParkir
                    getch();
                    break;
                case 8: 
                    return;
                default:
                    cout << "Pilihan tidak valid." << endl;
                    getch();
                    break;
            }
        }
    }
};

#endif // MANAJER_PARKIR_H