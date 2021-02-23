#pragma once
#include<iostream>
#include<iostream>
#include<fstream>
#include<string>

using namespace std;

class Exception
{
protected:
    int error;
public:
    Exception()
    {
        error = 0;
    }
    Exception(int _error)
    {
        error = _error;
    }
    ~Exception()
    {

    };
    void Show()
    {
        if (this->error == 0)
        {
            cout << "Input error! Enter int!" << endl;
        }
        if (this->error == 1)
        {
            cout << "Opening error!" << endl;
        }
    }
};


class Abonent
{
private:
    int _yearNumber;
    string _phoneNumber;
public:
    string _fio;
    Abonent() : _fio("abonent"), _yearNumber(0), _phoneNumber("0") {}
    Abonent(string f, int g, string s) : _fio(f), _yearNumber(g), _phoneNumber(s) {}
    void setFio(string f)
    {
        _fio = f;
    }
    void setYearNumber(int g)
    {
        _yearNumber = g;
    }
    void setPhoneNumber(string s)
    {
        _phoneNumber = s;
    }
    string getFio()
    {
        return _fio;
    }
    int getYearNumber()
    {
        return _yearNumber;
    }
    string getPhoneNumber()
    {
        return _phoneNumber;
    }

    friend ostream& operator<<(ostream& out, const Abonent t)
    {
        out << "\nFull-Сontact FIO = " << t._fio << "\nYEAR = " << t._yearNumber << "\nNumber =" << t._phoneNumber << endl;
        return out;
    }

    friend istream& operator>>(istream& in, Abonent& t)
    {
        in >> t._fio;
        in >> t._yearNumber;
        in >> t._phoneNumber;
        return in;
    }

    friend bool operator==(Abonent t1, Abonent t2) {
        if (t1._yearNumber == t2._yearNumber && t1._phoneNumber == t2._phoneNumber && t1._fio == t2._fio)
            return true;
        else
            return false;
    }

    friend bool operator<(const Abonent t1, const Abonent t2)
    {
        if (t1._yearNumber < t2._yearNumber)
            return true;
        else
            return false;
    }

    friend bool operator>(const Abonent t1, const Abonent t2)
    {
        if (t1._yearNumber > t2._yearNumber)
            return true;
        else
            return false;
    }
};

using namespace std;

class  Node
{
public:
    Abonent data;
    Node* next;
    Node* prev;
    Node() : data(), next(nullptr) { }

    Node(Abonent d) : data(d), next(nullptr) { }
};


class Deque
{
private:
    Node* head;
    Node* tail;
    int size;
public:

    Deque() :head(nullptr), size(0) {}

    void push_front(Abonent x)
    {
        if (head == nullptr)
        {
            Node* element = new Node(x);
            head = new Node(x);
            element->next = head;
            head = element;
            size++;
        }
        else
        {
            Node* tmp = head;
            int i = 0;
            while (i < size - 1)
            {
                tmp = tmp->next;
                i++;
            }
            tmp->next = new Node(x);
            tmp->next->next = head;
            size++;
        }
    }
    void pop_front()
    {
        if (size == 0)
            throw Exception();
        Node* temp = head;
        head = head->next;
        delete temp;
        size--;
        Node* t = head;
        int i = 0;
        while (i < size - 1)
        {
            t = t->next;
            i++;
        }
        t->next = head;
        tail = 0;
    }

    void outputTaskByYear(int ye)
    {
        if (size == 0)
            throw Exception();
        Node* temp = head;
        int i = 0;
        while (i < size)
        {
            if (temp->data.getYearNumber() >= ye) {
                cout << "   " << temp->data;
            }
            temp = temp->next;
            i++;
        }
    }

    Node* getNode()
    {
        return head;
    }

    int getSize()
    {
        return size;
    }

    friend ostream& operator<<(ostream& out, const Node& node)
    {
        out << node.data;
        return out;
    }
};


class Iterator
{
private:
    Node* node;
    int size;
    int current;
public:

    Iterator(Deque* element) : current(0)
    {
        node = element->getNode();
        size = element->getSize();
    }

    void begin()
    {
        if (size == 0)
            throw Exception();
        if (current != 0)
        {
            while (current < size)
            {
                current++;
                node = node->next;
            }
            current = 0;
        }

    }

    void searchAbn(Abonent x)
    {
        if (size == 0)
            throw Exception();
        int i = 0;
        while (i < size)
        {
            if (x == node->data)
            {
                cout << "\nAbonent\n " << x << "\nisFound" << endl;
                return;
            }
            if (current == size - 1)
                current = 0;
            else
                current++;
            node = node->next;
            i++;
        }
        cout << "\nAbonent\n " << x << " \nis not Found" << endl;
    }

    Iterator& operator++()
    {
        if (size == 0)
            throw Exception();
        if (size == 1)
            current = 0;
        else
            current++;
        node = node->next;
        return *this;
    }

    Abonent operator*() {
        return node->data;
    }

    void operator = (Abonent x)
    {
        if (size == 0)
            throw Exception();
        node->data = x;
    }

    Iterator& operator[](int index)
    {
        if (size == 0)
            throw Exception();
        Node* tmp = node;
        while (1)
        {
            if (current == index)
            {
                node = tmp;
                return *this;
            }
            tmp = tmp->next;
            if (current == size - 1)
                current = 0;
            else
                current++;
        }
    }

    friend ostream& operator<<(ostream& out, const Iterator& it)
    {
        out << it.node;
        return out;
    }
};

enum Key {
    no = 0,
    txt,
    bin
};

class File
{
private:
    fstream file;
    string name;
    Key key;
    int size;
public:
    File() : name("-"), key(no), size(0) {}
    File(string n, Key k) : name(n), key(k), size(0) {}
    friend File& operator<<(File& file, const char* s)
    {
        try {
            if (!file.file.is_open())
            {
                throw 1;
            }
        }
        catch (int i)
        {
            Exception ex(i);
            ex.Show();
        }

        string str = s;
        file.file << str;
        return file;
    }
    friend fstream& operator<<(fstream& out, Abonent& t) {

        try {
            if (!out.is_open())
            {
                throw 1;
            }
        }
        catch (int i)
        {
            Exception ex(i);
            ex.Show();
        }
        int a = out.tellg();
        string _fio = t.getFio();

        size_t len1 = _fio.length() + 1;

        int _yearNumber = t.getYearNumber();

        string _phoneNumber = t.getPhoneNumber();

        size_t len2 = _phoneNumber.length() + 1;

        out.write((char*)(&len1), sizeof(len1));

        out.write((char*)(_fio.c_str()), len1);

        out.write((char*)(&_yearNumber), sizeof(_yearNumber));

        out.write((char*)(&len2), sizeof(len2));

        out.write((char*)(_phoneNumber.c_str()), len2);


        return out;
    }
    friend File& operator<<(File& file, Abonent& t)
    {
        int pos = 0, a = 0, size = 0;

        if (!file.file.is_open())
        {
            cout << "1";
        }

        if (file.key == txt) {
            file.file << t.getFio() << " " << t.getYearNumber() << " " << t.getPhoneNumber() << endl;
        }
        else if (file.key == bin) {
            string _fio = t.getFio();

            size_t len1 = _fio.length() + 1;

            int _yearNumber = t.getYearNumber();

            string _phoneNumber = t.getPhoneNumber();

            size_t len2 = _phoneNumber.length() + 1;

            file.file.write((char*)(&len1), sizeof(len1));

            file.file.write((char*)(_fio.c_str()), len1);

            file.file.write((char*)(&_yearNumber), sizeof(_yearNumber));

            file.file.write((char*)(&len2), sizeof(len2));

            file.file.write((char*)(_phoneNumber.c_str()), len2);
        }
        return file;
    }
    friend ostream& operator<<(ostream& out, File& file)
    {
        int a = 0;

        if (!file.file.is_open())
        {
            throw 1;
        }

        if (file.key == txt) {
            string str;
            while (!file.file.eof()) {
                getline(file.file, str);
                a = file.file.tellg();
                cout << str << endl;
            }
        }
        else if (file.key == bin) {
            int i = 0;
            int size = 0, step = 0;
            while (i < file.size) {
                int _yearNumber = 0;

                size_t len = 0;

                file.file.read((char*)(&len), sizeof(len));
                char* f = new char[len];

                file.file.read((char*)(f), len);

                f[len - 1] = '\0';

                file.file.read((char*)(&_yearNumber), sizeof(int));

                file.file.read((char*)(&len), sizeof(len));
                char* s = new char[len];

                file.file.read((char*)(s), len);
                s[len - 1] = '\0';

                cout << f << " " << _yearNumber << " " << s << " " << endl;

                delete[] f;

                delete[] s;
                i++;

            }
        }
        return out;
    }

    ~File() {
        file.close();
    }
    void open(string mode)
    {
        switch (key)
        {
        case txt: {
            if (mode == "r") {
                file.open(name, ios_base::in);  
            }
            else if (mode == "add") {
                file.open(name, ios_base::app);  
            }
            else if (mode == "w") {
                file.open(name, ios_base::out); 
            }
            else if (mode == "wr") {
                file.open(name, ios_base::in | ios_base::out); 
            }
            break;
        }
        case bin: {
            if (mode == "r") {
                file.open(name, ios_base::binary | ios_base::in);   
            }
            else if (mode == "add") {
                file.open(name, ios_base::binary | ios_base::app);  
            }
            else if (mode == "w") {
                file.open(name, ios_base::binary | ios_base::out);   
            }
            else if (mode == "wr") {
                file.open(name, ios_base::binary | ios_base::in | ios_base::out);   
            }
            break;
        }

        }
        if (!file.is_open())
        {
            throw;
        }

    }
    void close()
    {
        file.close();
    }
    void begin()
    {
        file.seekg(0, ios_base::beg); 
    }
    void setSize(int s) {
        size = s;
    }
    int getSize() {
        return	size;
    }
    Abonent read()
    {
        string str;
        Abonent abnt;

        getline(this->file, str, ' ');
        abnt.setFio(str);

        getline(this->file, str, ' ');
        abnt.setYearNumber(stoi(str));

        getline(this->file, str);
        abnt.setPhoneNumber(str);

        return abnt;
    }

    void editAbonent(int line, Abonent mas[]) {
        this->open("wr");

        int a, b;

        try {
            if (!file.is_open())
            {
                throw 1;
            }
        }
        catch (int i)
        {
            Exception ex(i);
            ex.Show();
            return;
        }
        Abonent temp;
        if (this->key == txt) {
            string str;
            for (int i = 0; i < line - 1; i++)
                getline(this->file, str);

            getline(this->file, str, ' ');
            temp.setFio(str);

            getline(this->file, str, ' ');
            temp.setYearNumber(stoi(str));

            getline(this->file, str, ' ');
            temp.setPhoneNumber(str);
        }
        else if (this->key == bin) {
            size_t len = 0;
            for (int i = 0; i < line; i++) {
                this->file.read((char*)(&len), sizeof(len));
                char* n = new char[len];
                string _fio;
                string _phoneNumber;
                int _yearNumber = 0;

                this->file.read((char*)(n), len);
                _fio = n;
                _fio[len - 1] = '\0';
                this->file.read((char*)(&_yearNumber), sizeof(int));

                this->file.read((char*)(&len), sizeof(len));
                this->file.read((char*)(n), len);
                _phoneNumber = n;
                _phoneNumber[len - 1] = '\0';

                temp.setFio(_fio);
                temp.setYearNumber(_yearNumber);
                temp.setPhoneNumber(_phoneNumber);
                delete[]n;
            }
            a = file.tellg();
        }

        cout << "Petrovski_Petr_Petrovich 1990 11111 - edited" << endl;

        string fi = "Petrovski_Petr_Petrovich";

        rewind(stdin);
        temp.setFio(fi);

        int yn = 1990;
        temp.setYearNumber(yn);
        rewind(stdin);

        string pn = "  37511111";
        rewind(stdin);

        temp.setPhoneNumber(pn);

        a = file.tellg();
        file.seekg(0, ios_base::end);
        b = file.tellg();
        this->file.seekg(0, ios_base::beg);
        if (this->key == txt) {
            string tmp;
            for (int i = 0; i < line - 1; i++)
                getline(this->file, tmp);
            int pos = this->file.tellg();
            this->file.seekg(pos, ios_base::beg);
            this->file << tmp;
            return;
        }
        else if (this->key == bin) {
            for (int i = 0; i < line - 1; i++)
            {
                size_t len = 0;
                this->file.read((char*)(&len), sizeof(len));
                char* n = new char[len];

                string _fio;
                string _phoneNumber;
                int _yearNumber = 0;

                this->file.read((char*)(n), len);

                _fio = n;

                _fio[len - 1] = '\0';

                this->file.read((char*)(&_yearNumber), sizeof(int));

                this->file.read((char*)(&len), sizeof(len));

                this->file.read((char*)(n), len);
                delete[] n;
            }
            int pos = this->file.tellg();

            char* last = new char[b - a];
            this->file.read((char*)last, static_cast<std::streamsize>(b) - a);

            this->close();
            this->open("w");
            pos = this->file.tellg();

            for (int i = 0; i < getSize(); i++) {
                if (i != line - 1)
                    file << mas[i];
                else {
                    file << temp;
                    mas[i].setFio(temp.getFio());
                    mas[i].setYearNumber(temp.getYearNumber());
                    mas[i].setPhoneNumber(temp.getPhoneNumber());
                }
            }
        }

        this->close();
    }

    void deleteAbonent(int line, Abonent mas[]) {
        this->open("wr");

        int a, b;

        try {
            if (!file.is_open())
            {
                throw 1;
            }
        }
        catch (int i)
        {
            Exception ex(i);
            ex.Show();
            return;
        }
        Abonent temp;
        if (this->key == txt) {
            string str;
            for (int i = 0; i < line - 1; i++)
                getline(this->file, str);

            getline(this->file, str, ' ');
            temp.setFio(str);

            getline(this->file, str, ' ');
            temp.setYearNumber(stoi(str));

            getline(this->file, str, ' ');
            temp.setPhoneNumber(str);
        }
        else if (this->key == bin) {
            size_t len = 0;
            for (int i = 0; i < line; i++) {
                this->file.read((char*)(&len), sizeof(len));
                char* n = new char[len];
                string _fio;
                string _phoneNumber;
                int _yearNumber = 0;

                this->file.read((char*)(n), len);
                _fio = n;
                _fio[len - 1] = '\0';
                this->file.read((char*)(&_yearNumber), sizeof(int));

                this->file.read((char*)(&len), sizeof(len));
                this->file.read((char*)(n), len);
                _phoneNumber = n;
                _phoneNumber[len - 1] = '\0';

                temp.setFio(_fio);
                temp.setYearNumber(_yearNumber);
                temp.setPhoneNumber(_phoneNumber);
                delete[]n;
            }
            a = file.tellg();
        }

        string fi = "";
        rewind(stdin);
        temp.setFio(fi);

        int yn = NULL;
        temp.setYearNumber(yn);
        rewind(stdin);

        string pn = "";
        rewind(stdin);

        temp.setPhoneNumber(pn);

        a = file.tellg();
        file.seekg(0, ios_base::end);
        b = file.tellg();
        this->file.seekg(0, ios_base::beg);
        if (this->key == txt) {
            string tmp;
            for (int i = 0; i < line - 1; i++)
                getline(this->file, tmp);
            int pos = this->file.tellg();
            this->file.seekg(pos, ios_base::beg);
            this->file << tmp;
            return;
        }
        else if (this->key == bin) {
            for (int i = 0; i < line - 1; i++)
            {
                size_t len = 0;

                this->file.read((char*)(&len), sizeof(len));

                char* n = new char[len];

                string _fio;
                string _phoneNumber;
                int _yearNumber = 0;

                this->file.read((char*)(n), len);
                _fio = n;

                _fio[len - 1] = '\0';

                this->file.read((char*)(&_yearNumber), sizeof(int));

                this->file.read((char*)(&len), sizeof(len));

                this->file.read((char*)(n), len);

                delete[] n;
            }
            int position = this->file.tellg();

            char* last = new char[b - a];
            this->file.read((char*)last, static_cast<std::streamsize>(b) - a);

            this->close();
            this->open("w");
            position = this->file.tellg();

            for (int i = 0; i < getSize(); i++) {
                if (i != line - 1)
                    file << mas[i];
            }
        }

        this->close();
    }

    void boubleFio()
    {
        string temp;
        string str1, str2;
        int position;
        this->open("wr");
        try {
            if (!file.is_open())
            {
                throw 1;
            }
        }
        catch (int i)
        {
            Exception ex(i);
            ex.Show();
            return;
        }
        for (int i = 0; i < this->size - 1; i++) {
            for (int j = 0; j < this->size - i - 1; j++)
            {
                this->file.seekg(0, ios_base::beg);
                for (int k = 0; k < j; k++)
                    getline(this->file, str1);

                getline(this->file, str1, ' ');

                getline(this->file, str2);

                getline(this->file, str2, ' ');

                if (str1 > str2)
                {
                    this->file.seekg(0, ios_base::beg);
                    position = this->file.tellg();
                    for (int k = 0; k <= j; k++)
                    {
                        getline(this->file, str1);
                        position = this->file.tellg();
                    }
                    temp = str1;
                    getline(this->file, str2);
                    this->file.seekg(0, ios_base::beg);
                    for (int k = 0; k < j; k++)
                    {
                        getline(this->file, str1);
                    }
                    position = this->file.tellg();
                    this->file.seekg(position, ios_base::beg);
                    this->file << str2 << '\n';
                    this->file << temp << '\n';
                }
            }
        }
        this->close();
    }

    void boubleYearNumber() {
        string temp;
        string str1, str2;
        int i1, i2;
        int position;
        this->open("wr");
        try {
            if (!file.is_open())
            {
                throw 1;
            }
        }
        catch (int i)
        {
            Exception ex(i);
            ex.Show();
            return;
        }
        for (int i = 0; i < this->size - 1; i++) {
            for (int j = 0; j < this->size - i - 1; j++)
            {
                this->file.seekg(0, ios_base::beg);
                for (int k = 0; k < j; k++)
                    getline(this->file, str1);
                for (int t = 0; t < 2; t++)
                    getline(this->file, str1, ' ');
                getline(this->file, str2);
                for (int t = 0; t < 2; t++)
                    getline(this->file, str2, ' ');
                i1 = stoi(str1);
                i2 = stoi(str2);
                if (i1 > i2)
                {
                    this->file.seekg(0, ios_base::beg);
                    position = this->file.tellg();
                    for (int k = 0; k <= j; k++) {
                        getline(this->file, str1);
                        position = this->file.tellg();
                    }
                    temp = str1;
                    getline(this->file, str2);
                    this->file.seekg(0, ios_base::beg);
                    for (int k = 0; k < j; k++) {
                        getline(this->file, str1);
                    }
                    position = this->file.tellg();
                    this->file.seekg(position, ios_base::beg);
                    this->file << str2 << '\n';
                    this->file << temp << '\n';
                }
            }
        }
        this->close();
    }
    void BoublePhoneNumber()
    {
        string temp;
        string str1, str2;
        int i1, i2;
        int position;
        this->open("wr");
        try {
            if (!file.is_open())
            {
                throw 1;
            }
        }
        catch (int i)
        {
            Exception ex(i);
            ex.Show();
            return;
        }
        for (int i = 0; i < this->size - 1; i++) {
            for (int j = 0; j < this->size - i - 1; j++) {
                this->file.seekg(0, ios_base::beg);
                for (int k = 0; k < j; k++) {
                    getline(this->file, str1);
                }
                for (int t = 0; t < 4; t++)
                    getline(this->file, str1, ' ');
                getline(this->file, str1);
                for (int t = 0; t < 4; t++)
                    getline(this->file, str2, ' ');
                getline(this->file, str2);
                i1 = stoi(str1);
                i2 = stoi(str2);
                if (i1 > i2) {
                    this->file.seekg(0, ios_base::beg);
                    position = this->file.tellg();
                    for (int k = 0; k <= j; k++) {
                        getline(this->file, str1);
                        position = this->file.tellg();
                    }
                    temp = str1;
                    getline(this->file, str2);
                    this->file.seekg(0, ios_base::beg);
                    for (int k = 0; k < j; k++) {
                        getline(this->file, str1);
                    }
                    position = this->file.tellg();
                    this->file.seekg(position, ios_base::beg);
                    this->file << str2 << '\n';
                    this->file << temp << '\n';
                }
            }
        }
        this->close();
    }
};

using namespace std;

int main()
{
    Abonent abonent2("Demidov_Dima_Denisovich", 2008, "  37555555");
    Abonent abonent1("Cetov_Ceriy_Cergeevich", 2010, "  37566666");
    Abonent abonent3("Antipov_Anton_Antonovich", 2006, "  37522222");
    Abonent abonent4("Betalis_Beta_Borisovich", 2003, "  37533333");
    Abonent abonent5("Ekatova_Ekaterina_Elkovna", 2010, "  37577777");

    Abonent msAb[5] = {

        abonent1,
        abonent2,
        abonent3,
        abonent4,
        abonent5
    };

    cout << "\n .txt - .txt\n";

    File file1("1.txt", txt);

    file1.open("w");

    file1.setSize(5);

    file1 << abonent1;
    file1 << abonent2;
    file1 << abonent3;
    file1 << abonent4;
    file1 << abonent5;

    file1.close();

    file1.open("r");

    cout << file1;

    file1.close();

    cout << "\n.txt - .bin\n";

    File file2("2.txt", bin);

    file2.open("w");

    file2.setSize(5);
    file2 << abonent1;
    file2 << abonent2;
    file2 << abonent3;
    file2 << abonent4;
    file2 << abonent5;

    file2.close();

    file2.open("r");

    cout << file2;

    file2.close();

    cout << "\n.bin - .bin\n";

    File file3("3.bin", bin);

    file3.open("w");

    file3.setSize(5);

    file3 << abonent1;
    file3 << abonent2;
    file3 << abonent3;
    file3 << abonent4;
    file3 << abonent5;

    file3.close();

    file3.open("r");

    cout << file3;

    file3.close();

    cout << "\n";

    int i = 0, lineToEdit = 3;

    cout << "abonent to change - 3" << endl;
    lineToEdit = 3;
    cout << "abonent to delete - 5" << endl;
    int lineToDel = 5;

    file2.open("r");
    file2.editAbonent(lineToEdit, msAb);
    file2.close();

    file2.open("w");
    cout << "Ekatova_Ekaterina_Elkovna 2010 77777 - deleted" << endl;
    file2.deleteAbonent(lineToDel, msAb);
    file2.close();

    rewind(stdin);

    cout << "\n";

    cout << "Sort FIO:" << endl;
    file1.boubleFio();

    file1.open("r");

    cout << file1;

    file1.close();

    cout << "\n";
    cout << "Sort yearNumber:" << endl;
    file1.boubleYearNumber();

    file1.open("r");

    cout << file1;

    file1.close();

    cout << "\n";
    cout << "Sort phoneNumber:" << endl;
    file1.BoublePhoneNumber();

    file1.open("r");

    cout << file1;

    file1.close();

    file1.boubleFio();

    Deque deqq;

    file1.open("r");

    Abonent temp;

    for (int i = 0; i < file1.getSize(); i++)
    {
        temp = file1.read();
        deqq.push_front(temp);
    }

    Iterator I(&deqq);

    cout << "Alph Abonents by Year - 2007";

    deqq.outputTaskByYear(2007);
}
