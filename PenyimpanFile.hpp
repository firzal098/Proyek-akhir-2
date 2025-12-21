#ifndef PENYIMPANFILE_HPP
#define PENYIMPANFILE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "DoublyLinkedList.hpp"

using namespace std;

class PenyimpanFile {
public:
    // Helper: Split a string by a delimiter into a DoublyLinkedList
    static DoublyLinkedList<string> pisah(const string& str, char delimiter) {
        DoublyLinkedList<string> tokens;
        string token;
        stringstream ss(str);
        while (getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    // Generic Save: Writes each element of the list to a file using the serializer function
    template <typename T>
    static void simpanKeFile(const string& filename, DoublyLinkedList<T>& list, string (*serializer)(const T&)) {
        ofstream file(filename);
        if (file.is_open()) {
            for (auto it = list.begin(); it != list.end(); ++it) {
                file << serializer(*it) << endl;
            }
            file.close();
        } else {
            cerr << "[PenyimpanFile] Error: Could not open " << filename << " for writing." << endl;
        }
    }

    // Generic Load: Reads lines from a file and converts them to objects using the deserializer function
    template <typename T>
    static void muatDariFile(const string& filename, DoublyLinkedList<T>& list, T (*deserializer)(const string&)) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                if (!line.empty()) {
                    try {
                        T obj = deserializer(line);
                        list.push_back(obj);
                    } catch (...) {
                        cerr << "[PenyimpanFile] Warning: Skipped malformed line in " << filename << endl;
                    }
                }
            }
            file.close();
        } else {
            // It's okay if file doesn't exist yet (first run)
            // cerr << "[PenyimpanFile] Info: File " << filename << " not found. Creating new on save." << endl;
        }
    }
};

#endif // PENYIMPANFILE_HPP