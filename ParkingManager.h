#ifndef PARKING_MANAGER_H
#define PARKING_MANAGER_H

#include <iostream>
#include <string>
#include <limits>
#include <conio.h>
#include <iomanip> // untuk setw
#include <map>     // <-- TAMBAHKAN INI
#include <fstream> // <-- TAMBAHKAN INI
#include "ParkingSystem.h"
#include "json.hpp" // <-- TAMBAHKAN INI
#include "StaticArray.hpp" // <-- TAMBAHKAN INI

using namespace std;
using json = nlohmann::json;

class ParkingManager {
private:
    ParkingSystem parkingSys; // "Bos" memiliki "Otak"

    // --- PETA MILIK "BOS" ---
    StaticArray<StaticArray<string, 10>, 5> slotMap; 

    // --- DATABASE POIN MILIK "BOS" ---
    map<string, int> loyaltyPoints; // Map [PlatNomor -> Poin]
    const string loyaltyFile = "loyalty_points.json";

    // --- FUNGSI HELPER PETA ---
    void initializeParkingMap() {
        for (size_t i = 0; i < slotMap.size(); ++i) {
            for (size_t j = 0; j < slotMap[i].size(); ++j) {
                char rowChar = 'A' + i;
                slotMap[i][j] = rowChar + to_string(j + 1);
            }
        }
    }

    // --- FUNGSI HELPER POIN (BARU) ---
    void loadLoyaltyPoints() {
        ifstream file(loyaltyFile);
        if (file.is_open()) {
            json j_points;
            file >> j_points;
            // Konversi dari JSON object ke std::map
            loyaltyPoints = j_points.get<map<string, int>>(); 
            file.close();
            cout << "Data poin loyalitas dimuat." << endl;
        } else {
            cout << "File poin loyalitas tidak ditemukan, akan dibuat baru." << endl;
        }
    }

    void saveLoyaltyPoints() {
        ofstream file(loyaltyFile);
        if (file.is_open()) {
            json j_points(loyaltyPoints); // Konversi dari std::map ke JSON
            file << j_points.dump(4);
            file.close();
        } else {
            cerr << "Gagal menyimpan poin loyalitas!" << endl;
        }
    }

public:
    // --- FUNGSI PETA (Milik "Bos") ---
    // (Salin fungsi displayParkingMap, isSlotValid, occupySlot, vacateSlot kamu ke sini)
    void displayParkingMap() {
        system("cls");
        cout << "== PETA STATUS PARKIR ==\n\n";
        for (size_t i = 0; i < slotMap.size(); ++i) {
            for (size_t j = 0; j < slotMap[i].size(); ++j) {
                cout << "[" << setw(3) << slotMap[i][j] << "] ";
            }
            cout << "\n";
            }
        cout << "\n[XX] = Terisi\n";
    }

    bool isSlotValid(string slotName) {
        for (auto& row : slotMap) {
            for (auto& slot : row) {
                if (slot == slotName) return true;
            }
        }
        return false; // Tidak ada atau sudah "XX"
    }
    void occupySlot(string slotName) {
        for (auto& row : slotMap) {
            for (auto& slot : row) {
                if (slot == slotName) {
                    slot = "XX";
                    return;
                }
            }
        }
    }

    void vacateSlot(string slotName) {
        char rowChar = 'A' + (slotName[0] - 'A');
        int colNum = stoi(slotName.substr(1));
        size_t rowIndex = rowChar - 'A';
        size_t colIndex = colNum - 1;
        if (rowIndex >= 0 && rowIndex < slotMap.size() && colIndex >= 0 && colIndex < slotMap[0].size()) {
            slotMap[rowIndex][colIndex] = slotName;
        }
    }

    // --- KONSTRUKTOR ---
    ParkingManager() {
        initializeParkingMap();
        loadLoyaltyPoints(); // <-- PANGGIL FUNGSI LOAD POIN DI SINI
        // parkingSys otomatis dibuat
    }

    // --- FUNGSI MENU UTAMA ---
    void displayParkingMenu() {
        int pilihan = 0;
        // UBAH LOOP: Sekarang sampai 5
        while (pilihan != 5) { 
            system("cls");
            displayParkingMap();

            cout << "\n===== Manajemen Parkir =====" << endl;
            cout << "1. Masuk Parkir" << endl;
            cout << "2. Keluar Parkir" << endl;
            cout << "3. Tampilkan Status Parkir" << endl;
            cout << "4. Cek Poin Loyalitas (BARU)" << endl; // <-- MENU BARU
            cout << "5. Kembali ke Menu Utama" << endl;     // <-- KEMBALI JADI 5
            cout << "============================" << endl;
            cout << "Pilih opsi: ";

            cin >> pilihan;

            if (cin.fail()) {
                cout << "Input tidak valid." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getch();
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer

            switch (pilihan) {
                case 1: { // Masuk Parkir
                    // (Logika Masuk Parkir kamu sudah benar, tidak perlu diubah)
                    // ... (minta slot, plat, id customer) ...
                    // ... (panggil parkingSys.createTicket(...) dan occupySlot(...)) ...
                    
                    // Salin kode 'case 1' kamu dari versi sebelumnya ke sini
                    displayParkingMap();
                    string chosenSlot;
                    while (true) {
                        cout << "\nMasukkan slot parkir pilihan (misal: A1): ";
                        getline(cin, chosenSlot);
                        if (isSlotValid(chosenSlot)) { break; } 
                        else { cout << "Slot tidak valid atau terisi. Coba lagi." << endl; }
                    }
                    string platNomor;
                    cout << "Masukkan Plat Nomor: ";
                    getline(cin, platNomor);
                    int customerID;
                    cout << "Masukkan ID Customer (angka, ketik -1 jika non-member): ";
                    cin >> customerID;
                    if (cin.fail()) { /* ... */ break; }
                    
                    bool sukses = parkingSys.createTicket(customerID, platNomor, chosenSlot);
                    if (sukses) {
                        occupySlot(chosenSlot);
                    }
                    getch();
                    break;
                }
                
                // --- AWAL PERUBAHAN ---
                case 2: { // Keluar Parkir
                    uint64_t ticketID;
                    cout << "--- Keluar Parkir ---" << endl;
                    cout << "Masukkan ID Tiket: ";
                    cin >> ticketID;
                    if (cin.fail()) { /* (error handling) */ break; }

                    // 1. Panggil "Otak" untuk checkout
                    ParkingTicket tiketSelesai = parkingSys.selesaikanCheckout(ticketID);

                    // 2. Cek apakah checkout berhasil (ID != 0)
                    if (tiketSelesai.ticketID != 0) {
                        
                        // 3. Kosongkan slot di peta
                        vacateSlot(tiketSelesai.parkingSlot); 

                        // 4. --- LOGIKA POIN LOYALITAS (BARU) ---
                        string plat = tiketSelesai.platNomor;
                        double biaya = tiketSelesai.biaya;
                        
                        // Hitung poin (Misal: 1 poin per 1000 rupiah)
                        int poinDidapat = static_cast<int>(biaya / 1000);
                        
                        if (poinDidapat > 0) {
                            // Jika plat belum ada di database, buat baru
                            if (loyaltyPoints.find(plat) == loyaltyPoints.end()) {
                                loyaltyPoints[plat] = 0;
                            }
                            // Tambah poin
                            loyaltyPoints[plat] += poinDidapat;

                            cout << "Kendaraan " << plat << " mendapat " << poinDidapat << " poin!" << endl;
                            cout << "Total poin sekarang: " << loyaltyPoints[plat] << endl;

                            // 5. Simpan database poin ke file
                            saveLoyaltyPoints();
                        }
                    }
                    getch();
                    break;
                }
                // --- AKHIR PERUBAHAN ---

                case 3: // Tampilkan Status
                    parkingSys.displayActiveTickets();
                    getch();
                    break;
                
                // --- MENU BARU ---
                case 4: { // Cek Poin Loyalitas
                    string plat;
                    cout << "--- Cek Poin Loyalitas ---" << endl;
                    cout << "Masukkan Plat Nomor: ";
                    getline(cin, plat);
                    
                    if (loyaltyPoints.find(plat) != loyaltyPoints.end()) {
                        cout << "Kendaraan " << plat << " memiliki " << loyaltyPoints[plat] << " poin." << endl;
                    } else {
                        cout << "Kendaraan " << plat << " belum memiliki poin." << endl;
                    }
                    getch();
                    break;
                }

                case 5: // Kembali
                    cout << "Kembali ke Menu Utama..." << endl;
                    break;
                default:
                    cout << "Pilihan tidak valid." << endl;
                    getch();
                    break;
            }
        }
    }
};

#endif // PARKING_MANAGER_H