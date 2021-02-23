#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <deque>


namespace Space {
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
    ~Exception() { };
    void Print()
    {
        if (this->error == 1)
        {
            std::cout << "File isn't open!" << std::endl;
        }
        if (this->error == 2)
        {
            std::cout << "Is empty!" << std::endl;
        }
    }
};
}

namespace Space {
class Serial {
private:
    int id;
    std::string name;
    double rate;
public:
    Serial() :id(0), name("Serial tst"), rate(0.0)  {}

    Serial(int _id, std::string _name, double _rate) {
        if (_rate < 0 || _rate > 100)
            rate = 0.0;
        else rate = _rate;
        name = _name;
        id = _id;
    }
    ~Serial() { }
    bool operator< (const Serial& ser) {
        return rate < ser.rate;
    }
    bool operator== (const Serial& ser) {
        return rate == ser.rate && name == ser.name;
    }
    friend std::ostream& operator<< (std::ostream& out, const Serial& Sirik)
    {
        out << Sirik.id << ' ' << Sirik.name << ' ' << Sirik.rate << '\n'; return out;
    }

    std::string getName() {
        return name;
    }
    double getRate() {
        return rate;
    }
    int getId(){
        return id;
    }

    };
}

namespace Space {
class Algorithm {
public:
    void alg1() {
        std::string line;
        std::ifstream file("file.txt");
        std::ofstream ofile("outCountSymbol.txt");

        if (!file || !ofile)
        {
            std::cerr << "file.txt could not be opened for reading" << std::endl;
            exit(1);
        }
        file.seekg(0, std::ios_base::beg);
        std::stack<char> stack;
        while (file) {
            std::getline(file, line);
            if (!file) return;
            char symb = 'a'; // проверка на символ
            int symbb =0; // кол-во символова
            // std::cin >> symb ;
            for (unsigned long long i = 0; i < line.length(); i++) {
                if(line[i] == symb)
                    symbb++;
            }
            if(symbb == 0)
                std::cout << "no symbol" , ofile << "no symbol";
            else
                std::cout << symbb , ofile << symbb;
            file.close();
            ofile.close();
            std::cout << "\n\n";
        }
    }
    //////////////////////////////////
    void alg2(std::deque<Space::Serial>::iterator begin, std::deque<Space::Serial>::iterator end, Space::Serial obj)
    {
        while (begin != end)
        {
            begin++;
            if (begin == end) break;
            if (*begin == obj)
            {
                std::cout << "True - serial is found" << std::endl;
                return;
            }
        }
        std::cout << "False - serial isn't found" << std::endl;
    }

///////////////////////////////////////
    void alg3(std::deque<Space::Serial>& deque,std::deque<Space::Serial>::iterator begin, std::deque<Space::Serial>::iterator end, Space::Serial T)
    {
        std::deque<Space::Serial>::iterator It;
        It = begin;
        int i = 0;

        while (begin != end)
        {
            begin++;
            i++;
            if (begin == end) break;
            if (*begin == T)
            {
                if(i>0)
                deque.erase(It + i);
                return;
            }
        }

    }
};
}

int main()
{
    Space::Algorithm A;
    A.alg1();

    Space::Serial m1(32,"Boys", 89.4);
    Space::Serial m2(11,"Black Sales", 70.5);
    Space::Serial m3(7,"Hot boat", 4.2);
    Space::Serial m4(88,"Maniac", 50.2);
    Space::Serial m5(54,"Breaking bad", 94.7);
    Space::Serial m6(2,"Test", 00.0);


    std::deque<Space::Serial> deque;

    std::deque<Space::Serial>::iterator It;

    deque.push_back(m1);
    deque.push_back(m2);
    deque.push_back(m5);
    deque.push_back(m3);
    deque.push_back(m4);

    It = deque.begin();
    while (It != deque.end())
    {
        std::cout << *It;
        It++;
    }
    std::cout << std::endl;
     std::cout << "Search serial " << m4 << std::endl;

    A.alg2(It = deque.begin(), It = deque.end(), m4);

    std::cout << std::endl;
    std::cout << "delete serial " << m3 << std::endl;

    A.alg3(deque,It = deque.begin(), It = deque.end(),m3);

    std::cout << std::endl;
    It = deque.begin();
    while (It != deque.end()) {
        std::cout << *It;
        ++It;
    }

    return 0;
}
