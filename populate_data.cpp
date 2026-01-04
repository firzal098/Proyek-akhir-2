#include <iostream>
#include <string>
#include <fstream>
#include <random>
#include <chrono>
#include <iomanip>
#include <vector>
#include <sstream>
#include <algorithm> // Required for std::find

#include "Customer.h"    // For Pelanggan class and Pelanggan::serialisasi
#include "KTP.hpp"       // For KTP class
#include "PenyimpanFile.hpp" // For PenyimpanFile::simpanKeFile and PenyimpanFile::pisah
#include "RandomGenerator.hpp" // For RandomUtils::hasilkanStringAcak

using namespace std;
// using namespace RandomUtils; // No longer using this at top level as we're defining our own here

// --- Local helper function to generate a string of random digits ---
string generateRandomNumberString(int length) {
    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    static const string DIGITS = "0123456789";
    uniform_int_distribution<int> distribution(0, DIGITS.length() - 1);

    string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += DIGITS[distribution(rng)];
    }
    return result;
}

// --- Standalone KTP Serialization (copied from KTPManager.h private section) ---
static string serializeKTP_standalone(const KTP& ktp) {
    stringstream ss;
    ss << ktp.getNIK() << "|"
       << ktp.getNama() << "|"
       << ktp.getTempatLahir() << "|"
       << ktp.getTanggalLahir().hari << "|"
       << ktp.getTanggalLahir().bulan << "|"
       << ktp.getTanggalLahir().tahun << "|"
       << ktp.getGender() << "|"
       << ktp.getGolonganDarah() << "|"
       << ktp.getAlamat().jalan << "|"
       << ktp.getAlamat().rt << "|"
       << ktp.getAlamat().rw << "|"
       << ktp.getAlamat().kelurahanDesa << "|"
       << ktp.getAlamat().kecamatan << "|"
       << ktp.getAlamat().kotaKabupaten << "|"
       << ktp.getAlamat().provinsi << "|"
       << ktp.getAgama() << "|"
       << ktp.getStatus() << "|"
       << ktp.getPekerjaan() << "|"
       << ktp.getKewarganegaraan() << "|"
       << (ktp.isSeumurHidup() ? "1" : "0") << "|"
       << ktp.getTanggalBerlaku().hari << "|"
       << ktp.getTanggalBerlaku().bulan << "|"
       << ktp.getTanggalBerlaku().tahun;
    return ss.str();
}
// --------------------------------------------------------------------------------

int main() {
    // Seed random number generator (for local random generation)
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

    DoublyLinkedList<Pelanggan> customersList;
    DoublyLinkedList<KTP> ktpList;

    vector<string> generatedCustomerIDs;
    vector<string> generatedKTPNIKs;

    cout << "Populating data for customers.db and ktp_data.db..." << endl;

    for (int i = 0; i < 20; ++i) {
        // --- Generate KTP Data ---
        string nik;
        do {
            nik = generateRandomNumberString(16); 
        } while (find(generatedKTPNIKs.begin(), generatedKTPNIKs.end(), nik) != generatedKTPNIKs.end());
        generatedKTPNIKs.push_back(nik);

        string namaKTP = "Pelanggan " + to_string(i + 1);
        string tempatLahir = "Kota" + RandomUtils::hasilkanStringAcak(3); // Use RandomUtils::hasilkanStringAcak
        
        Tanggal tglLahir = {
            static_cast<int>(rng() % 28 + 1), // Day
            static_cast<int>(rng() % 12 + 1), // Month
            static_cast<int>(rng() % 40 + 1970) // Year (1970-2009)
        };
        
        JenisKelamin jk = (rng() % 2 == 0) ? LAKI_LAKI : PEREMPUAN;
        GolonganDarah golDarah = static_cast<GolonganDarah>(rng() % 5); // A, B, AB, O, TIDAK_TAHU
        
        Alamat alamat = {
            "Jalan" + RandomUtils::hasilkanStringAcak(5) + " No." + generateRandomNumberString(2),
            static_cast<int>(rng() % 20 + 1), // RT
            static_cast<int>(rng() % 20 + 1), // RW
            "Kelurahan" + RandomUtils::hasilkanStringAcak(4),
            "Kecamatan" + RandomUtils::hasilkanStringAcak(4),
            "Kota" + RandomUtils::hasilkanStringAcak(5),
            "Provinsi" + RandomUtils::hasilkanStringAcak(5)
        };
        
        Agama agama = static_cast<Agama>(rng() % 7); // ISLAM to LAINNYA
        StatusPerkawinan status = static_cast<StatusPerkawinan>(rng() % 4); // BELUM_KAWIN to CERAI_MATI
        string pekerjaan = "Pekerjaan" + RandomUtils::hasilkanStringAcak(5);
        Kewarganegaraan kwn = (rng() % 2 == 0) ? WNI : WNA;
        bool seumurHidup = (rng() % 2 == 0);
        
        Tanggal tglBerlaku = {0, 0, 0};
        if (!seumurHidup) {
            tglBerlaku = {
                static_cast<int>(rng() % 28 + 1),
                static_cast<int>(rng() % 12 + 1),
                static_cast<int>(rng() % 5 + 2025) // Valid for a few years in the future
            };
        }

        KTP ktpObj(nik, namaKTP, tempatLahir, tglLahir, jk, golDarah, alamat, agama, status, pekerjaan, kwn, seumurHidup, tglBerlaku);
        ktpList.push_back(ktpObj);

        // --- Generate Customer Data ---
        string customerId;
        do {
            customerId = "CUST" + generateRandomNumberString(5);
        } while (find(generatedCustomerIDs.begin(), generatedCustomerIDs.end(), customerId) != generatedCustomerIDs.end());
        generatedCustomerIDs.push_back(customerId);

        string namaPelanggan = namaKTP; // Link customer name to KTP name
        string telepon = "08" + generateRandomNumberString(10);
        string email = "pelanggan" + to_string(i + 1) + "@example.com";
        string password = "pass" + generateRandomNumberString(4);
        long double saldo = static_cast<long double>(rng() % 900000 + 100000); // 100,000 to 1,000,000

        Pelanggan customerObj(customerId, namaPelanggan, telepon, email, password, saldo);
        customerObj.setIdKTP(nik); // Link KTP to customer
        customersList.push_back(customerObj);
    }

    // Save data to files
    PenyimpanFile::simpanKeFile("pelanggan.db", customersList, Pelanggan::serialisasi);
    PenyimpanFile::simpanKeFile("ktp_data.db", ktpList, serializeKTP_standalone);

    cout << "Data population complete! " << customersList.size() << " customers and " << ktpList.size() << " KTPs generated and saved." << endl;
    cout << "Files updated: pelanggan.db, ktp_data.db" << endl;

    return 0;
}