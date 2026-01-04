#ifndef MANAJER_PARKIR_H
#define MANAJER_PARKIR_H
 
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <fstream> 
#include "ParkingSystem.h"
#include "PenyimpanFile.hpp" 
#include "DoublyLinkedList.hpp"
#include "DataStructures.hpp"
#include "Tampilan.hpp" // <-- Include library Tampilan

using namespace std;

// ==========================================
// KELAS UTAMA: MANAJER PARKIR
// ==========================================

class ManajerParkir {
private:
    SistemParkir sistemParkir; 
    
    DoublyLinkedList<DoublyLinkedList<string>> petaSlot;
    QueueParkir antrianMasuk;
    StackParkir historyKeluar;
    BSTLoyalitas databasePoin;

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
        cout << Tampilan::BLUE << "\n================ DENAH PARKIR (5x5) ================\n" << Tampilan::RESET;
        cout << Tampilan::DIM << "Legenda: [ XX ] = Terisi, [ A1 ] = Kosong\n\n" << Tampilan::RESET;
        for (auto itBaris = petaSlot.begin(); itBaris != petaSlot.end(); ++itBaris) {
            cout << "   ";
            for (auto itKolom = itBaris->begin(); itKolom != itBaris->end(); ++itKolom) {
                string slot = *itKolom;
                if (sistemParkir.apakahSlotTerisi(slot)) {
                    cout << Tampilan::RED << "[ XX ] " << Tampilan::RESET;
                } else {
                    cout << Tampilan::GREEN << "[ " << slot << " ] " << Tampilan::RESET;
                }
            }
            cout << endl;
        }
        cout << Tampilan::BLUE << "====================================================\n" << Tampilan::RESET;
    }

    void menuAntrian() {
        while (true) {
            Tampilan::printHeader("Menu Antrian Parkir");
            DoublyLinkedList<string> menuItems;
            menuItems.push_back("Tambah Antrian Masuk Parkir (QUEUE)");
            menuItems.push_back("Tampilkan Antrian");
            menuItems.push_back("Tampilkan Kendaraan Aktif");
            menuItems.push_back("Kembali");
            Tampilan::printMenu(menuItems);
            int pil = Tampilan::getChoice();

            if (pil == 1) {
                string id = Tampilan::getString("Masukkan ID Pelanggan");
                string plat = Tampilan::getString("Masukkan Plat Nomor");
                antrianMasuk.enqueue(id, plat);
                Tampilan::printMessage("Kendaraan berhasil ditambahkan ke antrian.", Tampilan::GREEN);
                Tampilan::pause();
            } else if (pil == 2) {
                Tampilan::printHeader("Antrian Kendaraan");
                antrianMasuk.tampilkan();
                Tampilan::pause();
            } else if (pil == 3) {
                Tampilan::printHeader("Kendaraan Parkir Aktif");
                sistemParkir.tampilkanTiketAktif();
                Tampilan::pause();
            } else if (pil == 4) {
                break;
            } else {
                Tampilan::printError("Pilihan tidak valid.");
                Tampilan::pause();
            }
        }
    }

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
    historyKeluar.push(tiket);
    databasePoin.tambahAtauUpdate(tiket.nomorPolisi, 10);
    simpanPoinLoyalitas();
    
}




    // --- MENU UTAMA ---
    void tampilkanMenu(Pelanggan* pelanggan, ManajerPelanggan& manajerPelanggan) {
        while (true) {
            Tampilan::printHeader("Manajemen Parkir");
            DoublyLinkedList<string> menuItems;
            menuItems.push_back("Ambil Antrian Parkir");
            menuItems.push_back("Masuk Parkir (Check-In)");
            menuItems.push_back("Keluar Parkir (Check-Out)");
            menuItems.push_back("Tampilkan Kendaraan Aktif");
            menuItems.push_back("Cek Poin Loyalitas (BST)");
            menuItems.push_back("Tampilkan Denah Parkir");
            menuItems.push_back("Lihat Kendaraan Terakhir Keluar");
            menuItems.push_back("Kembali");
            Tampilan::printMenu(menuItems);
            int pilihan = Tampilan::getChoice();

            switch (pilihan) {
                case 1: 
                    menuAntrian();
                    break;
                case 2: { // Check-In dari Queue
                    Tampilan::printHeader("Proses Check-In");
                    if (antrianMasuk.isEmpty()) {
                        Tampilan::printError("Gagal: Tidak ada kendaraan yang mengantri.");
                        Tampilan::printMessage("Silakan ambil antrian terlebih dahulu (Menu 1).");
                        Tampilan::pause();
                        break;
                    }

                    string idCust, plat;
                    antrianMasuk.dequeue(idCust, plat);
                    cout << "Memproses Antrian Terdepan:\n";
                    cout << "ID: " << idCust << " | Plat: " << plat << endl;

                    tampilkanPeta();
                    string slot = Tampilan::getString("Pilih Slot (misal A1)");

                    sistemParkir.checkIn(idCust, plat, slot);
                    Tampilan::printMessage("Check-in berhasil!", Tampilan::GREEN);
                    Tampilan::pause();
                    break;
                }
                case 3: { // Checkout
                    if (!pelanggan) {
                        Tampilan::printError("Checkout hanya bisa dilakukan oleh pelanggan yang login.");
                        Tampilan::pause();
                        break;
                    }
                    Tampilan::printHeader("Proses Check-Out");
                    string idTiket = Tampilan::getString("Masukkan ID Tiket");

                    TiketParkir* tiket = cariTiket(idTiket);
                    if (!tiket) {
                        Tampilan::printError("Tiket tidak ditemukan!");
                        Tampilan::pause();
                        break;
                    }

                    double biaya = hitungBiaya(*tiket);
                    cout << "\n--- INFORMASI PEMBAYARAN ---\n";
                    cout << "Biaya Parkir : " << Tampilan::YELLOW << "Rp" << biaya << Tampilan::RESET << endl;
                    cout << "Saldo Anda   : " << Tampilan::GREEN << "Rp" << pelanggan->getSaldo() << Tampilan::RESET << endl;

                    if (pelanggan->getSaldo() < biaya) {
                        Tampilan::printError("Saldo tidak mencukupi! Silakan isi saldo terlebih dahulu.");
                        Tampilan::pause();
                        break;
                    }

                    pelanggan->setSaldo(pelanggan->getSaldo() - biaya);
                    manajerPelanggan.simpanData();

                    checkoutTiket(idTiket);

                    Tampilan::printMessage("Checkout berhasil!", Tampilan::GREEN);
                    cout << "Sisa saldo: Rp" << pelanggan->getSaldo() << endl;
                    Tampilan::pause();
                    break;
                }
                case 4: 
                    Tampilan::printHeader("Kendaraan Parkir Aktif");
                    sistemParkir.tampilkanTiketAktif();
                    Tampilan::pause();
                    break;
                case 5: { // Cek BST
                    string plat;
                    cout << "--- Cek Poin Loyalitas (BST) ---" << endl;
                    cout << "Masukkan Plat Nomor: "; getline(cin, plat);
                    databasePoin.cariPoin(plat);
                    Tampilan::pause();
                    break;
                }
                case 6:
                    Tampilan::printHeader("Denah Parkir");
                    tampilkanPeta();
                    Tampilan::pause();
                    break;
                case 7:
                    Tampilan::printHeader("Riwayat Parkir Terakhir");
                    sistemParkir.tampilkanRiwayat();
                    Tampilan::pause();
                    break;
                case 8: 
                    return;
                default:
                    Tampilan::printError("Pilihan tidak valid.");
                    Tampilan::pause();
                    break;
            }
        }
    }
};

#endif // MANAJER_PARKIR_H