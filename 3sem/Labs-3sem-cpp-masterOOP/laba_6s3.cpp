//1.	 Создать проект консольной программы.
//2.	Создать шаблонную функцию или класс согласно заданию.
//3.	Реализовать явную специализацию шаблонной функции или класса для строк символов(char*).
//4.	В шаблонных классах использовать стандартные типы и аргументы по умолчанию.Данные в шаблонных классах хранить в статическом массиве.Набор методов класса должен обеспечить основные операции с элементами объекта(добавление, удаление, поиск, просмотр, упорядочивание и т.п.) и операции над однотипными объектами(сравнение, сложение и т.п.).
//5.	Дополнительно рассмотреть реализацию наследования шаблонных классов(например, от простого базового класса).
//6.	В функции main выполнить действия с шаблонной функцией или классом, которые продемонстрируют работу методов.
//7.	Отладить и выполнить полученную программу.Проверить использование специализаций, стандартных типов и аргументов по умолчанию.

#include <iostream>
#include <locale.h>


using namespace std;
template <class T>
class Mtrx {
private:

	int N; //str
	int M; // stlb
	T** array;

public:
	//ym
	Mtrx() {
		this->N = 0;
		this->M = 0;
		array = new T* [N];
		for (int i = 0; i < N; i++)
			array[i] = new T[M];

		for (int i = 0; i < N; i++)
			for (int j = 0; j < M; j++)
				array[i][j] = 0;
		/*cout << "Конструктор поумолчанию" << endl;*/
	}
	//init
	Mtrx(T** arr, int n, int m) {

		N = n; M = m;
		this->array = new T* [this->N];
		for (int i = 0; i < N; i++)
			this->array[i] = new T[this->M];

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < M; j++)
				this->array[i][j] = arr[i][j];
		}
		cout << "Конструктор инициализации" << endl;
	}
	//copy
	Mtrx(const Mtrx<T>& any) {
		this->N = any.N;
		this->M = any.M;

		this->array = new T* [any.N];
		for (int i = 0; i < N; i++)
			this->array[i] = new T[any.M];

		for (int i = 0; i < any.N; i++) {
			for (int j = 0; j < any.M; j++) {
				this->array[i][j] = any.array[i][j];
			}
		}
		cout << "Конструктор copy" << endl;
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
				std::cout << "ERROR!\n";
			}
			else
			{
				std::cin.ignore(32767, '\n');
				return a;
			}
		}
	}
	

	template <class T> friend void show_mrx(Mtrx<T>& prin);

	template <class T> friend bool operator> (const Mtrx<T>& b1, const  Mtrx<T>& b2);

	template <class T> friend bool operator< (const  Mtrx<T>& d1, const  Mtrx<T>& d2);

	template <class T> friend bool operator== (const  Mtrx<T>& c1, const  Mtrx<T>& c2);

	void create() {
		T** arr;
		int n; int m;
		const int size = 3;
		Mtrx<T> matr[size];
		int k;

		 for (k = 0; k < 3; k++) {
			cout << "Множество: ";
			cout << endl << "N(строки) = ";
			n= getValue();
			cout << "M(столбцы) = ";
			m= getValue();
			arr = new T* [n];
			for (T i = 0; i < n; i++)
				arr[i] = new T[m];

			for (int i = 0; i < n; i++) {
				for (int j = 0; j < m; j++) {
					cout << "Множество [" << i << "] [" << j << "] : ";
					cin >> arr[i][j];
					
				}
			}
			matr[k] = { arr, n, m };
			show_mrx(matr[k]);
		 }
		 int sw = 0, red_bt = 0, c, b, sh = 0;

		 while (red_bt != 5) {
			 cout << "Выберите номера 2-ух множест [0-2]" << endl;
			 c = b = -1;
			 c = matr->getValue();
			 if (c < 0 || c>2) {
				 cout << "Error";
				 c = matr->getValue();
			 }
			 b = matr->getValue();

			 if (b < 0 || b>2) {
				 cout << "Error";
				 b = matr->getValue();
			 }
			 cout << "Выбор sw_1 (>), sw_2(<), sw_3(==), sw_4(show), sw_5 (exit)" << endl;
			 sw = matr->getValue();
			 switch (sw)
			 {
			 case 1:
			 {
				 if (matr[c] > matr[b])
					 cout << "Первое множество больше второго\n";
				 else
					 cout << "Первое множество меньше второго или равно ему\n";
				 break;
			 }
			 case 2:
			 {
				 if (matr[c] < matr[b])
					 cout << "Первое множество меньше второго\n";
				 else
					 cout << "Первая матрица больше второй или равна ей\n";
				 break;
			 }
			 case 3:
			 {
				 if (matr[c] == matr[b])
					 cout << "Первое множество равно второму\n";
				 else
					 cout << "Первое множество не равно второму\n";
				 break;
			 }
			 case 4:
			 {
				 while (sh != 3)
				 {
					 show_mrx(matr[sh]);
					 sh++;
				 }
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
	}
	//destr
	/*~Mtrx() {
		delete[] this->array;
		cout << "Вызвался деструктор" << endl;
	}*/
};

template <class T> void show_mrx(Mtrx<T>& prin) {

	cout << "-------------" << endl;

	cout << "Множество(Функция show_mrx):" << endl;
	for (int i = 0; i < prin.N; i++) {
		for (int j = 0; j < prin.M; j++)
			cout << prin.array[i][j] << " ";
		cout << endl;
	}
	cout << "-------------" << endl;
}



template <class T> bool operator> (const  Mtrx<T>& b1, const  Mtrx<T>& b2)
{
	/*return b1.array > b2.array;*/
	return ((b1.M + b1.N) > (b2.M + b2.N));
}

template <class T> bool operator< (const  Mtrx<T>& d1, const  Mtrx<T>& d2)
{
	/*return d1.array < d2.array;*/
	return ((d1.M + d1.N) < (d2.M + d2.N));
}

template <class T> bool operator== (const  Mtrx<T>& c1, const  Mtrx<T>& c2)
{
	return (c1.N == c2.N && c1.M == c2.M);
}


int main()
{

	setlocale(LC_ALL, "Russian");
	const int size = 3;
	Mtrx<int> matr[size];
	matr->create();
	
	return 0;
}
