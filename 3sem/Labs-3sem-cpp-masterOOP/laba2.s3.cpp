#include <iostream>
#include <string>
#include <iomanip>
#include <locale.h>

using namespace std;

class Serial {

private:
	int id;
	string name;
	string review;
public:
	friend class Viewer;

	Serial() {   //ум
		id = 0;
		name = "";
		review = "";
	}

	//инит
	Serial(	int def_id, string def_name, string def_review = "Отзыв") :id(def_id), name(def_name), review(def_review) {};

	Serial(const Serial& S) :  //коп
		id(S.id),
		name(S.name),
		review(S.review) {};

	~Serial() {};

	friend void Show_Serial(Serial& serial);
};


class Viewer {

public:
	Viewer() {};
	~Viewer() {};

	void My_Review(Serial& serial) {
		std::cout << "Ваш отзыв о  сериале:\n ";
		std::cin >> serial.review;
	}
};

void Show_Serial(Serial& serial)
{
	cout << "Id:" << serial.id << endl;
	cout << "Название:" << serial.name << endl;
	cout << "Отзыв:" << serial.review << endl;
}

int getValue()
{
	while (true)
	{
		int a;
		std::cin >> a;
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(32767, '\n');
			std::cout << "Ошибка!\n";
		}
		else
		{
			std::cin.ignore(32767, '\n');
			return a;
		}
	}
}
int main()
{
	setlocale(LC_ALL, "Russian");
	Serial sirik_ms[5];
	Viewer sirik_rev;
	Show_Serial(sirik_ms[0]);
	cout << "================" << endl;

	Serial def(322, "Hi", "Top");
	Show_Serial(def);

	cout << "================" << endl;
	int t_id, i, n, sw = 1;
	string name;
	for (i = 1; i < 5; i++)
	{

		cout << "id сериала №" << i << endl;
		t_id = getValue();
		cout << "Название сериала №" << i << endl;
		cin >> name;
		sirik_ms[i] = { t_id,name };
		cout << endl;
	}
	cout << "================" << endl;
	for (i = 1; i < 5; i++) {
		Show_Serial(sirik_ms[i]);
	}
	cout << "================" << endl;
	for (i = 1; i < 5; i++) {
		cout << "Добавьте свой отзыв на сериал №" << i << endl;
		sirik_rev.My_Review(sirik_ms[i]);
	}
	system("CLS");
	cout << "Выши отзывы:" << endl;

	for (i = 1; i < 5; i++)
	{
		cout << "Сериал №" << i << endl;
		Show_Serial(sirik_ms[i]);
	}

	system("pause");
}
