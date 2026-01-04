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

struct TopPlat {
    string plat;
    int poin;
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

    // Fungsi rekursif untuk traversal dan update top 5
void traverseTop5(NodeBST* node, TopPlat top5[5]) {
    if (!node) return;

    // Traversal kanan dulu supaya urut descending
    traverseTop5(node->right, top5);

    // Masukkan node saat ini ke top5 jika poin lebih tinggi
    for (int i = 0; i < 5; ++i) {
        if (top5[i].poin < node->poin) {
            // Geser elemen ke bawah untuk sisip
            for (int j = 4; j > i; --j) {
                top5[j] = top5[j - 1];
            }
            top5[i].plat = node->platNomor;
            top5[i].poin = node->poin;
            break;
        }
    }

    traverseTop5(node->left, top5);
}

// Fungsi publik untuk tampilkan top 5
void tampilkanTop5() {
    TopPlat top5[5]; // default poin=0
    for (int i = 0; i < 5; ++i) top5[i].poin = 0;

    traverseTop5(root, top5);

    cout << "\n--- 5 Kendaraan Teratas (Poin Loyalitas) ---\n";
    for (int i = 0; i < 5; ++i) {
        if (top5[i].poin == 0) break; // stop jika slot kosong
        string level = hitungLevel(top5[i].poin);
        cout << i + 1 << ". Plat: " << top5[i].plat
             << " | Poin: " << top5[i].poin
             << " | Level: " << level << endl;
    }
}

    void cariPoin(string plat) {
    NodeBST* hasil = searchRec(root, plat);
    if (hasil) {
        string level = hitungLevel(hasil->poin);
        cout << "Kendaraan " << plat << " memiliki " << hasil->poin 
             << " poin loyalitas. Level: " << level << endl;
    } else {
        cout << "Data loyalitas tidak ditemukan untuk " << plat << ".\n";
    }
}


    string hitungLevel(int poin) {
    if (poin >= 500) return "Platinum Member";
    else if (poin >= 300) return "Diamond Member";
    else if (poin >= 150) return "Gold Member";
    else if (poin >= 70) return "Silver Member";
    else if (poin >= 0) return "Bronze Member";
    else return "eror system";
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