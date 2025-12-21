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
#include <iostream>
#include <string>
#include <limits>
#include <conio.h> // Untuk getch()

using namespace std;

// --- Deklarasi Fungsi ---
// Mendeklarasikan semua fungsi menu agar bisa dipanggil sebelum definisinya.
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
    system("cls");
    cout << "=== Beri Saldo ke Pelanggan ===\n";
    
    cout << "\n--- Daftar Pelanggan ---\n";
    manajerPelanggan.tampilkanSemuaPelanggan();

    string idPelanggan;
    cout << "\nMasukkan ID Pelanggan yang akan diberi saldo: ";
    getline(cin, idPelanggan);

    Pelanggan* pelanggan = manajerPelanggan.cariPelanggan(idPelanggan);
    if (!pelanggan) {
        cout << "Pelanggan dengan ID " << idPelanggan << " tidak ditemukan.\n";
        getch();
        return;
    }

    double jumlahSaldo;
    cout << "Masukkan jumlah saldo yang akan ditambahkan: ";
    cin >> jumlahSaldo;
    cin.ignore();

    if (jumlahSaldo <= 0) {
        cout << "Jumlah saldo harus positif.\n";
        getch();
        return;
    }

    // Update saldo dan simpan
    pelanggan->setSaldo(pelanggan->getSaldo() + jumlahSaldo);
    manajerPelanggan.simpanData();

    cout << "\n>> Saldo berhasil ditambahkan! <<\n";
    cout << "Saldo baru untuk " << pelanggan->getNama() << " adalah Rp" << pelanggan->getSaldo() << endl;
    getch();
}

/**
 * @brief Menampilkan menu belanja (toko) untuk pelanggan.
 * Pelanggan dapat melihat daftar vendor, memilih vendor, melihat produk, dan melakukan pembelian.
 * @param pelanggan Pointer ke objek pelanggan yang sedang login.
 * @param manajerVendor Referensi ke manajer vendor untuk data vendor dan produk.
 * @param manajerPelanggan Referensi ke manajer pelanggan untuk menyimpan perubahan saldo.
 */
void menuToko(Pelanggan* pelanggan, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan) {
    system("cls");
    cout << "=== Selamat Datang di Toko Vendor ===\n";
    cout << "Saldo Anda: Rp" << pelanggan->getSaldo() << endl;

    // 1. Tampilkan daftar vendor
    cout << "\n--- Daftar Vendor Tersedia ---\n";
    manajerVendor.tampilkanSemua();
    
    // 2. Minta pengguna memilih vendor
    int idVendor;
    cout << "\nMasukkan ID Vendor yang ingin dikunjungi: ";
    cin >> idVendor;
    cin.ignore();

    Vendor* targetVendor = nullptr;
    DoublyLinkedList<Vendor>& daftarVendor = manajerVendor.getDaftarVendor();
    for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
        if (it->getId() == idVendor) {
            targetVendor = &(*it);
            break;
        }
    }

    if (!targetVendor) {
        cout << "Vendor tidak ditemukan.\n";
        getch();
        return;
    }

    // 3. Tampilkan produk dari vendor yang dipilih
    system("cls");
    cout << "=== Produk dari Vendor: " << targetVendor->getNama() << " ===\n";
    targetVendor->tampilkanProduk();

    // 4. Proses pembelian
    string namaProduk;
    int jumlahBeli;
    cout << "\nMasukkan nama produk yang ingin dibeli: ";
    getline(cin, namaProduk);
    cout << "Masukkan jumlah yang ingin dibeli: ";
    cin >> jumlahBeli;
    cin.ignore();

    // Cari produk dalam daftar produk vendor
    Produk* targetProduk = nullptr;
    DoublyLinkedList<Produk>& daftarProduk = targetVendor->getDaftarProduk();
    for (auto it = daftarProduk.begin(); it != daftarProduk.end(); ++it) {
        if (it->nama == namaProduk) {
            targetProduk = &(*it);
            break;
        }
    }

    if (!targetProduk) {
        cout << "Produk tidak ditemukan.\n";
        getch();
        return;
    }

    // 5. Lakukan validasi dan transaksi
    double totalHarga = targetProduk->harga * jumlahBeli;
    if (pelanggan->getSaldo() < totalHarga) {
        cout << "Transaksi gagal: Saldo tidak mencukupi.\n";
        getch();
        return;
    }
    if (targetProduk->stok < jumlahBeli) {
        cout << "Transaksi gagal: Stok tidak mencukupi.\n";
        getch();
        return;
    }

    // 6. Update data jika transaksi berhasil
    pelanggan->setSaldo(pelanggan->getSaldo() - totalHarga);
    targetProduk->stok -= jumlahBeli;

    // 7. Simpan semua perubahan ke file
    manajerPelanggan.simpanData();
    manajerVendor.simpan();

    cout << "\n>> Pembelian berhasil! <<\n";
    cout << "Sisa saldo Anda: Rp" << pelanggan->getSaldo() << endl;
    cout << "Sisa stok produk '" << targetProduk->nama << "': " << targetProduk->stok << endl;
    getch();
}

/**
 * @brief Menangani proses login dan registrasi pengguna.
 * @param manajerPelanggan Referensi ke manajer pelanggan untuk validasi dan registrasi.
 * @param pelangganKeluar Pointer ganda untuk menyimpan objek pelanggan yang berhasil login.
 * @return String yang menandakan status hasil: "admin", "pelanggan", "terdaftar", "keluar", atau "gagal".
 */
string tanganiLogin(ManajerPelanggan& manajerPelanggan, Pelanggan** pelangganKeluar) {
    string pilihan;
    cout << "\n--- Selamat Datang! ---" << endl;
    cout << "1. Login" << endl;
    cout << "2. Register" << endl;
    cout << "3. Keluar" << endl;
    cout << "=======================" << endl;
    cout << "Pilihan: ";
    cin >> pilihan;
    cin.ignore(); // Membersihkan buffer setelah cin

    if (pilihan == "1") {
        string id;
        string password;
        cout << "\n--- Silakan Login ---" << endl;
        cout << "ID: ";
        getline(cin, id);
        cout << "Password: ";
        getline(cin, password);
        
        // Pengecekan kredensial admin secara hardcoded
        if (id == "admin" && password == "admin123") {
            return "admin";
        }

        // Cari pelanggan di database
        Pelanggan* ditemukan = manajerPelanggan.cariPelanggan(id);
        if (ditemukan && ditemukan->verifikasiPassword(password)) {
            *pelangganKeluar = ditemukan; // Kirim objek pelanggan yang login kembali ke main loop
            cout << "Login berhasil! Selamat datang, " << ditemukan->getNama() << endl;
            getch();
            return "pelanggan";
        } else {
            cout << "Login gagal! ID atau password salah." << endl;
            getch();
            return "gagal";
        }
    } else if (pilihan == "2") {
        manajerPelanggan.tambahPelanggan();
        return "terdaftar";
    } else if (pilihan == "3") {
        return "keluar";
    } else {
        cout << "Pilihan tidak valid." << endl;
        getch();
        return "tidak valid";
    }
}

/**
 * @brief Menampilkan menu utama untuk pelanggan yang telah login.
 * @param pelangganMasuk Pointer ke objek pelanggan yang sedang aktif.
 * @param manajerParkir Referensi ke manajer parkir.
 * @param manajerVendor Referensi ke manajer vendor.
 * @param manajerPelanggan Referensi ke manajer pelanggan.
 */
void menuKlien(Pelanggan* pelangganMasuk, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor, ManajerPelanggan& manajerPelanggan) {
    if (!pelangganMasuk) return;

    while (true) {
        system("cls");
        // Tampilkan info profil di setiap iterasi menu
        pelangganMasuk->tampilkanInfo(); 
        
        cout << "\n=== MENU PELANGGAN ===" << endl;
        cout << "1. Masuk Menu Parkir" << endl; 
        cout << "2. Jelajahi Vendor (Toko)\n";
        cout << "3. Logout" << endl;
        cout << "Pilihan: ";
        int pilihan;
        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {
            case 1:
                manajerParkir.tampilkanMenu();
                break;
            case 2:
                menuToko(pelangganMasuk, manajerVendor, manajerPelanggan);
                break;
            case 3:
                return; // Keluar dari loop dan kembali ke menu login
            default:
                cout << "Pilihan tidak valid." << endl;
                getch();
        }
    }
}

/**
 * @brief Menampilkan menu utama untuk admin.
 * @param manajerPelanggan Referensi ke manajer pelanggan.
 * @param manajerParkir Referensi ke manajer parkir.
 * @param manajerVendor Referensi ke manajer vendor.
 */
void menuAdmin(ManajerPelanggan& manajerPelanggan, ManajerParkir& manajerParkir, ManajerVendor& manajerVendor) {
    while (true) {
        system("cls");
        cout << "\n=== MENU ADMIN ===" << endl;
        cout << "1. Lihat Semua Pelanggan" << endl;
        cout << "2. Manajemen Parkir" << endl;
        cout << "3. Manajemen Vendor" << endl; 
        cout << "4. Beri Saldo ke Pelanggan" << endl;
        cout << "5. Logout" << endl;
        cout << "Pilihan: ";
        int pilihan;
        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {
            case 1:
                manajerPelanggan.tampilkanSemuaPelanggan();
                getch();
                break;
            case 2:
                manajerParkir.tampilkanMenu();
                break;
            case 3:
               manajerVendor.jalankan();
               break;
            case 4:
                menuBeriSaldo(manajerPelanggan);
                break;
            case 5:
                return; // Keluar dari loop dan kembali ke menu login
            default:
                cout << "Pilihan tidak valid." << endl;
                getch();
        }
    }
}

/**
 * @brief Fungsi main, titik awal dari eksekusi program.
 */
int main() {
    // Inisialisasi semua kelas manajer
    ManajerPelanggan manajerPelanggan("pelanggan.db"); 
    ManajerParkir manajerParkir;
    ManajerVendor manajerVendor; 

    // Loop aplikasi utama
    while (true) {
        system("cls");
        Pelanggan* pelangganMasuk = nullptr; // Pointer untuk menampung pelanggan yang login
        string hasilLogin = tanganiLogin(manajerPelanggan, &pelangganMasuk);

        // Arahkan ke menu yang sesuai berdasarkan hasil login
        if (hasilLogin == "admin") {
            menuAdmin(manajerPelanggan, manajerParkir, manajerVendor);
        } else if (hasilLogin == "pelanggan") {
            menuKlien(pelangganMasuk, manajerParkir, manajerVendor, manajerPelanggan);
        } else if (hasilLogin == "keluar") {
            cout << "Terima kasih telah menggunakan sistem. Sampai jumpa!" << endl;
            getch();
            break; // Hentikan loop aplikasi
        }
        // Jika hasil 'terdaftar' atau 'gagal', loop akan berlanjut ke menu login lagi
    }

    return 0;
}
