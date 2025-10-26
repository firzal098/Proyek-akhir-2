#ifndef PARKING_MANAGER_H
#define PARKING_MANAGER_H

#include <iostream>       // <-- Pastikan semua include ini ada
#include <string>
#include <limits>
#include <conio.h>
#include "ParkingSystem.h" // <-- Penting untuk logika parkir

// Kita gunakan std namespace di sini untuk keringkasan
using namespace std;

class ParkingManager {
private:
    ParkingSystem parkingSys; // Buat instance dari ParkingSystem

public:
    // KITA AKAN MELETAKKAN SEMUA LOGIKA DI DALAM FUNGSI INI
    void displayParkingMenu() {
        int pilihan = 0;

        // Loop sub-menu parkir, akan berhenti jika pengguna memilih 4
        while (pilihan != 4) {
            system("cls");
            std::cout << "\n===== Manajemen Parkir =====" << std::endl;
            std::cout << "1. Masuk Parkir" << std::endl;
            std::cout << "2. Keluar Parkir" << std::endl;
            std::cout << "3. Tampilkan Status Parkir" << std::endl;
            std::cout << "4. Kembali ke Menu Utama" << std::endl;
            std::cout << "============================" << std::endl;
            std::cout << "Pilih opsi: ";

            std::cin >> pilihan;

            if (std::cin.fail()) {
                std::cout << "Input tidak valid. Harap masukkan angka." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                getch();
                continue; // Ulangi loop sub-menu
            }

            // Hubungkan pilihan ke fungsi ParkingSystem
            switch (pilihan) {
                case 1: { // Masuk Parkir
                    system("cls");
                    int customerID;
                    std::string platNomor;
                    
                    std::cout << "--- Masuk Parkir ---" << std::endl;
                    std::cout << "Masukkan ID Customer (angka): "; 
                    std::cin >> customerID;
                    
                    if (std::cin.fail()) {
                         std::cout << "Input ID tidak valid." << std::endl;
                         std::cin.clear();
                         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                         getch();
                         break;
                    }

                    std::cout << "Masukkan Plat Nomor: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear buffer
                    std::getline(std::cin, platNomor);
                    
                    parkingSys.createTicket(customerID, platNomor); // Panggil fungsi ParkingSystem
                    getch();
                    break;
                }
                case 2: { // Keluar Parkir
                    system("cls");
                    uint64_t ticketID; 
                    std::cout << "--- Keluar Parkir ---" << std::endl;
                    std::cout << "Masukkan ID Tiket: ";
                    std::cin >> ticketID;

                    if (std::cin.fail()) {
                         std::cout << "Input ID Tiket tidak valid." << std::endl;
                         std::cin.clear();
                         std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                         getch();
                         break;
                    }

                    parkingSys.checkout(ticketID); // Panggil fungsi ParkingSystem
                    getch();
                    break;
                }
                case 3: // Tampilkan Status
                    system("cls");
                    parkingSys.displayActiveTickets(); // Panggil fungsi ParkingSystem
                    getch();
                    break;
                case 4: // Kembali
                    std::cout << "Kembali ke Menu Utama..." << std::endl;
                    // getch() akan ditangani oleh loop di main.cpp
                    break;
                default:
                    std::cout << "Pilihan tidak valid. Silakan coba lagi." << std::endl;
                    getch();
                    break;
            }
        }
    }
};

#endif // PARKING_MANAGER_H