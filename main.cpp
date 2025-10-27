#include "CustomerManager.h"
#include "ParkingManager.h"
#include "VendorManager.h"
#include <iostream>
#include <string>
#include <limits>
#include <conio.h>
#include "vendor.h"

void tampilkanMenu() {

    std::cout << "\n===== Sistem Manajemen Customer Mall =====" << std::endl;

    std::cout << "1. Tambah Customer Baru" << std::endl;
    std::cout << "2. Tampilkan Semua Customer" << std::endl;
    std::cout << "3. Cari Customer (berdasarkan ID)" << std::endl;
    std::cout << "4. Parkir" << std::endl;
    std::cout << "5. Vendor" << std::endl;
    std::cout << "6. Keluar" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Pilih opsi: ";
    std::cout << "wajib angka (1,2,3,4,5,6) ";
}

void admin() {
    // BARU: Saat membuat objek CustomerManager, kita berikan nama file database-nya.
    // File "customers.json" akan dibuat di folder yang sama dengan main.cpp.
    CustomerManager customerMgr("customers"); // Menggunakan direktori "customers"
    ParkingManager parkingMgr;
    AplikasiVendor vendorMgr;
    
    int pilihan = 0;

    // Logika menu utama tidak perlu diubah sama sekali
    while (pilihan != 6) {
        
        system("cls");
        customerMgr.logJumlahCustomer();
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
                system("cls");
                customerMgr.tambahCustomer();
                getch();
                break;
            case 2:
                system("cls");
                customerMgr.tampilkanSemuaCustomer();
                getch();
                break;
            case 3: {
                system("cls");
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
                getch();
                break;
            }
            case 4:
                system("cls");
                parkingMgr.displayParkingMenu();
                getch();
                break;
            case 5:
                system("cls");
                vendorMgr.jalankan();
                getch();
                break;
            case 6:
                std::cout << "Terima kasih telah menggunakan sistem. Sampai jumpa!" << std::endl;
                getch();
                break;
            default:
                std::cout << "Pilihan tidak valid. Silakan coba lagi." << std::endl;
                getch();
                break;
        }
    }

}

void client() {

}
 
int main() {

    admin();

}
