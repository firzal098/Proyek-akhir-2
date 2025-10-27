#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include "Customer.h"
#include <vector>
#include <string>
#include <fstream>   // BARU: Untuk operasi file (ifstream, ofstream)
#include <iostream>
#include <limits> // Diperlukan untuk numeric_limits
#include "json.hpp"  // BARU: Sertakan header JSON
// Class CustomerManager bertanggung jawab untuk mengelola seluruh data customer.
// Ini termasuk menambah, mencari, dan menampilkan customer.

using namespace std; // Menggunakan namespace std untuk keringkasan

// Menggunakan alias lagi
using json = nlohmann::json;

class CustomerManager {
private:
    vector<Customer> daftarCustomer;
    string namaFileDatabase; // BARU: Menyimpan nama file JSON

    bool apakahIDExists(const string& id) const;
    
    // BARU: Method privat untuk menyimpan semua data ke file
    void simpanKeFile() const;

public:
    // BARU: Constructor sekarang menerima nama file sebagai argumen
    CustomerManager(const string& filename);

    void tambahCustomer();
    Customer* cariCustomer(const string& id);
    void tampilkanSemuaCustomer() const;

    void logJumlahCustomer() {
        cout << "[Info] Berhasil memuat " << daftarCustomer.size() << " data customer dari " << namaFileDatabase << endl;
    }

    // BARU: Method publik untuk memuat data dari file (dipanggil di constructor)
    void muatDariFile();
};



// BARU: Constructor diubah untuk menerima nama file dan langsung memuat data
CustomerManager::CustomerManager(const string& filename) : namaFileDatabase(filename) {
    muatDariFile();
}

// BARU: Implementasi untuk memuat data dari file JSON
void CustomerManager::muatDariFile() {
    ifstream file(namaFileDatabase);
    // Jika file tidak ada atau kosong, tidak melakukan apa-apa
    if (!file.is_open() || file.peek() == ifstream::traits_type::eof()) {
        cout << "[Info] Database tidak ditemukan atau kosong. Memulai dengan data baru." << endl;
        return;
    }

    try {
        json data = json::parse(file);
        for (const auto& customerJson : data) {
            daftarCustomer.emplace_back(customerJson); // Menggunakan constructor Customer(json)
        }
        
    } catch (json::parse_error& e) {
        cerr << "Error parsing JSON: " << e.what() << endl;
    }
    file.close();
}

bool CustomerManager::apakahIDExists(const string& id) const {
    for (const auto& customer : daftarCustomer) {
        if (customer.getID() == id) {
            return true;
        }
    }
    return false;
}

void CustomerManager::tambahCustomer() {
    string id, nama, telepon, email;

    cout << "\n--- Pendaftaran Customer Baru ---" << endl;

    // Meminta dan memvalidasi ID
    while (true) {
        cout << "Masukkan ID Customer (unik): ";
        cin >> id;
        if (apakahIDExists(id)) {
            cout << "ID '" << id << "' sudah digunakan. Silakan gunakan ID lain." << endl;
        } else {
            break;
        }
    }
    
    // Membersihkan buffer input setelah `cin >>`
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Masukkan Nama Lengkap: ";
    getline(cin, nama);

    cout << "Masukkan No. Telepon: ";
    getline(cin, telepon);

    cout << "Masukkan Email: ";
    getline(cin, email);

    // Membuat objek customer baru
    Customer newCustomer(id, nama, telepon, email);

    daftarCustomer.push_back(newCustomer);

    // BARU: Setiap kali customer ditambahkan, langsung simpan ke file
    simpanKeFile();

    cout << "\n>> Customer '" << nama << "' berhasil didaftarkan dan data telah disimpan! <<" << endl;
}

Customer* CustomerManager::cariCustomer(const string& id) {
    for (auto& customer : daftarCustomer) {
        if (customer.getID() == id) {
            return &customer; // Mengembalikan alamat dari objek customer yang cocok
        }
    }
    return nullptr; // Mengembalikan null pointer jika tidak ditemukan
}

void CustomerManager::tampilkanSemuaCustomer() const {
    cout << "\n--- Daftar Semua Customer ---" << endl;
    if (daftarCustomer.empty()) {
        cout << "Belum ada customer yang terdaftar." << endl;
    } else {
        for (const auto& customer : daftarCustomer) {
            customer.displayInfo();
        }
    }
}

// BARU: Implementasi untuk menyimpan data ke file JSON
void CustomerManager::simpanKeFile() const {
    json dataJson = json::array();
    for (const auto& customer : daftarCustomer) {
        dataJson.push_back(customer.toJson());
    }

    ofstream file(namaFileDatabase);
    if (file.is_open()) {
        file << dataJson.dump(4); // dump(4) untuk format JSON yang rapi (pretty print)
        file.close();
    } else {
        cerr << "Error: Tidak bisa membuka file untuk menyimpan data." << endl;
    }
}


#endif 