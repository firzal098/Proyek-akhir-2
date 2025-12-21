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

using namespace std;

// Struktur untuk data loyalitas
struct EntriLoyalitas {
    string nomorPolisi;
    int poin;

    static string serialisasi(const EntriLoyalitas& e) {
        return e.nomorPolisi + "|" + to_string(e.poin);
    }

    static EntriLoyalitas deserialisasi(const string& baris) {
        auto token = PenyimpanFile::pisah(baris, '|');
        EntriLoyalitas e;
        if (!token.empty()) {
            auto it = token.begin();
            e.nomorPolisi = *it;
            if (++it != token.end()) {
                e.poin = stoi(*it);
            } else {
                e.poin = 0;
            }
        }
        return e;
    }
};

class ManajerParkir {
private:
    SistemParkir sistemParkir; 

    // --- PETA SLOT PARKIR ---
    DoublyLinkedList<DoublyLinkedList<string>> petaSlot; 

    // --- DATABASE LOYALITAS ---
    DoublyLinkedList<EntriLoyalitas> poinLoyalitas; 
    const string fileLoyalitas = "poin_loyalitas.db";

    void inisialisasiPetaParkir() {
        petaSlot.clear(); 
        // Pengaturan grid sederhana 5x5: A1..A5, B1..B5, dst.
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
        PenyimpanFile::muatDariFile(fileLoyalitas, poinLoyalitas, EntriLoyalitas::deserialisasi);
    }

    void simpanPoinLoyalitas() {
        PenyimpanFile::simpanKeFile(fileLoyalitas, poinLoyalitas, EntriLoyalitas::serialisasi);
    }

    EntriLoyalitas* cariLoyalitas(const string& plat) {
        for (auto it = poinLoyalitas.begin(); it != poinLoyalitas.end(); ++it) {
            if (it->nomorPolisi == plat) {
                return &(*it);
            }
        }
        return nullptr;
    }

    void cekPoinLoyalitas(const string& plat) {
        EntriLoyalitas* entri = cariLoyalitas(plat);
        if (entri) {
            cout << "Poin loyalitas untuk " << plat << ": " << entri->poin << endl;
        } else {
            cout << "Belum ada data loyalitas untuk kendaraan ini." << endl;
        }
    }

public:
    ManajerParkir() {
        inisialisasiPetaParkir();
        muatPoinLoyalitas();
    }

    // --- BARU: Tampilan Peta Visual ---
    void tampilkanPeta() {
        cout << "\n================ DENAH PARKIR (5x5) ================\n";
        cout << "Legenda: [ XX ] = Terisi, [ A1 ] = Kosong\n\n";

        // Iterasi melalui baris
        for (auto itBaris = petaSlot.begin(); itBaris != petaSlot.end(); ++itBaris) {
            cout << "   ";
            // Iterasi melalui kolom di baris saat ini
            for (auto itKolom = itBaris->begin(); itKolom != itBaris->end(); ++itKolom) {
                string slot = *itKolom;
                
                if (sistemParkir.apakahSlotTerisi(slot)) {
                    // Buat menonjol
                    cout << "[ XX ] ";
                } else {
                    cout << "[ " << slot << " ] ";
                }
            }
            cout << endl;
        }
        cout << "====================================================\n";
    }

    void tampilkanMenu() {
        int pilihan;
        while (true) {
            system("cls"); 
            cout << "=== SISTEM MANAJEMEN PARKIR ===" << endl;
            cout << "1. Masuk Parkir (Check-In)" << endl;
            cout << "2. Keluar Parkir (Check-Out)" << endl;
            cout << "3. Tampilkan Kendaraan Aktif" << endl;
            cout << "4. Cek Poin Loyalitas" << endl;
            cout << "5. Tampilkan Denah Parkir" << endl; // Opsi ditambahkan
            cout << "6. Kembali" << endl;
            cout << "Pilihan: ";
            cin >> pilihan;
            cin.ignore(); 

            switch (pilihan) {
                case 1: { // Check-in
                    tampilkanPeta(); // Tampilkan peta sebelum meminta slot
                    string idCust, plat, slot;
                    cout << "\n--- Check-In ---" << endl;
                    cout << "Masukkan ID Pelanggan: "; getline(cin, idCust);
                    cout << "Masukkan Plat Nomor: "; getline(cin, plat);
                    cout << "Masukkan Slot (dari denah diatas): "; getline(cin, slot);
                    
                    sistemParkir.checkIn(idCust, plat, slot);
                    getch();
                    break;
                }
                case 2: { // Check-out
                    string idTiket;
                    cout << "--- Check-Out ---" << endl;
                    cout << "Masukkan ID Tiket: "; getline(cin, idTiket);
                    
                    TiketParkir t = sistemParkir.checkOut(idTiket);
                    if (t.status == DIBAYAR) {
                        cout << "Check-out Berhasil!" << endl;
                        cout << "Biaya Parkir: Rp " << fixed << setprecision(0) << t.biaya << endl;

                        // Tambah Poin Loyalitas
                        int poinDidapat = static_cast<int>(t.biaya / 1000);
                        if (poinDidapat > 0) {
                            string plat = t.nomorPolisi;
                            EntriLoyalitas* entri = cariLoyalitas(plat);
                            
                            if (!entri) {
                                EntriLoyalitas entriBaru{plat, poinDidapat};
                                poinLoyalitas.push_back(entriBaru);
                                cout << "Anggota Loyalitas Baru! Poin awal: " << entriBaru.poin << endl;
                            } else {
                                entri->poin += poinDidapat;
                                cout << "Kendaraan " << plat << " mendapat tambahan " << poinDidapat << " poin!" << endl;
                                cout << "Total poin sekarang: " << entri->poin << endl;
                            }
                            simpanPoinLoyalitas();
                        }
                    }
                    getch();
                    break;
                }
                case 3: 
                    sistemParkir.tampilkanTiketAktif();
                    getch();
                    break;
                case 4: { 
                    string plat;
                    cout << "--- Cek Poin Loyalitas ---" << endl;
                    cout << "Masukkan Plat Nomor: "; getline(cin, plat);
                    cekPoinLoyalitas(plat); 
                    getch();
                    break;
                }
                case 5:
                    tampilkanPeta();
                    getch();
                    break;
                case 6: 
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