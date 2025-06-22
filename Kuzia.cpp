#include "Kuzia.h"
namespace fs = filesystem;
const vector<unsigned char> Sbox = {
    0xFC, 0xEE, 0xDD, 0x11, 0xCF, 0x6E, 0x31, 0x16, 0xFB, 0xC4, 0xFA, 0xDA, 0x23, 0xC5, 0x04, 0x4D,
    0xE9, 0x77, 0xF0, 0xDB, 0x93, 0x2E, 0x99, 0xBA, 0x17, 0x36, 0xF1, 0xBB, 0x14, 0xCD, 0x5F, 0xC1,
    0xF9, 0x18, 0x65, 0x5A, 0xE2, 0x5C, 0xEF, 0x21, 0x81, 0x1C, 0x3C, 0x42, 0x8B, 0x01, 0x8E, 0x4F,
    0x05, 0x84, 0x02, 0xAE, 0xE3, 0x6A, 0x8F, 0xA0, 0x06, 0x0B, 0xED, 0x98, 0x7F, 0xD4, 0xD3, 0x1F,
    0xEB, 0x34, 0x2C, 0x51, 0xEA, 0xC8, 0x48, 0xAB, 0xF2, 0x2A, 0x68, 0xA2, 0xFD, 0x3A, 0xCE, 0xCC,
    0xB5, 0x70, 0x0E, 0x56, 0x08, 0x0C, 0x76, 0x12, 0xBF, 0x72, 0x13, 0x47, 0x9C, 0xB7, 0x5D, 0x87,
    0x15, 0xA1, 0x96, 0x29, 0x10, 0x7B, 0x9A, 0xC7, 0xF3, 0x91, 0x78, 0x6F, 0x9D, 0x9E, 0xB2, 0xB1,
    0x32, 0x75, 0x19, 0x3D, 0xFF, 0x35, 0x8A, 0x7E, 0x6D, 0x54, 0xC6, 0x80, 0xC3, 0xBD, 0x0D, 0x57,
    0xDF, 0xF5, 0x24, 0xA9, 0x3E, 0xA8, 0x43, 0xC9, 0xD7, 0x79, 0xD6, 0xF6, 0x7C, 0x22, 0xB9, 0x03,
    0xE0, 0x0F, 0xEC, 0xDE, 0x7A, 0x94, 0xB0, 0xBC, 0xDC, 0xE8, 0x28, 0x50, 0x4E, 0x33, 0x0A, 0x4A,
    0xA7, 0x97, 0x60, 0x73, 0x1E, 0x00, 0x62, 0x44, 0x1A, 0xB8, 0x38, 0x82, 0x64, 0x9F, 0x26, 0x41,
    0xAD, 0x45, 0x46, 0x92, 0x27, 0x5E, 0x55, 0x2F, 0x8C, 0xA3, 0xA5, 0x7D, 0x69, 0xD5, 0x95, 0x3B,
    0x07, 0x58, 0xB3, 0x40, 0x86, 0xAC, 0x1D, 0xF7, 0x30, 0x37, 0x6B, 0xE4, 0x88, 0xD9, 0xE7, 0x89,
    0xE1, 0x1B, 0x83, 0x49, 0x4C, 0x3F, 0xF8, 0xFE, 0x8D, 0x53, 0xAA, 0x90, 0xCA, 0xD8, 0x85, 0x61,
    0x20, 0x71, 0x67, 0xA4, 0x2D, 0x2B, 0x09, 0x5B, 0xCB, 0x9B, 0x25, 0xD0, 0xBE, 0xE5, 0x6C, 0x52,
    0x59, 0xA6, 0x74, 0xD2, 0xE6, 0xF4, 0xB4, 0xC0, 0xD1, 0x66, 0xAF, 0xC2, 0x39, 0x4B, 0x63, 0xB6
};
const vector<unsigned char> IncSbox = {
    0xA5, 0x2D, 0x32, 0x8F, 0x0E, 0x30, 0x38, 0xC0, 0x54, 0xE6, 0x9E, 0x39, 0x55, 0x7E, 0x52, 0x91,
    0x64, 0x03, 0x57, 0x5A, 0x1C, 0x60, 0x07, 0x18, 0x21, 0x72, 0xA8, 0xD1, 0x29, 0xC6, 0xA4, 0x3F,
    0xE0, 0x27, 0x8D, 0x0C, 0x82, 0xEA, 0xAE, 0xB4, 0x9A, 0x63, 0x49, 0xE5, 0x42, 0xE4, 0x15, 0xB7,
    0xC8, 0x06, 0x70, 0x9D, 0x41, 0x75, 0x19, 0xC9, 0xAA, 0xFC, 0x4D, 0xBF, 0x2A, 0x73, 0x84, 0xD5,
    0xC3, 0xAF, 0x2B, 0x86, 0xA7, 0xB1, 0xB2, 0x5B, 0x46, 0xD3, 0x9F, 0xFD, 0xD4, 0x0F, 0x9C, 0x2F,
    0x9B, 0x43, 0xEF, 0xD9, 0x79, 0xB6, 0x53, 0x7F, 0xC1, 0xF0, 0x23, 0xE7, 0x25, 0x5E, 0xB5, 0x1E,
    0xA2, 0xDF, 0xA6, 0xFE, 0xAC, 0x22, 0xF9, 0xE2, 0x4A, 0xBC, 0x35, 0xCA, 0xEE, 0x78, 0x05, 0x6B,
    0x51, 0xE1, 0x59, 0xA3, 0xF2, 0x71, 0x56, 0x11, 0x6A, 0x89, 0x94, 0x65, 0x8C, 0xBB, 0x77, 0x3C,
    0x7B, 0x28, 0xAB, 0xD2, 0x31, 0xDE, 0xC4, 0x5F, 0xCC, 0xCF, 0x76, 0x2C, 0xB8, 0xD8, 0x2E, 0x36,
    0xDB, 0x69, 0xB3, 0x14, 0x95, 0xBE, 0x62, 0xA1, 0x3B, 0x16, 0x66, 0xE9, 0x5C, 0x6C, 0x6D, 0xAD,
    0x37, 0x61, 0x4B, 0xB9, 0xE3, 0xBA, 0xF1, 0xA0, 0x85, 0x83, 0xDA, 0x47, 0xC5, 0xB0, 0x33, 0xFA,
    0x96, 0x6F, 0x6E, 0xC2, 0xF6, 0x50, 0xFF, 0x5D, 0xA9, 0x8E, 0x17, 0x1B, 0x97, 0x7D, 0xEC, 0x58,
    0xF7, 0x1F, 0xFB, 0x7C, 0x09, 0x0D, 0x7A, 0x67, 0x45, 0x87, 0xDC, 0xE8, 0x4F, 0x1D, 0x4E, 0x04,
    0xEB, 0xF8, 0xF3, 0x3E, 0x3D, 0xBD, 0x8A, 0x88, 0xDD, 0xCD, 0x0B, 0x13, 0x98, 0x02, 0x93, 0x80,
    0x90, 0xD0, 0x24, 0x34, 0xCB, 0xED, 0xF4, 0xCE, 0x99, 0x10, 0x44, 0x40, 0x92, 0x3A, 0x01, 0x26,
    0x12, 0x1A, 0x48, 0x68, 0xF5, 0x81, 0x8B, 0xC7, 0xD6, 0x20, 0x0A, 0x08, 0x00, 0x4C, 0xD7, 0x74
};
const vector<unsigned char> Vec = {
    1, 0x94, 0x20, 0x85, 0x10, 0xC2, 0xC0, 1,
    0xFB, 1, 0xC0, 0xC2, 0x10, 0x85, 0x20, 0x94
};
string toHexString(const vector<unsigned char>& vec) {
    stringstream ss;
    ss << hex << uppercase << setfill('0');
    for (unsigned char byte : vec) {
        ss << setw(2) << (int)byte;
    }
    return ss.str();
}
vector<unsigned char> XOR(const vector<unsigned char>& a, const vector<unsigned char>& b) {
    vector<unsigned char> res(LENGTH);
    for (int i = 0; i < LENGTH; ++i) {
        res[i] = a[i] ^ b[i];
    }
    return res;
}
unsigned char GALUA(unsigned char a, unsigned char b) {
    unsigned char res = 0;
    unsigned char hibit;
    for (int i = 0; i < 8; ++i) {
        if ((b & 1) == 1)
            res ^= a;
        hibit = a & 0x80;
        a <<= 1;
        if (hibit)
            a ^= 0xc3; 
        b >>= 1;
    }
    return res;
}
vector<unsigned char> R(const vector<unsigned char>& a) {
    vector<unsigned char> internal(LENGTH);
    unsigned char a_15 = 0;
    for (int i = 15; i >= 0; --i) {
        if (i == 0)
            internal[15] = a[i];
        else
            internal[i - 1] = a[i];
        a_15 ^= GALUA(a[i], Vec[i]);
    }
    internal[15] = a_15;
    return internal;
}
vector<unsigned char> reverseR(const vector<unsigned char>& a) {
    vector<unsigned char> b(LENGTH);
    unsigned char a_0 = a[15];
    for (int i = 1; i < 16; ++i) {
        b[i] = a[i - 1];
        a_0 ^= GALUA(b[i], Vec[i]);
    }
    b[0] = a_0;
    return b;
}
vector<unsigned char> S(const vector<unsigned char>& a) {
    vector<unsigned char> b(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        b[i] = Sbox[a[i]];
    }
    return b;
}
vector<unsigned char> reverseS(const vector<unsigned char>& a) {
    vector<unsigned char> b(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        b[i] = IncSbox[a[i]];
    }
    return b;
}
vector<unsigned char> L(const vector<unsigned char>& a) {
    vector<unsigned char> b = a;
    for (int i = 0; i < 16; ++i) {
        b = R(b);
    }
    return b;
}
vector<unsigned char> reverseL(const vector<unsigned char>& a) {
    vector<unsigned char> b = a;
    for (int i = 0; i < 16; ++i) {
        b = reverseR(b);
    }
    return b;
}
void GOSTCi(vector<vector<unsigned char>>& Ci) {
    vector<unsigned char> n(LENGTH, 0);
    for (int i = 0; i < 32; ++i) {
        n[0] = i + 1;
        vector<unsigned char> temp = L(n);
        Ci[i] = temp;
    }
}
void ExpandKey(const vector<unsigned char>& key, vector<vector<unsigned char>>& iterKey, vector<vector<unsigned char>>& Ci) {
    GOSTCi(Ci);
    iterKey[0] = vector<unsigned char>(key.begin(), key.begin() + LENGTH);
    vector<unsigned char> currentKey = iterKey[0];
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 8; ++j) {
            vector<unsigned char> temp = XOR(currentKey, Ci[8*i + j]);
            temp = S(temp);
            temp = L(temp);
            if (j % 2 == 0) {
                iterKey[2*i + 1] = temp;
            } else {
                iterKey[2*i + 2] = temp;
            }
            currentKey = temp;
        }
    }
}
vector<unsigned char> EncryptBlock(const vector<unsigned char>& block, const vector<vector<unsigned char>>& key) {
    vector<unsigned char> resblock = block;
    for (int i = 0; i < 9; ++i) {
        resblock = XOR(key[i], resblock);
        resblock = S(resblock);
        resblock = L(resblock);
    }
    resblock = XOR(resblock, key[9]);
    return resblock;
}
vector<unsigned char> DecryptBlock(const vector<unsigned char>& block, const vector<vector<unsigned char>>& key) {
    vector<unsigned char> resblock = block;
    resblock = XOR(resblock, key[9]);
    for (int i = 8; i >= 0; --i) {
        resblock = reverseL(resblock);
        resblock = reverseS(resblock);
        resblock = XOR(key[i], resblock);
    }
    return resblock;
}
vector<unsigned char> generateRandomKey(int len) {
    random_device rd;
    ranlux24_base gen(rd());
    uniform_int_distribution<> distrib(0, 255);
    vector<unsigned char> key(len);
    for (auto& el : key) {
        el = static_cast<unsigned char>(distrib(gen));
    }
    return key;
}
vector<unsigned char> stringToVector(const string& str) {
    return vector<unsigned char>(str.begin(), str.end());
}
string vectorToString(const vector<unsigned char>& vec) {
    return string(vec.begin(), vec.end());
}
vector<unsigned char> hexStringToVector(const string& hex) {
    vector<unsigned char> bytes;
    for (int i = 0; i < hex.length(); i += 2) {
        string byteString = hex.substr(i, 2);
        unsigned char byte = (unsigned char)stoul(byteString, nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}
void saveKey(const vector<unsigned char>& key) {
    ofstream keyFile("keykuz.txt");
    if (!keyFile) {
        throw runtime_error("Не удалось создать файл для сохранения ключа!");
    }
    keyFile << toHexString(key);
    keyFile.close();
}
vector<unsigned char> readKeyFromFile() {
    ifstream keyFile("keykuz.txt");
    if (!keyFile) {
        throw runtime_error("Не удалось открыть файл с ключом!");
    }
    string keyHex;
    keyFile >> keyHex;
    keyFile.close();
    return hexStringToVector(keyHex);
}
bool createFile(const string& filePath) {
    bool forDecryption = false;
    if (fs::exists(filePath)) {
        return true;
    }
    if (forDecryption) {
        return false;
    }
    fs::path path(filePath);
    fs::path dir = path.parent_path();
    if (!dir.empty() && !fs::exists(dir)) {
        cout << "Директория " << dir.string() << " не существует. Создать? (y/n): ";
        char choice;
        cin >> choice;
        if (tolower(choice) != 'y') {
            cout << "Операция отменена." << endl;
            return false;
        }
        try {
            fs::create_directories(dir);
        } catch (const fs::filesystem_error& e) {
            cerr << "Ошибка при создании директории: " << e.what() << endl;
            return false;
        }
    }
    cout << "Файл " << filePath << " не существует. Создать новый файл? (y/n): ";
    char choice;
    cin >> choice;
    cin.ignore();
    if (tolower(choice) != 'y') {
        cout << "Операция отменена." << endl;
        return false;
    }
    cout << "Введите текст для записи в файл: ";
    string fileContent;
    getline(cin, fileContent);
    ofstream outFile(filePath);
    if (!outFile) {
        throw runtime_error("Не удалось создать файл.");
    }
    outFile << fileContent;
    outFile.close();
    cout << "Файл успешно создан: " << filePath << endl;
    return true;
}
void processFile(const string& filePath, bool encrypt, const vector<unsigned char>& key) {
    if (!createFile(filePath)) {
        return;
    }
    ifstream inFile(filePath, ios::binary);
    if (!inFile) {
        throw runtime_error("Не удалось открыть файл: " + filePath);
    }
    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    vector<vector<unsigned char>> Ci(32, vector<unsigned char>(LENGTH));
    vector<vector<unsigned char>> iterKey(10, vector<unsigned char>(LENGTH));
    ExpandKey(key, iterKey, Ci);
    string result;
    if (encrypt) {
        size_t padding = LENGTH - (content.length() % LENGTH);
        if (padding != LENGTH) {
            content.append(padding, ' ');
        }
        for (int i = 0; i < content.length(); i += LENGTH) {
            string blockStr = content.substr(i, LENGTH);
            vector<unsigned char> block = stringToVector(blockStr);
            vector<unsigned char> encryptedBlock = EncryptBlock(block, iterKey);
            result += toHexString(encryptedBlock);
        }
    } 
    else {
        for (int i = 0; i < content.length(); i += 2*LENGTH) {
            string hexBlock = content.substr(i, 2*LENGTH);
            vector<unsigned char> block = hexStringToVector(hexBlock);
            vector<unsigned char> decryptedBlock = DecryptBlock(block, iterKey);
            result += vectorToString(decryptedBlock);
        }
        size_t end = result.find_last_not_of(' ');
        if (end != string::npos) {
            result = result.substr(0, end + 1);
        }
    }
    ofstream outFile(filePath, ios::binary);
    if (!outFile) {
        throw runtime_error("Не удалось перезаписать файл: " + filePath);
    }
    outFile.write(result.c_str(), result.size());
    outFile.close();
}
void KuzFileEnc(const string& filePath) {
    if (!createFile(filePath)) {
        return;
    }
    vector<unsigned char> key = generateRandomKey(LENGTH * 2);
    saveKey(key);
    ifstream inFile(filePath, ios::binary);
    if (!inFile) {
        throw runtime_error("Не удалось открыть файл: " + filePath);
    }
    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    vector<vector<unsigned char>> Ci(32, vector<unsigned char>(LENGTH));
    vector<vector<unsigned char>> iterKey(10, vector<unsigned char>(LENGTH));
    ExpandKey(key, iterKey, Ci);
    size_t padding = LENGTH - (content.length() % LENGTH);
    if (padding != LENGTH) {
        content.append(padding, ' ');
    }
    string result;
    for (int i = 0; i < content.length(); i += LENGTH) {
        string blockStr = content.substr(i, LENGTH);
        vector<unsigned char> block = stringToVector(blockStr);
        vector<unsigned char> encryptedBlock = EncryptBlock(block, iterKey);
        result += toHexString(encryptedBlock);
    }
    ofstream outFile(filePath, ios::binary);
    if (!outFile) {
        throw runtime_error("Не удалось перезаписать файл: " + filePath);
    }
    outFile.write(result.c_str(), result.size());
    outFile.close();
    cout << "Файл успешно зашифрован." << endl;
}
void KuzFileDec(const string& filePath) {
    if (!fs::exists(filePath)) {
        cerr << "Файл не существует." << endl;
        return;
    }
    vector<unsigned char> key;
    try {
        key = readKeyFromFile();
        if (key.empty()) {
            cerr << "Не удалось загрузить ключи из файла." << endl;
            return;
        }
        if (key.size() != LENGTH * 2) {
            cerr << "Не удалось загрузить ключи из файла." << endl;
            return;
        }
    } 
    catch (const exception& e) {
        cerr << e.what() << endl;
        return;
    }
    ifstream inFile(filePath, ios::binary);
    if (!inFile) {
        cerr << "Не удалось открыть файл " << filePath << endl;
        return;
    }
    string content((istreambuf_iterator<char>(inFile)), istreambuf_iterator<char>());
    inFile.close();
    if (content.empty()) {
        cerr << "Файл пуст." << endl;
        return;
    }
    vector<vector<unsigned char>> Ci(32, vector<unsigned char>(LENGTH));
    vector<vector<unsigned char>> iterKey(10, vector<unsigned char>(LENGTH));
    try {
        ExpandKey(key, iterKey, Ci);
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return;
    }
    string result;
        for (size_t i = 0; i < content.length(); i += 2*LENGTH) {
            string hexBlock = content.substr(i, 2*LENGTH);
            if (hexBlock.empty()) continue;
            vector<unsigned char> block = hexStringToVector(hexBlock);
            vector<unsigned char> decBlock = DecryptBlock(block, iterKey);
            result += vectorToString(decBlock);
        }
    size_t end = result.find_last_not_of(' ');
    if (end != string::npos) {
        result = result.substr(0, end + 1);
    }
    ofstream outFile(filePath, ios::binary);
    if (!outFile) {
        cerr << "Ошибка: Не удалось записать результат в файл." << endl;
        return;
    }
    outFile.write(result.c_str(), result.size());
    outFile.close();
    cout << "Файл успешно расшифрован." << endl;
}
void KuzTerEnc() {
    cin.ignore();
    string message;
    cout << "Введите текст для шифровки: ";
    getline(cin, message);
    vector<unsigned char> key = generateRandomKey(LENGTH * 2);
    saveKey(key);
    vector<vector<unsigned char>> Ci(32, vector<unsigned char>(LENGTH));
    vector<vector<unsigned char>> iterkey(10, vector<unsigned char>(LENGTH));
    ExpandKey(key, iterkey, Ci);
    size_t padding = LENGTH - (message.length() % LENGTH);
    if (padding != LENGTH) {
        message.append(padding, ' ');
    }
    string encHex;
    for (int i = 0; i < message.length(); i += LENGTH) {
        string blockStr = message.substr(i, LENGTH);
        vector<unsigned char> block = stringToVector(blockStr);
        vector<unsigned char> encBlock = EncryptBlock(block, iterkey);
        encHex += toHexString(encBlock);
    }
    cout << "\nЗашифрованное сообщение (hex): " << encHex << endl;
}
void KuzTerDec() {
    vector<unsigned char> key;
    try {
        key = readKeyFromFile();
        if (key.empty()) {
            cerr << "Не удалось загрузить ключи из файла." << endl;
            return;
        }
        if (key.size() != LENGTH * 2) {
            cerr << "Не удалось загрузить ключи из файла." << endl;
            return;
        }
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return;
    }
    cin.ignore();
    string encHex;
    cout << "Введите зашифрованное сообщение (hex): ";
    getline(cin, encHex);
    if (encHex.empty()) {
        cerr << "Текст пуст." << endl;
        return;
    }
    vector<vector<unsigned char>> Ci(32, vector<unsigned char>(LENGTH));
    vector<vector<unsigned char>> iterkey(10, vector<unsigned char>(LENGTH));
    try {
        ExpandKey(key, iterkey, Ci);
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return;
    }
    string decText;
    try {
        for (size_t i = 0; i < encHex.length(); i += 2*LENGTH) {
            string hexBlock = encHex.substr(i, 2*LENGTH);
            if (hexBlock.empty()) continue;
            
            vector<unsigned char> block = hexStringToVector(hexBlock);
            if (block.size() != LENGTH) {
                cerr << "Предупреждение: Неполный блок в позиции " << i << endl;
                continue;
            }
            vector<unsigned char> decryptedBlock = DecryptBlock(block, iterkey);
            decText += vectorToString(decryptedBlock);
        }
    } catch (const exception& e) {
        cerr << "Ошибка расшифровки: " << e.what() << endl;
        return;
    }
    size_t end = decText.find_last_not_of(' ');
    if (end != string::npos) {
        decText = decText.substr(0, end + 1);
    }
    cout << "Расшифрованное сообщение: " << decText << endl;
}
int Kuzia() {
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
                KuzTerEnc();
            } else {
                KuzTerDec();
            }
        } else {
            cin.ignore();
            string filePath;
            cout << "Введите путь к файлу: ";
            getline(cin, filePath);
            if (mode == 1) {
                KuzFileEnc(filePath);
            } else {
                KuzFileDec(filePath);
            }
        }
        return 0;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
}
