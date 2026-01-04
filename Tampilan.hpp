#pragma once

#include <iostream>
#include <string>
#include <limits>
#include <conio.h> // Untuk getch()
#include "DoublyLinkedList.hpp" // Menggunakan DoublyLinkedList dari proyek

// Menggunakan namespace std agar tidak perlu menulis std:: berulang kali
using namespace std;

namespace Tampilan {

// --- Kode Warna ANSI ---
const string RESET = "\033[0m";
const string BOLD = "\033[1m";
const string DIM = "\033[2m";
const string CYAN = "\033[36m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string YELLOW = "\033[33m";
const string GREEN = "\033[32m";
const string RED = "\033[31m";

// --- Utilitas Dasar ---
void clearScreen() {
    system("cls");
}

void pause() {
    cout << DIM << "\nTekan tombol apa saja untuk melanjutkan..." << RESET;
    getch();
}

// --- Komponen UI ---

/**
 * @brief Mencetak header yang terpusat dan bergaya.
 * @param title Teks judul yang akan ditampilkan.
 */
void printHeader(const string& title) {
    clearScreen();
    cout << "\n" << MAGENTA << BOLD;
    cout << "==================================================" << endl;
    cout << "          MALL DIGITAL - " << title << endl;
    cout << "==================================================" << RESET << "\n" << endl;
}

/**
 * @brief Mencetak pesan dengan warna tertentu.
 * @param message Pesan yang akan ditampilkan.
 * @param color Kode warna ANSI untuk digunakan.
 */
void printMessage(const string& message, const string& color = CYAN) {
    cout << color << ">> " << message << RESET << endl;
}

/**
 * @brief Mencetak pesan error dengan warna merah.
 * @param message Pesan error yang akan ditampilkan.
 */
void printError(const string& message) {
    cout << RED << "!! " << message << RESET << endl;
}

/**
 * @brief Menampilkan menu dengan border dan penomoran.
 * @param items DoublyLinkedList string yang berisi item-item menu.
 */
void printMenu(const DoublyLinkedList<string>& items) {
    cout << BLUE;
    cout << "+-----------------------------------------------+" << endl;
    int i = 1;
    for (auto it = items.begin(); it != items.end(); ++it) {
        cout << "| " << BOLD << i++ << ". " << RESET << BLUE << *it << endl;
    }
    cout << "+-----------------------------------------------+" << RESET << endl;
}

/**
 * @brief Meminta input pilihan dari pengguna dengan gaya.
 * @return Pilihan integer dari pengguna.
 */
int getChoice() {
    int choice = 0;
    cout << BOLD << YELLOW << "\n-> Pilihan Anda: " << RESET;
    cin >> choice;

    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return -1;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choice;
}

/**
 * @brief Meminta input string dari pengguna.
 * @param prompt Pesan yang ditampilkan kepada pengguna.
 * @return String yang dimasukkan oleh pengguna.
 */
string getString(const string& prompt) {
    string input;
    cout << BOLD << YELLOW << "-> " << prompt << ": " << RESET;
    getline(cin, input);
    return input;
}

/**
 * @brief Meminta input integer dari pengguna.
 * @param prompt Pesan yang ditampilkan kepada pengguna.
 * @return Integer yang dimasukkan oleh pengguna.
 */
int getInt(const string& prompt) {
    int input;
    cout << BOLD << YELLOW << "-> " << prompt << ": " << RESET;
    cin >> input;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        printError("Input harus berupa angka!");
        return -1; // Mengembalikan nilai error
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return input;
}

} // namespace Tampilan
