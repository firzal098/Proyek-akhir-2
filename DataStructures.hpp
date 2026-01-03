#ifndef DATA_STRUCTURES_HPP
#define DATA_STRUCTURES_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "ParkingTicket.h" // Diperlukan karena Stack menyimpan TiketParkir

using namespace std;

// ==========================================
// 1. STRUKTUR DATA: QUEUE (FIFO) - Antrian
// ==========================================
struct NodeAntrian {
    string idPelanggan;
    string platNomor;
    NodeAntrian* next;
};

class QueueParkir {
private:
    NodeAntrian* depan;
    NodeAntrian* belakang;
    int jumlah;
    const int MAX_ANTRIAN = 5;
    const string fileAntrian = "antrian_parkir.db";

    void simpanKeFile() {
        ofstream file(fileAntrian);
        NodeAntrian* temp = depan;
        while(temp) {
            file << temp->idPelanggan << "|" << temp->platNomor << endl;
            temp = temp->next;
        }
    }

    void muatDariFile() {
        ifstream file(fileAntrian);
        if(!file.is_open()) return;
        string baris;
        while(getline(file, baris)) {
            size_t pos = baris.find('|');
            if(pos != string::npos) {
                string id = baris.substr(0,pos);
                string plat = baris.substr(pos+1);
                enqueue(id, plat, false); // false = jangan simpan lagi ke file
            }
        }
    }

    // overload enqueue internal untuk load file
    void enqueue(string id, string plat, bool simpan) {
        NodeAntrian* baru = new NodeAntrian{id, plat, nullptr};
        if(isEmpty()) depan = belakang = baru;
        else {
            belakang->next = baru;
            belakang = baru;
        }
        jumlah++;
        if(simpan) simpanKeFile();
    }

public:
    QueueParkir() : depan(nullptr), belakang(nullptr), jumlah(0) {
        muatDariFile();
    }

    bool isFull() { return jumlah >= MAX_ANTRIAN; }
    bool isEmpty() { return jumlah == 0; }

    void enqueue(string id, string plat) { 
        enqueue(id, plat, true); 
        cout << "Kendaraan [" << plat << "] berhasil masuk antrian.\n";
    }

    bool dequeue(string& outId, string& outPlat) {
        if(isEmpty()) return false;
        NodeAntrian* temp = depan;
        outId = temp->idPelanggan;
        outPlat = temp->platNomor;
        depan = depan->next;
        if(depan==nullptr) belakang=nullptr;
        delete temp;
        jumlah--;
        simpanKeFile(); // update file setelah dequeue
        return true;
    }

    void tampilkan() { 
        if(isEmpty()){cout<<"Antrian Kosong.\n"; return;}
        NodeAntrian* temp=depan; int no=1;
        cout<<"\n--- Antrian Masuk Parkir ---\n";
        while(temp){cout<<no++<<". [ID: "<<temp->idPelanggan<<"] - Plat: "<<temp->platNomor<<endl; temp=temp->next;}
    }
};


// ==========================================
// 2. STRUKTUR DATA: STACK (LIFO) - Terakhir Keluar
// ==========================================
struct NodeStack {
    TiketParkir tiket;
    NodeStack* next;
};

class StackParkir {
private:
    NodeStack* top;

public:
    StackParkir() : top(nullptr) {}

    void push(TiketParkir t) {
        NodeStack* baru = new NodeStack{t, top};
        top = baru;
    }

    void pop() { 
        if (top) {
            NodeStack* temp = top;
            top = top->next;
            delete temp;
        }
    }

    void peek() {
        if (!top) {
            cout << "Belum ada kendaraan yang keluar sesi ini.\n";
        } else {
            cout << "\n--- Kendaraan Terakhir Keluar ---\n";
            cout << "ID Tiket   : " << top->tiket.idTiket << endl;
            cout << "Plat Nomor : " << top->tiket.nomorPolisi << endl;
            cout << "Biaya      : Rp " << (long)top->tiket.biaya << endl;
            cout << "Durasi     : Selesai" << endl;
        }
    }
};

// ==========================================
// 3. STRUKTUR DATA: BST - Poin Loyalitas
// ==========================================
struct NodeBST {
    string platNomor;
    int poin;
    NodeBST* left;
    NodeBST* right;
};

class BSTLoyalitas {
private:
    NodeBST* root;

    NodeBST* insertRec(NodeBST* node, string plat, int poin) {
        if (node == nullptr) {
            return new NodeBST{plat, poin, nullptr, nullptr};
        }
        if (plat < node->platNomor)
            node->left = insertRec(node->left, plat, poin);
        else if (plat > node->platNomor)
            node->right = insertRec(node->right, plat, poin);
        else
            node->poin = poin; 
        return node;
    }

    NodeBST* searchRec(NodeBST* node, string plat) {
        if (node == nullptr || node->platNomor == plat)
            return node;
        if (plat < node->platNomor)
            return searchRec(node->left, plat);
        return searchRec(node->right, plat);
    }

    void inorderSave(NodeBST* node, ofstream& file) {
        if (node) {
            inorderSave(node->left, file);
            file << node->platNomor << "|" << node->poin << endl;
            inorderSave(node->right, file);
        }
    }

public:
    BSTLoyalitas() : root(nullptr) {}

    void tambahAtauUpdate(string plat, int poinTambah) {
        NodeBST* existing = searchRec(root, plat);
        if (existing) {
            existing->poin += poinTambah;
            cout << "Poin terupdate! Total: " << existing->poin << endl;
        } else {
            root = insertRec(root, plat, poinTambah);
            cout << "Member baru! Poin awal: " << poinTambah << endl;
        }
    }

    void insertDirect(string plat, int poin) {
        root = insertRec(root, plat, poin);
    }

    void cariPoin(string plat) {
        NodeBST* hasil = searchRec(root, plat);
        if (hasil) {
            cout << "Kendaraan " << plat << " memiliki " << hasil->poin << " poin loyalitas.\n";
        } else {
            cout << "Data loyalitas tidak ditemukan untuk " << plat << ".\n";
        }
    }

    void simpanKeFile(string namaFile) {
        ofstream file(namaFile);
        if (file.is_open()) {
            inorderSave(root, file);
            file.close();
        }
    }
};

#endif // DATA_STRUCTURES_HPP