#ifndef SISTEM_PARKIR_H
#define SISTEM_PARKIR_H
 
#include <iostream>
#include <fstream>
#include <iomanip> 
#include <string>
#include "ParkingTicket.h"
#include "PenyimpanFile.hpp" 
#include "DoublyLinkedList.hpp"

using namespace std;

class SistemParkir {
private:
    DoublyLinkedList<TiketParkir> tiketAktif; 
    DoublyLinkedList<TiketParkir> riwayatTiket; 
    
    const string fileAktif = "parkir_aktif.db";
    const string fileRiwayat = "riwayat_parkir.db";

    // --- Logika Bantuan Serialisasi untuk TiketParkir ---
    static string serialisasiTiket(const TiketParkir& t) {
        return t.idTiket + "|" + 
               t.idPelanggan + "|" + 
               t.nomorPolisi + "|" + 
               to_string(t.waktuMasuk) + "|" + 
               to_string(t.waktuKeluar) + "|" + 
               to_string(t.biaya) + "|" + 
               to_string(t.status) + "|" + 
               t.slotParkir;
    }

    static TiketParkir deserialisasiTiket(const string& baris) {
        DoublyLinkedList<string> token = PenyimpanFile::pisah(baris, '|');
        auto it = token.begin();
        
        TiketParkir t;
        if(token.empty()) return t;

        t.idTiket = *it; ++it;
        t.idPelanggan = (it != token.end()) ? *it : ""; ++it;
        t.nomorPolisi = (it != token.end()) ? *it : ""; ++it;
        t.waktuMasuk = (it != token.end()) ? stoll(*it) : 0; ++it;
        t.waktuKeluar = (it != token.end()) ? stoll(*it) : 0; ++it;
        t.biaya = (it != token.end()) ? stod(*it) : 0.0; ++it;
        int stat = (it != token.end()) ? stoi(*it) : 0; ++it;
        t.status = (stat == 1) ? DIBAYAR : AKTIF;
        t.slotParkir = (it != token.end()) ? *it : "";

        return t;
    }

    void simpanAktif() {
        PenyimpanFile::simpanKeFile(fileAktif, tiketAktif, serialisasiTiket);
    }

    void simpanRiwayat() {
        PenyimpanFile::simpanKeFile(fileRiwayat, riwayatTiket, serialisasiTiket);
    }

public:
    SistemParkir() {
        PenyimpanFile::muatDariFile(fileAktif, tiketAktif, deserialisasiTiket);
        PenyimpanFile::muatDariFile(fileRiwayat, riwayatTiket, deserialisasiTiket);
    }

    // --- BARU: Bantuan untuk Tampilan Peta ---
    bool apakahSlotTerisi(const string& slot) {
        for(auto it = tiketAktif.begin(); it != tiketAktif.end(); ++it) {
            if (it->slotParkir == slot && it->status == AKTIF) {
                return true;
            }
        }
        return false;
    }

    void checkIn(string idCust, string plat, string slot) {
        // Validasi jika slot sudah terisi
        if (apakahSlotTerisi(slot)) {
            cout << "Error: Slot " << slot << " sudah terisi!" << endl;
            return;
        }

        time_t sekarang = time(0);
        string idTiket = "TKT-" + to_string(sekarang); 

        TiketParkir t;
        t.idTiket = idTiket;
        t.idPelanggan = idCust;
        t.nomorPolisi = plat;
        t.waktuMasuk = sekarang;
        t.status = AKTIF;
        t.slotParkir = slot;
        t.biaya = 0;
        t.waktuKeluar = 0;

        tiketAktif.push_back(t);
        simpanAktif(); 

        cout << "Check-in Berhasil!" << endl;
        cout << "ID Tiket: " << idTiket << endl;
    }

    TiketParkir checkOut(string idTiket) {
        bool ditemukan = false;
        TiketParkir tiketDitemukan;
        
        DoublyLinkedList<TiketParkir> tempAktif;
        
        while(!tiketAktif.empty()) {
            TiketParkir t = tiketAktif.front();
            tiketAktif.pop_front();

            if (t.idTiket == idTiket && t.status == AKTIF) {
                t.waktuKeluar = time(0);
                t.status = DIBAYAR;
                
                double durasi = difftime(t.waktuKeluar, t.waktuMasuk);
                t.biaya = (durasi > 0 ? durasi : 1) * 2000; 

                tiketDitemukan = t;
                ditemukan = true;
                
                riwayatTiket.push_back(t);
            } else {
                tempAktif.push_back(t);
            }
        }

        tiketAktif = tempAktif; 

        if (ditemukan) {
            simpanAktif();
            simpanRiwayat();
            return tiketDitemukan;
        } else {
            cout << "Error: Tiket dengan ID " << idTiket << " tidak ditemukan." << endl;
            return TiketParkir(); 
        }
    }

    void tampilkanTiketAktif() {
        cout << "\n--- Kendaraan Saat Ini di Dalam Parkir ---\n";
        if (tiketAktif.empty()) {
            cout << "Tidak ada kendaraan di dalam.\n";
            return;
        }
        
        for(auto it = tiketAktif.begin(); it != tiketAktif.end(); ++it) {
            cout << "ID Tiket: " << it->idTiket 
                 << ", Plat: " << it->nomorPolisi 
                 << ", Slot: " << it->slotParkir << endl;
        }
    }
};

#endif // SISTEM_PARKIR_H