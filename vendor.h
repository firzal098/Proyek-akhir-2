#ifndef VENDOR_H
#define VENDOR_H

#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
using namespace std;

// --- UTILITY ---
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

// ===== CLASS VENDOR =====
class Vendor {
private:
    int id;
    string namaToko;
    string namaPemilik;
    string kategori;
    string kontak;
    string status;

    Penjualan* daftarPenjualan;
    int jumlahPenjualan;
    int kapasitasPenjualan;
    int nextPenjualanId = 1;

public:
    Vendor(int id = 0, string toko = "", string pemilik = "", string kategori = "", string kontak = "", string status = "Aktif")
        : id(id), namaToko(toko), namaPemilik(pemilik), kategori(kategori), kontak(kontak), status(status) {
        kapasitasPenjualan = 2;
        jumlahPenjualan = 0;
        daftarPenjualan = new Penjualan[kapasitasPenjualan];
    }

    ~Vendor() {
        delete[] daftarPenjualan;
    }

    int getId() const { return id; }
    string getNamaToko() const { return namaToko; }
    string getKategori() const { return kategori; }
    string getStatus() const { return status; }
    string getKontak() const { return kontak; }
    string getNamaPemilik() const { return namaPemilik; }

    void setNamaToko(const string& nama) { namaToko = nama; }
    void setNamaPemilik(const string& pemilik) { namaPemilik = pemilik; }
    void setKategori(const string& kat) { kategori = kat; }
    void setKontak(const string& kont) { kontak = kont; }
    void setStatus(const string& stat) { status = stat; }

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

        if (jumlahPenjualan >= kapasitasPenjualan) {
            kapasitasPenjualan *= 2;
            Penjualan* temp = new Penjualan[kapasitasPenjualan];
            for (int i = 0; i < jumlahPenjualan; ++i)
                temp[i] = daftarPenjualan[i];
            delete[] daftarPenjualan;
            daftarPenjualan = temp;
        }

        daftarPenjualan[jumlahPenjualan++] = Penjualan(nextPenjualanId++, produk, jumlah, harga);
        cout << "Penjualan berhasil ditambahkan!\n";
    }

    void tampilkanPenjualan() const {
        if (jumlahPenjualan == 0) {
            cout << "Belum ada penjualan.\n";
            return;
        }
        for (int i = 0; i < jumlahPenjualan; ++i)
            daftarPenjualan[i].tampilkan();
    }

    void tampilkan() const {
        cout << "------------------------------\n";
        cout << "ID: " << id << endl;
        cout << "Nama Toko: " << namaToko << endl;
        cout << "Pemilik: " << namaPemilik << endl;
        cout << "Kategori: " << kategori << endl;
        cout << "Kontak: " << kontak << endl;
        cout << "Status: " << status << endl;
    }
};

// ==== DOUBLE LINKED LIST NODE ====
struct NodeVendor {
    Vendor data;
    NodeVendor* next;
    NodeVendor* prev;

    NodeVendor(Vendor v) : data(v), next(nullptr), prev(nullptr) {}
};

// ===== CLASS APLIKASI =====
class AplikasiVendor {
private:
    NodeVendor* head;
    NodeVendor* tail;
    int nextId;

public:
    AplikasiVendor() {
        head = tail = nullptr;
        nextId = 1;
    }

    ~AplikasiVendor() {
        while (head) {
            NodeVendor* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void jalankan() {
        int pilihan;
        do {
            cout << "\n=== SISTEM MANAJEMEN VENDOR MALL ===\n";
            cout << "1. Tambah Vendor\n";
            cout << "2. Lihat Semua Vendor\n";
            cout << "3. Cari Vendor\n";
            cout << "4. Edit Vendor\n";
            cout << "5. Menu Penjualan Vendor\n";
            cout << "6. Hapus Vendor\n";
            cout << "7. Tampilkan Double Linked List\n";
            cout << "0. Keluar\n";
            cout << "Pilih: ";
            cin >> pilihan;
            cin.ignore();

            switch (pilihan) {
                case 1: tambahVendor(); break;
                case 2: tampilkanSemua(); break;
                case 3: cariVendor(); break;
                case 4: editVendor(); break;
                case 5: menuVendor(); break;
                case 6: hapusVendor(); break;
                case 7: tampilkanDLL(); break;
                case 0: cout << "Keluar...\n"; break;
                default: cout << "Pilihan salah\n";
            }
        } while (pilihan != 0);
    }

private:

    // ====== TAMBAH VENDOR ======
    void tambahVendor() {
        string toko, pemilik, kategori, kontak;

        cout << "\nNama Toko: ";
        getline(cin, toko);
        cout << "Nama Pemilik: ";
        getline(cin, pemilik);
        cout << "Kategori: ";
        getline(cin, kategori);
        cout << "Kontak: ";
        getline(cin, kontak);

        Vendor v(nextId++, toko, pemilik, kategori, kontak);
        NodeVendor* baru = new NodeVendor(v);

        if (!head) {
            head = tail = baru;
        } else {
            tail->next = baru;
            baru->prev = tail;
            tail = baru;
        }

        cout << "Vendor berhasil ditambahkan!\n";
    }

    // ====== TAMPILKAN SEMUA ======
    void tampilkanSemua() {
        if (!head) {
            cout << "Tidak ada vendor.\n";
            return;
        }

        NodeVendor* temp = head;
        while (temp) {
            temp->data.tampilkan();
            temp = temp->next;
        }
    }

    // ====== CARI VENDOR ======
    void cariVendor() {
        string cari;
        cout << "Masukkan nama/kategori: ";
        getline(cin, cari);
        string key = toLower(cari);

        NodeVendor* temp = head;
        bool ketemu = false;

        while (temp) {
            if (toLower(temp->data.getNamaToko()).find(key) != string::npos ||
                toLower(temp->data.getKategori()).find(key) != string::npos) {

                temp->data.tampilkan();
                ketemu = true;
            }
            temp = temp->next;
        }

        if (!ketemu) cout << "Vendor tidak ditemukan.\n";
    }

    // ====== EDIT VENDOR ======
    void editVendor() {
        int id;
        cout << "Masukkan ID: ";
        cin >> id; cin.ignore();

        NodeVendor* temp = head;
        while (temp) {
            if (temp->data.getId() == id) {
                string nama;
                cout << "Nama Toko baru: ";
                getline(cin, nama);
                temp->data.setNamaToko(nama);
                cout << "Data diperbarui!\n";
                return;
            }
            temp = temp->next;
        }
        cout << "Vendor tidak ditemukan.\n";
    }

    // ====== HAPUS VENDOR ======
    void hapusVendor() {
        int id;
        cout << "Masukkan ID yang akan dihapus: ";
        cin >> id; cin.ignore();

        NodeVendor* temp = head;

        while (temp) {
            if (temp->data.getId() == id) {
                if (temp == head && temp == tail) {
                    head = tail = nullptr;
                } else if (temp == head) {
                    head = head->next;
                    head->prev = nullptr;
                } else if (temp == tail) {
                    tail = tail->prev;
                    tail->next = nullptr;
                } else {
                    temp->prev->next = temp->next;
                    temp->next->prev = temp->prev;
                }

                delete temp;
                cout << "Vendor berhasil dihapus.\n";
                return;
            }
            temp = temp->next;
        }

        cout << "Vendor tidak ditemukan.\n";
    }

    // ====== MENU PENJUALAN ======
    void menuVendor() {
        int id;
        cout << "Masukkan ID Vendor: ";
        cin >> id; cin.ignore();

        NodeVendor* temp = head;
        while (temp) {
            if (temp->data.getId() == id) {
                int pilih;
                do {
                    cout << "\n1. Tambah Penjualan\n";
                    cout << "2. Lihat Penjualan\n";
                    cout << "0. Kembali\n";
                    cout << "Pilih: ";
                    cin >> pilih; cin.ignore();

                    if (pilih == 1) temp->data.tambahPenjualan();
                    else if (pilih == 2) temp->data.tampilkanPenjualan();

                } while (pilih != 0);

                return;
            }
            temp = temp->next;
        }

        cout << "Vendor tidak ditemukan.\n";
    }

    // ====== TAMPILKAN DOUBLE LINKED LIST ======
    void tampilkanDLL() {
        cout << "\n(Double Linked List - Forward)\n";
        NodeVendor* temp = head;
        while (temp) {
            cout << temp->data.getId() << " -> ";
            temp = temp->next;
        }
        cout << "NULL\n";

        cout << "\n(Reverse)\n";
        temp = tail;
        while (temp) {
            cout << temp->data.getId() << " -> ";
            temp = temp->prev;
        }
        cout << "NULL\n";
    }
};

#endif
