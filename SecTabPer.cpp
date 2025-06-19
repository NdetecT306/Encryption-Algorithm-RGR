#include "SecTabPer.h"
using namespace std;
const string KEY_FILENAME = "keyTab.txt";
string wstringToString(const wstring& wstr) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}
wstring stringToWstring(const string& str) {
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}
vector<unsigned char> genKey(int n) {
    vector<unsigned char> key(n);
    for (int i = 0; i < n; ++i) {
        key[i] = i+1;
    }
    random_device rd;
    ranlux24_base g(rd());
    shuffle(key.begin(), key.end(), g);
    return key;
}
vector<unsigned char> readKey(int N) {
    vector<unsigned char> key;
    wstring input;
    while (key.size() != N) {
        wcout << L"Введите " << N << L" чисел через пробел: ";
        getline(wcin, input);
        wstringstream ss(input);
        key.clear();
        int num;
        while (ss >> num) {
            if (num < 1 || num > N) {
                wcout << L"Число должно быть от 1 до " << N << L". Попробуйте снова." << endl;
                key.clear();
                break;
            }
            key.push_back(static_cast<unsigned char>(num));
        }
        if (key.size() != N) {
            wcout << L"Нужно ввести ровно " << N << L" чисел. Попробуйте снова." << endl;
        }
    }
    return key;
}
vector<unsigned char> readKeyFromFile(const string& filename, int keyNum) {
    vector<unsigned char> key;
    ifstream file(filename);
    if (!file.is_open()) {
        wcout << L"Ошибка открытия файла с ключом: " << filename.c_str() << endl;
        return key;
    }
    string line;
    int curKey = 0;
    while (curKey <= keyNum && getline(file, line)) {
        if (curKey == keyNum) {
            istringstream iss(line);
            int num;
            while (iss >> num) {
                key.push_back(static_cast<unsigned char>(num));
            }
        }
        curKey++;
    }
    return key;
}
vector<unsigned char> SecTabEnc(const vector<unsigned char>& vec, const vector<unsigned char>& key1, const vector<unsigned char>& key2) {
    int len = vec.size();
    int N = key1.size();
    vector<unsigned char> vecNSize = vec;
    vecNSize.resize(pow(N, 2), 0x00);
    vector<vector<unsigned char>> matrix1(N, vector<unsigned char>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix1[i][j] = vecNSize[i * N + j];
        }
    }
    vector<unsigned char> temp1;
    for (int i = 0; i < N; ++i) {
        for (auto e : key1) {
            temp1.push_back(matrix1[i][e - 1]);
        }
    }
    vector<vector<unsigned char>> matrix2(N, vector<unsigned char>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix2[i][j] = temp1[i * N + j];
        }
    }  
    vector<unsigned char> ciphertext;
    for (auto e : key2) {
        for (int j = 0; j < N; ++j) {
            ciphertext.push_back(matrix2[e - 1][j]);
        }
    }
    return ciphertext;
}
vector<unsigned char> SecTabDec(const vector<unsigned char>& ciphertext, const vector<unsigned char>& key1, const vector<unsigned char>& key2, int originalSize = 0) {
    int N = key1.size();
    if (ciphertext.size() != pow(N, 2)) {
        wcout << L"Неверный размер данных!" << endl;
        return vector<unsigned char>();
    }
    vector<vector<unsigned char>> matrix2(N, vector<unsigned char>(N));
    int ind = 0;
    for (auto e : key2) {
        for (int j = 0; j < N; ++j) {
            matrix2[e - 1][j] = ciphertext[ind++];
        }
    }
    vector<unsigned char> temp2;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            temp2.push_back(matrix2[i][j]);
        }
    }
    vector<vector<unsigned char>> matrix1(N, vector<unsigned char>(N));
    ind = 0;
    for (int i = 0; i < N; ++i) {
        for (auto e : key1) {
            matrix1[i][e - 1] = temp2[ind++];
        }
    }
    vector<unsigned char> plaintext;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            plaintext.push_back(matrix1[i][j]);
        }
    }
    if (originalSize > 0 && plaintext.size() > originalSize) {
        plaintext.resize(originalSize);
    }
    return plaintext;
}
void saveKeysToFile(const vector<unsigned char>& key1, const vector<unsigned char>& key2) {
    ofstream file(KEY_FILENAME);
    if (!file.is_open()) {
        wcout << L"Ошибка открытия файла для записи ключей!" << endl;
        return;
    }
    for (int i = 0; i < key1.size(); ++i) {
        file << static_cast<int>(key1[i]);
        if (i != key1.size() - 1) file << " ";
    }
    file << "\n";
    for (int i = 0; i < key2.size(); ++i) {
        file << static_cast<int>(key2[i]);
        if (i != key2.size() - 1) file << " ";
    }
    wcout << L"Ключи сохранены в " << KEY_FILENAME.c_str() << endl;
}
vector<vector<unsigned char>> getKeys(int tableSize) {
    vector<vector<unsigned char>> keys(2);
    keys[0] = readKeyFromFile(KEY_FILENAME, 0);
    keys[1] = readKeyFromFile(KEY_FILENAME, 1);
    if (keys[0].size() != tableSize || keys[1].size() != tableSize) {
        wcout << L"Ключи не найдены или неверного размера. Введите новые ключи:" << endl;
        wcout << L"Первый ключ:" << endl;
        keys[0] = readKey(tableSize);
        wcout << L"Второй ключ:" << endl;
        keys[1] = readKey(tableSize);
        saveKeysToFile(keys[0], keys[1]);
    }
    return keys;
}
void encConsole() {
    wstring input;
    wcout << L"Введите текст для шифрования: ";
    getline(wcin, input);
    string str = wstringToString(input);
    vector<unsigned char> data(str.begin(), str.end());
    int N = ceil(sqrt(data.size()));
    while (pow(N, 2) < data.size()) {
        N++;
    }
    vector<unsigned char> key1 = genKey(N);
    vector<unsigned char> key2 = genKey(N);
    saveKeysToFile(key1, key2);
    vector<unsigned char> ciphertext = SecTabEnc(data, key1, key2);
    wcout << L"Зашифрованный текст: ";
    for (unsigned char c : ciphertext) {
        if (isprint(c)) {  
            wcout << static_cast<wchar_t>(c);
        } else {
            wcout << L".";  
        }
    }
    wcout << endl;
}
void decConsole() {
    wstring input;
    wcout << L"Введите зашифрованный текст: ";
    getline(wcin, input);
    vector<unsigned char> ciphertext(input.begin(), input.end());
    int tableSize = ceil(sqrt(ciphertext.size()));
    while (tableSize * tableSize < ciphertext.size()) {
        tableSize++;
    }
    vector<vector<unsigned char>> keys = getKeys(tableSize);
    if (keys[0].size() != tableSize || keys[1].size() != tableSize) {
        wcout << L"Ошибка: неверный размер ключей!" << endl;
        return;
    }
    vector<unsigned char> plaintext = SecTabDec(ciphertext, keys[0], keys[1]);
    string result(plaintext.begin(), plaintext.end());
    wstring wresult = stringToWstring(result);
    wcout << L"Расшифрованный текст: " << wresult << endl;
}
void encFile() {
    wstring wfilename;
    wcout << L"Введите имя файла для шифрования: ";
    getline(wcin, wfilename);
    string filename = wstringToString(wfilename);
    try {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Файл не существует");
        }
        file.seekg(0, ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, ios::beg);
        if (fileSize == 0) {
            throw runtime_error("Файл пуст");
        }
        vector<unsigned char> data(fileSize);
        file.read(reinterpret_cast<char*>(data.data()), fileSize);
        file.close();
        int tableSize = ceil(sqrt(data.size()));
        while (tableSize * tableSize < data.size()) {
            tableSize++;
        }
        vector<unsigned char> key1 = genKey(tableSize);
        vector<unsigned char> key2 = genKey(tableSize);
        saveKeysToFile(key1, key2);
        vector<unsigned char> ciphertext = SecTabEnc(data, key1, key2);
        ofstream outFile(filename, ios::binary | ios::trunc);
        outFile.write(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size());
        outFile.close();
        wcout << L"Файл успешно зашифрован и перезаписан." << endl;
    } catch (const exception& e) {
        wcout << stringToWstring(e.what()) << endl;
    }
}
void decFile() {
    wstring wfilename;
    wcout << L"Введите имя файла для расшифровки: ";
    getline(wcin, wfilename);
    string filename = wstringToString(wfilename);
    try {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Файл не существует");
        }
        file.seekg(0, ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, ios::beg);
        if (fileSize == 0) {
            throw runtime_error("Файл пуст");
        }
        vector<unsigned char> ciphertext(fileSize);
        file.read(reinterpret_cast<char*>(ciphertext.data()), fileSize);
        file.close();
        int tableSize = ceil(sqrt(ciphertext.size()));
        while (tableSize * tableSize < ciphertext.size()) {
            tableSize++;
        }
        vector<vector<unsigned char>> keys = getKeys(tableSize);
        if (keys[0].size() != tableSize || keys[1].size() != tableSize) {
            throw runtime_error("Неверный размер ключей");
        }
        vector<unsigned char> plaintext = SecTabDec(ciphertext, keys[0], keys[1], 0);
        while (!plaintext.empty() && plaintext.back() == 0) {
            plaintext.pop_back();
        }
        ofstream outFile(filename, ios::binary | ios::trunc);
        outFile.write(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
        outFile.close();
        wcout << L"Файл успешно расшифрован и перезаписан." << endl;
    } catch (const exception& e) {
        wcout << stringToWstring(e.what()) << endl;
    }
}
int SecTab() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    wcout.imbue(locale("ru_RU.UTF-8"));
    wcin.imbue(locale("ru_RU.UTF-8"));
    int operation, mode;
    wcout << L"1 - Шифровка, 2 - Расшифровка: ";
    wcin >> operation;
    wcin.ignore();
    wcout << L"1 - Использовать терминал, 2 - Использовать файл: ";
    wcin >> mode;
    wcin.ignore();
    try {
        if (operation == 1 && mode == 1) {
            encConsole();
        } else if (operation == 1 && mode == 2) {
            encFile();
        } else if (operation == 2 && mode == 1) {
            decConsole();
        } else if (operation == 2 && mode == 2) {
            decFile();
        } else {
            wcout << L"Неверный выбор!" << endl;
        }
    } catch (const exception& e) {
        wcout << L"Ошибка: " << stringToWstring(e.what()) << endl;
        return 1;
    }
    return 0;
} 
