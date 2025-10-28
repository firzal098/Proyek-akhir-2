#ifndef PARKING_SYSTEM_H
#define PARKING_SYSTEM_H

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip> // Untuk put_time
#include <string>
#include "ParkingTicket.h"
// HAPUS: #include "ParkingManager.h" 
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class ParkingSystem {
private:
    vector<ParkingTicket> activeTickets; 
    vector<ParkingTicket> ticketHistory; 
    const string historyFile = "parking_history.json";
    // HAPUS: ParkingManager manager;

    // (Salin fungsi to_json dan from_json kamu ke sini...)
    void to_json(json& j, const ParkingTicket& t) {
        j = json{
            {"ticketID", t.ticketID}, {"customerID", t.customerID},
            {"platNomor", t.platNomor}, {"waktuMasuk", t.waktuMasuk},
            {"waktuKeluar", t.waktuKeluar}, {"biaya", t.biaya},
            {"status", (t.status == ACTIVE ? "ACTIVE" : "PAID")},
            {"parkingSlot", t.parkingSlot}
        };
    }

    void from_json(const json& j, ParkingTicket& t) {
        j.at("ticketID").get_to(t.ticketID);
        j.at("customerID").get_to(t.customerID);
        j.at("platNomor").get_to(t.platNomor);
        j.at("waktuMasuk").get_to(t.waktuMasuk);
        j.at("waktuKeluar").get_to(t.waktuKeluar);
        j.at("biaya").get_to(t.biaya);
        t.status = (j.at("status") == "ACTIVE" ? ACTIVE : PAID);
        if (j.find("parkingSlot") != j.end()) {
            j.at("parkingSlot").get_to(t.parkingSlot);
        } else {
            t.parkingSlot = "N/A";
        }
    }

    // (Salin fungsi saveHistory dan loadHistory kamu ke sini...)
    void saveHistory() {
        json j_history;
        for (const auto& ticket : ticketHistory) {
            json j_ticket;
            to_json(j_ticket, ticket);
            j_history.push_back(j_ticket);
        }
        ofstream file(historyFile);
        if (file.is_open()) {
            file << j_history.dump(4);
            file.close();
        } else {
            cerr << "Gagal menyimpan riwayat." << endl;
        }
    }

    void loadHistory() {
        ifstream file(historyFile);
        if (file.is_open()) {
            if (file.peek() == ifstream::traits_type::eof()) {
                cout << "File riwayat parkir kosong." << endl;
                file.close();
                return;
            }
            json j_history;
            file >> j_history;
            if (j_history.is_array()) {
                for (const auto& j_ticket : j_history) {
                    ParkingTicket ticket;
                    from_json(j_ticket, ticket);
                    ticketHistory.push_back(ticket);
                }
            }
            file.close();
            cout << "Riwayat parkir berhasil dimuat." << endl;
        } else {
            cout << "File riwayat tidak ditemukan." << endl;
        }
    }


public:
    ParkingSystem() {
        loadHistory();
    }

    // --- FUNGSI INI DIMODIFIKASI ---
    // Kembalikan 'bool' (true/false)
    bool createTicket(int customerID, const string& platNomor, const string& slot) {
        for(const auto& ticket : activeTickets) {
            if (ticket.platNomor == platNomor) {
                cout << "Error: Kendaraan " << platNomor << " sudah terparkir." << endl;
                return false; // Gagal
            }
        }
        ParkingTicket newTicket(customerID, platNomor, slot); 
        activeTickets.push_back(newTicket);
        cout << "Tiket berhasil dibuat untuk plat " << platNomor << " di slot " << slot << endl;
        cout << "ID Tiket: " << newTicket.ticketID << endl;
        return true; // Sukses
    }

    // --- FUNGSI INI DIMODIFIKASI ---
    // Kembalikan 'string' (nama slot)
    ParkingTicket selesaikanCheckout(uint64_t ticketID) {
        for (size_t i = 0; i < activeTickets.size(); ++i) {
            if (activeTickets[i].ticketID == ticketID) {
                // 1. Hitung biaya
                activeTickets[i].hitungBiaya();
                
                // 2. Siapkan struk
                cout << "\n--- Struk Parkir ---\n";
                cout << "ID Tiket: " << activeTickets[i].ticketID << endl;
                cout << "Plat Nomor: " << activeTickets[i].platNomor << endl;
                cout << "Slot Parkir: " << activeTickets[i].parkingSlot << endl;
                cout << "Total Biaya: Rp" << activeTickets[i].biaya << endl;
                cout << "Status: LUNAS" << endl;
                cout << "--------------------\n";

                // 3. Salin tiketnya sebelum dihapus
                ParkingTicket tiketSelesai = activeTickets[i];

                // 4. Pindahkan ke history dan hapus dari aktif
                ticketHistory.push_back(tiketSelesai);
                activeTickets.erase(activeTickets.begin() + i);
                saveHistory();

                // 5. Kembalikan data tiket lengkap
                return tiketSelesai; 
            }
        }
        cout << "Error: Tiket dengan ID " << ticketID << " tidak ditemukan." << endl;
        return ParkingTicket(); // Kembalikan tiket kosong (ID=0) jika gagal
    }

    // (displayActiveTickets tidak berubah)
    void displayActiveTickets() const {
        cout << "\n--- Kendaraan Saat Ini di Dalam Parkir ---\n";
        if (activeTickets.empty()) {
            cout << "Tidak ada kendaraan di dalam.\n";
            return;
        }
        for(const auto& ticket : activeTickets) {
            cout << "ID Tiket: " << ticket.ticketID 
                 << ", Plat: " << ticket.platNomor 
                 << ", Slot: " << ticket.parkingSlot << endl;
        }
        cout << "-------------------------------------------\n";
    }

    // HAPUS FUNGSI: displayMap() dan isSlotValid()
};

#endif // PARKING_SYSTEM_H