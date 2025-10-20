#ifndef CUSTOMER_MANAGER_H
#define CUSTOMER_MANAGER_H

#include "Customer.h"
#include <vector>
#include <string>
#include <fstream>   // BARU: Untuk operasi file (ifstream, ofstream)
#include "json.hpp"  // BARU: Sertakan header JSON

#include <iostream>
#include <limits> // Diperlukan untuk std::numeric_limits
// Class CustomerManager bertanggung jawab untuk mengelola seluruh data customer.
// Ini termasuk menambah, mencari, dan menampilkan customer.

// Menggunakan alias lagi
using json = nlohmann::json;

class CustomerManager {
private:
    std::vector<Customer> daftarCustomer;
    std::string namaFileDatabase; // BARU: Menyimpan nama file JSON

    bool apakahIDExists(const std::string& id) const;
    
    // BARU: Method privat untuk menyimpan semua data ke file
    void simpanKeFile() const;

public:
    // BARU: Constructor sekarang menerima nama file sebagai argumen
    CustomerManager(const std::string& filename);

    void tambahCustomer();
    Customer* cariCustomer(const std::string& id);
    void tampilkanSemuaCustomer() const;

    // BARU: Method publik untuk memuat data dari file (dipanggil di constructor)
    void muatDariFile();
};

#endif // CUSTOMER_MANAGER_H


// BARU: Constructor diubah untuk menerima nama file dan langsung memuat data
CustomerManager::CustomerManager(const std::string& filename) : namaFileDatabase(filename) {
    muatDariFile();
}

// BARU: Implementasi untuk memuat data dari file JSON
void CustomerManager::muatDariFile() {
    std::ifstream file(namaFileDatabase);
    // Jika file tidak ada atau kosong, tidak melakukan apa-apa
    if (!file.is_open() || file.peek() == std::ifstream::traits_type::eof()) {
        std::cout << "[Info] Database tidak ditemukan atau kosong. Memulai dengan data baru." << std::endl;
        return;
    }

    try {
        json data = json::parse(file);
        for (const auto& customerJson : data) {
            daftarCustomer.emplace_back(customerJson); // Menggunakan constructor Customer(json)
        }
        std::cout << "[Info] Berhasil memuat " << daftarCustomer.size() << " data customer dari " << namaFileDatabase << std::endl;
    } catch (json::parse_error& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
    file.close();
}

bool CustomerManager::apakahIDExists(const std::string& id) const {
    for (const auto& customer : daftarCustomer) {
        if (customer.getID() == id) {
            return true;
        }
    }
    return false;
}

void CustomerManager::tambahCustomer() {
    std::string id, nama, telepon, email;

    std::cout << "\n--- Pendaftaran Customer Baru ---" << std::endl;

    // Meminta dan memvalidasi ID
    while (true) {
        std::cout << "Masukkan ID Customer (unik): ";
        std::cin >> id;
        if (apakahIDExists(id)) {
            std::cout << "ID '" << id << "' sudah digunakan. Silakan gunakan ID lain." << std::endl;
        } else {
            break;
        }
    }
    
    // Membersihkan buffer input setelah `cin >>`
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Masukkan Nama Lengkap: ";
    std::getline(std::cin, nama);

    std::cout << "Masukkan No. Telepon: ";
    std::getline(std::cin, telepon);

    std::cout << "Masukkan Email: ";
    std::getline(std::cin, email);

    // Membuat objek customer baru
    Customer newCustomer(id, nama, telepon, email);

    // Menambahkan plat nomor
    std::string plat;
    while (true) {
        std::cout << "Masukkan Plat Nomor Kendaraan (ketik 'q' untuk selesai): ";
        std::getline(std::cin, plat);
        if (plat == "q" || plat == "Q") {
            break;
        }
        newCustomer.tambahPlatNomor(plat);
    }

    daftarCustomer.push_back(newCustomer);

    // BARU: Setiap kali customer ditambahkan, langsung simpan ke file
    simpanKeFile();

    std::cout << "\n>> Customer '" << nama << "' berhasil didaftarkan dan data telah disimpan! <<" << std::endl;
}

Customer* CustomerManager::cariCustomer(const std::string& id) {
    for (auto& customer : daftarCustomer) {
        if (customer.getID() == id) {
            return &customer; // Mengembalikan alamat dari objek customer yang cocok
        }
    }
    return nullptr; // Mengembalikan null pointer jika tidak ditemukan
}

void CustomerManager::tampilkanSemuaCustomer() const {
    std::cout << "\n--- Daftar Semua Customer ---" << std::endl;
    if (daftarCustomer.empty()) {
        std::cout << "Belum ada customer yang terdaftar." << std::endl;
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

    std::ofstream file(namaFileDatabase);
    if (file.is_open()) {
        file << dataJson.dump(4); // dump(4) untuk format JSON yang rapi (pretty print)
        file.close();
    } else {
        std::cerr << "Error: Tidak bisa membuka file untuk menyimpan data." << std::endl;
    }
}
