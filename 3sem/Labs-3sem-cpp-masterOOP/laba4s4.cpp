
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
    kursach_po_kpi9p() {
        tema = "ОАИП";
        time_for_work = 1;
        mentor = "Луцик Ю.А.";
        language = "С";
    }

    kursach_po_kpi9p(string _tema, double _time_for_work, string _mentor, string _language) :tema(_tema), time_for_work(_time_for_work), mentor(_mentor), language(_language) {};

    void Show_main()
    {
        cout << endl << "--------- Курсач по КПиЯП ---------" << endl;
        cout << "Тема :" << tema << std::endl;
        cout << "Время работы над КП:" << time_for_work << std::endl;
        cout << "Mentor :" << mentor << std::endl;
        cout << "language(prog) :" << language << std::endl;
    }

    ~kursach_po_kpi9p() {};
};

class common_proga: virtual public kursach_po_kpi9p 
{
protected:
    string subd;
    int kol_vo_func;
    int kol_vo_strok;

public:
    common_proga() {
        
        subd = "Отсутствует";
        kol_vo_func = 0;
        kol_vo_strok = 0;
    }

    common_proga(string _subd, int _kol_vo_func, int _kol_vo_strok) :subd(_subd), kol_vo_func(_kol_vo_func), kol_vo_strok(_kol_vo_strok) {};

    void Show_cammon()
    {
        cout << endl << "--------- Обычная Прога ---------" << endl;
        cout << "Название выбранной СУБД:" << subd << std::endl;
        cout << "Кол-во оригинальных функций :" << kol_vo_func << std::endl;
        cout << "Кол-во строк кода :" << kol_vo_strok << std::endl;
    }

    ~common_proga() {};

};

class gui: virtual public common_proga
{
protected:
    string language_gui;
    string main_color;
    int kol_vo_colors;

public:

    gui() {
        language_gui = "Отсутвует";
            main_color = "Цвет не выбран";
            kol_vo_colors = 0;
    }
    gui(string _language_gui, string _main_color, int _kol_vo_colors) :language_gui(_language_gui),  main_color(_main_color),  kol_vo_colors(_kol_vo_colors){}

    void Show_gui()
    {
        cout << endl << "--------- GUI ---------" << endl;
        cout << "Навзание GUI :" << language_gui << std::endl;
        cout << "Основной цвет  :" << main_color << std::endl;
        cout << "Кол-во цветов :) :" << kol_vo_colors << std::endl;
    }
    ~gui() {};


};

class game: virtual public kursach_po_kpi9p
{
protected:
    string type_of_game;
    double hours_for_win;
    int kol_vo_players;

public:
    game() {
        type_of_game = "Zero";
            hours_for_win = 0;
            kol_vo_players = 0;
    };

    game(string _type_of_game, double _hours_for_win, int _kol_vo_players) :type_of_game(_type_of_game), hours_for_win(_hours_for_win), kol_vo_players(_kol_vo_players){}

    void Show_game()
    {
        cout << endl << "--------- Игра ---------" << endl;
        cout << "Жанр игры:" << type_of_game << std::endl;
        cout << "Кол-во часов для прохождения Игры :" << hours_for_win << std::endl;
        cout << "Кол-во игроков :" << kol_vo_players << std::endl;
    }
    ~game() {};
};

class game_II:  virtual public game
{
protected:
    double time_to_machine_learning;
    string difficulty_of_the_ii;
    int kol_vo_v_strategy;

public:
    game_II() {
        time_to_machine_learning = 0;
            difficulty_of_the_ii = "Сложность не выбрана";
            kol_vo_v_strategy = 0;
    }
    game_II(double _time_to_machine_learning, string _difficulty_of_the_ii, int _kol_vo_v_strategy) :time_to_machine_learning(_time_to_machine_learning),  difficulty_of_the_ii(_difficulty_of_the_ii),  kol_vo_v_strategy(_kol_vo_v_strategy) {}

    void Show_II()
    {
        cout << endl << "--------- ИИ ---------" << endl;
        cout << "Время затраченное на машинное обучение:" << time_to_machine_learning << std::endl;
        cout << "Сложность Игры :" << difficulty_of_the_ii << std::endl;
        cout << "Кол-во стратегий бота :" << kol_vo_v_strategy << std::endl;
    }

    ~game_II() {}

};

class po9snitelna9_zapiska final : virtual public game_II, virtual public gui
{
protected:
    int mark;
    string student;
    int kol_vo_stranic;

public:


    po9snitelna9_zapiska() {
        mark = 10;
        student = "Киреев";
            kol_vo_stranic = 1337;
    }

    po9snitelna9_zapiska(
    string tema_,
    double time_for_work_,
    string mentor_,
    string language_,
    string subd_,
    int kol_vo_func_,
    int kol_vo_strok_,
    string language_gui_,
    string main_color_,
    int kol_vo_colors_,
    string type_of_game_,
    double hours_for_win_,
    int kol_vo_players_,
    double time_to_machine_learning_,
    string difficulty_of_the_ii_,
    int kol_vo_v_strategy_,
    int mark_,
    string student_,
    int kol_vo_stranic_
    )
    {
        tema = tema_;
            time_for_work = time_for_work_;
            mentor = mentor_;
            language = language_;
            subd = subd_;
            kol_vo_func = kol_vo_func_;
            kol_vo_strok = kol_vo_strok_;
            language_gui = language_gui_;
            main_color = main_color_;
            kol_vo_colors = kol_vo_colors_;
            type_of_game = type_of_game_;
            hours_for_win = hours_for_win_;
            kol_vo_players = kol_vo_players_;
            time_to_machine_learning = time_to_machine_learning_;
            difficulty_of_the_ii = difficulty_of_the_ii_;
            kol_vo_v_strategy = kol_vo_v_strategy_;
            mark = mark_;
            student = student_;
            kol_vo_stranic = kol_vo_stranic_;
            
    };

    void show_PZ()
    {
        cout << endl << "--------- Пояснительная записка ---------" << endl;
        cout << "Тема :" << tema << std::endl;
        cout << "Время работы над КП:" << time_for_work << std::endl;
        cout << "Mentor :" << mentor << std::endl;
        cout << "language(prog) :" << language << std::endl;
        cout << "Название выбранной СУБД:" << subd << std::endl;
        cout << "Кол-во оригинальных функций :" << kol_vo_func << std::endl;
        cout << "Кол-во строк кода :" << kol_vo_strok << std::endl;
        cout << "Жанр игры:" << type_of_game << std::endl;
        cout << "Кол-во часов для прохождения Игры :" << hours_for_win << std::endl;
        cout << "Кол-во игроков :" << kol_vo_players << std::endl;
        cout << "Время затраченное на машинное обучение:" << time_to_machine_learning << std::endl;
        cout << "Сложность Игры :" << difficulty_of_the_ii << std::endl;
        cout << "Кол-во стратегий бота :" << kol_vo_v_strategy << std::endl;
        cout << "===========" << std::endl;
        cout << "Оценка:" << mark << std::endl;
        cout << "Фамилия студента :" << student << std::endl;
        cout << "Кол-во страниц ПЗ :" << kol_vo_stranic << std::endl;
    };

    ~po9snitelna9_zapiska(){}
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

string getValue_str()

{
    while (true)
    {
        string a;
        std::cin >> a;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(32767, '\n');
            std::cout << "ERROR! STRING!\n";
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

    setlocale(LC_ALL, "Ru");
    string tema;
    double time_for_work;
    string mentor;
    string language;

    cout << "--------- Курсач по КПиЯП ---------" << endl;
    cout << "Тема :" << endl;
    tema = getValue_str();
    cout << "Время работы над КП:" << endl;
    time_for_work = getValue_dbl();
    cout << "Mentor :" << endl;
    mentor = getValue_str();
    cout << "Language(prog) :" << endl;
    language = getValue_str();

    kursach_po_kpi9p kp(tema, time_for_work, mentor, language);

    string subd;
    int kol_vo_func;
    int kol_vo_strok;

    cout << endl << "--------- Обычная Прога ---------" << endl;
    cout << "Название выбранной СУБД:" << endl;
    subd = getValue_str();
    cout << "Кол-во оригинальных функций :" << endl;
    kol_vo_func = getValue_int();
    cout << "Кол-во строк кода :" << endl;
    kol_vo_strok = getValue_int();

    common_proga ct(subd, kol_vo_func, kol_vo_strok);

    string language_gui;
    string main_color;
    int kol_vo_colors;

    cout << endl << "--------- GUI ---------" << endl;
    cout << "Навзание GUI :" << std::endl;
    language_gui = getValue_str();
    cout << "Основной цвет  :" << std::endl;
    main_color = getValue_str();
    cout << "Кол-во цветов :) :" << std::endl;
    kol_vo_colors = getValue_int();

    gui guuu(language_gui, main_color, kol_vo_colors);

    string type_of_game;
    double hours_for_win;
    int kol_vo_players;


    cout << endl << "--------- Игра ---------" << endl;
    cout << "Жанр игры:" << endl;
    type_of_game = getValue_str();
    cout << "Кол-во часов для прохождения Игры :" << endl;
    hours_for_win = getValue_dbl();
    cout << "Кол-во игроков :" << endl;
    kol_vo_players = getValue_int();

    game gm(type_of_game, hours_for_win, kol_vo_players);

    double time_to_machine_learning;
    string difficulty_of_the_ii;
    int kol_vo_v_strategy;

    cout << endl << "--------- ИИ ---------" << endl;
    cout << "Время затраченное на машинное обучение:" << endl;
    time_to_machine_learning = getValue_dbl();
    cout << "Сложность Игры :" << endl;
    difficulty_of_the_ii = getValue_str();
    cout << "Кол-во стратегий бота :" << endl;
    kol_vo_v_strategy = getValue_int();

    game_II ii(time_to_machine_learning, difficulty_of_the_ii, kol_vo_v_strategy);

    int mark;
    string student;
    int kol_vo_stranic;

    cout << endl << "--------- Пояснительная записка ---------" << endl;
    cout << "Оценка:" << endl;
    mark = getValue_int();
    cout << "Фамилия студента :" << endl;
    student = getValue_str();
    cout << "Кол-во страниц ПЗ :" << endl;
    kol_vo_stranic = getValue_int();

    cout << endl << "--------- ---------" << endl;
    cout << endl << "--------- ---------" << endl;
    cout << endl << "--------- ---------" << endl;


    po9snitelna9_zapiska ZP
    (
        tema,
        time_for_work,
        mentor,
        language,
        subd,
        kol_vo_func,
        kol_vo_strok,
        language_gui,
        main_color,
        kol_vo_colors,
        type_of_game,
        hours_for_win,
        kol_vo_players,
        time_to_machine_learning,
        difficulty_of_the_ii,
        kol_vo_v_strategy,
        mark,
        student,
        kol_vo_stranic
    );

    ZP.show_PZ();
}

