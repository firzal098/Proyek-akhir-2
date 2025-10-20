#ifndef VENDOR_H
#define VENDOR_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // Untuk std::transform
#include <limits>    // Untuk penanganan error cin
using namespace std;

// --- UTILITY FUNCTION ---
string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ===== CLASS VENDOR (Disempurnakan dengan ENKAPSULASI) =====
class Vendor {
private:
    int id;
    string namaToko;
    string namaPemilik; // Private
    string kategori;
    string kontak;
    string status;

public:
    // Constructor
    Vendor(int id = 0, string toko = "", string pemilik = "", string kategori = "", string kontak = "", string status = "Aktif") {
        this->id = id;
        this->namaToko = toko;
        this->namaPemilik = pemilik;
        this->kategori = kategori;
        this->kontak = kontak;
        this->status = status;
    }

    // --- GETTERS (Untuk membaca nilai) ---
    int getId() const { return id; }
    string getNamaToko() const { return namaToko; }
    string getKategori() const { return kategori; }
    string getStatus() const { return status; }
    string getKontak() const { return kontak; }
    // V V V V V V V V V V V V V V V V V V V V V V V V V V V
    string getNamaPemilik() const { return namaPemilik; } // <-- FUNGSI YANG HILANG TELAH DITAMBAHKAN
    // ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^

    // --- SETTERS (Untuk mengubah nilai) ---
    void setNamaToko(const string& nama) { namaToko = nama; }
    void setNamaPemilik(const string& pemilik) { namaPemilik = pemilik; }
    void setKategori(const string& kat) { kategori = kat; }
    void setKontak(const string& kont) { kontak = kont; }
    void setStatus(const string& stat) { status = stat; }

    // Metode menampilkan data
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

// ===== CLASS APLIKASI VENDOR (TIDAK ADA PERUBAHAN) =====
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
            cout << "5. Hapus Vendor\n";
            cout << "0. Keluar\n";
            cout << "Pilih menu: ";

            if (!(cin >> pilihan)) {
                cout << "? Input tidak valid. Masukkan angka untuk memilih menu.\n";
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
                case 5: hapusVendor(); break;
                case 0: cout << "Terima kasih, program dihentikan.\n"; break;
                default: cout << "Pilihan tidak valid.\n"; break;
            }
        } while (pilihan != 0);
    }

private:
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
        cout << "? Vendor " << toko << " berhasil ditambahkan dengan ID " << nextId - 1 << "!\n";
    }

    void tampilkanSemua() {
        if (daftarVendor.empty()) {
            cout << "Belum ada vendor terdaftar.\n";
            return;
        }
        cout << "\n=== DAFTAR VENDOR (" << daftarVendor.size() << " Total) ===\n";
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

        cout << "\n--- HASIL PENCARIAN ---\n";
        for (const auto &v : daftarVendor) {
            if (toLower(v.getNamaToko()).find(cariLower) != string::npos || 
                toLower(v.getKategori()).find(cariLower) != string::npos) {
                v.tampilkan();
                ditemukan = true;
            }
        }

        if (!ditemukan)
            cout << "Vendor dengan kriteria '" << cari << "' tidak ditemukan.\n";
    }

    void editVendor() {
        int idCari;
        cout << "\n=== EDIT VENDOR ===\n";
        cout << "Masukkan ID vendor yang akan diubah: ";
        
        if (!(cin >> idCari)) {
            cout << "? Input ID tidak valid.\n";
            clearCin();
            return;
        }
        cin.ignore();

        for (auto &v : daftarVendor) {
            if (v.getId() == idCari) {
                cout << "\n? Vendor ID " << idCari << " ditemukan. Detail saat ini:\n";
                v.tampilkan();

                string input;
                cout << "\nMasukkan detail baru (kosongkan untuk mempertahankan nilai lama):\n";
                
                cout << "Nama Toko Baru (" << v.getNamaToko() << "): ";
                getline(cin, input);
                if (!input.empty()) v.setNamaToko(input);
                
                // TIDAK ADA ERROR LAGI KARENA getNamaPemilik() SUDAH DITAMBAHKAN
                cout << "Nama Pemilik Baru (" << v.getNamaPemilik() << "): "; 
                getline(cin, input);
                if (!input.empty()) v.setNamaPemilik(input);
                
                cout << "Kategori Baru (" << v.getKategori() << "): ";
                getline(cin, input);
                if (!input.empty()) v.setKategori(input);

                cout << "Kontak Baru (" << v.getKontak() << "): ";
                getline(cin, input);
                if (!input.empty()) v.setKontak(input);

                cout << "Status Baru (Aktif/Tidak Aktif) (" << v.getStatus() << "): ";
                getline(cin, input);
                if (!input.empty()) v.setStatus(input);

                cout << "\n? Data vendor berhasil diperbarui!\n";
                v.tampilkan();
                return;
            }
        }
        cout << "Vendor dengan ID " << idCari << " tidak ditemukan.\n";
    }

    void hapusVendor() {
        int id;
        cout << "\n=== HAPUS VENDOR ===\n";
        cout << "Masukkan ID vendor yang akan dihapus: ";
        
        if (!(cin >> id)) {
            cout << "? Input ID tidak valid.\n";
            clearCin();
            return;
        }
        cin.ignore();

        for (auto it = daftarVendor.begin(); it != daftarVendor.end(); ++it) {
            if (it->getId() == id) {
                cout << "Yakin ingin menghapus vendor '" << it->getNamaToko() << "' (ID: " << id << ")? (y/n): ";
                char konfirmasi;
                cin >> konfirmasi;
                cin.ignore();
                
                if (tolower(konfirmasi) == 'y') {
                    daftarVendor.erase(it);
                    cout << "? Vendor berhasil dihapus.\n";
                } else {
                    cout << "Pembatalan penghapusan.\n";
                }
                return;
            }
        }
        cout << "Vendor dengan ID tersebut tidak ditemukan.\n";
    }
};

#endif