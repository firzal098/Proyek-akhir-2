#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include "Customer.h"
#include "DoublyLinkedList.hpp" 
#include "json.hpp"  // Sertakan header JSON
#include "RandomGenerator.hpp"

#include <string>
#include <fstream>   // Untuk operasi file (ifstream, ofstream)
#include <filesystem> // Untuk operasi direktori
#include <iostream>
#include <limits> // Diperlukan untuk numeric_limits
#include <conio.h>


// Class CustomerManager bertanggung jawab untuk mengelola seluruh data customer.
// Ini termasuk menambah, mencari, dan menampilkan customer.

using namespace std; // Menggunakan namespace std untuk keringkasan
namespace fs = std::filesystem; // Alias untuk std::filesystem

using namespace RandomUtils;

// Menggunakan alias lagi
using json = nlohmann::json;

class CustomerManager {
private:
    DoublyLinkedList<Customer> daftarCustomer; // DIUBAH: Menggunakan DoublyLinkedList, bukan DynamicArray
    string customerDataDir; // Directory to store individual customer JSON files

    bool apakahIDExists(const string& id) const;
    

public:
    // Constructor sekarang menerima nama file sebagai argumen
    CustomerManager(const string& filename);

    void tambahCustomer();
    Customer* cariCustomer(const string& id);
    void tampilkanSemuaCustomer() const;

    void logJumlahCustomer() const {
        cout << "[Info] Berhasil memuat " << daftarCustomer.size() << " data customer dari direktori " << customerDataDir << endl;
    }

    // Method publik untuk memuat data dari file (dipanggil di constructor)
    void muatDariFile();
};

// Constructor diubah untuk menerima nama file dan langsung memuat data
CustomerManager::CustomerManager(const string& dataDir) : customerDataDir(dataDir) {
    // Pastikan direktori ada
    if (!fs::exists(customerDataDir)) {
        fs::create_directory(customerDataDir);
        cout << "[Info] Direktori '" << customerDataDir << "' dibuat." << endl;
    }
    muatDariFile();
}

// Implementasi untuk memuat data dari file JSON
void CustomerManager::muatDariFile() {
    daftarCustomer.clear(); // Bersihkan data yang ada di memori sebelum memuat ulang

    if (!fs::exists(customerDataDir) || fs::is_empty(customerDataDir)) {
        cout << "[Info] Database tidak ditemukan atau kosong. Memulai dengan data baru." << endl;
        return;
    }

    for (const auto& entry : fs::directory_iterator(customerDataDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            ifstream file(entry.path());
            if (!file.is_open()) {
                cerr << "Error: Gagal membuka file customer: " << entry.path() << endl;
                continue;
            }
            try {
                json customerJson = json::parse(file);
                // DIUBAH: DynamicArray tidak punya emplace_back, jadi gunakan push_back 
                // dengan memanggil constructor Customer secara eksplisit.
                daftarCustomer.push_back(Customer(customerJson)); 
            } catch (json::parse_error& e) {
                cerr << "Error parsing JSON from " << entry.path() << ": " << e.what() << endl;
            }
            file.close();
        }
    }
}

bool CustomerManager::apakahIDExists(const string& id) const {
    // Range-based for loop tetap bekerja karena DynamicArray memiliki begin() dan end()
    for (const auto& customer : daftarCustomer) {
        if (customer.getID() == id) {
            return true;
        }
    }
    return false;
}

void CustomerManager::tambahCustomer() {
    string id, nama, telepon, email, password;

    cout << "\n--- Pendaftaran Customer Baru ---" << endl;

    // Membersihkan buffer sebelum meminta ID
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // Meminta dan memvalidasi ID dari user
    while (true) {
        cout << "Masukkan ID yang diinginkan: ";
        getline(cin, id);
        if (id.empty()) {
            cout << "ID tidak boleh kosong." << endl;
        } else if (apakahIDExists(id)) {
           cout << "ID '" << id << "' sudah digunakan. Silakan gunakan ID lain." << endl;
        } else {
            break;
        }
    }

    cout << "Masukkan Nama Lengkap: ";
    getline(cin, nama);

    cout << "Masukkan No. Telepon: ";
    getline(cin, telepon);

    cout << "Masukkan Email: ";
    getline(cin, email);

    cout << "Masukkan Password: ";
    getline(cin, password);

    // Membuat objek customer baru
    Customer newCustomer(id, nama, telepon, email, password);

    daftarCustomer.push_back(newCustomer);

    newCustomer.save();
    
    cout << "\n>> Customer '" << nama << "' berhasil didaftarkan dan data telah disimpan! <<" << endl;
    cout << "Silakan login dengan ID dan password Anda." << endl;
    getch();
}

Customer* CustomerManager::cariCustomer(const string& id) {
    // Loop ini bekerja karena DynamicArray mengembalikan pointer di begin() dan end()
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

#endif