#ifndef PELANGGAN_H
#define PELANGGAN_H
 
#include <string>
#include <iostream>
#include <fstream>
#include "PenyimpanFile.hpp" // Akan diganti namanya nanti

using namespace std;

/**
 * @class Pelanggan
 * @brief Merepresentasikan data seorang pelanggan dalam sistem.
 *
 * Kelas ini menyimpan semua informasi yang relevan tentang pelanggan,
 * termasuk kredensial untuk login, informasi pribadi, dan saldo mereka.
 */
class Pelanggan {
private:
    string idPelanggan;     // ID unik untuk setiap pelanggan.
    string nama;            // Nama lengkap pelanggan.
    string nomorTelepon;    // Nomor telepon yang bisa dihubungi.
    string email;           // Alamat email pelanggan.
    string password;        // Password untuk otentikasi.
    double saldo;           // Saldo moneter yang dimiliki pelanggan untuk melakukan transaksi.

public:
    /**
     * @brief Konstruktor untuk membuat objek Pelanggan baru dengan semua detail.
     * @param id_ ID unik pelanggan.
     * @param nama_ Nama lengkap pelanggan.
     * @param telepon_ Nomor telepon pelanggan.
     * @param email_ Alamat email pelanggan.
     * @param password_ Password untuk akun.
     * @param saldo_ Saldo awal (default 0.0).
     */
    Pelanggan(string id_, string nama_, string telepon_, string email_, string password_, double saldo_ = 0.0)
        : idPelanggan(id_), nama(nama_), nomorTelepon(telepon_), email(email_), password(password_), saldo(saldo_) {}

    /**
     * @brief Konstruktor default.
     * Membuat objek Pelanggan kosong dengan saldo 0.
     */
    Pelanggan() : saldo(0.0) {};

    // --- Bagian Getter ---
    // Metode-metode ini digunakan untuk mengakses (mendapatkan) nilai dari
    // variabel-variabel pribadi (private) kelas Pelanggan.

    string getId() const { return idPelanggan; }
    string getNama() const { return nama; }
    string getNomorTelepon() const { return nomorTelepon; }
    string getEmail() const { return email; }
    string getPassword() const { return password; }
    double getSaldo() const { return saldo; }

    // --- Bagian Setter ---
    /**
     * @brief Mengatur atau memperbarui saldo pelanggan.
     * @param newSaldo Jumlah saldo baru.
     */
    void setSaldo(double newSaldo) {
        saldo = newSaldo;
    }

    /**
     * @brief Menampilkan semua informasi pelanggan ke konsol.
     * Informasi yang ditampilkan diformat agar mudah dibaca.
     */
    void tampilkanInfo() const {
        cout << "----------------------------------------" << endl;
        cout << "ID Pelanggan   : " << idPelanggan << endl;
        cout << "Nama           : " << nama << endl;
        cout << "No. Telepon    : " << nomorTelepon << endl;
        cout << "Email          : " << email << endl;
        cout << "Saldo          : Rp" << saldo << endl;
        cout << "----------------------------------------" << endl;
    }

    /**
     * @brief Memverifikasi apakah password yang diberikan cocok dengan password pelanggan.
     * @param pw Password yang akan diverifikasi.
     * @return true jika password cocok, false jika tidak.
     */
    bool verifikasiPassword(const string& pw) const {
        return password == pw;
    }

    // --- Serialisasi untuk PenyimpanFile ---

    /**
     * @brief Mengubah (serialize) objek Pelanggan menjadi format string tunggal.
     * Format ini digunakan untuk menyimpan data ke dalam file.
     * Format: ID|Nama|Telepon|Email|Password|Saldo
     * @param p Objek Pelanggan yang akan diserialisasi.
     * @return String yang merepresentasikan objek.
     */
    static string serialisasi(const Pelanggan& p) {
        return p.idPelanggan + "|" + p.nama + "|" + p.nomorTelepon + "|" + p.email + "|" + p.password + "|" + to_string(p.saldo);
    }

    /**
     * @brief Membuat (deserialize) objek Pelanggan dari sebuah string.
     * Fungsi ini kebalikan dari `serialisasi`, digunakan saat memuat data dari file.
     * @param data String dengan format ID|Nama|Telepon|Email|Password|Saldo.
     * @return Objek Pelanggan yang telah direkonstruksi.
     */
    static Pelanggan deserialisasi(const string& data) {
        // Memecah string data menggunakan delimiter '|'
        DoublyLinkedList<string> token = PenyimpanFile::pisah(data, '|');
        
        if (token.empty()) throw runtime_error("Data pelanggan kosong saat deserialisasi.");
        
        auto it = token.begin();
        string id = *it; ++it;
        string nama = (it != token.end()) ? *it : ""; ++it;
        string telepon = (it != token.end()) ? *it : ""; ++it;
        string email = (it != token.end()) ? *it : ""; ++it;
        string password = (it != token.end()) ? *it : ""; ++it;
        double saldo = (it != token.end()) ? stod(*it) : 0.0;

        return Pelanggan(id, nama, telepon, email, password, saldo);
    }
};

#endif // PELANGGAN_H