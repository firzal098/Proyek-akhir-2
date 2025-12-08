#include "CustomerManager.h"
#include "ParkingManager.h"
#include "VendorManager.h"
#include "vendor.h"
#include <iostream>
#include <string>
#include <limits>
#include <conio.h>

using namespace std;

// Forward declarations for functions now in this file
string handleLogin(CustomerManager& customerMgr, Customer** customerOut);
void clientMenu(Customer* loggedInCustomer, ParkingManager& parkingMgr);
void adminMenu(CustomerManager& customerMgr, ParkingManager& parkingMgr, AplikasiVendor& vendorMgr);

// --- Implementation from client.cpp ---

// Function to handle the login process
string handleLogin(CustomerManager& customerMgr, Customer** customerOut) {
    string choice;
    cout << "\n--- Selamat Datang! ---" << endl;
    cout << "1. Login" << endl;
    cout << "2. Register" << endl;
    cout << "3. Exit" << endl;
    cout << "=======================" << endl;
    cout << "Pilihan: ";
    cin >> choice;

    if (choice == "1") {
        string id;
        string password;
        cout << "\n--- Silakan Login ---" << endl;
        cout << "ID: ";
        cin >> id;
        cout << "Password: ";
        cin >> password;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (id == "admin" && password == "123") {
            cout << "\n>> Login sebagai Admin berhasil! <<" << endl;
            getch();
            return "admin";
        }

        Customer* foundCustomer = customerMgr.cariCustomer(id);
        if (foundCustomer != nullptr && foundCustomer->verifyPassword(password)) {
            cout << "\n>> Login sebagai Customer '" << foundCustomer->getNama() << "' berhasil! <<" << endl;
            *customerOut = foundCustomer;
            getch();
            return "customer";
        }

        cout << "\n>> ID atau Password salah. Coba lagi. <<" << endl;
        getch();
        return "failed";

    } else if (choice == "2") {
        system("cls");
        customerMgr.tambahCustomer(); // This function now handles the full registration UI
        return "registered"; // Signal to main loop that registration happened
    
    } else if (choice == "3") {
        return "exit";
    
    } else {
        cout << "\nPilihan tidak valid. Coba lagi." << endl;
        getch();
        return "failed";
    }
}

// Function to display the client menu
void clientMenu(Customer* loggedInCustomer, ParkingManager& parkingMgr) {
    if (loggedInCustomer == nullptr) {
        return;
    }

    int pilihan = 0;
    while (pilihan != 4) {
        system("cls");
        cout << "--- Selamat Datang, " << loggedInCustomer->getNama() << "! ---" << endl;
        cout << "1. Lihat Informasi Akun Saya" << endl;
        cout << "2. Cek Status Parkir Aktif" << endl;
        cout << "3. Cek Poin Loyalitas Saya" << endl;
        cout << "4. Logout" << endl;
        cout << "=================================" << endl;
        cout << "Pilih opsi: ";
        cin >> pilihan;

        if (cin.fail()) {
            cout << "Input tidak valid." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getch();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (pilihan) {
            case 1:
                system("cls");
                cout << "--- Informasi Akun Anda ---" << endl;
                loggedInCustomer->displayInfo();
                getch();
                break;
            case 2:
                system("cls");
                parkingMgr.displayParkingMenu();
                break;
            case 3: {
                system("cls");
                string plat;
                cout << "--- Cek Poin Loyalitas ---" << endl;
                cout << "Masukkan Plat Nomor Anda: ";
                getline(cin, plat);
                parkingMgr.checkLoyaltyPoints(plat);
                getch();
                break;
            }
            case 4:
                cout << "Logout berhasil. Kembali ke halaman login..." << endl;
                getch();
                break;
            default:
                cout << "Pilihan tidak valid." << endl;
                getch();
                break;
        }
    }
}


// The admin menu, refactored from the old admin() function
void adminMenu(CustomerManager& customerMgr, ParkingManager& parkingMgr, AplikasiVendor& vendorMgr) {
    int pilihan = 0;

    while (pilihan != 6) {
        system("cls");
        cout << "--- ADMIN PANEL ---" << endl;
        customerMgr.logJumlahCustomer();
        
        cout << "\n===== Sistem Manajemen Customer Mall =====" << endl;
        cout << "1. Tambah Customer Baru" << endl;
        cout << "2. Tampilkan Semua Customer" << endl;
        cout << "3. Cari Customer (berdasarkan ID)" << endl;
        cout << "4. Masuk ke Menu Parkir" << endl;
        cout << "5. Masuk ke Menu Vendor" << endl;
        cout << "6. Logout (Kembali ke Halaman Login)" << endl;
        cout << "========================================" << endl;
        cout << "Pilih opsi: ";
        cin >> pilihan;

        if (cin.fail()) {
            cout << "Input tidak valid. Harap masukkan angka." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getch();
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer for getline

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
                string idCari;
                cout << "Masukkan ID Customer yang ingin dicari: ";
                cin >> idCari;
                Customer* foundCustomer = customerMgr.cariCustomer(idCari);
                if (foundCustomer != nullptr) {
                    cout << "\n>> Customer Ditemukan! <<" << endl;
                    foundCustomer->displayInfo();
                } else {
                    cout << ">> Customer dengan ID '" << idCari << "' tidak ditemukan. <<" << endl;
                }
                getch();
                break;
            }
            case 4:
                system("cls");
                parkingMgr.displayParkingMenu();
                // getch() is handled inside displayParkingMenu loop
                break;
            case 5:
                system("cls");
                vendorMgr.jalankan();
                getch();
                break;
            case 6:
                cout << "Logout berhasil. Kembali ke halaman login..." << endl;
                getch();
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi." << endl;
                getch();
                break;
        }
    }
}

int main() {
    // Instantiate managers once
    CustomerManager customerMgr("customers");
    ParkingManager parkingMgr;
    AplikasiVendor vendorMgr;

    while (true) {
        system("cls");
        Customer* loggedInCustomer = nullptr;
        string loginResult = handleLogin(customerMgr, &loggedInCustomer);

        if (loginResult == "admin") {
            adminMenu(customerMgr, parkingMgr, vendorMgr);
        } else if (loginResult == "customer") {
            clientMenu(loggedInCustomer, parkingMgr);
        } else if (loginResult == "exit") {
            cout << "Terima kasih telah menggunakan sistem. Sampai jumpa!" << endl;
            getch();
            break;
        }
        // If login "failed" or "registered", the loop will clear screen and repeat
    }

    return 0;
}
