#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <vector>
#include <iostream>
#include "json.hpp" // BARU: Sertakan header library JSON

// Menggunakan alias untuk nlohmann::json agar lebih mudah
using json = nlohmann::json;

class Customer {
private:
    std::string customerID;
    std::string nama;
    std::string noTelepon;
    std::string email;
    std::vector<std::string> platNomorKendaraan;

public:
    // Constructor tidak berubah
    Customer(std::string id, std::string name, std::string phone, std::string mail);

    // BARU: Constructor untuk membuat objek Customer dari data JSON
    Customer(const json& j);

    // Method lain tidak berubah
    void tambahPlatNomor(std::string plat);
    void displayInfo() const;
    std::string getID() const;
    std::string getNama() const;

    // BARU: Method untuk mengubah objek Customer menjadi format JSON
    json toJson() const;
};

#endif // CUSTOMER_H

// Constructor lama tidak berubah
Customer::Customer(std::string id, std::string name, std::string phone, std::string mail)
    : customerID(id), nama(name), noTelepon(phone), email(mail) {}

// BARU: Implementasi constructor dari JSON
Customer::Customer(const json& j) {
    j.at("id").get_to(customerID);
    j.at("nama").get_to(nama);
    j.at("telepon").get_to(noTelepon);
    j.at("email").get_to(email);
    j.at("plat_nomor").get_to(platNomorKendaraan);
}

// Method lain tidak berubah
void Customer::tambahPlatNomor(std::string plat) {
    platNomorKendaraan.push_back(plat);
}

void Customer::displayInfo() const {
    // Implementasi tidak berubah... (sama seperti sebelumnya)
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "ID Customer    : " << customerID << std::endl;
    std::cout << "Nama           : " << nama << std::endl;
    std::cout << "No. Telepon    : " << noTelepon << std::endl;
    std::cout << "Email          : " << email << std::endl;
    std::cout << "Plat Kendaraan : ";
    if (platNomorKendaraan.empty()) {
        std::cout << " (Belum ada)";
    } else {
        for (size_t i = 0; i < platNomorKendaraan.size(); ++i) {
            std::cout << platNomorKendaraan[i] << (i == platNomorKendaraan.size() - 1 ? "" : ", ");
        }
    }
    std::cout << std::endl;
    std::cout << "----------------------------------------" << std::endl;
}

std::string Customer::getID() const { return customerID; }
std::string Customer::getNama() const { return nama; }

// BARU: Implementasi method toJson
json Customer::toJson() const {
    return json{
        {"id", customerID},
        {"nama", nama},
        {"telepon", noTelepon},
        {"email", email},
        {"plat_nomor", platNomorKendaraan}
    };
}
