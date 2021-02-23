#include <iostream>
#include <string>
#include <locale.h>
#include <algorithm> 

using namespace std;

class Serial
{

public:

	Serial()
	{
		_serialId = 0;
		_serialName = "";
		next = NULL;
		prev = NULL;
	}
	//Serial(const string& a) :_serialName(a) {}

	int getId() {
		return this->_serialId;
	}

	string getName() {
		return this->_serialName;
	}


	Serial(int id, string name) : _serialId(id), _serialName(name) {}

	Serial(const Serial& sirik) :
		_serialId(sirik._serialId), _serialName(sirik._serialName) {}

	void print()
	{
		cout << _serialId << " " << _serialName;
	}

	friend ostream& operator<< (ostream& out, const Serial& point);

	Serial* next;
	Serial* prev;
	int _serialId;
	string _serialName;
};


class ExceptionDeq
{

public:
	ExceptionDeq(string error)
	{
		_exception = error;
	}
	const char* getError()
	{

		return _exception.c_str();
	}

private:
	string _exception;
};

ostream& operator<< (ostream& out, const Serial& point)
{
	out << point._serialId << ' ' << point._serialName << endl;
	return out;
}


void qsortRecursive(int* mas, int size) {
	//Указатели в начало и в конец массива
	int i = 0;
	int j = size - 1;

	//Центральный элемент массива
	int mid = mas[size / 2];

	//Делим массив
	do {
		//Пробегаем элементы, ищем те, которые нужно перекинуть в другую часть
		//В левой части массива пропускаем(оставляем на месте) элементы, которые меньше центрального
		while (mas[i] < mid) {
			i++;
		}
		//В правой части пропускаем элементы, которые больше центрального
		while (mas[j] > mid) {
			j--;
		}

		//Меняем элементы местами
		if (i <= j) {
			int tmp = mas[i];
			mas[i] = mas[j];
			mas[j] = tmp;

			i++;
			j--;
		}
	} while (i <= j);


	//Рекурсивные вызовы, если осталось, что сортировать
	if (j > 0) {
		//"Левый кусок"
		qsortRecursive(mas, j + 1);
	}
	if (i < size) {
		//"Првый кусок"
		qsortRecursive(&mas[i], size - i);
	}
}


template<typename T>
class Deque {
public:
	T* head;
	T* tail;
	int size;

	friend class Serial;

	Deque() {
		head = NULL;
		tail = NULL;
		size = 0;
	}

	class iterator
	{
	public:
		friend class TDeque;
		friend class Serial;
		iterator(Serial* el) :_current(el) {}

		iterator() :_current(0) {}
		iterator(const iterator& it) :_current(it._current) {}

		bool operator==(const iterator& it) const
		{
			return (_current == it._current);
		}
		bool operator!=(const iterator& it) const
		{
			return !(it == *this);
		}

		iterator& operator++()
		{
			if ((_current != 0) && (_current->next != 0))
				_current = _current->next;
			return *this;
		}
		iterator& operator--()
		{
			if ((_current != 0) && (_current->prev != 0))
				_current = _current->prev;
			return *this;
		}
		Serial& operator*() const
		{
			if (_current != 0)
				return _current->_serialName;
			else {
				cout << "Null iterator!" << endl;
				exit(1);
			}
		}
		Serial* _current;     
	};

	void pushBack(Serial &ser) {
		size++;
		Serial* t = new Serial(ser);
		if (head != NULL) {
			tail->prev = t;
			t->next = tail;
			tail = t;
		}
		else {
			tail = t;
			head = tail;
		}
		cout << "pushBack()" << endl;
	}

	void pushFront(Serial &ser) {
		size++;
		Serial* t = new Serial(ser);
		if (head != NULL) {
			head->next = t;
			t->prev = head;
			head = t;
		}
		else {
			head = t;
			tail = head;
		}
		cout << "pushFront()" << endl;
	}

	void popFront() {
		if (head != NULL) {
			size--;
			head = this->head->next;
		}
		else {
			cout << "Error" << endl;
		}
		cout << "popFront()" << endl;
	}

	void popBack() {
		if (tail != NULL) {
			size--;
			tail = this->tail->prev;
		}
		else {
			cout << "Error" << endl;
		}
		cout << "popBack()" << endl;
	}


	int sizeDec() {
		return size;
	}

	void printDec() {
		Serial* t = head;
		while (t != NULL) {
			cout << t->_serialId << t->_serialName << endl;
			t = t->prev;
		}
	}

		Deque front(T data) {
			return data->data;
		}

		Deque back(T data) {
			return data->data;
		}


		void quickSortR(int* a, long N) {

			long i = 0, j = N - 1; 		
			int temp, p;
			p = a[N >> 1];		

			do {
				while (a[i] < p) i++;
				while (a[j] > p) j--;

				if (i <= j) {
					temp = a[i]; a[i] = a[j]; a[j] = temp;
					i++; j--;
				}
			} while (i <= j);
			if (j > 0) quickSortR(a, j);
			if (N > i) quickSortR(a + i, N - i);
		}

		void clear() {
			while (!empty())
				popBack();
			cout << "ok" << endl;
		}

		bool empty() {
			return size == 0;
		}
	};

	int getValue_int()
	{
		while (true)
		{
			int a;
			std::cin >> a;
			if (std::cin.fail())
			{
				std::cin.clear();
				std::cin.ignore(32767, '\n');
				std::cout << "ERROR! INT!\n";
			}
			else
			{
				std::cin.ignore(32767, '\n');
				return a;
			}
		}
	}

	void mainMenu()
	{
		cout << " -----------------------------------------------------------------------------------" << endl;
		cout << " Выбор 1-push_front 2-pop_front 3-sort 4-print 5-pushBack 6-popBack 0-exit" << endl;
		cout << " -----------------------------------------------------------------------------------" << endl;
	}

	int main()
	{
		using namespace std;
		setlocale(LC_ALL, "Russian");
		int index;
		Deque<Serial>deque;
		Deque<Serial>::iterator it;

		while (1)
		{
			mainMenu();
			try
			{
				index = getValue_int();
				if (index < 0 || index>6)
					throw ExceptionDeq("Недопустимое число\n");
			}
			catch (ExceptionDeq exception)
			{
				cerr << exception.getError();
			}

			if (index == 1)
			{
				cout << "Введите id и name:\n";
				int id;
				string name;
				try
				{
					id = getValue_int();
					cin >> name;
					if (id < 0)
					{
						throw ExceptionDeq("Недопустимый id\n");
					}
				}
				catch (ExceptionDeq exception)
				{
					cerr << exception.getError();
				}
				Serial ser = Serial(id, name);
				deque.pushFront(ser);
				

			}
			if (index == 2)
			{
				try
				{
					deque.popFront();
					
				}
				catch (ExceptionDeq exception)
				{
					cerr << exception.getError();
				}

			}

			if (index == 3)
			{
				//std::iterator it1 = it;
				int* ms = new int[deque.sizeDec()*8];
				for (int i = 0; i < deque.sizeDec(); i++)
				{
					ms[i] = it._current->_serialId;
					--it;
				}


				deque.quickSortR(ms, deque.sizeDec());

				for (int i = 0; i < deque.sizeDec(); i++)
				{
					it._current->_serialId = ms[i];
					--it;
				}

			}

			if (index == 4)
			{

				try
				{
					deque.printDec();
				}
				catch (ExceptionDeq exception)
				{
					cerr << exception.getError();
				}
			}
			if (index == 0)
			{
				break;
			}

			if (index == 5)
			{
				cout << "Введите id и name:\n";
				int id;
				string name;
				try
				{
					id = getValue_int();
					cin >> name;
					if (id < 0)
					{
						throw ExceptionDeq("Недопустимый id\n");
					}
				}
				catch (ExceptionDeq exception)
				{
					cerr << exception.getError();
				}
				Serial ser = Serial(id, name);
				deque.pushBack(ser);

			}
			if (index == 6)
			{
				try
				{
					deque.popBack();
				}
				catch (ExceptionDeq exception)
				{
					cerr << exception.getError();
				}

			}
		}
	}

