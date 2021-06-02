#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#define MAX 100         //Максимальное кол-во символов в строке
#define w 600           //ширина
#define h 480           //высота
#define IDENinput 2     //Индентификаторы входов выходов
#define IDENoutput 3
#define IDENcombo1 4    //Списков
#define IDENcombo2 5

HBITMAP hbackground, hbut1, hbut2;  //Рисунки: кнопки и заднний фон
HWND hwndbackground;                //Объект задний фон
HWND input;                         //Поля ввода вывода
HWND output;
HWND combo;                         //Списки
HWND combo2;
HWND but1;                          //Кнопки
HWND but2;

int selectnumber;                    //Выбранный номер из списка
int globalsystem = 2;                //Выбранная система (двоичная по умолчанию)
char ck[MAX];                        //Строка считывания с ввода
char BUF[MAX];                       //Строка-буффер для вывода
int result;                          //Результат рачсёта
int MODE = 0;                        //Режим (0 - в десятичную, 1 - из десятичной)
void loadimages(void);               //Прототип функции загрузки картинок

/*Функция reverse переворачивает строку str*/
void reverse (char *str){
    int i, j, jj,  prom[MAX];
    for(i = 0; str[i] != '\0' && (prom[i] = str[i]); i++);  //Копируем в промежуточную строку строку str
    for(j = i - 1, jj = 0; j >= 0; j--, jj++){
        str[jj] = prom[j];                                  //Записываем строку prom в str наоборот
    }
    str[jj] = '\0';
}

/*Функция from10 конвертирует число a из десятичной системы в систему system, и помещает результат в buf*/
void from10(int a, char *buf, int system){
    int del = system, i = 0;
    while(a > 0){                             //Пока частное больше 0
        if((a % system) > 9)                  //Если остаток от частного и системы > 9 то значит это буква
            buf[i] = (a % system) + 'A' - 10;
        else                                  //Иначе цифра
            buf[i] = (a % system) + '0';
        a /= system;                          //Делим частное
        i++;                                  //Мы испольуем i для индекса для buf
    }
    buf[i] = '\0';
    reverse(buf);                             //Переворачиваем
}

/*Функция to10 возращает a в десятичной системы из системы system*/
long to10(char *a, int system){
    int i, len, j;
    long res = 0;
    for(len = 0; a[len] != '\0'; len++);                //Вычисляем длину строки a
    for(j = len - 1, i = 0; a[i] != '\0'; i++, j--){    //Проходимся по строке 2 переменными 
        if(a[i] >= '0' && a[i] <= '9')                  //Если a[i] цифра
            res+= (a[i] - '0') * pow((double)system, (double)j);  //К результату прибавляем a[i] умноженую на system в степени j
        else
            if(system > 10 && system <= 16 && ((a[i] >= 'a' && a[i] <= 'f') || (a[i] >= 'A' && a[i] <= 'F'))){  //Если буква
                a[i] = (a[i] >= 'A' && a[i] <= 'Z') ? a[i] + 'a' - 'A' : a[i];                                  //Если буква большая то переносим в нижний реестр
                res += (a[i] - 'a' + 10) * pow((double)system, (double)j);                                      //Прибавляем
            }
            else                     //Если нет то возращаем ошибку
                return -1;
    }
    return res;                      //Если прошли циклы, возращаем результат
}

/*Функция convert конвертирует вход в результат (зависит от режима)*/
void convert(void){
    if(!MODE){                            //Если режим 0 то 
        result = to10(ck, globalsystem);  //Конвертируем в десятичную
        if(result != -1)
            itoa(result, BUF, 10);            //Переводим в строку
        else
            strcpy(BUF, "ERROR");
        //printf("Result = %d\n",result);
    } else {                                 //Если режим 1 то
        from10(atoi(ck), BUF, globalsystem); //Конвертируем из десятичной (перевод в строку не требуется, т.к. результат и так в строке)
        //printf("Result = %s\n",BUF);
    }
}

/*Функция comboboxupdate меняет выпадающие списки (зависит от режима)*/
void comboboxupdate(void){
    convert();                  //Конвертируем
    SetWindowText(output, BUF); //Показываем результат конвертации в поле output
    if(MODE){                   //Если режим = 1 то
        ShowWindow(combo, SW_HIDE);    //Убираем первый список
        ShowWindow(combo2, SW_NORMAL); //Показываем второй список
    }
    else{
        ShowWindow(combo, SW_NORMAL); //И наоборот
        ShowWindow(combo2, SW_HIDE);
    }
}

//Функция Mydef обрабатывает сообщения окна*/
LRESULT Mydef(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam){
    if(message == WM_DESTROY)       //Если окно закрылось любым способом, то завершаем программу
        PostQuitMessage(0);
    else
    if(message == WM_CREATE)        //При создании окна загружаем изображения
        loadimages();
    else
    if(LOWORD(wparam) == IDENcombo1){               //Если получили сигнал от первого списка то
        if(HIWORD(wparam) == CBN_SELCHANGE){        //Если поменяли курсор то
            selectnumber = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);     //Присваиваем selectnumber позицию курсора
            switch(selectnumber){                                             //Проверяем selectnumber и меняем систему счисления, и позицию курсора 2 списка
                case 0: {globalsystem = 2; SendMessage(combo2, CB_SETCURSEL, 0, (LPARAM)0); }break;
                case 1: {globalsystem = 8; SendMessage(combo2, CB_SETCURSEL, 1, (LPARAM)0); }break;
                case 2: {globalsystem = 16; SendMessage(combo2, CB_SETCURSEL, 2, (LPARAM)0);}break;
            }
            convert();                                  //Конвертируем
            //printf("Selected -> %d\n", globalsystem);
            SetWindowText(output, BUF);                 //Показываем результат в поле вывода
        }
    } else
    if(LOWORD(wparam) == IDENcombo2){                  //Всё тоже самое и со вторым списком
        if(HIWORD(wparam) == CBN_SELCHANGE){
            selectnumber = SendMessage((HWND)lparam, CB_GETCURSEL, 0, 0);
            switch(selectnumber){                                           //Только теперь меняем позицию курсора первого списка
                case 0: {globalsystem = 2; SendMessage(combo, CB_SETCURSEL, 0, (LPARAM)0); } break;
                case 1: {globalsystem = 8; SendMessage(combo, CB_SETCURSEL, 1, (LPARAM)0); } break;
                case 2: {globalsystem = 16;SendMessage(combo, CB_SETCURSEL, 2, (LPARAM)0); } break;
            }
            convert();
            //printf("Selected -> %d\n", globalsystem);
            SetWindowText(output, BUF);
        }
    } else
    if(message == WM_COMMAND){                //Если получили сообщение WM_COMMAND
        if(LOWORD(wparam) == IDENinput){      //И если это поле ввода
            int nc = HIWORD(wparam);
            GetWindowText((HWND)lparam, ck, 100); //Записываем с строку ck текст с поле ввода
            if(nc == EN_UPDATE){                  //Если мы обновили поле ввода
                convert();                        //Конвертируем
                SetWindowText(output, BUF);       //Обновляем поле вывода
            }
        }
        if(but1 == (HWND)lparam){               //Если нажата 1 кнопка
            MODE = 0;                           //Меняем режим на 0
            comboboxupdate();                   //Обновляем списки
            //printf("MODE 0");
        }
        if(but2 == (HWND)lparam){
            MODE = 1;                          //Меняем режим на 1
            comboboxupdate();
            //printf("MODE 1");
        }
    } else
    return DefWindowProcA(hwnd, message, wparam, lparam);  //Если не получили ничего возращаем DefWindowProcA
}

/*Функция loadimgaes загружает изображения .bmp из файлов*/
void loadimages(void){
    hbackground = (HBITMAP)LoadImage(NULL, "RES/back.bmp", IMAGE_BITMAP, w,h, LR_LOADFROMFILE);
    hbut1 = (HBITMAP)LoadImage(NULL, "RES/but1.bmp", IMAGE_BITMAP, 128,71, LR_LOADFROMFILE);
    hbut2 = (HBITMAP)LoadImage(NULL, "RES/but2.bmp", IMAGE_BITMAP, 128,71, LR_LOADFROMFILE);
}

int main(void){
    WNDCLASSA wcl;                          /*Регистрация окна*/
        memset(&wcl, 0, sizeof(WNDCLASSA)); //Заполняем структуру нулями
        wcl.lpszClassName = "win1";         //Класс окна
        wcl.lpfnWndProc = Mydef;            //В качестве обработчика событий используем нашу функцию 
    RegisterClassA(&wcl);                   //Регистрируем класс

    HWND hwnd;       //Дескриптер окна
    hwnd = CreateWindow("win1", "NCovert", WS_OVERLAPPEDWINDOW, 10, 10, w, h, NULL, NULL, NULL, NULL); //Создаём окно

    /*Проходимся по всем элементам интерфейса и создаём их*/

    hwndbackground = CreateWindow("static",NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, 0, 0, w, h, hwnd, NULL, NULL, NULL);
    SendMessage(hwndbackground, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbackground);

    input = CreateWindow("edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_RIGHT, 
    150, 118, 300, 20, hwnd, (HMENU)IDENinput, NULL, NULL);

    output = CreateWindow("edit", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_RIGHT | ES_READONLY, 
    150, 202, 300, 20, hwnd, (HMENU)IDENoutput, NULL, NULL);

    combo = CreateWindow("combobox", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 
    460, 115, 98, 150, hwnd, (HMENU)IDENcombo1, NULL, NULL);

    combo2 = CreateWindow("combobox", "", WS_VISIBLE | WS_CHILD | CBS_DROPDOWN, 
    460, 199, 98, 150, hwnd, (HMENU)IDENcombo2, NULL, NULL);
    
    but1 = CreateWindow("Button",NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, 150, 256, 128, 71, hwnd, NULL, NULL, NULL);
    SendMessage(but1, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hbut1);

    but2 = CreateWindow("Button",NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, 323, 256, 128, 71, hwnd, NULL, NULL, NULL);
    SendMessage(but2, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hbut2);
    
    /*Добавление строк в списки*/
    SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"2");
    SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"8");
    SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"16");
    SendMessage(combo, CB_SETCURSEL, 0, (LPARAM)0);

    SendMessage(combo2, CB_ADDSTRING, 0, (LPARAM)"2");
    SendMessage(combo2, CB_ADDSTRING, 0, (LPARAM)"8");
    SendMessage(combo2, CB_ADDSTRING, 0, (LPARAM)"16");
    SendMessage(combo2, CB_SETCURSEL, 0, (LPARAM)0);

    comboboxupdate();               //Обновляем списки

    ShowWindow (GetConsoleWindow(), SW_HIDE);
    ShowWindow(hwnd, SW_SHOWNORMAL); //Показываем окно

    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)){  //Если получаем сообщения
        TranslateMessage(&msg);           //ОБрабатываем их
        DispatchMessage(&msg);
    }
}