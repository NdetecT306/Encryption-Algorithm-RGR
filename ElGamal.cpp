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
                    cout << "Директория успешно создана: " << dir.string() << endl;
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
                cout << "Файл успешно создан: " << filePath << endl;
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
void setRussianLocale() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    std::locale::global(std::locale("ru_RU.UTF-8"));
}
string Terminal() {
    setRussianLocale();
    string input;
    cout << "Введите текст для шифровки: ";
    getline(cin, input);
    return input;
}
vector<int> getUnicodeCodepoints(const string& utf8_string) {
    vector<int> codepoints;
    wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    wstring wide_string = converter.from_bytes(utf8_string);
    for (wchar_t wc : wide_string) {
        codepoints.push_back(static_cast<int>(wc));
    }
    return codepoints;
}
string utf8_encode(const std::vector<int>& codepoints) {
    wstring wide_string;
    for (int codepoint : codepoints) {
        wide_string += static_cast<wchar_t>(codepoint);
    }
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wide_string);
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
pair<mpz_class, mpz_class> encryptCodepoint(int codepoint, mpz_t g, mpz_t y, mpz_t p, ofstream& keyFile) {
    mpz_class m = codepoint; 
    mpz_t k;
    mpz_init(k);
    mpz_t p_minus_2;
    mpz_init(p_minus_2);
    mpz_sub_ui(p_minus_2, p, 2);
    RandomNumbers_gmp(k, mpz_class(1).get_mpz_t(), p_minus_2); 
    if (keyFile.is_open()) {
        keyFile << mpz_class(k).get_str() << " ";
    }
    mpz_class a;
    mpz_powm(a.get_mpz_t(), g, k, p);
    mpz_class b;
    mpz_powm(b.get_mpz_t(), y, k, p);
    b = (b * m) % mpz_class(p);
    mpz_clear(k);
    mpz_clear(p_minus_2);
    return {a, b};
}
int decryptCodepoint(const std::pair<mpz_class, mpz_class>& ciphertext, mpz_t x, mpz_t p) {
    mpz_t s, s_inv, m;
    mpz_init(s);
    mpz_init(s_inv);
    mpz_init(m);
    mpz_powm(s, ciphertext.first.get_mpz_t(), x, p);
    if (mpz_invert(s_inv, s, p) == 0) {
        cerr << "Ошибка: невозможно вычислить обратный элемент." << endl;
        mpz_clear(s);
        mpz_clear(s_inv);
        mpz_clear(m);
        return -1; 
    }
    mpz_mul(m, ciphertext.second.get_mpz_t(), s_inv);
    mpz_mod(m, m, p);
    int decryptedCodepoint = mpz_get_si(m); 
    mpz_clear(s);
    mpz_clear(s_inv);
    mpz_clear(m);
    return decryptedCodepoint;
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
        if (!file.read(reinterpret_cast<char*>(&a_len), sizeof(a_len))) {
            if (file.eof()) break;
            else {
                throw runtime_error("Ошибка при чтении длины a: " + filename);
            }
        }
        vector<char> a_buf(a_len);
        if (!file.read(a_buf.data(), a_len)) {
            throw runtime_error("Ошибка при чтении данных a: " + filename);
        }
        string a_str(a_buf.begin(), a_buf.end());
        size_t b_len;
        if (!file.read(reinterpret_cast<char*>(&b_len), sizeof(b_len))) {
            throw runtime_error("Ошибка при чтении длины b: " + filename);
        }
        vector<char> b_buf(b_len);
        if (!file.read(b_buf.data(), b_len)) {
             throw runtime_error("Ошибка при чтении данных b: " + filename);
        }
        string b_str(b_buf.begin(), b_buf.end());
        mpz_class a(a_str, 10);
        mpz_class b(b_str, 10);
        ciphertext.emplace_back(a, b);
    }
    return ciphertext;
}
int ElGamDec(bool useFile) {
    setlocale(LC_ALL, "Russian");
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());
    string filePath;
    bool fileWasCreated = false;
    vector<pair<mpz_class, mpz_class>> ciphertext;
    if (useFile) {
        filePath = getFilePathWithChecks("Введите полный путь к файлу для расшифровки: ", fileWasCreated, true);
        try {
            ciphertext = readEncryptedData(filePath);
        } catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
    } else {
        cout << "Введите зашифрованный текст((a,b) (a,b) и т.д.): ";
        string ciphertextStr;
        getline(cin, ciphertextStr);
        ciphertextStr.erase(remove_if(ciphertextStr.begin(), ciphertextStr.end(), ::isspace), ciphertextStr.end());
        size_t start = 0;
        size_t end;
        while ((start = ciphertextStr.find('(', start)) != string::npos) {
            end = ciphertextStr.find(')', start);
            if (end == string::npos) {
                cerr << "Неверный формат зашифрованного текста." << endl;
                return 1;
            }
            string currentPair = ciphertextStr.substr(start + 1, end - start - 1);
            size_t commaPos = currentPair.find(',');
            if (commaPos == string::npos) {
                cerr << "Неверный формат пары: отсутствует запятая." << endl;
                return 1;
            }
            try {
                string a_str = currentPair.substr(0, commaPos);
                string b_str = currentPair.substr(commaPos + 1);
                if (a_str.empty() || b_str.empty()) {
                    cerr << "Ошибка: пустые значения в паре." << endl;
                    return 1;
                }
                mpz_class a(a_str.c_str());
                mpz_class b(b_str.c_str());
                ciphertext.emplace_back(a, b);
            } catch (...) {
                cerr << "Ошибка преобразования строки в число. Убедитесь, что числа корректны." << endl;
                return 1;
            }
            start = end + 1;
        }
        if (ciphertext.empty()) {
            cerr << "Не найдено ни одной пары зашифрованных данных." << endl;
            return 1;
        }
    }
    mpz_class x, p;
    ifstream keyFile("keyElGamal.txt");
    if (!keyFile.is_open()) {
        cerr << "Не удалось открыть файл keyElGamal.txt" << endl;
        return 1;
    }
    string line;
    bool x_found = false, p_found = false;
    while (getline(keyFile, line)) {
        if (line.find("x = ") == 0) {
            x.set_str(line.substr(4), 10);
            x_found = true;
        } else if (line.find("p = ") == 0) {
            p.set_str(line.substr(4), 10);
            p_found = true;
        }
    }
    keyFile.close();
    if (!x_found || !p_found) {
        cerr << "Не удалось загрузить ключи x или p из файла." << endl;
        return 1;
    }
    mpz_t p_mpz, x_mpz;
    mpz_init(p_mpz);
    mpz_init(x_mpz);
    mpz_set(p_mpz, p.get_mpz_t());
    mpz_set(x_mpz, x.get_mpz_t());
    vector<int> decryptedCodepoints; 
    for (const auto& pair : ciphertext) {
        int decryptedCodepoint = decryptCodepoint(pair, x_mpz, p_mpz);
        if (decryptedCodepoint != -1) { 
            decryptedCodepoints.push_back(decryptedCodepoint);
        } else {
            cerr << "Ошибка при расшифровке кодовой точки." << endl;
            mpz_clear(p_mpz);
            mpz_clear(x_mpz);
            return 1; 
        }
    }
    mpz_clear(p_mpz);
    mpz_clear(x_mpz);
    if (useFile) {
        try {
             string outputFilename = filePath;
            if (outputFilename.rfind(".enc") == outputFilename.length() - 4) {
                outputFilename = outputFilename.substr(0, outputFilename.length() - 4);
            } else {
                 outputFilename += ".dec";
            }
            string decryptedText = utf8_encode(decryptedCodepoints);
            vector<unsigned char> decryptedData(decryptedText.begin(), decryptedText.end());
            writeBinaryFile(outputFilename, decryptedData);
            cout << "Файл успешно расшифрован и сохранен как: " << outputFilename << endl;
        } catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
    } else {
       try {
            std::string decryptedText = utf8_encode(decryptedCodepoints);
            cout << "Расшифрованный текст: " << decryptedText << endl;
        } catch (const exception& e) {
            cerr << e.what() << endl;
            cout << "Расшифрованные данные: ";
                for (int codepoint : decryptedCodepoints) {
                     if (codepoint >= 0 && codepoint <= 255 && isprint(static_cast<unsigned char>(codepoint))) {
                        cout << static_cast<unsigned char>(codepoint);
                    } else {
                        cout << "\\x" << hex << setw(2) << setfill('0') << codepoint;
                    }
                }
            cout << endl;
        }
    }
    return 0;
}
int ElGamEnc(bool useFile) {
    setRussianLocale();
    string myText;
    string filePath;
    bool fileWasCreated = false;
    vector<int> inputCodepoints;
    if (useFile) {
        filePath = getFilePathWithChecks("Введите полный путь к файлу для шифрования: ", fileWasCreated);
        try {
            vector<unsigned char> inputData = readBinaryFile(filePath);
            string utf8Text(inputData.begin(), inputData.end());
            inputCodepoints = getUnicodeCodepoints(utf8Text);
        } catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
    } else {
        string inputText = Terminal();
        inputCodepoints = getUnicodeCodepoints(inputText);
    }
        mpz_t p, g, x, y;
    mpz_init(p);
    mpz_init(g);
    mpz_init(x);
    mpz_init(y);
    mpz_class prime_mpz = generatePrime(8); 
    mpz_set(p, prime_mpz.get_mpz_t());
    findPrimitiveRoot(g, p);
    mpz_t p_minus_1;
    mpz_init(p_minus_1);
    mpz_sub_ui(p_minus_1, p, 1);
    RandomNumbers_gmp(x, mpz_class(2).get_mpz_t(), p_minus_1);
    Bin(y, g, x, p);
    ofstream keyFile("keyElGamal.txt");
    if (!keyFile.is_open()) {
        cerr << "Не удалось создать файл keyElGamal.txt для хранения ключей." << endl;
        return 1;
    }
    keyFile << "x = " << mpz_class(x).get_str() << endl;
    keyFile << "p = " << mpz_class(p).get_str() << endl;
    keyFile << "g = " << mpz_class(g).get_str() << endl;
    keyFile << "k = ";  
    vector<pair<mpz_class, mpz_class>> ciphertext;
    for (int codepoint : inputCodepoints) {
        ciphertext.push_back(encryptCodepoint(codepoint, g, y, p, keyFile));
    }
    if (useFile) {
        try {
            writeEncryptedData(filePath + ".enc", ciphertext); 
            cout << "Файл успешно зашифрован и сохранен как: " << filePath + ".enc" << endl;
        } catch (const exception& e) {
            cerr << e.what() << endl;
            return 1;
        }
    } else {
        cout << "Зашифрованный текст:" << endl;
        for (const auto& pair : ciphertext) {
            cout << "(" << pair.first.get_str() << "," << pair.second.get_str() << ") ";
        }
        cout << endl;
    }
    keyFile.close();
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















