/*1. Рассмотреть теоретические сведения по теме лабораторной работы.
2. Создать проект консольной программы.
3. Реализовать класс - контейнер и класс - итератор согласно варианту.
4. Перемещение по контейнеру и изменение элементов контейнера осуществлять только через итераторы.
Для итераторов предусмотреть возможность доступа к произвольному элементу контейнера.
Элементом контейнера является класс, заданный по варианту в 2 лабораторной работе.
5. Реализовать сортировку контейнера согласно варианту.Продемонстрировать работу заданных итераторов.
6. Любую работу с данными реализовывать через обработку исключительных ситуаций.
Реализовать перегрузку операторов для контейнера и итератора.Для обработки всех ошибочных ситуаций использовать конструкцию try…catch().
Реализовать собственный класс обработки исключительной ситуации.
Рассмотреть замену обработчиков функций terminate() и unexpected() собственными функциями.
7. Для ассоциативных контейнеров либо ключ, либо значение должны быть уникальными, 
так что на свое усмотрение выбирайте уникальное поле.
8. Для полученного варианта задания, 
построить контейнерный шаблонный класс операций над элементами данных, включающий операции :
--добавления;
--удаления;
--поиска;
--просмотра;
9. Для данного контейнерного класса предусмотреть при формировании элемента задание режима уникальных элементов
(т.е.проверку на дублирование значений элементов).
10. В функции main создать три экземпляра шаблонного класса - контейнера для разных типов данных.
Работа с этими объектами должна демонстрироваться на следующих операциях : 
добавить – просмотреть – найти – удалить – найти – просмотреть.
11. Отладить и выполнить полученную программу.Проверить обработку исключительных ситуаций
(например, чтение из пустого стека, дублирование объектов и т.п.).
Контейнер: двунаправленный стек. 
Итераторы: вывода и двунаправленный. 
Сортировка: быстрая сортировка.
*/

#include <iostream>
#include <string>
#include <exception>
#include<time.h>
#include <cstdlib>        

using namespace std;

template <class T>
class Stac;

class Serial {
private:
	int id;
	string name;
	double rate;
public:
	Serial() :id(0), name("Blank "), rate(0) { }
	Serial(string t_name, double _rate, int id) :name(t_name), rate(_rate), id(id) { }
	~Serial() { }
	bool operator< (const Serial& co) { return id < co.id; }
	bool operator> (const Serial& co) { return id > co.id; }
	bool operator== (const Serial& co) { return id == co.id && name == co.name && rate == co.rate; }
	friend ostream& operator<< (ostream& out, const Serial& c)
	{
		out << c.id << ' ' << c.name << ' ' << c.rate << '\n'; return out;
	}

};

class Exception
{
public:
	Exception(const string errorMessage = "Something wrong") : m_ErrorMessage(errorMessage) { }
	friend ostream& operator<< (ostream& out, const Exception& ex) { out << ex.m_ErrorMessage << '\n'; return out; }
private:
	string m_ErrorMessage;
};

template <class T>
struct Node {
	T value;
	Node* left, * right;
	Node() : value() { left = right = nullptr; }
};

template <class T>
class Iterator
{
public:
	friend class Stac<T>;
	Iterator(Node<T>* node = nullptr) : node(node) { }
	~Iterator() { }
	Iterator(const Iterator& it) { node = it.node; }
	Iterator& operator=(const Iterator& it) { node = it.node; return *this; }
	T& operator*() {
		if (node == nullptr)
			throw Exception("Empty iterator");
		return node->value;
	}
	T* operator->() { if (node == nullptr) throw Exception("Empty iterator"); return &node->value; }
	Iterator& operator++() { if (node == nullptr) throw Exception("Empty iterator"); node = node->right; return *this; }
	Iterator& operator--() { if (node == nullptr) throw Exception("Empty iterator"); node = node->left; return *this; }
	Iterator operator++(int unused) { if (node == nullptr) throw Exception("Empty iterator"); Iterator res = *this; node = node->right; return res; }
	Iterator operator--(int unused) { if (node == nullptr) throw Exception("Empty iterator"); Iterator res = *this; node = node->left; return res; }
	bool operator== (const Iterator& it) { return node == it.node; }
	bool operator!= (const Iterator& it) { return node != it.node; }
public:
	Node<T>* node;
};

template<class T>
class Stac
{
public:
	typedef Iterator<T> Iterator;
public:
	Stac() : head(nullptr) {}
	~Stac() {}
	void push_front(T value, bool unique = false)
	{
		if (unique) {
			Iterator it = find(value);
			if (it != null()) {
				throw Exception("duplicate value find");
			}
		}
		Node<T>* newNode = new Node<T>();
		newNode->value = value;
		newNode->right = nullptr;
		newNode->left = nullptr;
		if (head != nullptr) {
			head->right = newNode;
			newNode->left = head;
		}
		head = newNode;
	}
	void pop_front()
	{
		if (head == nullptr)
			throw Exception("Stuck is empty");
		Node<T>* temp = head;
		head = head->left;
		head->right = nullptr;
		delete temp;
	}
	Iterator erase(Iterator);
	Iterator find(T value) const;
	void show() const;
	void show(Iterator) const;
	Iterator begin() { return Iterator(head); }
	Iterator null() { return Iterator(); }
	void swap(Iterator, Iterator);
private:
	Node<T>* head;
};

template<class T>
typename Stac<T>::Iterator Stac<T>::erase(Stac<T>::Iterator it)
{
	if (head == nullptr)
		throw Exception("Stuck is empty");
	if (it == null())
		throw Exception("Invalid iterator");
	Node<T>* temp = it.node;
	if (it.node->left != nullptr) {
		it.node = it.node->left;
		it.node->right = temp->right;
		if (temp->right != nullptr)
			temp->right->left = it.node;
		delete temp;
		return it;
	}
	else {
		Node<T>* temp = it.node;
		it.node = it.node->right;
		it.node->left = nullptr;
		delete temp;
		return it;
	}
}

template<class T>
typename Stac<T>::Iterator Stac<T>::find(T value) const
{
	Node<T>* it = head;
	do
	{
		if (it->value == value)
			return Iterator(it);
		it = it->left;
	} while (it != nullptr);
	return Iterator();
}

template<class T>
void Stac<T>::show() const
{
	Node<T>* it = head;
	while (it->right != nullptr) it = it->right;
	do
	{
		cout << ' ' << it->value;
		it = it->left;
	} while (it != nullptr);
	cout << '\n';
}

template<class T>
void Stac<T>::show(Iterator it) const
{
	while (it.node->right != nullptr) it.node = it.node->right;
	do
	{
		cout << ' ' << it.node->value;
		it.node = it.node->left;
	} while (it.node != nullptr);
	cout << '\n';
}

template<class T>
void Stac<T>::swap(Stac<T>::Iterator it1, Stac<T>::Iterator it2) {
	Node<T>* temp = nullptr;

	if (it2.node->right) it2.node->right->left = it1.node;
	temp = it1.node->right;
	it1.node->right = it2.node->right;
	it2.node->right = temp;
	if (temp) temp->left = it2.node;
	if (it2.node->left) it2.node->left->right = it1.node;
	temp = it1.node->left;
	it1.node->left = it2.node->left;
	it2.node->left = temp;
	if (temp) temp->right = it2.node;

	if (this->head == it1.node) {
		this->head = it2.node;
	}
	if (this->head == it2.node) {
		this->head = it1.node;
	}
}

template<class T>
void fastSort(Iterator<T> begin, Iterator<T> end, Stac<T> stac)
{
	if (begin == Iterator<T>())
		throw Exception("Invalid iterator");
	if (end == Iterator<T>())
		throw Exception("Invalid iterator");
	if (begin == end) return;
	Iterator<T> bgn = begin;
	Iterator<T> nd = end;
	for (Iterator<T> i = begin; i != end; i--)
	{
		if (*i < *bgn)
		{
			stac.swap(i, bgn);
			begin = i;
			bgn = i;
		}
	}
	for (Iterator<T> i = end; i != begin; i++)
	{
		if (*i > *nd)
		{
			stac.swap(i, nd);
			end = i;
			nd = i;
		}
	}
	begin--;
	fastSort(begin, end, stac);
	begin++; end++;
	fastSort(begin, end, stac);
	return;
}

void my_terminate() {
	cerr << "terminate handler called\n";
	abort();
}
void my_unexpected_handler()
{
	cerr << "Unexpected exception\n";
	terminate();
}

int main() {
	srand(time(NULL));
	set_terminate(my_terminate);
	set_unexpected(my_unexpected_handler);
	Stac<Serial> stac;
	try {
		stac.pop_front();
	}
	catch (Exception ex) {
		cerr << ex;
	}
	cout << "\nInserting\n";
	try {
		stac.push_front(Serial("Breakong bad", 98, 10));
		stac.show();
		stac.push_front(Serial("BBT", 97, 45));
		stac.show();
		stac.push_front(Serial("CHZO", 99, 6));
		stac.show();
		stac.push_front(Serial("Friends", 93, 7));
		stac.show();
		stac.push_front(Serial("Star Wars", 95, 9));
		stac.show();
		stac.push_front(Serial("Prizon break", 95, 4));
		stac.show();
	}
	catch (Exception ex) { cerr << ex; }
	try {
		stac.pop_front();
		stac.show();
		stac.show(stac.begin());
		try {
			cout << "\nErase BBT\n";
			stac.erase(stac.find(Serial("BBT", 97, 45)));
		}
		catch (Exception ex) { cerr << ex; };
		stac.show();
		try {
			auto dan = stac.find(Serial("Friends", 93, 7));
			auto anz = stac.find(Serial("Star Wars", 95, 9));
			stac.swap(dan, anz);
		}
		catch (Exception ex) { cerr << ex; };
		stac.show();
		try {
			Stac<Serial>::Iterator bgn = stac.begin();
			Stac<Serial>::Iterator end = stac.begin();
			while (end.node->left != nullptr) end--;
			fastSort(bgn, end, stac);
		}
		catch (Exception ex) { cerr << ex; };
		cout << "\nAfter Sorting\n";
		stac.show();
	}
	catch (Exception ex) { cerr << ex; }
	cout << "\nNew Temp\n";
	try {
		Stac<double> stac1;
		try {
			stac1.push_front(4);
			stac1.show();
			stac1.push_front(2);
			stac1.show();
			stac1.push_front(1);
			stac1.show();
			stac1.push_front(0);
			stac1.show();
			stac1.push_front(3);
			stac1.show();
			stac1.push_front(7);
			stac1.show();
		}
		catch (Exception ex) {
			cerr << ex;
		}
		try {
			Stac<double>::Iterator bgn = stac1.begin();
			Stac<double>::Iterator end = stac1.begin();
			while (end.node->left != nullptr) end--;
			fastSort(bgn, end, stac1);
		}
		catch (Exception ex) { cerr << ex; };
		cout << "\nAfter Sorting\n";
		stac1.show();
	}
	catch (Exception ex) { cerr << ex; };
	return 0;
}