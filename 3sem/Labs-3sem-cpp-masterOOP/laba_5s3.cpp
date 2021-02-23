//Общие требования к выполнению работы
//1.	Базовый класс должен быть абстрактным.
//2.	По полученному базовому классу создать классы наследников по двум разным ветвям наследования(B←P1←P11 и B←P2) :
//    a.во всех классах должны быть свои данные(характеристики объектов), расположенные в private или protected секциях;
//b.во всех классах создать конструкторы инициализации объектов для всех классов(не забыть про передачу параметров в конструкции базовых классов);
//c.во всех классах создать деструкторы;
//d.остальные методы создавать по необходимости.
//3.	Создать в базовом классе чисто виртуальные функции расчета(например, расчет площади фигуры и т.п.) и вывода объекта на экран(всех его параметров).Выполнить реализацию этих виртуальных функций в классах наследниках.
//4.	Задать в базовом классе деструктор как виртуальный.
//5.	В головной функции динамически создать массив указателей на базовый класс.Заполнить массив указателями на динамически создаваемые объекты производных классов(P1, P11, P2).Для каждого элемента массива проверить работу виртуальных функций.Удалить из памяти динамически выделенные объекты.
//6.	Всё взаимодействие с экземплярами производных классов производить через указатель на базовый класс.
//7.	Сгенерировать диаграмму классов


#include <iostream>
#include <clocale>
#include <cstring>

using namespace std;

class kursach_po_kpi9p
{
protected:
    string tema;
    double time_for_work;
    string mentor;
    string language;

public:
    virtual void ShowData() = 0;

    void setTema(string _tema)
    {
        tema = _tema;
    }

    void setTime_for_work(double _time_for_work)
    {
        time_for_work = _time_for_work;
    }

    void setMentor(string _mentor)
    {
        mentor = _mentor;
    }

    void setLanguage(string _language)
    {
        language = _language;
    }

    kursach_po_kpi9p() {
        tema = "ОАИП";
        time_for_work = 1;
        mentor = "Луцик Ю.А.";
        language = "С";
    }

    kursach_po_kpi9p(string _tema, double _time_for_work, string _mentor, string _language) :tema(_tema), time_for_work(_time_for_work), mentor(_mentor), language(_language) {};

    virtual ~kursach_po_kpi9p() {
        cout << "DS kp";
    };
};

class common_proga : virtual public kursach_po_kpi9p
{
protected:
    string subd;
    int kol_vo_func;
    int kol_vo_strok;

public:

    void setSubd(string _subd)
    {
        subd = _subd;
    }
    void setKol_vo_func(int _kol_vo_func)
    {
        kol_vo_func = _kol_vo_func;
    }
    void setKol_vo_strok(int _kol_vo_strok)
    {
        kol_vo_strok = _kol_vo_strok;
    }

    common_proga() {

        subd = "Отсутствует";
        kol_vo_func = 0;
        kol_vo_strok = 0;
    }

    common_proga(string _subd, int _kol_vo_func, int _kol_vo_strok) :subd(_subd), kol_vo_func(_kol_vo_func), kol_vo_strok(_kol_vo_strok) {};

    void ShowData()
    {
        cout << endl << "--------- Обычная Прога ---------" << endl;
        cout << "Тема :" << tema << std::endl;
        cout << "Время работы над КП:" << time_for_work << std::endl;
        cout << "Mentor :" << mentor << std::endl;
        cout << "language(prog) :" << language << std::endl;
        cout << "Название выбранной СУБД:" << subd << std::endl;
        cout << "Кол-во оригинальных функций :" << kol_vo_func << std::endl;
        cout << "Кол-во строк кода :" << kol_vo_strok << std::endl;
    }

    ~common_proga() {
        cout << "DS proga";
    };

};

class gui : virtual public common_proga
{
protected:
    string language_gui;
    string main_color;
    int kol_vo_colors;

public:

    void setLanguage_gui(string _language_gui)
    {
        language_gui = _language_gui;
    }
    void setMain_color(string _main_color)
    {
        main_color = _main_color;
    }
    void setKol_vo_colors(int _kol_vo_colors)
    {
        kol_vo_colors = _kol_vo_colors;
    }

    gui() {
        language_gui = "Отсутвует";
        main_color = "Цвет не выбран";
        kol_vo_colors = 0;
    }
    gui(string _language_gui, string _main_color, int _kol_vo_colors) :language_gui(_language_gui), main_color(_main_color), kol_vo_colors(_kol_vo_colors) {}

    void ShowData()
    {
        cout << endl << "--------- GUI ---------" << endl;
        cout << "Тема :" << tema << std::endl;
        cout << "Время работы над КП:" << time_for_work << std::endl;
        cout << "Mentor :" << mentor << std::endl;
        cout << "language(prog) :" << language << std::endl;
        cout << "Название выбранной СУБД:" << subd << std::endl;
        cout << "Кол-во оригинальных функций :" << kol_vo_func << std::endl;
        cout << "Кол-во строк кода :" << kol_vo_strok << std::endl;
        cout << "Навзание GUI :" << language_gui << std::endl;
        cout << "Основной цвет  :" << main_color << std::endl;
        cout << "Кол-во цветов :) :" << kol_vo_colors << std::endl;
    }
    ~gui() {
        cout << "DS gui";
    };


};

class game : virtual public kursach_po_kpi9p
{
protected:
    string type_of_game;
    double hours_for_win;
    int kol_vo_players;

public:

    void settype_of_game(string _type_of_game)
    {
        type_of_game = _type_of_game;
    }
    void sethours_for_win(double _hours_for_win)
    {
        hours_for_win = _hours_for_win;
    }
    void setkol_vo_players(int _kol_vo_players)
    {
        kol_vo_players = _kol_vo_players;
    }
    game() {
        type_of_game = "Zero";
        hours_for_win = 0;
        kol_vo_players = 0;
    };

    game(string _type_of_game, double _hours_for_win, int _kol_vo_players) :type_of_game(_type_of_game), hours_for_win(_hours_for_win), kol_vo_players(_kol_vo_players) {}

    void ShowData()
    {
        cout << endl << "--------- Игра ---------" << endl;
        cout << "Тема :" << tema << std::endl;
        cout << "Время работы над КП:" << time_for_work << std::endl;
        cout << "Mentor :" << mentor << std::endl;
        cout << "language(prog) :" << language << std::endl;
        cout << "Жанр игры:" << type_of_game << std::endl;
        cout << "Кол-во часов для прохождения Игры :" << hours_for_win << std::endl;
        cout << "Кол-во игроков :" << kol_vo_players << std::endl;
    }
    ~game() {
        cout << "DS game";
    };
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

double getValue_dbl()
{
    while (true)
    {
        double a;
        std::cin >> a;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(32767, '\n');
            std::cout << "ERROR! DOUBLE\n";
        }
        else
        {
            std::cin.ignore(32767, '\n');
            return a;
        }
    }
}

//string getValue_str()
//
//{
//    while (true)
//    {
//        string a;
//        std::cin >> a;
//        if (std::cin.fail())
//        {
//            std::cin.clear();
//            std::cin.ignore(32767, '\n');
//            std::cout << "ERROR! STRING!\n";
//        }
//        else
//        {
//            std::cin.ignore(32767, '\n');
//            return a;
//        }
//    }
//}

string getValue_str(string s)
{
    int i = 0;
    string str;
    while (s[i])
    {
        if ((int(s[i]) >= 48 && (int(s[i]) <= 57)) || ((s[i] >= 97) && (s[i] <= 122)))
            str = str + s[i];
        else
        {
            cout << "Ошибочка в строке. Не надо так =( " <<endl;
            cin >> s;
            return getValue_str(s);
        }
        i++;
    }
    return str;
}

void SetKp(kursach_po_kpi9p& kursach)
{
    string tema;
    double time_for_work;
    string mentor;
    string language;
    cout << "======== Курсач по КПиЯП ========" << endl;
    cout << "Тема :" << endl;
    cin >> tema;
    getValue_str(tema);
    cout << "Время работы над КП:" << endl;
    time_for_work = getValue_dbl();
    cout << "Mentor :" << endl;
    cin >> mentor;
    getValue_str(mentor);
    cout << "Language(prog) :" << endl;
    cin >> language;
    getValue_str(tema);
    kursach.setTema(tema);
    kursach.setTime_for_work(time_for_work);
    kursach.setMentor(mentor);
    kursach.setLanguage(language);
}



void SetCommon_proga(common_proga& proga)
{
    SetKp(proga);
    string subd;
    int kol_vo_func;
    int kol_vo_strok;
    cout << endl << "--------- Обычная Прога ---------" << endl;
    cout << "Название выбранной СУБД:" << endl;
    cin >> subd;
    getValue_str(subd);
    cout << "Кол-во оригинальных функций :" << endl;
    kol_vo_func = getValue_int();
    cout << "Кол-во строк кода :" << endl;
    kol_vo_strok = getValue_int();
    proga.setSubd(subd);
    proga.setKol_vo_func(kol_vo_func);
    proga.setKol_vo_strok(kol_vo_strok);
    
}

void SetGui(gui& guii)
{
    SetCommon_proga(guii);
    string language_gui;
    string main_color;
    int kol_vo_colors;

    cout << endl << "--------- GUI ---------" << endl;
    cout << "Навзание GUI :" << std::endl;
    cin >> language_gui;
    getValue_str(language_gui);
    cout << "Основной цвет  :" << std::endl;
    cin >> main_color;
    getValue_str(main_color);
    cout << "Кол-во цветов :) :" << std::endl;
    kol_vo_colors = getValue_int();
    guii.setLanguage_gui(language_gui);
    guii.setMain_color(main_color);
    guii.setKol_vo_colors(kol_vo_colors);

}

void SetGame(game& gg)
{
    SetKp(gg);
    string type_of_game;
    double hours_for_win;
    int kol_vo_players;


    cout << endl << "--------- Игра ---------" << endl;
    cout << "Жанр игры:" << endl;
    cin >> type_of_game;
    getValue_str(type_of_game);
    cout << "Кол-во часов для прохождения Игры :" << endl;
    hours_for_win = getValue_dbl();
    cout << "Кол-во игроков :" << endl;
    kol_vo_players = getValue_int();
    gg.settype_of_game(type_of_game);
    gg.sethours_for_win(hours_for_win);
    gg.setkol_vo_players(kol_vo_players);
}

int main()
{

    setlocale(LC_ALL, "Ru");
   

    gui* ptrGui = new gui[2];
    common_proga* ptrProga = new common_proga[2];
    game* ptrGame = new game[2];
    kursach_po_kpi9p* Kp;

    for (int i = 0; i < 2; i++)
	{
		SetGui(ptrGui[i]);
		Kp = &ptrGui[i];
		
		Kp->ShowData();
		
	}

	for (int i = 0; i < 2; i++)
	{
		SetCommon_proga(ptrProga[i]);
		Kp = &ptrProga[i];
		Kp->ShowData();
	}

	for (int i = 0; i < 2; i++)
	{
		SetGame(ptrGame[i]);
        Kp = &ptrGame[i];
        Kp->ShowData();
	}

    cout << endl << "===================" << endl;
    cout << endl << "========END=======" << endl;
    cout << endl << "===================" << endl;
}