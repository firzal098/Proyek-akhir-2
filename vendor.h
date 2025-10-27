#ifndef VENDOR_H
#define VENDOR_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

// --- UTILITY FUNCTION ---
string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ===== CLASS PENJUALAN =====
class Penjualan {
private:
    int idPenjualan;
    string namaProduk;
    int jumlah;
    double hargaSatuan;

public:
    Penjualan(int id = 0, string produk = "", int jml = 0, double harga = 0.0)
        : idPenjualan(id), namaProduk(produk), jumlah(jml), hargaSatuan(harga) {}

    double getTotal() const { return jumlah * hargaSatuan; }

    void tampilkan() const {
        cout << "----------------------------------\n";
        cout << "ID Penjualan : " << idPenjualan << endl;
        cout << "Produk       : " << namaProduk << endl;
        cout << "Jumlah       : " << jumlah << endl;
        cout << "Harga Satuan : " << hargaSatuan << endl;
        cout << "Total Harga  : " << getTotal() << endl;
    }
};

// ===== CLASS VENDOR (DENGAN ENKAPSULASI & PENJUALAN) =====
class Vendor {
private:
    int id;
    string namaToko;
    string namaPemilik;
    string kategori;
    string kontak;
    string status;
    vector<Penjualan> daftarPenjualan; // <-- daftar penjualan dinamis
    int nextPenjualanId = 1;

public:
    // Constructor
    Vendor(int id = 0, string toko = "", string pemilik = "", string kategori = "", string kontak = "", string status = "Aktif")
        : id(id), namaToko(toko), namaPemilik(pemilik), kategori(kategori), kontak(kontak), status(status) {}

    // Getters
    int getId() const { return id; }
    string getNamaToko() const { return namaToko; }
    string getKategori() const { return kategori; }
    string getStatus() const { return status; }
    string getKontak() const { return kontak; }
    string getNamaPemilik() const { return namaPemilik; }

    // Setters
    void setNamaToko(const string& nama) { namaToko = nama; }
    void setNamaPemilik(const string& pemilik) { namaPemilik = pemilik; }
    void setKategori(const string& kat) { kategori = kat; }
    void setKontak(const string& kont) { kontak = kont; }
    void setStatus(const string& stat) { status = stat; }

    // Menambah penjualan
    void tambahPenjualan() {
        string produk;
        int jumlah;
        double harga;

        cout << "\n=== TAMBAH PENJUALAN UNTUK " << namaToko << " ===\n";
        cout << "Nama Produk: ";
        getline(cin, produk);
        cout << "Jumlah: ";
        cin >> jumlah;
        cout << "Harga Satuan: ";
        cin >> harga;
        cin.ignore();

        daftarPenjualan.push_back(Penjualan(nextPenjualanId++, produk, jumlah, harga));
        cout << "Penjualan berhasil ditambahkan!\n";
    }

    // Menampilkan seluruh penjualan vendor
    void tampilkanPenjualan() const {
        if (daftarPenjualan.empty()) {
            cout << "Belum ada penjualan untuk vendor ini.\n";
            return;
        }
        cout << "\n=== DAFTAR PENJUALAN " << namaToko << " ===\n";
        for (const auto &p : daftarPenjualan)
            p.tampilkan();
    }

    // Menampilkan data vendor
    void tampilkan() const {
        cout << "------------------------------\n";
        cout << "ID: " << id << endl;
        cout << "Nama Toko: " << namaToko << endl;
        cout << "Pemilik: " << namaPemilik << endl;
        cout << "Kategori: " << kategori << endl;
        cout << "Kontak: " << kontak << endl;
        cout << "Status: " << status << endl;
        cout << "------------------------------\n";
    }
};

// ===== CLASS APLIKASI VENDOR =====
class AplikasiVendor {
private:
    vector<Vendor> daftarVendor;
    int nextId = 1;

    void clearCin() { 
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

public:
    void jalankan() {
        int pilihan;
        do {
            cout << "\n=== SISTEM MANAJEMEN VENDOR MALL ===\n";
            cout << "1. Tambah Vendor\n";
            cout << "2. Lihat Semua Vendor\n";
            cout << "3. Cari Vendor (Nama/Kategori)\n";
            cout << "4. Edit Detail Vendor\n";
            cout << "5. Masuk ke Menu Vendor (Penjualan)\n"; // <-- menu baru
            cout << "6. Hapus Vendor\n";
            cout << "0. Keluar\n";
            cout << "Pilih menu: ";

            if (!(cin >> pilihan)) {
                cout << "Input tidak valid.\n";
                clearCin();
                pilihan = -1;
                continue;
            }
            cin.ignore();

            switch (pilihan) {
                case 1: tambahVendor(); break;
                case 2: tampilkanSemua(); break;
                case 3: cariVendor(); break;
                case 4: editVendor(); break;
                case 5: menuVendor(); break; // <-- submenu vendor
                case 6: hapusVendor(); break;
                case 0: cout << "Terima kasih, program dihentikan.\n"; break;
                default: cout << "Pilihan tidak valid.\n"; break;
            }
        } while (pilihan != 0);
    }

private:
    // CRUD dasar
    void tambahVendor() {
        string toko, pemilik, kategori, kontak;
        cout << "\n=== TAMBAH VENDOR ===\n";
        cout << "Nama Toko: ";
        getline(cin, toko);
        cout << "Nama Pemilik: ";
        getline(cin, pemilik);
        cout << "Kategori Toko: ";
        getline(cin, kategori);
        cout << "Nomor Kontak: ";
        getline(cin, kontak);

        daftarVendor.push_back(Vendor(nextId++, toko, pemilik, kategori, kontak));
        cout << "Vendor " << toko << " berhasil ditambahkan!\n";
    }

    void tampilkanSemua() {
        if (daftarVendor.empty()) {
            cout << "Belum ada vendor terdaftar.\n";
            return;
        }
        cout << "\n=== DAFTAR VENDOR ===\n";
        for (const auto &v : daftarVendor)
            v.tampilkan();
    }

    void cariVendor() {
        cout << "\n=== CARI VENDOR ===\n";
        string cari;
        cout << "Masukkan Nama Toko atau Kategori: ";
        getline(cin, cari);
        string cariLower = toLower(cari);
        bool ditemukan = false;

        for (const auto &v : daftarVendor) {
            if (toLower(v.getNamaToko()).find(cariLower) != string::npos || 
                toLower(v.getKategori()).find(cariLower) != string::npos) {
                v.tampilkan();
                ditemukan = true;
            }
        }
        if (!ditemukan)
            cout << "Vendor tidak ditemukan.\n";
    }

    void editVendor() {
        int id;
        cout << "\nMasukkan ID Vendor: ";
        if (!(cin >> id)) {
            cout << "Input tidak valid.\n";
            clearCin();
            return;
        }
        cin.ignore();

        for (auto &v : daftarVendor) {
            if (v.getId() == id) {
                string input;
                cout << "Edit Nama Toko (" << v.getNamaToko() << "): ";
                getline(cin, input);
                if (!input.empty()) v.setNamaToko(input);
                cout << "Data vendor diperbarui.\n";
                return;
            }
        }
        cout << "Vendor tidak ditemukan.\n";
    }

    void hapusVendor() {
        int id;
        cout << "\nMasukkan ID Vendor yang akan dihapus: ";
        if (!(cin >> id)) {
            cout << "Input tidak valid.\n";
            clearCin();
            return;
        }
        cin.ignore();

        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            if (it->getId() == id) {
                daftarVendor.erase(it);
                cout << "Vendor dihapus.\n";
                return;
            }
        }
        cout << "Vendor tidak ditemukan.\n";
    }

    // ===== MENU KHUSUS UNTUK VENDOR =====
    void menuVendor() {
        if (daftarVendor.empty()) {
            cout << "Belum ada vendor untuk dikelola.\n";
            return;
        }

        int id;
        cout << "\nMasukkan ID Vendor yang ingin dikelola: ";
        if (!(cin >> id)) {
            cout << "Input tidak valid.\n";
            clearCin();
            return;
        }
        cin.ignore();

        for (auto &v : daftarVendor) {
            if (v.getId() == id) {
                int pilih;
                do {
                    cout << "\n=== MENU VENDOR: " << v.getNamaToko() << " ===\n";
                    cout << "1. Tambah Penjualan\n";
                    cout << "2. Lihat Semua Penjualan\n";
                    cout << "0. Kembali ke Menu Utama\n";
                    cout << "Pilih: ";
                    if (!(cin >> pilih)) {
                        clearCin();
                        pilih = -1;
                        continue;
                    }
                    cin.ignore();

                    switch (pilih) {
                        case 1: v.tambahPenjualan(); break;
                        case 2: v.tampilkanPenjualan(); break;
                        case 0: break;
                        default: cout << "Pilihan tidak valid.\n"; break;
                    }
                } while (pilih != 0);
                return;
            }
        }
        cout << "Vendor dengan ID tersebut tidak ditemukan.\n";
    }
};

/*
int main() {
    AplikasiVendor app;
    app.jalankan();
    return 0;
}
*/

#endif
