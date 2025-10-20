#ifndef PARKING_SYSTEM_H
#define PARKING_SYSTEM_H

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include "ParkingTicket.h"
#include "CustomerManager.h" 
#include "json.hpp"

// Tambahkan using namespace std
using namespace std;
using json = nlohmann::json;

class ParkingSystem {
private:
    vector<ParkingTicket> activeTickets; 
    vector<ParkingTicket> ticketHistory; 
    const string historyFile = "parking_history.json";

    void to_json(json& j, const ParkingTicket& t) {
        j = json{
            {"ticketID", t.ticketID},
            {"customerID", t.customerID},
            {"platNomor", t.platNomor},
            {"waktuMasuk", t.waktuMasuk},
            {"waktuKeluar", t.waktuKeluar},
            {"biaya", t.biaya},
            {"status", (t.status == ACTIVE ? "ACTIVE" : "PAID")}
        };
    }

    void from_json(const json& j, ParkingTicket& t) {
        j.at("ticketID").get_to(t.ticketID);
        j.at("customerID").get_to(t.customerID);
        j.at("platNomor").get_to(t.platNomor);
        j.at("waktuMasuk").get_to(t.waktuMasuk);
        j.at("waktuKeluar").get_to(t.waktuKeluar);
        j.at("biaya").get_to(t.biaya);
        string statusStr = j.at("status");
        t.status = (statusStr == "ACTIVE" ? ACTIVE : PAID);
    }


public:
    ParkingSystem() {
        loadHistory();
    }

    void createTicket(int customerID, const string& platNomor) {
        ParkingTicket newTicket(customerID, platNomor);
        activeTickets.push_back(newTicket);
        cout << "Tiket berhasil dibuat untuk plat " << platNomor << endl;
        cout << "ID Tiket: " << newTicket.ticketID << endl;
    }

    void checkout(uint64_t ticketID) {
        for (size_t i = 0; i < activeTickets.size(); ++i) {
            if (activeTickets[i].ticketID == ticketID) {
                activeTickets[i].hitungBiaya();
                
                cout << "\n--- Struk Parkir ---\n";
                cout << "ID Tiket: " << activeTickets[i].ticketID << endl;
                cout << "ID Customer: " << activeTickets[i].customerID << endl;
                cout << "Plat Nomor: " << activeTickets[i].platNomor << endl;
                
                time_t masukTime = activeTickets[i].waktuMasuk;
                time_t keluarTime = activeTickets[i].waktuKeluar;
                cout << "Waktu Masuk : " << put_time(localtime(&masukTime), "%Y-%m-%d %H:%M:%S") << endl;
                cout << "Waktu Keluar: " << put_time(localtime(&keluarTime), "%Y-%m-%d %H:%M:%S") << endl;

                cout << "Total Biaya: Rp" << activeTickets[i].biaya << endl;
                cout << "Status: LUNAS" << endl;
                cout << "--------------------\n";

                ticketHistory.push_back(activeTickets[i]);
                activeTickets.erase(activeTickets.begin() + i);

                saveHistory();
                return;
            }
        }
        cout << "Error: Tiket dengan ID " << ticketID << " tidak ditemukan atau sudah checkout." << endl;
    }
    
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
            cerr << "Gagal membuka file untuk menyimpan riwayat parkir." << endl;
        }
    }

    void loadHistory() {
        ifstream file(historyFile);
        if (file.is_open()) {
            json j_history;
            file >> j_history;
            for (const auto& j_ticket : j_history) {
                ParkingTicket ticket;
                from_json(j_ticket, ticket);
                ticketHistory.push_back(ticket);
            }
            file.close();
            cout << "Riwayat parkir berhasil dimuat." << endl;
        } else {
            cout << "File riwayat parkir tidak ditemukan. File baru akan dibuat." << endl;
        }
    }

    void displayActiveTickets() const {
        cout << "\n--- Kendaraan Saat Ini di Dalam Parkir ---\n";
        if (activeTickets.empty()) {
            cout << "Tidak ada kendaraan di dalam.\n";
            return;
        }
        for(const auto& ticket : activeTickets) {
            cout << "ID Tiket: " << ticket.ticketID 
                      << ", Plat: " << ticket.platNomor 
                      << ", ID Customer: " << ticket.customerID << endl;
        }
        cout << "-------------------------------------------\n";
    }
};

#endif // PARKING_SYSTEM_H

