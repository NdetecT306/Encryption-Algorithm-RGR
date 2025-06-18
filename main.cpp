#include "SecTabPer.h"
#include "ElGamal.h"
#include "Kuzia.h"
#include <clocale>
#include <termios.h>
#include <unistd.h>
#include <thread>
#include <limits>
#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;
string getPassword() {
    string password = "";
    termios oldtc, newtc;
    tcgetattr(STDIN_FILENO, &oldtc);
    newtc = oldtc;
    newtc.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
    char ch;
    while ((ch = getchar()) != '\n') {
        password += ch;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
    cout << endl;  
    return password;
}
void printMenu() {
    cout << "\nДобро пожаловать в криптографическую систему!\n"
         << "1) Эль-Гамаль (не такой как все)\n"
         << "2) Кузнечик (патриот)\n"
         << "3) Двойная табличная перестановка (для любителей попроще)\n"
         << "Выберите алгоритм (1-3): ";
}
int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    ios_base::sync_with_stdio(false);
    string key = "123";
    int count = 0, time = 0;
    string password;
    do {
        cout << "Введите пароль: ";
        cout.flush();  
        password = getPassword();
        if (password != key) {
            count++;
            cout << "Неверный пароль" << endl;
            if (count % 3 == 0) {
                time += 5;
                cout << "Вы ввели неправильный пароль уже " << count << " раз! Ждите " << time << " секунд для новой попытки." << endl;
                for (int i = 0; i < time; ++i) {
                    int dots = (i % 3) + 1;
                    cout << "\rОжидание: ";
                    for (int j = 0; j < dots; ++j) cout << ".";
                    cout << flush;
                    this_thread::sleep_for(chrono::seconds(1));
                }
                cout << endl;
            }
        }
    } while (password != key);
    cout << "Введен верный пароль.\nДобро пожаловать!" << endl;
    cout << "Для начала работы введите START, для выхода - END, для справки - HELP\n";
    string command;
    while (true) {
        cout << ">> ";
        cin >> command;
        if (command == "START") {
            printMenu();
            int choice;
            while (!(cin >> choice) || choice < 1 || choice > 3) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Пожалуйста, введите число от 1 до 3: ";
            }
            try {
                switch (choice) {
                    case 1:
                        ElGamal_gmp();
                        break;
                    case 2:
                        Kuzia();
                        break;
                    case 3:
                        SecTab();
                        break;
                }
            } catch (const exception& e) {
                cerr << "Произошла ошибка: " << e.what() << endl;
            }
        }
        else if (command == "HELP") {
            string filename = "Reference.txt"; 
            ifstream file_check(filename);
            if (!file_check.good()) {
                cerr << "Файл " << filename << " не найден." << endl;
                continue;
            }
            file_check.close();
            string command = "xdg-open " + filename;
            int result = system(command.c_str());  
            if (result != 0) {
                cerr << "Ошибка при открытии файла." << endl;
            }
        }
        else if (command == "END") {
            cout << "До свидания!\n";
            break;
        }
        else {
            cout << "Неизвестная команда. Введите HELP для справки.\n";
        }
    }
    return 0;
}
