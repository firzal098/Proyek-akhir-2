#ifndef KTP_MANAGER_H
#define KTP_MANAGER_H

#include "KTP.hpp"
#include "AVLTree.hpp"
#include "Tampilan.hpp"
#include "DoublyLinkedList.hpp"
#include "PenyimpanFile.hpp"
#include <fstream>
#include <sstream>

/**
 * @class KTPManager
 * @brief Mengelola data KTP dengan penyimpanan file dan AVL Tree.
 */
class KTPManager {
private:
    AVLTree<KTP> pohonKTP; // AVL Tree dengan NIK sebagai key (double)
    string fileKTP = "ktp_data.db";
    
    /**
     * @brief Generate unique KTP key dari NIK
     */
    double generateKTPKey(const string& nik) {
        // Gunakan 10 digit pertama NIK sebagai key
        return stod(nik.substr(0, 10));
    }

public:
    KTPManager() {
        muatDariFile();
    }

    /**
     * @brief Simpan KTP baru dan return ID-nya.
     */
    string simpanKTP(const KTP& ktp) {
        // Simpan ke file
        string ktpData = serializeKTP(ktp);
        ofstream file(fileKTP, ios::app);
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file KTP untuk penulisan.");
        }
        file << ktpData << endl;
        file.close();
        
        // Simpan ke AVL Tree menggunakan 10 digit NIK sebagai key
        double key = generateKTPKey(ktp.getNIK());
        pohonKTP.insert(key, ktp);
        
        return ktp.getNIK(); // Return NIK sebagai ID KTP
    }

    /**
     * @brief Cari KTP berdasarkan NIK.
     */
    KTP* cariKTPByNIK(const string& nik) {
        double key = generateKTPKey(nik);
        DoublyLinkedList<KTP> hasil;
        if (pohonKTP.search(key, hasil)) {
            // Cari KTP dengan NIK yang tepat (karena multiple values per key)
            for (auto it = hasil.begin(); it != hasil.end(); ++it) {
                if (it->getNIK() == nik) {
                    return new KTP(*it); // Return copy
                }
            }
        }
        return nullptr;
    }

    /**
     * @brief Tampilkan semua KTP (untuk admin).
     */
    void tampilkanSemuaKTP() const {
        DoublyLinkedList<KTP> semuaKTP = pohonKTP.getInOrderList();
        
        if (semuaKTP.empty()) {
            cout << "Tidak ada data KTP terdaftar." << endl;
            return;
        }

        int no = 1;
        for (auto it = semuaKTP.begin(); it != semuaKTP.end(); ++it) {
            cout << "\n" << Tampilan::BOLD << "KTP #" << no++ << Tampilan::RESET << endl;
            it->cetak();
        }
    }

    /**
     * @brief Cari KTP berdasarkan kriteria (nama, NIK).
     */
    void cariKTP(const string& kriteria) {
        DoublyLinkedList<KTP> semuaKTP = pohonKTP.getInOrderList();
        
        DoublyLinkedList<KTP> hasil;
        for (auto it = semuaKTP.begin(); it != semuaKTP.end(); ++it) {
            // Cari di NIK atau Nama
            if (it->getNIK().find(kriteria) != string::npos ||
                it->getNama().find(kriteria) != string::npos) {
                hasil.push_back(*it);
            }
        }

        if (hasil.empty()) {
            Tampilan::printError("KTP tidak ditemukan dengan kriteria: " + kriteria);
            return;
        }

        int no = 1;
        for (auto it = hasil.begin(); it != hasil.end(); ++it) {
            cout << "\n" << Tampilan::BOLD << "KTP #" << no++ << Tampilan::RESET << endl;
            it->cetak();
        }
    }

    /**
     * @brief Muatkan KTP dari file ke AVL Tree.
     */
    void muatDariFile() {
        ifstream file(fileKTP);
        if (!file.is_open()) {
            return; // File belum ada, mulai dari kosong
        }

        string baris;
        while (getline(file, baris)) {
            if (baris.empty()) continue;
            try {
                KTP ktp = deserializeKTP(baris);
                double key = generateKTPKey(ktp.getNIK());
                pohonKTP.insert(key, ktp);
            } catch (...) {
                cerr << "Gagal membaca baris KTP: " << baris << endl;
            }
        }
        file.close();
    }

    /**
     * @brief Simpan semua KTP dari AVL Tree ke file.
     */
    void simpanSemuaKeFile() {
        DoublyLinkedList<KTP> semuaKTP = pohonKTP.getInOrderList();
        ofstream file(fileKTP);
        if (!file.is_open()) {
            throw runtime_error("Gagal membuka file KTP untuk penulisan.");
        }

        for (auto it = semuaKTP.begin(); it != semuaKTP.end(); ++it) {
            file << serializeKTP(*it) << endl;
        }
        file.close();
    }

private:
    /**
     * @brief Serialize KTP ke format string.
     */
    static string serializeKTP(const KTP& ktp) {
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

    /**
     * @brief Deserialize KTP dari string.
     */
    static KTP deserializeKTP(const string& data) {
        stringstream ss(data);
        string segment;
        DoublyLinkedList<string> tokens;
        
        while (getline(ss, segment, '|')) {
            tokens.push_back(segment);
        }

        auto it = tokens.begin();
        string nik = *it; ++it;
        string nama = *it; ++it;
        string tempatLahir = *it; ++it;
        
        int hariLahir = stoi(*it); ++it;
        int bulanLahir = stoi(*it); ++it;
        int tahunLahir = stoi(*it); ++it;
        Tanggal tglLahir = {hariLahir, bulanLahir, tahunLahir};
        
        int genderVal = stoi(*it); ++it;
        JenisKelamin gender = (genderVal == 0) ? LAKI_LAKI : PEREMPUAN;
        
        int darahVal = stoi(*it); ++it;
        GolonganDarah darah;
        switch(darahVal) {
            case 0: darah = A; break;
            case 1: darah = B; break;
            case 2: darah = AB; break;
            case 3: darah = O; break;
            default: darah = TIDAK_TAHU;
        }
        
        string jalan = *it; ++it;
        int rt = stoi(*it); ++it;
        int rw = stoi(*it); ++it;
        string kelurahan = *it; ++it;
        string kecamatan = *it; ++it;
        string kotaKabupaten = *it; ++it;
        string provinsi = *it; ++it;
        Alamat alamat = {jalan, rt, rw, kelurahan, kecamatan, kotaKabupaten, provinsi};
        
        int amaVal = stoi(*it); ++it;
        Agama agama;
        switch(amaVal) {
            case 0: agama = ISLAM; break;
            case 1: agama = KRISTEN; break;
            case 2: agama = KATOLIK; break;
            case 3: agama = HINDU; break;
            case 4: agama = BUDDHA; break;
            case 5: agama = KONGHUCU; break;
            default: agama = LAINNYA;
        }
        
        int statusVal = stoi(*it); ++it;
        StatusPerkawinan status;
        switch(statusVal) {
            case 0: status = BELUM_KAWIN; break;
            case 1: status = KAWIN; break;
            case 2: status = CERAI_HIDUP; break;
            case 3: status = CERAI_MATI; break;
            default: status = BELUM_KAWIN;
        }
        
        string pekerjaan = *it; ++it;
        
        int kwnVal = stoi(*it); ++it;
        Kewarganegaraan kwn = (kwnVal == 0) ? WNI : WNA;
        
        bool seumurHidup = (stoi(*it) == 1); ++it;
        
        int hariBerlaku = stoi(*it); ++it;
        int bulanBerlaku = stoi(*it); ++it;
        int tahunBerlaku = stoi(*it);
        Tanggal tglBerlaku = {hariBerlaku, bulanBerlaku, tahunBerlaku};

        return KTP(nik, nama, tempatLahir, tglLahir, gender, darah, alamat, agama, status, pekerjaan, kwn, seumurHidup, tglBerlaku);
    }
};

#endif // KTP_MANAGER_H
