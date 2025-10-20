#include "CustomerManager.h"
#include <iostream>
#include <string>
#include <limits>

void tampilkanMenu() {
    std::cout << "\n===== Sistem Manajemen Customer Mall =====" << std::endl;
    std::cout << "1. Tambah Customer Baru" << std::endl;
    std::cout << "2. Tampilkan Semua Customer" << std::endl;
    std::cout << "3. Cari Customer (berdasarkan ID)" << std::endl;
    std::cout << "4. Keluar" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Pilih opsi: ";
}

int main() {
    // BARU: Saat membuat objek CustomerManager, kita berikan nama file database-nya.
    // File "customers.json" akan dibuat di folder yang sama dengan main.cpp.
    CustomerManager customerMgr("customers.json");
    
    int pilihan = 0;

    // Logika menu utama tidak perlu diubah sama sekali
    while (pilihan != 4) {
        tampilkanMenu();
        std::cin >> pilihan;

        if (std::cin.fail()) {
            std::cout << "Input tidak valid. Harap masukkan angka." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (pilihan) {
            case 1:
                customerMgr.tambahCustomer();
                break;
            case 2:
                customerMgr.tampilkanSemuaCustomer();
                break;
            case 3: {
                std::string idCari;
                std::cout << "Masukkan ID Customer yang ingin dicari: ";
                std::cin >> idCari;
                Customer* foundCustomer = customerMgr.cariCustomer(idCari);
                if (foundCustomer != nullptr) {
                    std::cout << "\n>> Customer Ditemukan! <<" << std::endl;
                    foundCustomer->displayInfo();
                } else {
                    std::cout << ">> Customer dengan ID '" << idCari << "' tidak ditemukan. <<" << std::endl;
                }
                break;
            }
            case 4:
                std::cout << "Terima kasih telah menggunakan sistem. Sampai jumpa!" << std::endl;
                break;
            default:
                std::cout << "Pilihan tidak valid. Silakan coba lagi." << std::endl;
                break;
        }
    }

    return 0;
}

