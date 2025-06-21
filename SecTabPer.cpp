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
bool isValidUTF8(const vector<unsigned char>& bytes) {
    try {
        wstring_convert<codecvt_utf8<wchar_t>> converter;
        converter.from_bytes(reinterpret_cast<const char*>(bytes.data()), reinterpret_cast<const char*>(bytes.data() + bytes.size()));
        return true;
    } catch (const std::range_error&) {
        return false;
    }
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
        int num;
        while (ss >> num) {
            if (num < 1 || num > N) {
                wcout << L"Число должно быть от 1 до " << N << L". Попробуйте снова." << endl;
                key.clear();
                break;
            }
            key.push_back(static_cast<unsigned char>(num));
        }
        if (ss.fail()) {
            wcout << L"Ошибка: Некорректный ввод. Пожалуйста, введите числа." << endl;
            ss.clear(); 
            key.clear();
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
wstring SecTabEnc(const wstring& wstr, const vector<unsigned char>& key1, const vector<unsigned char>& key2) {
    int len = wstr.size();
    int N = key1.size();
    if (!isValidKey(key1, N) || !isValidKey(key2, N)) {
        wcout << L"Некорректные ключи!" << endl;
        return L""; 
    }
    wstring paddedStr = wstr;
    paddedStr.resize(N * N, L' '); 
    vector<vector<wchar_t>> originalMatrix(N, vector<wchar_t>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            originalMatrix[i][j] = paddedStr[i * N + j];
        }
    }
    wstring permutedData;
    for (int i = 0; i < N; ++i) {
        for (auto e : key1) {
            permutedData += originalMatrix[i][e - 1];
        }
    }
    wstring ciphertext;
    vector<vector<wchar_t>> intermediateMatrix(N, vector<wchar_t>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            intermediateMatrix[i][j] = permutedData[i * N + j];
        }
    }
    for (auto e : key2) {
        for (int j = 0; j < N; ++j) {
            ciphertext += intermediateMatrix[e - 1][j];
        }
    }
    return ciphertext;
}
wstring SecTabDec(const wstring& ciphertext, const vector<unsigned char>& key1, const vector<unsigned char>& key2, int originalSize = 0) {
    int N = key1.size();
    if (!isValidKey(key1, N) || !isValidKey(key2, N)) {
        wcout << L"Некорректные ключи!" << endl;
        return L""; 
    }
    if (ciphertext.size() != pow(N, 2)) {
        wcout << L"Неверный размер данных!" << endl;
        return L"";
    }
    vector<vector<wchar_t>> matrix2(N, vector<wchar_t>(N));
    int ind = 0;
    for (auto e : key2) {
        for (int j = 0; j < N; ++j) {
            matrix2[e - 1][j] = ciphertext[ind++];
        }
    }
    wstring temp2;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            temp2 += matrix2[i][j];
        }
    }
    vector<vector<wchar_t>> matrix1(N, vector<wchar_t>(N));
    ind = 0;
    for (int i = 0; i < N; ++i) {
        for (auto e : key1) {
            matrix1[i][e - 1] = temp2[ind++];
        }
    }
    wstring plaintext;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            plaintext += matrix1[i][j];
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
    wcout << L"Ключи сохранены в " << stringToWstring(KEY_FILENAME) << endl;
}
vector<vector<unsigned char>> getKeys(int tableSize) {
    const int KEY1_INDEX = 0;
    const int KEY2_INDEX = 1;
    vector<vector<unsigned char>> keys(2);
    keys[0] = readKeyFromFile(KEY_FILENAME, KEY1_INDEX);
    keys[1] = readKeyFromFile(KEY_FILENAME, KEY2_INDEX);
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
int calculateTableSize(size_t dataSize) {
    int tableSize = ceil(sqrt(dataSize));
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
    wstring ciphertext = SecTabEnc(input, key1, key2);
    if (ciphertext.empty()) {
        wcout << L"Ошибка шифрования. Возможно, неверные ключи." << endl;
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
    int tableSize = calculateTableSize(input.size());
    auto keys = getKeys(tableSize);
    if (keys[0].size() != tableSize || keys[1].size() != tableSize) {
        wcout << L"Ошибка: неверный размер ключей! Дешифрование невозможно." << endl;
        return;
    }
    wstring plaintext = SecTabDec(input, keys[0], keys[1]);
    if (plaintext.empty()) {
        wcout << L"Ошибка при расшифровке. Возможно, неверные ключи или поврежденный шифротекст." << endl;
        return;
    }
    wcout << L"Расшифрованный текст: " << plaintext << endl;
}
vector<unsigned char> SecTabEncFile(const vector<unsigned char>& vec, const vector<unsigned char>& key1, const vector<unsigned char>& key2) {
    int len = vec.size();
    int N = key1.size();
    if (!isValidKey(key1, N) || !isValidKey(key2, N)) {
        wcout << L"Некорректные ключи!" << endl;
        return vector<unsigned char>();
    }
    vector<unsigned char> paddedVec = vec;
    paddedVec.resize(N * N, 0x00); 
    vector<vector<unsigned char>> originalMatrix(N, vector<unsigned char>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; i < N; ++j) {
            originalMatrix[i][j] = paddedVec[i * N + j];
        }
    }
    vector<unsigned char> permutedData;
    for (int i = 0; i < N; ++i) {
        for (auto e : key1) {
            permutedData.push_back(originalMatrix[i][e - 1]);
        }
    }
    vector<vector<unsigned char>> intermediateMatrix(N, vector<unsigned char>(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; i < N; ++j) {
            intermediateMatrix[i][j] = permutedData[i * N + j];
        }
    }
    vector<unsigned char> ciphertext;
    for (auto e : key2) {
        for (int j = 0; j < N; ++j) {
            ciphertext.push_back(intermediateMatrix[e - 1][j]);
        }
    }
    return ciphertext;
}
vector<unsigned char> SecTabDecFile(const vector<unsigned char>& ciphertext, const vector<unsigned char>& key1, const vector<unsigned char>& key2, int originalSize = 0) {
    int N = key1.size();
    if (!isValidKey(key1, N) || !isValidKey(key2, N)) {
        wcout << L"Некорректные ключи!" << endl;
        return vector<unsigned char>();
    }
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
void encFile() {
    setlocale(LC_ALL, "");
    wcout.imbue(locale(""));
    wcin.imbue(locale(""));
    wstring wfilename;
    wcout << L"Введите имя файла для шифрования: ";
    getline(wcin, wfilename);
    string filename = wstringToString(wfilename);
    wstring woutputFilename;
    wcout << L"Введите имя выходного файла для зашифрованного текста: ";
    getline(wcin, woutputFilename);
    string outputFilename = wstringToString(woutputFilename);
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
            throw runtime_error("Ошибка при шифровании. Некорректные ключи.");
        }
        ofstream outFile(outputFilename, ios::binary);
        if (!outFile.is_open()) {
            throw runtime_error("Не удалось открыть выходной файл для записи.");
        }
        outFile.write(reinterpret_cast<const char*>(&fileSize), sizeof(fileSize));
        outFile.write(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size());
        outFile.close();
        wcout << L"Файл успешно зашифрован. Зашифрованный текст сохранен в " << stringToWstring(outputFilename) << endl;
        wcout << L"Ключи сохранены в " << stringToWstring(KEY_FILENAME) << endl;

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
    string filename = wstringToString(wfilename);
    wstring woutputFilename;
    wcout << L"Введите имя выходного файла для расшифрованного текста: ";
    getline(wcin, woutputFilename);
    string outputFilename = wstringToString(woutputFilename);
    try {
        ifstream file(filename, ios::binary);
        if (!file.is_open()) {
            throw runtime_error("Файл не существует");
        }
        size_t fileSizeEncrypted = file.tellg();
        file.seekg(0, ios::end);
        fileSizeEncrypted = file.tellg();
        file.seekg(0, ios::beg);
        if (fileSizeEncrypted < sizeof(size_t)) {
            throw runtime_error("Файл слишком мал и не содержит размер исходного файла.");
        }
        size_t originalSize;
        file.read(reinterpret_cast<char*>(&originalSize), sizeof(originalSize));
        if (file.fail()) {
             throw runtime_error("Ошибка при чтении размера исходного файла.");
        }
        vector<unsigned char> ciphertext(fileSizeEncrypted - sizeof(size_t));
        file.read(reinterpret_cast<char*>(ciphertext.data()), fileSizeEncrypted - sizeof(size_t));
        file.close();
        int tableSize = calculateTableSize(ciphertext.size());
        vector<vector<unsigned char>> keys = getKeys(tableSize);
        if (keys[0].size() != tableSize || keys[1].size() != tableSize) {
            throw runtime_error("Неверный размер ключей");
        }
        vector<unsigned char> plaintext = SecTabDecFile(ciphertext, keys[0], keys[1], originalSize);
        if(plaintext.empty())
        {
             throw runtime_error("Ошибка при расшифровке. Возможно, использованы неверные ключи.");
        }
        ofstream outFile(outputFilename, ios::binary);
        if (!outFile.is_open()) {
            throw runtime_error("Не удалось открыть выходной файл для записи.");
        }
        outFile.write(reinterpret_cast<const char*>(plaintext.data()), plaintext.size());
        outFile.close();
        wcout << L"Файл успешно расшифрован. Расшифрованный текст сохранен в " << stringToWstring(outputFilename) << endl;

    } catch (const exception& e) {
        wcout << stringToWstring(e.what()) << endl;
    }
}
enum class Mode {
    Encrypt = 1,
    Decrypt = 2
};
enum class Method {
    Console = 1,
    File = 2
};
int SecTab() {
    setlocale(LC_ALL, "");
    wcout.imbue(locale(""));
    wcin.imbue(locale(""));
    try {
        Mode mode;
        Method method;
        bool validInput = false;
        while (!validInput) {
            try {
                wcout << L"1 - Шифровка, 2 - Расшифровка: ";
                int modeInput;
                if (!(wcin >> modeInput) || (modeInput != static_cast<int>(Mode::Encrypt) && modeInput != static_cast<int>(Mode::Decrypt))) {
                    wcin.clear();
                    wcin.ignore(numeric_limits<streamsize>::max(), '\n');
                    this_thread::sleep_for(chrono::milliseconds(100));
                    throw runtime_error("Допустимые операции обозначены ЦИФРАМИ 1 или 2.");
                }
                mode = static_cast<Mode>(modeInput);
                validInput = true;
            } catch (const exception& e) {
                wcerr << stringToWstring(e.what()) << endl;
            }
        }
        validInput = false;
        while (!validInput) {
            try {
                wcout << L"1 - Использовать терминал, 2 - Использовать файл: ";
                int methodInput;
                if (!(wcin >> methodInput) || (methodInput != static_cast<int>(Method::Console) && methodInput != static_cast<int>(Method::File))) {
                    wcin.clear();
                    wcin.ignore(numeric_limits<streamsize>::max(), '\n');
                    this_thread::sleep_for(chrono::milliseconds(100));
                    throw runtime_error("Допустимые операции обозначены ЦИФРАМИ 1 или 2.");
                }
                method = static_cast<Method>(methodInput);
                validInput = true;
            } catch (const exception& e) {
                wcerr << stringToWstring(e.what()) << endl;
            }
        }
        wcin.ignore(); 
        if (method == Method::Console) {
            if (mode == Mode::Encrypt) {
                encConsole();
            } else {
                decConsole();
            }
        } else {
            if (mode == Mode::Encrypt) {
                encFile();
            } else {
                decFile();
            }
        }
        return 0;
    } catch (const exception& e) {
        wcerr << stringToWstring(e.what()) << endl;
        return 1;
    }
}
