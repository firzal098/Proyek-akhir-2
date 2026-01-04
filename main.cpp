/**
 * @file main.cpp
 * @brief Titik masuk utama dan pengendali alur aplikasi.
 *
 * File ini berisi fungsi `main` yang menjalankan loop aplikasi utama,
 * serta semua fungsi yang menampilkan menu untuk admin dan pelanggan (klien).
 * File ini mengorkestrasi interaksi antara berbagai manajer (Pelanggan, Parkir, Vendor).
 */

#include "CustomerManager.h"
#include "ParkingManager.h"
#include "VendorManager.h" 
#include "Tampilan.hpp"       // <-- Include library Tampilan
#include <iostream>
#include <string>
#include <limits>

using namespace std;


// --- Deklarasi Fungsi ---
void menuBeriSaldo(ManajerPelanggan& manajerPelanggan);
void menuToko(Pelanggan* pelanggan, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan);
string tanganiLogin(ManajerPelanggan& manajerPelanggan, Pelanggan** pelangganKeluar);
void menuKlien(Pelanggan* pelangganMasuk, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan);
void menuAdmin(ManajerPelanggan& manajerPelanggan, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor); 

// --- Implementasi Fungsi ---

/**
 * @brief Menampilkan menu untuk admin agar dapat menambahkan saldo ke akun pelanggan.
 * @param manajerPelanggan Referensi ke manajer pelanggan untuk mengakses data pelanggan.
 */
void menuBeriSaldo(ManajerPelanggan& manajerPelanggan) {
    Tampilan::printHeader("Beri Saldo");
    
    cout << "\n--- Daftar Pelanggan ---\n";
    manajerPelanggan.tampilkanSemuaPelanggan();

    string idPelanggan = Tampilan::getString("\nMasukkan ID Pelanggan yang akan diberi saldo");

    Pelanggan* pelanggan = manajerPelanggan.cariPelanggan(idPelanggan);
    if (!pelanggan) {
        Tampilan::printError("Pelanggan dengan ID " + idPelanggan + " tidak ditemukan.");
        Tampilan::pause();
        return;
    }

    cout << Tampilan::BOLD << Tampilan::YELLOW << "-> Masukkan jumlah saldo yang akan ditambahkan: " << Tampilan::RESET;
    double jumlahSaldo;
    cin >> jumlahSaldo;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');


    if (jumlahSaldo <= 0) {
        Tampilan::printError("Jumlah saldo harus positif.");
        Tampilan::pause();
        return;
    }

    pelanggan->setSaldo(pelanggan->getSaldo() + jumlahSaldo);
    manajerPelanggan.simpanData();

    Tampilan::printMessage("Saldo berhasil ditambahkan!", Tampilan::GREEN);
    cout << "Saldo baru untuk " << pelanggan->getNama() << " adalah Rp" << pelanggan->getSaldo() << endl;
    Tampilan::pause();
}

/**
 * @brief Menampilkan menu belanja (toko) untuk pelanggan.
 */
void menuToko(Pelanggan* pelanggan, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan) {
    Tampilan::printHeader("Toko Vendor");
    cout << "Saldo Anda: " << Tampilan::GREEN << "Rp" << pelanggan->getSaldo() << Tampilan::RESET << endl;

    cout << "\n--- Daftar Vendor Tersedia ---\n";
    manajerVendor.tampilkanSemua();
    
    int idVendor = Tampilan::getChoice();

    Vendor* targetVendor = nullptr;
    DoublyLinkedList<Vendor>& daftarVendor = manajerVendor.getDaftarVendor();
    for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
        if (it->getId() == idVendor) {
            targetVendor = &(*it);
            break;
        }
    }

    if (!targetVendor) {
        Tampilan::printError("Vendor tidak ditemukan.");
        Tampilan::pause();
        return;
    }

    Tampilan::printHeader("Produk Vendor: " + targetVendor->getNama());
    targetVendor->tampilkanProduk();

    string namaProduk = Tampilan::getString("Masukkan nama produk yang ingin dibeli");
    cout << Tampilan::BOLD << Tampilan::YELLOW << "-> Masukkan jumlah yang ingin dibeli: " << Tampilan::RESET;
    int jumlahBeli;
    cin >> jumlahBeli;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Produk* targetProduk = nullptr;
    DoublyLinkedList<Produk>& daftarProduk = targetVendor->getDaftarProduk();
    for (auto it = daftarProduk.begin(); it != daftarProduk.end(); ++it) {
        if (it->nama == namaProduk) {
            targetProduk = &(*it);
            break;
        }
    }

    if (!targetProduk) {
        Tampilan::printError("Produk tidak ditemukan.");
        Tampilan::pause();
        return;
    }

    double totalHarga = targetProduk->harga * jumlahBeli;
    if (pelanggan->getSaldo() < totalHarga) {
        Tampilan::printError("Transaksi gagal: Saldo tidak mencukupi.");
        Tampilan::pause();
        return;
    }
    if (targetProduk->stok < jumlahBeli) {
        Tampilan::printError("Transaksi gagal: Stok tidak mencukupi.");
        Tampilan::pause();
        return;
    }

    pelanggan->setSaldo(pelanggan->getSaldo() - totalHarga);
    targetProduk->stok -= jumlahBeli;

    manajerPelanggan.simpanData();
    manajerVendor.simpan();

    Tampilan::printMessage("Pembelian berhasil!", Tampilan::GREEN);
    cout << "Sisa saldo Anda: Rp" << pelanggan->getSaldo() << endl;
    cout << "Sisa stok produk '" << targetProduk->nama << "': " << targetProduk->stok << endl;
    Tampilan::pause();
}

/**
 * @brief Menangani proses login dan registrasi pengguna.
 */
string tanganiLogin(ManajerPelanggan& manajerPelanggan, Pelanggan** pelangganKeluar) {
    Tampilan::printHeader("Selamat Datang");
    DoublyLinkedList<string> menuItems;
    menuItems.push_back("Login");
    menuItems.push_back("Register");
    menuItems.push_back("Keluar");
    Tampilan::printMenu(menuItems);
    int pilihan = Tampilan::getChoice();

    if (pilihan == 1) {
        Tampilan::printHeader("Login");
        string id = Tampilan::getString("ID");
        string password = Tampilan::getString("Password");
        
        if (id == "admin" && password == "admin123") {
            Tampilan::printMessage("Login admin berhasil!", Tampilan::GREEN);
            Tampilan::pause();
            return "admin";
        }

        Pelanggan* ditemukan = manajerPelanggan.cariPelanggan(id);
        if (ditemukan && ditemukan->verifikasiPassword(password)) {
            *pelangganKeluar = ditemukan; 
            Tampilan::printMessage("Login berhasil! Selamat datang, " + ditemukan->getNama(), Tampilan::GREEN);
            Tampilan::pause();
            return "pelanggan";
        } else {
            Tampilan::printError("Login gagal! ID atau password salah.");
            Tampilan::pause();
            return "gagal";
        }
    } else if (pilihan == 2) {
        manajerPelanggan.tambahPelanggan();
        return "terdaftar";
    } else if (pilihan == 3) {
        return "keluar";
    } else {
        Tampilan::printError("Pilihan tidak valid.");
        Tampilan::pause();
        return "tidak valid";
    }
}

/**
 * @brief Menampilkan menu utama untuk pelanggan yang telah login.
 */
void menuKlien(Pelanggan* pelangganMasuk, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan) {
    if (!pelangganMasuk) return;

    while (true) {
        Tampilan::clearScreen();
        cout << Tampilan::CYAN << "Profil Anda:" << Tampilan::RESET << endl;
        pelangganMasuk->tampilkanInfo(); 
        
        DoublyLinkedList<string> menuItems;
        menuItems.push_back("Masuk Menu Parkir");
        menuItems.push_back("Jelajahi Vendor (Toko)");
        menuItems.push_back("Logout");
        Tampilan::printMenu(menuItems);
        int pilihan = Tampilan::getChoice();

        switch (pilihan) {
            case 1:
                manajerParkir.tampilkanMenu(pelangganMasuk, manajerPelanggan);
                break;
            case 2:
                menuToko(pelangganMasuk, manajerVendor, manajerPelanggan);
                break;
            case 3:
                return;
            default:
                Tampilan::printError("Pilihan tidak valid.");
                Tampilan::pause();
        }
    }
}

/**
 * @brief Menampilkan menu utama untuk admin.
 */
void menuAdmin(ManajerPelanggan& manajerPelanggan, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor) {
    while (true) {
        Tampilan::printHeader("Menu Admin");
        DoublyLinkedList<string> menuItems;
        menuItems.push_back("Lihat Semua Pelanggan");
        menuItems.push_back("Manajemen Parkir");
        menuItems.push_back("Manajemen Vendor");
        menuItems.push_back("Beri Saldo ke Pelanggan");
        menuItems.push_back("Logout");
        Tampilan::printMenu(menuItems);
        int pilihan = Tampilan::getChoice();

        switch (pilihan) {
            case 1:
                Tampilan::printHeader("Daftar Pelanggan");
                manajerPelanggan.tampilkanSemuaPelanggan();
                Tampilan::pause();
                break;
            case 2:
                manajerParkir.tampilkanMenu(nullptr, manajerPelanggan);
                break;
            case 3:
               manajerVendor.jalankan();
               break;
            case 4:
                menuBeriSaldo(manajerPelanggan);
                break;
            case 5:
                return;
            default:
                Tampilan::printError("Pilihan tidak valid.");
                Tampilan::pause();
        }
    }
}

/**
 * @brief Fungsi main, titik awal dari eksekusi program.
 */
int main() {
    ManajerPelanggan manajerPelanggan("pelanggan.db"); 
    ManajerParkir manajerParkir;
    ManajerVendor manajerVendor; 

    while (true) {
        Pelanggan* pelangganMasuk = nullptr; 
        string hasilLogin = tanganiLogin(manajerPelanggan, &pelangganMasuk);

        if (hasilLogin == "admin") {
            menuAdmin(manajerPelanggan, manajerParkir, manajerVendor);
        } else if (hasilLogin == "pelanggan") {
            menuKlien(pelangganMasuk, manajerParkir, manajerVendor, manajerPelanggan);
        } else if (hasilLogin == "keluar") {
            Tampilan::clearScreen();
            Tampilan::printMessage("Terima kasih telah menggunakan sistem. Sampai jumpa!", Tampilan::MAGENTA);
            Tampilan::pause();
            break;
        }
    }

    return 0;
}
