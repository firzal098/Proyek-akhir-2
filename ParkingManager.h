#ifndef PARKING_MANAGER_H
#define PARKING_MANAGER_H

#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <conio.h>
#include <iomanip> // untuk setw
#include "ParkingSystem.h" // <-- "Bos" punya "Otak"

using namespace std;

class ParkingManager {
private:
    ParkingSystem parkingSys; // "Bos" memiliki "Otak"

    // --- PETA MILIK "BOS" ---
    int totalRows;
    int totalCols;
    vector<vector<string>> slotMap; 

    void initializeParkingMap() {
        totalRows = 5;
        totalCols = 10;
        slotMap.resize(totalRows);
        for (int i = 0; i < totalRows; ++i) {
            slotMap[i].resize(totalCols);
            for (int j = 0; j < totalCols; ++j) {
                char rowChar = 'A' + i;
                slotMap[i][j] = rowChar + to_string(j + 1);
            }
        }
    }

public:
    // --- FUNGSI PETA (Milik "Bos") ---
    void displayParkingMap() {
        system("cls");
        cout << "== PETA STATUS PARKIR ==\n\n";
        for (int i = 0; i < totalRows; ++i) {
            for (int j = 0; j < totalCols; ++j) {
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
        int rowIndex = rowChar - 'A';
        int colIndex = colNum - 1;
        if (rowIndex >= 0 && rowIndex < totalRows && colIndex >= 0 && colIndex < totalCols) {
            slotMap[rowIndex][colIndex] = slotName;
        }
    }

    // --- KONSTRUKTOR ---
    ParkingManager() {
        initializeParkingMap();
        // parkingSys otomatis dibuat
    }

    // --- FUNGSI MENU (Satu-satunya fungsi yang dipanggil main.cpp) ---
    void displayParkingMenu() {
        int pilihan = 0;
        while (pilihan != 4) {
            system("cls");
            displayParkingMap(); // Tampilkan peta milik sendiri

            cout << "\n===== Manajemen Parkir =====" << endl;
            cout << "1. Masuk Parkir" << endl;
            cout << "2. Keluar Parkir" << endl;
            cout << "3. Tampilkan Status Parkir" << endl;
            cout << "4. Kembali ke Menu Utama" << endl;
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
                    displayParkingMap(); // Tampilkan peta lagi
                    string chosenSlot;
                    while (true) {
                        cout << "\nMasukkan slot parkir pilihan (misal: A1): ";
                        getline(cin, chosenSlot);
                        if (isSlotValid(chosenSlot)) { // Cek peta sendiri
                            break;
                        } else {
                            cout << "Slot tidak valid atau terisi. Coba lagi." << endl;
                        }
                    }

                    string platNomor;
                    cout << "Masukkan Plat Nomor: ";
                    getline(cin, platNomor);

                    // --- INI PERBAIKAN UNTUK ERROR 'isCustomer' ---
                    int customerID;
                    cout << "Masukkan ID Customer (angka, ketik -1 jika non-member): ";
                    cin >> customerID;
                    if (cin.fail()) { /* (error handling) */ break; }
                    
                    // 1. Panggil "Otak" untuk buat tiket
                    bool sukses = parkingSys.createTicket(customerID, platNomor, chosenSlot);
                    
                    // 2. Jika "Otak" berhasil, ubah "Peta"
                    if (sukses) {
                        occupySlot(chosenSlot); // Panggil fungsi peta sendiri
                    }

                    getch();
                    break;
                }
                case 2: { // Keluar Parkir
                    uint64_t ticketID;
                    cout << "--- Keluar Parkir ---" << endl;
                    cout << "Masukkan ID Tiket: ";
                    cin >> ticketID;
                    if (cin.fail()) { /* (error handling) */ break; }

                    // 1. Panggil "Otak" untuk checkout
                    string slotYgKosong = parkingSys.checkout(ticketID);

                    // 2. Jika "Otak" berhasil, ubah "Peta"
                    if (!slotYgKosong.empty()) {
                        vacateSlot(slotYgKosong); // Panggil fungsi peta sendiri
                    }
                    getch();
                    break;
                }
                case 3: // Tampilkan Status
                    parkingSys.displayActiveTickets(); // Panggil "Otak"
                    getch();
                    break;
                case 4: // Kembali
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