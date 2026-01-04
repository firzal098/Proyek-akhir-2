#ifndef KTP_HPP
#define KTP_HPP

#include <iostream>
#include <string>
#include <iomanip>

using namespace std; // Disertakan sesuai request awal agar syntax tetap simpel

// ==========================================
// 1. Definisi Struct & Enum
// ==========================================

struct Tanggal {
    int hari;
    int bulan;
    int tahun;
};

struct Alamat {
    string jalan;
    int rt;
    int rw;
    string kelurahanDesa;
    string kecamatan;
    string kotaKabupaten;
    string provinsi;
};

enum JenisKelamin { LAKI_LAKI, PEREMPUAN };
enum GolonganDarah { A, B, AB, O, TIDAK_TAHU };
enum Agama { ISLAM, KRISTEN, KATOLIK, HINDU, BUDDHA, KONGHUCU, LAINNYA };
enum StatusPerkawinan { BELUM_KAWIN, KAWIN, CERAI_HIDUP, CERAI_MATI };
enum Kewarganegaraan { WNI, WNA };

// ==========================================
// 2. Definisi Class KTP
// ==========================================

class KTP {
private:
    string nik;
    string nama;
    string tempatLahir;
    Tanggal tanggalLahir;
    JenisKelamin gender;
    GolonganDarah golDarah;
    
    Alamat alamatLengkap;
    
    Agama agama;
    StatusPerkawinan statusKawin;
    string pekerjaan;
    Kewarganegaraan kewarganegaraan;
    
    bool seumurHidup;
    Tanggal tanggalBerlaku;

    // Helper functions (Private)
    string getGenderStr() const {
        return (gender == LAKI_LAKI) ? "LAKI-LAKI" : "PEREMPUAN";
    }

    string getDarahStr() const {
        switch(golDarah) {
            case A: return "A";
            case B: return "B";
            case AB: return "AB";
            case O: return "O";
            default: return "-";
        }
    }

    string getAgamaStr() const {
        switch(agama) {
            case ISLAM: return "ISLAM";
            case KRISTEN: return "KRISTEN";
            case KATOLIK: return "KATOLIK";
            case HINDU: return "HINDU";
            case BUDDHA: return "BUDDHA";
            case KONGHUCU: return "KONGHUCU";
            default: return "LAINNYA";
        }
    }

    string getStatusStr() const {
        switch(statusKawin) {
            case BELUM_KAWIN: return "BELUM KAWIN";
            case KAWIN: return "KAWIN";
            case CERAI_HIDUP: return "CERAI HIDUP";
            case CERAI_MATI: return "CERAI MATI";
            default: return "-";
        }
    }

public:
    // Constructor
    KTP(string _nik, string _nama, string _tmptLahir, Tanggal _tglLahir, 
        JenisKelamin _jk, GolonganDarah _gol, Alamat _alamat, 
        Agama _agama, StatusPerkawinan _status, string _pekerjaan, 
        Kewarganegaraan _kwn, bool _seumurHidup, Tanggal _tglBerlaku = {0,0,0}) 
    {
        nik = _nik;
        nama = _nama;
        tempatLahir = _tmptLahir;
        tanggalLahir = _tglLahir;
        gender = _jk;
        golDarah = _gol;
        alamatLengkap = _alamat;
        agama = _agama;
        statusKawin = _status;
        pekerjaan = _pekerjaan;
        kewarganegaraan = _kwn;
        seumurHidup = _seumurHidup;
        tanggalBerlaku = _tglBerlaku;
    }

    // --- Getter methods ---
    string getNIK() const { return nik; }
    string getNama() const { return nama; }
    string getTempatLahir() const { return tempatLahir; }
    Tanggal getTanggalLahir() const { return tanggalLahir; }
    int getGender() const { return (int)gender; }
    int getGolonganDarah() const { return (int)golDarah; }
    Alamat getAlamat() const { return alamatLengkap; }
    int getAgama() const { return (int)agama; }
    int getStatus() const { return (int)statusKawin; }
    string getPekerjaan() const { return pekerjaan; }
    int getKewarganegaraan() const { return (int)kewarganegaraan; }
    bool isSeumurHidup() const { return seumurHidup; }
    Tanggal getTanggalBerlaku() const { return tanggalBerlaku; }

    // Fungsi Cetak
    void cetak() const {
        cout << "================================================" << endl;
        cout << "              PROVINSI " << alamatLengkap.provinsi << endl;
        cout << "            " << alamatLengkap.kotaKabupaten << endl;
        cout << "================================================" << endl;
        
        cout << left << setw(18) << "NIK" << ": " << nik << endl;
        cout << left << setw(18) << "Nama" << ": " << nama << endl;
        
        cout << left << setw(18) << "Tempat/Tgl Lahir" << ": " 
             << tempatLahir << ", " 
             << tanggalLahir.hari << "-" << tanggalLahir.bulan << "-" << tanggalLahir.tahun << endl;
        
        cout << left << setw(18) << "Jenis Kelamin" << ": " << setw(15) << getGenderStr() 
             << "Gol. Darah: " << getDarahStr() << endl;
        
        cout << left << setw(18) << "Alamat" << ": " << alamatLengkap.jalan << endl;
        cout << left << "    " << setw(14) << "RT/RW" << ": " 
             << setfill('0') << setw(3) << alamatLengkap.rt << "/" 
             << setw(3) << alamatLengkap.rw << setfill(' ') << endl;
        
        cout << left << "    " << setw(14) << "Kel/Desa" << ": " << alamatLengkap.kelurahanDesa << endl;
        cout << left << "    " << setw(14) << "Kecamatan" << ": " << alamatLengkap.kecamatan << endl;
        
        cout << left << setw(18) << "Agama" << ": " << getAgamaStr() << endl;
        cout << left << setw(18) << "Status Perkawinan" << ": " << getStatusStr() << endl;
        cout << left << setw(18) << "Pekerjaan" << ": " << pekerjaan << endl;
        cout << left << setw(18) << "Kewarganegaraan" << ": " << (kewarganegaraan == WNI ? "WNI" : "WNA") << endl;
        
        cout << left << setw(18) << "Berlaku Hingga" << ": ";
        if (seumurHidup) {
            cout << "SEUMUR HIDUP" << endl;
        } else {
            cout << tanggalBerlaku.hari << "-" << tanggalBerlaku.bulan << "-" << tanggalBerlaku.tahun << endl;
        }
        cout << "================================================" << endl;
    }
};

#endif // KTP_HPP