#include <iostream>
#include <windows.h>
#include <string>
#include <Poclass.h>
#pragma warning(suppress : 99)
using namespace std;



bool SleepHibernation(bool parametr)
{
    OSVERSIONINFO ver;
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    // в семействе NT для выключения необходимо иметь привилегию SE_SHUTDOWN_NAME
        HANDLE hToken;
        TOKEN_PRIVILEGES* NewState;
        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
        NewState = (TOKEN_PRIVILEGES*)malloc(sizeof(TOKEN_PRIVILEGES) + sizeof(LUID_AND_ATTRIBUTES));
        NewState->PrivilegeCount = 1;
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &NewState->Privileges[0].Luid);
        NewState->Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, NewState, NULL, NULL, NULL);
        free(NewState);
        CloseHandle(hToken);

    UINT mod = 0;

    if (parametr)
        mod += 1; //Сон
    else
        mod += 0; //Гибернация

    return SetSystemPowerState(mod, 1);
}


int main()
{
    SYSTEM_POWER_STATUS power;
    _BATTERY_INFORMATION battery;
    setlocale(LC_ALL, "Russian");
    int sw;
    if (GetSystemPowerStatus(&power)) {
        while (1) {
            system("CLS");
            cout << "Энергопитание. Сделайте выбор" << endl;
            cout << "1 - Тип энергопитания " << endl;
            cout << "2 - Процент ёмкости батареи " << endl;
            cout << "3 - Состояние режима энергосбережения " << endl;
            cout << "4 - Состояние питания переменного тока " << endl;
            cout << "5 - Количество оставшихся секунд заряда аккумулятора " << endl;
            cout << "6 - Погрузить в спящий режим" << endl;
            cout << "7 - Гибернация" << endl;
            cout << "8 - Выход" << endl;

            cin >> sw;
            switch (sw) {

            case 1: {
                system("CLS");
                cout << "Тип энергопитания - " << static_cast<double>(power.BatteryFlag) << endl;
                cout << "Описание\n";
                cout << "1 - Высокая - емкость аккумулятора превышает 66 процентов" << endl;
                cout << "2 - Низкий - емкость аккумулятора менее 33 процентов." << endl;
                cout << "4 - Критично - емкость аккумулятора менее пяти процентов." << endl;
                cout << "8 - Зарядка" << endl;
                cout << "128 - Нет системной батареи" << endl;
                cout << "255 - Неизвестный статус - невозможно прочитать информацию о флаге батареи." << endl;
                system("pause");
                break;
            }
            case 2: {
                system("CLS");
                cout << "Процент ёмкости батареи - " << static_cast<double>(power.BatteryLifePercent) << endl;
                cout << "При (255) - статус неизвестен (ПК)" << endl;
                system("pause");
                break;
            }
            case 3: {
                system("CLS");
                cout << "Состояние режима энергосбережения - " << static_cast<double>(power.SystemStatusFlag) << endl;
                cout << "Описание\n";
                cout << "0 - Режим энергосбережения выключен. " << endl;
                cout << "1 - Экономия заряда батареи включена. " << endl;
                system("pause");
                break;
            }
            case 4: {
                system("CLS");
                cout << "Состояние питания переменного тока - " << static_cast<double>(power.ACLineStatus) << endl;
                cout << "Описание\n";
                cout << "0 - Не в сети " << endl;
                cout << "1 - Онлайн " << endl;
                cout << "255 - Неизвестный статус " << endl;
                system("pause");
                break;
            }
            case 5: {
                system("CLS");
                cout << "Количество оставшихся секунд заряда аккумулятора - " << static_cast<double>(power.BatteryLifeTime) << endl;
                cout << "–1, если полный срок службы аккумулятора неизвестен или если устройство подключено к источнику переменного тока " << endl;
                system("pause");
                break;
            }
            case 6: {
                 return SleepHibernation(1);
            }
            case 7: {
                return SleepHibernation(0);
            }
            case 8: {
                return 0;
            }
            }
        }
    }
    else
        cout << "error" << endl;
}

