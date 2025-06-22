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
        key[i] = i + 1;
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
        vector<bool> seen(N + 1, false); 
        bool valid = true;
        int num;
        while (ss >> num) {
            if (num < 1 || num > N) {
                wcout << L"Число должно быть от 1 до " << N << L". Попробуйте снова." << endl;
                valid = false;
                break;
            }
            if (seen[num]) {
                wcout << L"Число " << num << L" повторяется. Все числа должны быть уникальными. Попробуйте снова." << endl;
                valid = false;
                break;
            }
            seen[num] = true;
            key.push_back(static_cast<unsigned char>(num));
        }
        if (!valid) {
            continue;
        }
        if (key.size() != N) {
            wcout << L"Нужно ввести ровно " << N << L" чисел. Попробуйте снова." << endl;
            continue;
        }
        if (ss >> num) {
            wcout << L"Введено больше чем " << N << L" чисел. Попробуйте снова." << endl;
            key.clear();
            continue;
        }
    }
    return key;
}
vector<unsigned char> readKeyFromFile(const string& filename, int keyNum) {
    vector<unsigned char> key;
    ifstream file(filename);
    if (!file.is_open()) {
        wcout << L"Ошибка открытия файла с ключом: " << stringToWstring(filename) << endl;
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
bool isValidKey(const vector<unsigned char>& key, int N) {
    if (key.size() != N) {
        return false;
    }
    vector<bool> seen(N + 1, false);  
    for (unsigned char val : key) {
        if (val < 1 || val > N || seen[val]) {
            return false;  
        }
        seen[val] = true;
    }
    return true;
}
wstring SecTabEncConsole(const wstring& wstr, const vector<unsigned char>& key1, const vector<unsigned char>& key2) {
    int N = key1.size();
    if (!isValidKey(key1, N) || !isValidKey(key2, N)) {
        wcout << L"Некорректные ключи!" << endl;
        return L"";
    }
    wstring paddedStr = wstr;
    paddedStr.resize(N * N, L' '); 
    vector<vector<wchar_t>> matrix(N, vector<wchar_t>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = paddedStr[i * N + j];
        }
    }
    vector<vector<wchar_t>> encryptedMatrix(N, vector<wchar_t>(N));
    for (int i = 0; i < N; ++i) {
        int row = key1[i] - 1;
        for (int j = 0; j < N; ++j) {
            encryptedMatrix[i][j] = matrix[row][j];
        }
    }
    vector<vector<wchar_t>> finalMatrix(N, vector<wchar_t>(N));
    for (int j = 0; j < N; ++j) {
        int col = key2[j] - 1;
        for (int i = 0; i < N; ++i) {
            finalMatrix[i][j] = encryptedMatrix[i][col];
        }
    }
    wstring ciphertext;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            ciphertext += finalMatrix[i][j];
        }
    }
    return ciphertext;
}
wstring SecTabDecConsole(const wstring& ciphertext, const vector<unsigned char>& key1, const vector<unsigned char>& key2) {
    int N = key1.size();
    if (!isValidKey(key1, N) || !isValidKey(key2, N)) {
        wcout << L"Некорректные ключи!" << endl;
        return L"";
    }
    vector<vector<wchar_t>> matrix(N, vector<wchar_t>(N));
    wstring paddedCipher = ciphertext;
    paddedCipher.resize(N * N, L' ');
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = paddedCipher[i * N + j];
        }
    }
    vector<vector<wchar_t>> decryptedMatrix(N, vector<wchar_t>(N));
    for (int j = 0; j < N; ++j) {
        int col = key2[j] - 1;
        for (int i = 0; i < N; ++i) {
            decryptedMatrix[i][col] = matrix[i][j];
        }
    }
    vector<vector<wchar_t>> finalMatrix(N, vector<wchar_t>(N));
    for (int i = 0; i < N; ++i) {
        int row = key1[i] - 1;
        for (int j = 0; j < N; ++j) {
            finalMatrix[row][j] = decryptedMatrix[i][j];
        }
    }
    wstring plaintext;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            plaintext += finalMatrix[i][j];
        }
    }
    return plaintext;
}
vector<unsigned char> SecTabEncFile(const vector<unsigned char>& vec, const vector<unsigned char>& key1, const vector<unsigned char>& key2) {
    int N = key1.size();
    if (!isValidKey(key1, N) || !isValidKey(key2, N)) {
        wcout << L"Некорректные ключи!" << endl;
        return vector<unsigned char>();
    }
    vector<unsigned char> paddedVec = vec;
    paddedVec.resize(pow(N,2), 0x00);
    vector<vector<unsigned char>> matrix(N, vector<unsigned char>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = paddedVec[i * N + j];
        }
    }
    vector<vector<unsigned char>> encryptedMatrix(N, vector<unsigned char>(N));
    for (int i = 0; i < N; ++i) {
        int row = key1[i] - 1;
        for (int j = 0; j < N; ++j) {
            encryptedMatrix[i][j] = matrix[row][j];
        }
    }
    vector<vector<unsigned char>> finalMatrix(N, vector<unsigned char>(N));
    for (int j = 0; j < N; ++j) {
        int col = key2[j] - 1;
        for (int i = 0; i < N; ++i) {
            finalMatrix[i][j] = encryptedMatrix[i][col];
        }
    }
    vector<unsigned char> ciphertext;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            ciphertext.push_back(finalMatrix[i][j]);
        }
    }
    return ciphertext;
}
vector<unsigned char> SecTabDecFile(const vector<unsigned char>& ciphertext, const vector<unsigned char>& key1, const vector<unsigned char>& key2, int originalSize) {
    int N = key1.size();
    if (!isValidKey(key1, N) || !isValidKey(key2, N)) {
        wcout << L"Некорректные ключи!" << endl;
        return vector<unsigned char>();
    }
    vector<unsigned char> paddedCipher = ciphertext;
    paddedCipher.resize(N * N, 0x00);
    vector<vector<unsigned char>> matrix(N, vector<unsigned char>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            matrix[i][j] = paddedCipher[i * N + j];
        }
    }
    vector<vector<unsigned char>> decryptedMatrix(N, vector<unsigned char>(N));
    for (int j = 0; j < N; ++j) {
        int col = key2[j] - 1;
        for (int i = 0; i < N; ++i) {
            decryptedMatrix[i][col] = matrix[i][j];
        }
    }
    vector<vector<unsigned char>> finalMatrix(N, vector<unsigned char>(N));
    for (int i = 0; i < N; ++i) {
        int row = key1[i] - 1;
        for (int j = 0; j < N; ++j) {
            finalMatrix[row][j] = decryptedMatrix[i][j];
        }
    }
    vector<unsigned char> plaintext;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            plaintext.push_back(finalMatrix[i][j]);
        }
    }
    size_t paddingStartIndex = plaintext.size();
    for (size_t i = 0; i < plaintext.size(); ++i) {
        if (plaintext[i] == 0x00) {
            paddingStartIndex = i;
            break;
        }
    }
    plaintext.resize(paddingStartIndex);
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
    wcout << L"Ключи сохранены в " << stringToWstring(KEY_FILENAME) << endl;
}
vector<vector<unsigned char>> getKeys(int N) {
    const int KEY1_INDEX = 0;
    const int KEY2_INDEX = 1;
    vector<vector<unsigned char>> keys(2);
    keys[0] = readKeyFromFile(KEY_FILENAME, KEY1_INDEX);
    keys[1] = readKeyFromFile(KEY_FILENAME, KEY2_INDEX);
    if (keys[0].size() != N || keys[1].size() != N || 
        !isValidKey(keys[0], N) || !isValidKey(keys[1], N)) {
        return vector<vector<unsigned char>>(); 
    }
    return keys;
}
int calculateTableSize(size_t N) {
    int tableSize = ceil(sqrt(N));
    return tableSize;
}
void encConsole() {
    setlocale(LC_ALL, "");
    wcout.imbue(locale(""));
    wcin.imbue(locale(""));
    wstring input;
    wcout << L"Введите текст для шифрования: ";
    getline(wcin, input);
    int N = calculateTableSize(input.size());
    vector<unsigned char> key1 = genKey(N);
    vector<unsigned char> key2 = genKey(N);
    saveKeysToFile(key1, key2);
    wstring ciphertext = SecTabEncConsole(input, key1, key2);
    if (ciphertext.empty()) {
        wcout << L"Ошибка шифрования." << endl;
        return;
    }
    wcout << L"Зашифрованный текст: " << ciphertext << endl;
}
void decConsole() {
    setlocale(LC_ALL, "");
    wcout.imbue(locale(""));
    wcin.imbue(locale(""));

    wstring input;
    wcout << L"Введите зашифрованный текст: ";
    getline(wcin, input);
    if (input.empty()) {
        wcout << L"Ошибка: Введена пустая строка!" << endl;
        return;
    }
    int N = calculateTableSize(input.size());
    ifstream keyFile(KEY_FILENAME);
    if (!keyFile.is_open()) {
        wcout << L"Не удалось загрузить ключи из файла." << endl;
        return;
    }
    keyFile.close();
    auto keys = getKeys(N);
    if (keys.empty() || keys[0].size() != N || keys[1].size() != N) {
        wcout << L"Не удалось загрузить ключи из файла." << endl;
        return;
    }
    wstring plaintext = SecTabDecConsole(input, keys[0], keys[1]);
    if (plaintext.empty()) {
        wcout << L"Ошибка при расшифровке." << endl;
        return;
    }
    wcout << L"Расшифрованный текст: " << plaintext << endl;
}
void encFile() {
    setlocale(LC_ALL, "");
    wcout.imbue(locale(""));
    wcin.imbue(locale(""));
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

        int tableSize = calculateTableSize(data.size());
        vector<unsigned char> key1 = genKey(tableSize);
        vector<unsigned char> key2 = genKey(tableSize);
        saveKeysToFile(key1, key2);
        vector<unsigned char> ciphertext = SecTabEncFile(data, key1, key2);
        if (ciphertext.empty()) {
            throw runtime_error("Некорректные ключи.");
        }
        ofstream outFile(filename, ios::binary | ios::trunc);
        outFile.write(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size());
        outFile.close();
        wcout << L"Файл успешно зашифрован и перезаписан." << endl;
    } catch (const exception& e) {
        wcout << stringToWstring(e.what()) << endl;
    }
}
void decFile() {
    setlocale(LC_ALL, "");
    wcout.imbue(locale(""));
    wcin.imbue(locale(""));
    wstring wfilename;
    wcout << L"Введите имя файла для расшифровки: ";
    getline(wcin, wfilename);
    if (wfilename.empty()) {
        wcout << L"Ошибка: Не указано имя файла!" << endl;
        return;
    }
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
            file.close();
            throw runtime_error("Файл пуст");
        }
        vector<unsigned char> ciphertext(fileSize);
        file.read(reinterpret_cast<char*>(ciphertext.data()), fileSize);
        file.close();
        int tableSize = calculateTableSize(ciphertext.size());
        ifstream keyFile(KEY_FILENAME);
        if (!keyFile.is_open()) {
            wcout << L"Не удалось загрузить ключи из файла." << endl;
            return;
        }
        keyFile.close();
        auto keys = getKeys(tableSize);
        if (keys.empty() || keys[0].size() != tableSize || keys[1].size() != tableSize) {
            wcout << L"Не удалось загрузить ключи из файла." << endl;
            return;
        }
        vector<unsigned char> plaintext = SecTabDecFile(ciphertext, keys[0], keys[1], fileSize);
        if (plaintext.empty()) {
            throw runtime_error("Ошибка расшифровки");
        }
        ofstream outFile(filename, ios::binary | ios::trunc);
        if (!outFile.is_open()) {
            throw runtime_error("Ошибка записи файла");
        }
        outFile.write(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
        outFile.close();

        wcout << L"Файл успешно расшифрован." << endl;

    } catch (const exception& e) {
        wcout << L"Ошибка: " << stringToWstring(e.what()) << endl;
    }
}
int SecTab() {
    setlocale(LC_ALL, "");
    wcout.imbue(locale(""));
    wcin.imbue(locale(""));
    try {
        int mode = 0, method = 0;
        bool validInput = false;
        while (!validInput) {
            try {
                cout << "1 - Шифровка, 2 - Расшифровка: ";
                if (!(cin >> mode) || (mode != 1 && mode != 2)) {
                    cin.clear(); 
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
                    throw runtime_error("Допустимые операции обозначены ЦИФРАМИ 1 или 2.");
                }
                validInput = true;
            } catch (const exception& e) {
                cerr << e.what() << endl;
            }
        }
        validInput = false; 
        while (!validInput) {
            try {
                cout << "1 - Использовать терминал, 2 - Использовать файл: ";
                if (!(cin >> method) || (method != 1 && method != 2)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    throw runtime_error("Допустимые операции обозначены ЦИФРАМИ 1 или 2.");
                }
                validInput = true;
            } catch (const exception& e) {
                cerr << e.what() << endl;
            }
        }
        if (method == 1) {
            if (mode == 1) {
                encConsole();
            } else {
                decConsole();
            }
        } else {
            if (mode == 1) {
                encFile();
            } else {
                decFile();
            }
        }
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
