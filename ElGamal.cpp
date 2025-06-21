#include "ElGamal.h"
namespace fs = std::filesystem;
using namespace std;
bool fileExists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}
bool dirExists(const string& dirPath) {
    struct stat buffer;
    if (stat(dirPath.c_str(), &buffer)) return false;
    return S_ISDIR(buffer.st_mode);
}
bool createFileWithContent(const string& filePath, const string& content = "") {
    ofstream file(filePath);
    if (file.is_open()) {
        file << content;
        file.close();
        return true;
    }
    return false;
}
string getFilePathWithChecks(const string& prompt, bool& wasCreated, bool forDecryption = false) {
    wasCreated = false;
    string filePath;
    while (true) {
        cout << prompt;
        getline(cin, filePath);
        if (!fs::exists(filePath)) {
            if (forDecryption) {
                return filePath;
            }
            fs::path path(filePath);
            fs::path dir = path.parent_path();
            if (!dir.empty() && !fs::exists(dir)) {
                cout << "Директория не существует. Создать? (y/n): ";
                char choice;
                cin >> choice;
                cin.ignore();
                if (tolower(choice) != 'y') {
                    cout << "Операция отменена. Возвращаю на главный экран." << endl;
                    exit(0);
                }
                try {
                    fs::create_directories(dir);
                    cout << "Директория успешно создана." << endl;
                } catch (const fs::filesystem_error& e) {
                    cerr << "Ошибка при создании директории: " << e.what() << endl;
                    continue;
                }
            }
            cout << "Файл не существует. Создать новый файл? (y/n): ";
            char choice;
            cin >> choice;
            cin.ignore();
            if (tolower(choice) != 'y') {
                cout << "Операция отменена. Возвращаю на главный экран." << endl;
                exit(0);
            }
            cout << "Введите текст для записи в файл: ";
            string fileContent;
            getline(cin, fileContent);
            ofstream file(filePath);
            if (file.is_open()) {
                file << fileContent;
                file.close();
                wasCreated = true;
                cout << "Файл успешно создан." << endl;
                return filePath;
            } else {
                cerr << "Не удалось создать файл." << endl;
                continue;
            }
        } else {
            return filePath;
        }
    }
}
bool prostota_gmp(mpz_t p) {
    int reps = 25;
    if (mpz_cmp_ui(p, 2) < 0) return false;
    if (mpz_cmp_ui(p, 2) == 0 || mpz_cmp_ui(p, 3) == 0) return true;
    if (mpz_even_p(p)) return false;
    return mpz_probab_prime_p(p, reps) > 0;
}
void Bin(mpz_t result, mpz_t a, mpz_t x, mpz_t p) {
    mpz_t b;
    mpz_init(b);
    mpz_mod(b, a, p);
    mpz_set_ui(result, 1);
    mpz_t temp_x;
    mpz_init(temp_x);
    mpz_set(temp_x, x);
    while (mpz_cmp_ui(temp_x, 0) > 0) {
        if (mpz_odd_p(temp_x)) {
            mpz_mul(result, result, b);
            mpz_mod(result, result, p);
        }
        mpz_tdiv_q_2exp(temp_x, temp_x, 1);
        mpz_mul(b, b, b);
        mpz_mod(b, b, p);
    }
    mpz_clear(b);
    mpz_clear(temp_x);
}
void RandomNumbers_gmp(mpz_t result, mpz_t start, mpz_t end) {
    mpz_t range;
    mpz_init(range);
    mpz_sub(range, end, start);
    mpz_add_ui(range, range, 1);
    gmp_randstate_t randstate;
    gmp_randinit_mt(randstate);
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    gmp_randseed_ui(randstate, seed);
    mpz_urandomm(result, randstate, range);
    mpz_add(result, result, start);
    mpz_clear(range);
    gmp_randclear(randstate);
}
void findPrimitiveRoot(mpz_t result, mpz_t p) {
    mpz_t phi;
    mpz_init(phi);
    mpz_sub_ui(phi, p, 1);
    mpz_t n;
    mpz_init_set(n, phi);
    mpz_t i;
    mpz_init(i);
    mpz_t factors[100];
    int factorCount = 0;
    for (mpz_set_ui(i, 2); mpz_cmp(i, n) <= 0; mpz_add_ui(i, i, 1)) {
        mpz_t temp;
        mpz_init(temp);
        mpz_set(temp, n);
        if (prostota_gmp(p)) {
            while (mpz_divisible_p(temp, i)) {
                mpz_tdiv_q(temp, temp, i);
                if (factorCount < 100) {
                    mpz_init(factors[factorCount]);
                    mpz_set(factors[factorCount], i);
                    factorCount++;
                } else {
                    exit(1);
                }
                mpz_set(n, temp);
            }
        }
        mpz_clear(temp);
        if (mpz_cmp(i, n) > 0)
            break;
    }
    mpz_t a;
    mpz_init_set_ui(a, 2);
    mpz_t limit;
    mpz_init(limit);
    mpz_sub_ui(limit, p, 1);
    bool found = false;
    while (mpz_cmp(a, limit) < 0 && !found) {
        bool isPrimitiveRoot = true;
        for (int j = 0; j < factorCount; j++) {
            mpz_t exp;
            mpz_init(exp);
            mpz_tdiv_q(exp, phi, factors[j]);
            mpz_t result_pow;
            mpz_init(result_pow);
            Bin(result_pow, a, exp, p);
            if (mpz_cmp_ui(result_pow, 1) == 0) {
                isPrimitiveRoot = false;
                mpz_clear(result_pow);
                mpz_clear(exp);
                break;
            }
            mpz_clear(result_pow);
            mpz_clear(exp);
        }
        if (isPrimitiveRoot) {
            mpz_set(result, a);
            found = true;
            break;
        }
        mpz_add_ui(a, a, 1);
    }
    if (!found) {
        mpz_set_ui(result, 0);
    }
    mpz_clear(phi);
    mpz_clear(n);
    mpz_clear(i);
    mpz_clear(a);
    mpz_clear(limit);
    for (int j = 0; j < factorCount; j++) {
        mpz_clear(factors[j]);
    }
}
string Terminal() {
    string input;
    cout << "Введите текст: ";
    getline(cin, input);
    return input;
}
mpz_class generatePrime(int numBits) {
    gmp_randclass rnd(gmp_randinit_mt);
    rnd.seed(time(NULL));
    mpz_class prime;
    do {
        prime = rnd.get_z_bits(numBits);
        mpz_setbit(prime.get_mpz_t(), numBits - 1);
        mpz_nextprime(prime.get_mpz_t(), prime.get_mpz_t());
    } while (mpz_probab_prime_p(prime.get_mpz_t(), 25) == 0);
    return prime;
}
pair<mpz_class, mpz_class> encryptByte(unsigned char byte, mpz_t g, mpz_t y, mpz_t p) {
    mpz_class m = static_cast<int>(byte);
    mpz_t k;
    mpz_init(k);
    mpz_t p_minus_2;
    mpz_init(p_minus_2);
    mpz_sub_ui(p_minus_2, p, 2);
    RandomNumbers_gmp(k, mpz_class(1).get_mpz_t(), p_minus_2);
    mpz_class a;
    mpz_powm(a.get_mpz_t(), g, k, p);
    mpz_class b;
    mpz_powm(b.get_mpz_t(), y, k, p);
    b = (b * m) % mpz_class(p);
    mpz_clear(k);
    mpz_clear(p_minus_2);
    return {a, b};
}
unsigned char decryptByte(const pair<mpz_class, mpz_class>& ciphertext, mpz_t x, mpz_t p) {
    mpz_t s, s_inv, m;
    mpz_init(s);
    mpz_init(s_inv);
    mpz_init(m);
    mpz_powm(s, ciphertext.first.get_mpz_t(), x, p);
    if (mpz_invert(s_inv, s, p) == 0) {
        return 0;
    }
    mpz_mul(m, ciphertext.second.get_mpz_t(), s_inv);
    mpz_mod(m, m, p);
    unsigned char decryptedByte = static_cast<unsigned char>(mpz_get_ui(m));
    mpz_clear(s);
    mpz_clear(s_inv);
    mpz_clear(m);
    return decryptedByte;
}
vector<unsigned char> readBinaryFile(const string& filename) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw runtime_error("Не удалось прочитать файл: " + filename);
    }
    return buffer;
}
void writeBinaryFile(const string& filename, const vector<unsigned char>& data) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл для записи: " + filename);
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
}
void writeEncryptedData(const string& filename, const vector<pair<mpz_class, mpz_class>>& ciphertext) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл для записи: " + filename);
    }
    for (const auto& pair : ciphertext) {
        string a_str = pair.first.get_str();
        string b_str = pair.second.get_str();
        size_t a_len = a_str.size();
        file.write(reinterpret_cast<const char*>(&a_len), sizeof(a_len));
        file.write(a_str.c_str(), a_len);
        size_t b_len = b_str.size();
        file.write(reinterpret_cast<const char*>(&b_len), sizeof(b_len));
        file.write(b_str.c_str(), b_len);
    }
}
vector<pair<mpz_class, mpz_class>> readEncryptedData(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл: " + filename);
    }
    vector<pair<mpz_class, mpz_class>> ciphertext;
    while (true) {
        size_t a_len;
        file.read(reinterpret_cast<char*>(&a_len), sizeof(a_len));
        if (file.eof()) break;
        vector<char> a_buf(a_len);
        file.read(a_buf.data(), a_len);
        string a_str(a_buf.begin(), a_buf.end());
        size_t b_len;
        file.read(reinterpret_cast<char*>(&b_len), sizeof(b_len));
        vector<char> b_buf(b_len);
        file.read(b_buf.data(), b_len);
        string b_str(b_buf.begin(), b_buf.end());
        mpz_class a(a_str, 10);
        mpz_class b(b_str, 10);
        ciphertext.emplace_back(a, b);
    }
    return ciphertext;
}
int ElGamDec(bool useFile) {
    string filePath;
    bool fileWasCreated = false;
    if (useFile) {
        filePath = getFilePathWithChecks("Введите полный путь к файлу: ", fileWasCreated, true);
    } else {
        cout << "Введите зашифрованный текст в формате ((a,b) (a,b) и т.д.): ";
        string ciphertextStr;
        getline(cin, ciphertextStr);
        vector<pair<mpz_class, mpz_class>> ciphertext;
        string currentPair;
        mpz_class a, b;
        size_t start = 0;
        size_t end;
        while ((start = ciphertextStr.find('(', start)) != string::npos) {
            end = ciphertextStr.find(')', start);
            if (end == string::npos) {
                cerr << "Неверный формат зашифрованного текста." << endl;
                return 1;
            }
            currentPair = ciphertextStr.substr(start + 1, end - start - 1);
            size_t commaPos = currentPair.find(',');
            if (commaPos == string::npos) {
                cerr << "Неверный формат зашифрованного текста." << endl;
                return 1;
            }
            try {
                a.set_str(currentPair.substr(0, commaPos), 10);
                b.set_str(currentPair.substr(commaPos + 1), 10);
                ciphertext.push_back({a, b});
            }
            catch (std::invalid_argument const& e) {
                std::cerr << e.what() << std::endl;
                return 1;
            }
            start = end + 1;
        }
        if (ciphertext.empty()) {
            cerr << "Не найдено ни одной пары зашифрованных данных." << endl;
            return 1;
        }
        mpz_class x, p;
        ifstream keyFile("keyElGamal.txt");
        if (!keyFile.is_open()) {
            cerr << "Не удалось открыть файл с ключом!" << endl;
            return 1;
        }
        string line;
        getline(keyFile, line);
        size_t xPos = line.find("x = ");
        if (xPos != string::npos) {
            x.set_str(line.substr(xPos + 4), 10);
        } else {
            cerr << "Неверный формат файла ключей (x)." << endl;
            keyFile.close();
            return 1;
        }
        getline(keyFile, line);
        size_t pPos = line.find("p = ");
        if (pPos != string::npos) {
            p.set_str(line.substr(pPos + 4), 10);
        } else {
            cerr << "Неверный формат файла ключей (p)." << endl;
            keyFile.close();
            return 1;
        }
        keyFile.close();
        mpz_t p_mpz, x_mpz;
        mpz_init(p_mpz);
        mpz_init(x_mpz);
        mpz_set(p_mpz, p.get_mpz_t());
        mpz_set(x_mpz, x.get_mpz_t());
        vector<unsigned char> decryptedBytes;
        for (const auto& pair : ciphertext) {
            decryptedBytes.push_back(decryptByte(pair, x_mpz, p_mpz));
        }
        mpz_clear(p_mpz);
        mpz_clear(x_mpz);
        cout << "Расшифрованные данные: ";
        for (unsigned char byte : decryptedBytes) {
            if (isprint(byte)) {
                cout << byte;
            } else {
                cout << "\\x" << hex << setw(2) << setfill('0') << static_cast<int>(byte);
            }
        }
        cout << endl;
        return 0;
    }
    vector<pair<mpz_class, mpz_class>> ciphertext;
    try {
        ciphertext = readEncryptedData(filePath);
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    mpz_class x, p;
    ifstream keyFile("keyElGamal.txt");
    if (!keyFile.is_open()) {
        cerr << "Не удалось открыть файл с ключом!" << endl;
        return 1;
    }
    string line;
    getline(keyFile, line);
    size_t xPos = line.find("x = ");
    if (xPos != string::npos) {
        x.set_str(line.substr(xPos + 4), 10);
    } else {
        cerr << "Неверный формат файла ключей (x)." << endl;
        keyFile.close();
        return 1;
    }
    getline(keyFile, line);
    size_t pPos = line.find("p = ");
    if (pPos != string::npos) {
        p.set_str(line.substr(pPos + 4), 10);
    } else {
        cerr << "Неверный формат файла ключей (p)." << endl;
        keyFile.close();
        return 1;
    }
    keyFile.close();
    mpz_t p_mpz, x_mpz;
    mpz_init(p_mpz);
    mpz_init(x_mpz);
    mpz_set(p_mpz, p.get_mpz_t());
    mpz_set(x_mpz, x.get_mpz_t());
    vector<unsigned char> decryptedBytes;
    for (const auto& pair : ciphertext) {
        decryptedBytes.push_back(decryptByte(pair, x_mpz, p_mpz));
    }
    mpz_clear(p_mpz);
    mpz_clear(x_mpz);
    try {
        writeBinaryFile(filePath, decryptedBytes);
        cout << "Файл успешно расшифрован и перезаписан: " << filePath << endl;
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    return 0;
}
int ElGamEnc(bool useFile) {
    string myText;
    string filePath;
    bool fileWasCreated = false;
    if (useFile) {
        filePath = getFilePathWithChecks("Введите полный путь к файлу: ", fileWasCreated);
    } else {
        myText = Terminal();
    }
    mpz_t p, g, x, y;
    mpz_init(p);
    mpz_init(g);
    mpz_init(x);
    mpz_init(y);
    mpz_class prime_mpz = generatePrime(16);
    mpz_set(p, prime_mpz.get_mpz_t());
    findPrimitiveRoot(g, p);
    mpz_t p_minus_1;
    mpz_init(p_minus_1);
    mpz_sub_ui(p_minus_1, p, 1);
    RandomNumbers_gmp(x, mpz_class(2).get_mpz_t(), p_minus_1);
    ofstream keyFile("keyElGamal.txt");
    if (!keyFile.is_open()) {
        cerr << "Не удалось создать файл для сохранения ключа!" << endl;
        return 1;
    }
    keyFile << "x = " << mpz_class(x).get_str() << endl;
    keyFile << "p = " << mpz_class(p).get_str() << endl;
    keyFile.close();
    cout << "Ключи сохранены в keyElGamal.txt" << endl;
    Bin(y, g, x, p);
    vector<pair<mpz_class, mpz_class>> ciphertext;
    if (useFile) {
        vector<unsigned char> fileData;
        try {
            fileData = readBinaryFile(filePath);
        } catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
        for (unsigned char byte : fileData) {
            ciphertext.push_back(encryptByte(byte, g, y, p));
        }
        try {
            writeEncryptedData(filePath, ciphertext);
            cout << "Файл успешно зашифрован и перезаписан: " << filePath << endl;
        } catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
    } else {
        for (unsigned char character : myText) {
            ciphertext.push_back(encryptByte(character, g, y, p));
        }
        cout << "Зашифрованный текст:" << endl;
        for (const auto& pair : ciphertext) {
            cout << "(" << pair.first.get_str() << "," << pair.second.get_str() << ") ";
        }
        cout << endl;
    }
    mpz_clear(p);
    mpz_clear(g);
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(p_minus_1);
    return 0;
}
int ElGamal_gmp() {
    int mode = 0, ioMethod = 0;
    bool validInput = false;
    while (!validInput) {
        try {
            cout << "1 - Шифровка, 2 - Расшифровка: ";
            if (!(cin >> mode) || (mode != 1 && mode != 2)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("Допустимые операции обозначены ЦИФРАМИ 1 и 2.");
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
            if (!(cin >> ioMethod) || (ioMethod != 1 && ioMethod != 2)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                throw runtime_error("Допустимые операции обозначены ЦИФРАМИ 1 и 2.");
            }
            validInput = true;
        } catch (const exception& e) {
            cerr << e.what() << endl;
        }
    }
    cin.ignore();
    bool useFile = (ioMethod == 2);
    if (mode == 1) {
        return ElGamEnc(useFile);
    } else {
        return ElGamDec(useFile);
    }
}
