#ifndef VENDOR_H
#define VENDOR_H

#include <iostream>
#include <string>
#include <algorithm>
#include <limits>
#include <sstream>
#include <conio.h> 
#include "DoublyLinkedList.hpp"
#include "PenyimpanFile.hpp"

using namespace std;

// --- UTILITAS ---
inline string keHurufKecil(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

// ===== KELAS PRODUK =====
class Produk {
public:
    string nama;
    double harga;
    int stok;

    Produk(string n = "", double h = 0.0, int s = 0) : nama(n), harga(h), stok(s) {}

    void tampilkan() const {
        cout << "    - " << nama << " (Stok: " << stok << ") - Rp" << harga;
    }

    // Serialize: nama,harga,stok
    string serialisasi() const {
        return nama + "," + to_string(harga) + "," + to_string(stok);
    }

    static Produk deserialisasi(const string& data) {
        stringstream ss(data);
        string segment;
        DoublyLinkedList<string> tokens;
        while(getline(ss, segment, ',')) {
            tokens.push_back(segment);
        }

        auto it = tokens.begin();
        string nama = (it != tokens.end()) ? *it : ""; ++it;
        double harga = (it != tokens.end()) ? stod(*it) : 0.0; ++it;
        int stok = (it != tokens.end()) ? stoi(*it) : 0;
        
        return Produk(nama, harga, stok);
    }
};

// ===== KELAS PENJUALAN (SALES) =====
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
        cout << "   [ID: " << idPenjualan << "] " << namaProduk 
             << " | Qty: " << jumlah << " | @ Rp" << (long)hargaSatuan 
             << " | Total: Rp" << (long)getTotal() << endl;
    }

    // Serialize: id,nama,jumlah,harga
    string serialisasi() const {
        return to_string(idPenjualan) + "," + namaProduk + "," + to_string(jumlah) + "," + to_string(hargaSatuan);
    }

    // Deserialize
    static Penjualan deserialisasi(const string& data) {
        // Simple manual parsing for internal CSV style inside the pipe-separated file
        stringstream ss(data);
        string segment;
        DoublyLinkedList<string> tokens;
        while(getline(ss, segment, ',')) {
            tokens.push_back(segment);
        }

        auto it = tokens.begin();
        int id = (it != tokens.end()) ? stoi(*it) : 0; if(it!=tokens.end()) ++it;
        string nama = (it != tokens.end()) ? *it : ""; if(it!=tokens.end()) ++it;
        int jml = (it != tokens.end()) ? stoi(*it) : 0; if(it!=tokens.end()) ++it;
        double harga = (it != tokens.end()) ? stod(*it) : 0.0;
        
        return Penjualan(id, nama, jml, harga);
    }
};

// ===== KELAS VENDOR =====
class Vendor {
private:
    int id;
    string nama;
    string produkUtama;
    string kategori;
    DoublyLinkedList<Produk> daftarProduk;

public:
    Vendor(int i = 0, string n = "", string p = "", string k = "")
        : id(i), nama(n), produkUtama(p), kategori(k) {}

    int getId() const { return id; }
    string getNama() const { return nama; }
    string getKategori() const { return kategori; }
    DoublyLinkedList<Produk>& getDaftarProduk() { return daftarProduk; }

    void tambahProduk() {
        string namaProduk;
        double harga;
        int stok;

        cout << "   >> Tambah Produk Baru untuk Vendor " << nama << endl;
        cout << "   Nama Produk: "; getline(cin, namaProduk);
        cout << "   Harga Satuan: "; cin >> harga;
        cout << "   Stok Awal: "; cin >> stok;
        cin.ignore();

        daftarProduk.push_back(Produk(namaProduk, harga, stok));
        cout << "   [OK] Produk berhasil ditambahkan!\n";
    }

    void tampilkanProduk() const {
        cout << "\n   --- Daftar Produk: " << nama << " ---\n";
        if (daftarProduk.empty()) {
            cout << "   (Belum ada produk)\n";
        } else {
            for (auto it = daftarProduk.begin(); it != daftarProduk.end(); ++it) {
                it->tampilkan();
                cout << endl;
            }
        }
    }

    void info() const {
        cout << "ID: " << id << " | Vendor: " << nama 
             << " | Kategori: " << kategori 
             << " | Jml Produk: " << daftarProduk.size() << endl;
    }

    // --- SERIALIZATION FOR FILESTORE ---
    // Format: ID|Nama|ProdukUtama|Kategori|Produk1;Produk2;Produk3
    static string serialisasi(const Vendor& v) {
        string res = to_string(v.id) + "|" + v.nama + "|" + v.produkUtama + "|" + v.kategori + "|";
        
        bool first = true;
        for (auto it = v.daftarProduk.begin(); it != v.daftarProduk.end(); ++it) {
            if (!first) res += ";";
            res += it->serialisasi();
            first = false;
        }
        return res;
    }

    static Vendor deserialisasi(const string& line) {
        DoublyLinkedList<string> parts = PenyimpanFile::pisah(line, '|');
        auto it = parts.begin();
        
        Vendor v;
        if(parts.empty()) return v;

        v.id = stoi(*it); ++it;
        v.nama = (it != parts.end()) ? *it : ""; ++it;
        v.produkUtama = (it != parts.end()) ? *it : ""; ++it;
        v.kategori = (it != parts.end()) ? *it : ""; ++it;
        
        if (it != parts.end()) {
            string productData = *it;
            if (!productData.empty()) {
                // Split products by ';'
                stringstream ss(productData);
                string productStr;
                while (getline(ss, productStr, ';')) {
                    if (!productStr.empty()) {
                        v.daftarProduk.push_back(Produk::deserialisasi(productStr));
                    }
                }
            }
        }
        return v;
    }
};



#endif // VENDOR_H