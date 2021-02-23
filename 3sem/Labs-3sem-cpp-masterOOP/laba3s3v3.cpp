#include <iostream>
#include <locale.h>
using namespace std;

class Mtrx {
private:

	int N; //str
	int M; // stlb
	int** array;

public:
	//ym
	Mtrx() {
		this->N = 0;
		this->M = 0;
		array = new int*[N];
		for (int i = 0; i < N; i++)
			array[i] = new int[M];

		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++)
				array[i][j] = 0;
		/*cout << "Конструктор поумолчанию" << endl;*/
	}
	//init
	Mtrx(int** arr, int n, int m) {

		N = n; M = m;
		this->array = new int*[this->N];
		for (int i = 0; i < N; i++)
			this->array[i] = new int[this->M];

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++)
				this->array[i][j] = arr[i][j];
		}
		cout << "Конструктор инициализации" << endl;
	}
	//copy
	Mtrx(const Mtrx& any) {
		this->N = any.N;
		this->M = any.M;

		this->array = new int*[any.N];
		for (int i = 0; i < N; i++)
			this->array[i] = new int[any.M];

		for (int i = 0; i < any.N; i++) {
			for (int j = 0; j < any.M; j++) {
				this->array[i][j] = any.array[i][j];
			}
		}
		cout << "Конструктор копипаста" << endl;
	}

	friend void show_mrx(Mtrx& prin);

	friend bool operator> (const Mtrx& b1, const  Mtrx& b2);

	friend bool operator< (const  Mtrx& d1, const  Mtrx& d2);

	friend bool operator== (const  Mtrx& c1, const  Mtrx& c2);

	friend const Mtrx& operator--(Mtrx& d1);
	//destr
	/*~Mtrx() {
		delete[] this->array;
		cout << "Вызвался деструктор" << endl;
	}*/
};


void show_mrx(Mtrx& prin) {

	cout << "-------------" << endl;

	cout << "Матрица(Функция show_mrx):" << endl;
	for (int i = 0; i < prin.N; i++) {
		for (int j = 0; j < prin.M; j++)
			cout << prin.array[i][j] << " ";
		cout << endl;
	}
	cout << "-------------" << endl;
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
			std::cout << "ERROR! INT ONLY!\n";
		}
		else
		{
			std::cin.ignore(32767, '\n');
			return a;
		}
	}
}

bool operator> (const  Mtrx& b1, const  Mtrx& b2)
{
	/*return b1.array > b2.array;*/
	return ((b1.M + b1.N) > (b2.M + b2.N));
}

bool operator< (const  Mtrx& d1, const  Mtrx& d2)
{
	/*return d1.array < d2.array;*/
	return ((d1.M + d1.N) < (d2.M + d2.N));
}

bool operator== (const  Mtrx& c1, const  Mtrx& c2)
{
	return (c1.N == c2.N && c1.M == c2.M );
}
const Mtrx& operator--(Mtrx& d1) {
	d1.N -= 1;
	d1.M -= 1;
	return d1;
}

int main()
{
	int** arr;
	int n; int m;
	setlocale(LC_ALL, "Russian");
	cout << "Матрица: ";
	cout << endl << "N(строки) = ";
	n = getValue();
	cout << "M(столбцы) = ";
	m = getValue();
	arr = new int*[n];
	for (int i = 0; i < n; i++)
		arr[i] = new int[m];

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cout << "Матрица [" << i << "] [" << j << "] : ";
			cin >> arr[i][j];
		}
	}
	Mtrx def(arr, n, m);
	Mtrx Copy = def;
	show_mrx(def);
	cout << "Скопировали :" << endl;
	show_mrx(Copy);
	const int size = 4;
	Mtrx matr[size];
	int k;

	for (k = 0; k < 4; k++) {
		cout << "Матрица: ";
		cout << endl << "N(строки) = ";
		n = getValue();
		cout << "M(столбцы) = ";
		m = getValue();
		arr = new int*[n];
		for (int i = 0; i < n; i++)
			arr[i] = new int[m];

		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				cout << "Матрица [" << i << "] [" << j << "] : ";
				cin >> arr[i][j];
			}
		}
		matr[k] = { arr, n, m };
		show_mrx(matr[k]);
	}
	int sw = 0, red_bt = 0, c, b;

	while (red_bt != 5) {
		cout << "Выберите номера 2-ух матриц [0-3]" << endl;
		c = b = 0;
		c = getValue();
		b = getValue();

		cout << "Выбор sw_1 (>), sw_2(<), sw_3(==), sw_4(--), sw_5 (exit)" << endl;
		sw = getValue();
		switch (sw)
		{
		case 1:
		{
			if (matr[c] > matr[b])
				cout << "Первая матрица больше второй\n";
			else
				cout << "Первая матрица меньше второй или равна ей\n";
			break;
		}
		case 2:
		{
			if (matr[c] < matr[b])
				cout << "Первая матрица меньше второй\n";
			else
				cout << "Первая матрица больше второй или равна ей\n";
			break;
		}
		case 3:
		{
			if (matr[c] == matr[b])
				cout << "Первая матрица равна второй\n";
			else
				cout << "Первая матрица не равна второй\n";
			break;
		}
		case 4:
		{
			--matr[c];
			show_mrx(matr[c]);
			break;
		}
		case 5:
		{
			red_bt = 5;
			break;
		}
		default:
			cout << "Введен неверный case";
			break;
		}

	}

	cout << "Game over";
	return 0;
}
