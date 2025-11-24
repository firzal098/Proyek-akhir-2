#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>
#include <iostream>
#include <fstream>
#include "json.hpp" // BARU: Sertakan header library JSON

// Menggunakan alias untuk nlohmann::json agar lebih mudah
using json = nlohmann::json;

using namespace std;

class Customer {
private:
    string customerID;
    string nama;
    string noTelepon;
    string email;
    string password;

public:
    // Constructor tidak berubah
    Customer(string id, string name, string phone, string mail, string password);

    // BARU: Constructor untuk membuat objek Customer dari data JSON
    Customer(const json& j);

    Customer() {};

    // Method lain tidak berubah
    void displayInfo() const;
    string getID() const;
    string getNama() const;

    // BARU: Method untuk mengubah objek Customer menjadi format JSON
    json toJson() const;

    void save() const;
};

#endif // CUSTOMER_H

// Constructor lama tidak berubah
Customer::Customer(string id, string name, string phone, string mail, string pw)
    : customerID(id), nama(name), noTelepon(phone), email(mail), password(pw) {}

// BARU: Implementasi constructor dari JSON
Customer::Customer(const json& j) {
    j.at("id").get_to(customerID);
    j.at("nama").get_to(nama);
    j.at("telepon").get_to(noTelepon);
    j.at("email").get_to(email);
    j.at("password").get_to(password);
}


void Customer::displayInfo() const {
    // Implementasi tidak berubah... (sama seperti sebelumnya)
    cout << "----------------------------------------" << endl;
    cout << "ID Customer    : " << customerID << endl;
    cout << "Nama           : " << nama << endl;
    cout << "No. Telepon    : " << noTelepon << endl;
    cout << "Email          : " << email << endl;
    cout << "----------------------------------------" << endl;
}

void Customer::save() const {
    string filePath = "customers/" + getID() + ".json";
    ofstream file(filePath);
    if (file.is_open()) {
        file << toJson().dump(4); // dump(4) untuk format JSON yang rapi (pretty print)
        file.close();
        // cout << "[Info] Customer '" << customer.getNama() << "' disimpan ke " << filePath << endl;
    } else {
        cerr << "Error: Tidak bisa membuka file untuk menyimpan customer '"
             << getNama() << "' ke " << filePath << endl;
    }
}

string Customer::getID() const { return customerID; }
string Customer::getNama() const { return nama; }

// BARU: Implementasi method toJson
json Customer::toJson() const {
    return json{
        {"id", customerID},
        {"nama", nama},
        {"telepon", noTelepon},
        {"email", email},
        {"password", password},
    };
}
