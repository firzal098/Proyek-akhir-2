#ifndef PELANGGAN_H
#define PELANGGAN_H
 
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "PenyimpanFile.hpp" // Akan diganti namanya nanti
#include "Tampilan.hpp"
#include "DataStructures.hpp" // Untuk StackRiwayat
#include "KTP.hpp"
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
    long double saldo;           // Saldo moneter yang dimiliki pelanggan untuk melakukan transaksi.
    string idKTP;           // ID KTP (NIK) - dapat kosong jika belum diisi. Backward compatible.

public:
    StackRiwayat riwayatBelanja; // Riwayat belanja per pelanggan
    
    /**
     * @brief Konstruktor untuk membuat objek Pelanggan baru dengan semua detail.
     * @param id_ ID unik pelanggan.
     * @param nama_ Nama lengkap pelanggan.
     * @param telepon_ Nomor telepon pelanggan.
     * @param email_ Alamat email pelanggan.
     * @param password_ Password untuk akun.
     * @param saldo_ Saldo awal (default 0.0).
     */
    Pelanggan(string id_, string nama_, string telepon_, string email_, string password_, long double saldo_ = 0.0)
        : idPelanggan(id_), nama(nama_), nomorTelepon(telepon_), email(email_), password(password_), saldo(saldo_), idKTP("") {}

    /**
     * @brief Konstruktor default.
     * Membuat objek Pelanggan kosong dengan saldo 0.
     */
    Pelanggan() : saldo(0.0), idKTP("") {};

    // --- Bagian Getter ---
    // Metode-metode ini digunakan untuk mengakses (mendapatkan) nilai dari
    // variabel-variabel pribadi (private) kelas Pelanggan.

    string getId() const { return idPelanggan; }
    string getNama() const { return nama; }
    string getNomorTelepon() const { return nomorTelepon; }
    string getEmail() const { return email; }
    string getPassword() const { return password; }
    long double getSaldo() const { return saldo; }
    
    /**
     * @brief Mendapatkan ID KTP pelanggan.
     * @return ID KTP (NIK) atau string kosong jika belum diisi.
     */
    string getIdKTP() const { return idKTP; }
    
    /**
     * @brief Memeriksa apakah pelanggan sudah memiliki KTP.
     * @return true jika sudah memiliki KTP, false jika belum.
     */
    bool hasKTP() const { return !idKTP.empty(); }

    // --- Bagian Setter ---
    /**
     * @brief Mengatur atau memperbarui saldo pelanggan.
     * @param newSaldo Jumlah saldo baru.
     */
    void setSaldo(long double newSaldo) {
        saldo = newSaldo;
    }
    
    /**
     * @brief Set KTP ID (referensi ke file KTP).
     * @param ktpId ID KTP (NIK) pelanggan.
     */
    void setIdKTP(const string& ktpId) {
        idKTP = ktpId;
    }

    /**
     * @brief Menampilkan semua informasi pelanggan ke konsol.
     * Informasi yang ditampilkan diformat agar mudah dibaca.
     */
    void tampilkanInfo() const {
        cout << Tampilan::BLUE << "----------------------------------------" << Tampilan::RESET << endl;
        cout << Tampilan::BOLD << "ID Pelanggan   : " << Tampilan::RESET << idPelanggan << endl;
        cout << Tampilan::BOLD << "Nama           : " << Tampilan::RESET << nama << endl;
        cout << Tampilan::BOLD << "No. Telepon    : " << Tampilan::RESET << nomorTelepon << endl;
        cout << Tampilan::BOLD << "Email          : " << Tampilan::RESET << email << endl;
        cout << Tampilan::BOLD << "Saldo          : " << Tampilan::GREEN << "Rp" << fixed << setprecision(2) << saldo << Tampilan::RESET << endl;
        cout << Tampilan::BOLD << "Status KTP     : " << Tampilan::RESET << (hasKTP() ? Tampilan::GREEN + string("Sudah diisi") + Tampilan::RESET : Tampilan::YELLOW + string("Belum diisi") + Tampilan::RESET) << endl;
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
     * @brief Serialisasi Pelanggan ke string.
     * Format: ID|Nama|Telepon|Email|Password|Saldo|IdKTP
     */
    static string serialisasi(const Pelanggan& p) {
        return p.idPelanggan + "|" + p.nama + "|" + p.nomorTelepon + "|" + p.email + "|" + p.password + "|" + to_string(p.saldo) + "|" + p.idKTP;
    }

    /**
     * @brief Membuat (deserialize) objek Pelanggan dari sebuah string.
     * Fungsi ini kebalikan dari `serialisasi`, digunakan saat memuat data dari file.
     * @param data String dengan format ID|Nama|Telepon|Email|Password|Saldo|IdKTP.
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
        long double saldo = (it != token.end()) ? stold(*it) : 0.0; ++it;
        string idKTP = (it != token.end()) ? *it : "";

        Pelanggan p(id, nama, telepon, email, password, saldo);
        if (!idKTP.empty()) {
            p.setIdKTP(idKTP);
        }
        return p;
    }
    
    /**
     * @brief Destruktor.
     */
    ~Pelanggan() {}
};

#endif // PELANGGAN_H