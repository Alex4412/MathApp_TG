#include <iostream>
#include <stack>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <tgbot/tgbot.h>


#define PI 3.14159265
int code = 0; ///код ошибки

using namespace TgBot;
using namespace std;

int Priority (const string &c);
bool TryParse (const string &symbol);
bool isOperator (const string &c);
bool isError(string resh);
bool isNum(string num);
string resh();
string formatstring (string a);
string CheckForCorrect();
string expression(string infix);
void ReversePolishNotation(string infix);
void clean();


vector <string> bot_commands = {"start", "help"};
vector <string> outputList;/// выходной vector
vector <string> tokens; /// хранилище токенов
stack <string> s;///main stack
string stroke;
int I = 0, Stop = 0;


int main() {

    //Bot bot("5117515650:AAGP9Z9m3IFurSIJCxuLvVbHbgwfQFNwKlE"); /// тестовый бот
    Bot bot("5022579824:AAE0610-5i3jE_EBZfvsx68XJC2QgPS5Uy0"); /// основной бот

    bot.getEvents().onCommand("start", [&bot](Message::Ptr message) {
        try {
            string username;
            username = message->chat->firstName;
            bot.getApi().sendMessage(message->chat->id, "Привет, " + message->chat->firstName + "!");
            bot.getApi().sendMessage(message->chat->id, "Хотите узнать, как работать с этим ботом? Нажмите на /help");
            bot.getApi().sendMessage(470844976, "Пользователь " + username + " присоединился!" + '\n' + message->chat->username);
        }
        catch (TgException &e) {
            printf("error: %s\n", e.what());
        }
    });

    bot.getEvents().onCommand("help", [&bot](Message::Ptr message) {
        try {
            bot.getApi().sendMessage(message->chat->id, "Формат ввода математического выражения: \n"
                                                        "Выражение надо записывать без = и прочих символов, кроме нижеперечисленных команд \n"
                                                        "sin a , где а - число (в градусах)\n"
                                                        "cos a , где а - число (в градусах)\n"
                                                        "tg a , где а - число (в градусах)\n"
                                                        "ctg a , где а - число (в градусах)\n"
                                                        "log a b , где а - основание логарифма, б - число \n"
                                                        "root a b, где root - корень, a - степень корня, b - число\n"
                                                        "mod a - модуль значения а\n"
                                                        "a ^ b - а в степени b, где а и b - числа\n"
                                                        "a * b - а умножить на b, где а и b - числа\n"
                                                        "a / b или  a : b - а разделить на b, где а и b - числа\n"
                                                        "a + b - а плюс b, где а и b - числа\n"
                                                        "a - b - а минус b, где а и b - числа\n"
                                                        "( - скобка открывется \n"
                                                        ") - скобка закрывется\n"
                                                        "Пример записи математического выражения:\n"
                                                        "(sin 90 / cos 0 + 9 ) * ( 431 - tg 180)\n"
                                                        "3 ^ 4 + ( 11 - ( 3 * 2 ) ) / 2");
            bot.getApi().sendMessage(message->chat->id, "Попробуйте ввести 2 + 2");

        }
        catch (TgException &e) {
            printf("error: %s\n", e.what());
        }
    });

    bot.getEvents().onAnyMessage([&bot](Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());

        for (const auto &command: bot_commands)
            if ("/" + command == message->text)
                return;
        try {
            string username;
            username = message->chat->firstName;
            bot.getApi().sendMessage(message->chat->id, "Ответ: " + expression(message->text));
            bot.getApi().sendMessage(470844976, "Пользователь " + username + " написал: " + message->text);
            clean();
        }
        catch (TgException &e) {
            printf("error: %s\n", e.what());
        }
    });

    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (TgException &e) {

        printf("error: %s\n", e.what());
    }
    return 0;
}


string expression(string infix) {
        code = 0;
        Stop = 0;
        cout << "Решаю..." << endl;
        clean();
        infix = formatstring(infix);
        if (Stop == 1 || code != 0) {
            cout << "a1" << endl;
            return "Ошибка";
        }
        ReversePolishNotation(infix);
        if (Stop == 1 or outputList.size() == 0) {
            cout << "a2" << endl;
            return "Ошибка";
        }

        if (CheckForCorrect() == "1") {
            string answer = resh();
            if (isNum(answer) || isError(answer)) {
                cout << "Ответ: " << answer << " " << code << endl;
                return answer;
            }
            else{
                cout << "Ошибка ";
                cout << "a3" << endl;
                return "Ошибка";
            }
        } else {
            if (Stop == 1) {
                cout << "a4" << endl;
                return "Ошибка";
            }
            string error = CheckForCorrect();
            cout << error << " " << code << endl;
            return error;
        }
}

void clean(){
    outputList.clear();/// выходной vector
    tokens.clear(); /// хранилище токенов
    s = stack<string>();///main stack
}


void ReversePolishNotation(string infix) {
    try {

        istringstream iss(infix);

        while (iss) {
            string temp;
            iss >> temp;
            tokens.push_back(temp);
        }


        for (int i = 0; i < tokens.size(); ++i) {
            if (TryParse(tokens[i]))
                outputList.push_back(tokens[i]);
            if (tokens[i] == "(")
                s.push(tokens[i]);
            if (tokens[i] == ")") {
                while (!s.empty() && s.top() != "(") {
                    outputList.push_back(s.top());
                    s.pop();
                }
                s.pop();
            }
            if (isOperator(tokens[i])) {
                while (!s.empty() && Priority(s.top()) >= Priority(tokens[i])) {
                    outputList.push_back(s.top());
                    s.pop();
                }
                s.push(tokens[i]);
            }
        }
        while (!s.empty()) {
            outputList.push_back(s.top());
            s.pop();
        }
    }
    catch (const exception& e){
        printf("error: %s\n", e.what());
        Stop = 1;
    }
}

bool TryParse(const string &symbol) {
    try {
        bool isNumber = false;
        for (int i = 0; i < symbol.size(); ++i) {
            if (!isdigit(symbol[i]))
                isNumber = false;
            else
                isNumber = true;
        }
        return isNumber;
    }
    catch (const exception& e){
        printf("error: %s\n", e.what());
        Stop = 1;
    }
}


int Priority(const string &c) {
    if (c == "sin" || c == "cos" || c == "tg" || c == "ctg" || c == "-sin" || c == "-cos" || c == "-tg" ||
        c == "-ctg" || c == "log" || c == "root" || c == "mod")
        return 4;
    if (c == "^")
        return 3;
    if (c == "*" || c == "/")
        return 2;
    if (c == "+" || c == "-")
        return 1;
    else
        return 0;
}


bool isError(string resh) {
    return (resh == "Ошибка выполнения." || resh == "Деление на 0 невозможно." || resh == "Некорректный log." ||
            resh == "Нету значения tg." || resh == "Нету значения ctg.");
}


bool isOperator(const string &c) {
    return (c == "+" || c == "-" || c == "*" || c == "/" || c == "^" || c == "sin" || c == "cos" || c == "tg" ||
            c == "ctg" || c == "-sin" || c == "-cos" || c == "-tg" || c == "-ctg" || c == "log" || c == "root" || c == "mod");
}

bool isNum(string num){
    int point = 0;
    if(num[0] != '-') {
        if (num[0] == '.')
            return false;
        else if (num[0] < '0' || num[0] > '9')
            return false;
    }


    for (int i = 1; i < num.size(); ++i) {
        if (num[i] == '-' && (i != 0)) {
            cout << num << " " << 1 << endl;
            return false;
        }
        else if (num[i] == '.') {
            if (point == 0)
                point++;
            else {
                return false;
            }
        } else if (num[i] < '0' || num[i] > '9') {
            return false;
        }
    }
    return true;
}

/// Чтение обратной польской записи ///
string resh(){
    for (int i = 0; i < outputList.size(); ++i) {
        cout << outputList[i] + " ";
    }
    cout << endl;
    try {
        for (int i = 0; i < outputList.size(); ++i) {
            if (outputList[i] == "+" || outputList[i] == "-" || outputList[i] == "*" || outputList[i] == "/" ||
                outputList[i] == "^" || outputList[i] == "log" || outputList[i] == "root") {
                if (outputList.size() <= 2) {
                    code = 2;
                    return "Ошибка выполнения.";
                }
                if (!isNum(outputList[i - 2]) || !isNum(outputList[i - 1])) {
                    code = 3;
                    return "Ошибка выполнения.";
                }

                double a, b;
                a = stod(outputList[i - 2]);
                b = stod(outputList[i - 1]);

                if (outputList[i] == "^") {
                    outputList[i - 2] = to_string(pow(a, b));
                    outputList.erase(outputList.begin() + i - 1, outputList.begin() + i + 1);
                    i -= 2;
                } else if (outputList[i] == "/") {
                    if (b == 0)
                        return "Деление на 0 невозможно.";
                    outputList[i - 2] = to_string(a / b);
                    outputList.erase(outputList.begin() + i - 1, outputList.begin() + i + 1);
                    i -= 2;
                } else if (outputList[i] == "*") {
                    outputList[i - 2] = to_string(a * b);
                    outputList.erase(outputList.begin() + i - 1, outputList.begin() + i + 1);
                    i -= 2;
                } else if (outputList[i] == "-") {
                    outputList[i - 2] = to_string(a - b);
                    outputList.erase(outputList.begin() + i - 1, outputList.begin() + i + 1);
                    i -= 2;
                } else if (outputList[i] == "+") {
                    outputList[i - 2] = to_string(a + b);
                    outputList.erase(outputList.begin() + i - 1, outputList.begin() + i + 1);
                    i -= 2;
                } else if (outputList[i] == "log") {
                    if (b == 1) {
                        outputList[i - 2] = to_string(0);
                        outputList.erase(outputList.begin() + i - 1, outputList.begin() + i + 1);
                        i -= 2;
                    } else {
                        if (a <= 0 || a == 1 || b <= 0)
                            return "Некорректный log.";
                        outputList[i - 2] = to_string(log(b) / log(a));
                        outputList.erase(outputList.begin() + i - 1, outputList.begin() + i + 1);
                        i -= 2;
                    }
                } else if (outputList[i] == "root") {
                    if (b < 0 && int(a) % 2 == 0)
                        return "Некорректный корень.";
                    outputList[i - 2] = to_string(pow(b, (1 / a)));
                    outputList.erase(outputList.begin() + i - 1, outputList.begin() + i + 1);
                    i -= 2;
                }
            }
            if (outputList[i] == "sin" || outputList[i] == "cos" || outputList[i] == "tg" || outputList[i] == "ctg" ||
                outputList[i] == "-sin" || outputList[i] == "-cos" || outputList[i] == "-tg" ||
                outputList[i] == "-ctg" || outputList[i] == "mod") {
                if (outputList.size() <= 1) {
                    code = 4;
                    return "Ошибка выполнения.";
                }
                if (!isNum(outputList[i - 1])) {
                    code = 5;
                    return "Ошибка выполнения.";
                }
                double a;
                a = stod(outputList[i - 1]);
                if (outputList[i] == "sin") {
                    outputList[i - 1] = to_string(sin(a * PI / 180));
                    outputList.erase(outputList.begin() + i);
                    i -= 1;
                } else if (outputList[i] == "cos") {
                    outputList[i - 1] = to_string(cos(a * PI / 180));
                    outputList.erase(outputList.begin() + i);
                    i -= 1;
                } else if (outputList[i] == "tg") {
                    if (int(a) % 180 == 90)
                        return "Нету значения tg.";
                    outputList[i - 1] = to_string(sin(a * PI / 180) / cos(a * PI / 180));
                    outputList.erase(outputList.begin() + i);
                    i -= 1;
                } else if (outputList[i] == "ctg") {
                    if (int(a) % 180 == 0)
                        return "Нету значения ctg.";
                    outputList[i - 1] = to_string(cos(a * PI / 180) / sin(a * PI / 180));
                    outputList.erase(outputList.begin() + i);
                    i -= 1;
                } else if (outputList[i] == "-sin") {
                    outputList[i - 1] = to_string(-sin(a * PI / 180));
                    outputList.erase(outputList.begin() + i);
                    i -= 1;
                } else if (outputList[i] == "-cos") {
                    outputList[i - 1] = to_string(-cos(a * PI / 180));
                    outputList.erase(outputList.begin() + i);
                    i -= 1;
                } else if (outputList[i] == "-tg") {
                    if (int(a) % 180 == 90)
                        return "Нету значения tg.";
                    else {
                        outputList[i - 1] = to_string(sin(a * PI / 180) / cos(a * PI / 180));
                        outputList.erase(outputList.begin() + i);
                        i -= 1;
                    }
                } else if (outputList[i] == "-ctg") {
                    if (int(a) % 180 == 0)
                        return "Нету значения ctg.";
                    else {
                        outputList[i - 1] = to_string(-cos(a * PI / 180) / sin(a * PI / 180));
                        outputList.erase(outputList.begin() + i);
                        i -= 1;
                    }
                } else if(outputList[i] == "mod"){
                    if (int (a) < 0)
                        outputList[i - 1] = to_string(a * -1);
                    else
                        outputList[i - 1] = to_string(a);
                    outputList.erase(outputList.begin() + i);
                    i -= 1;
                }
            }
        }
        return outputList[0];
    }
    catch (const exception& e){
        printf("error: %s\n", e.what());
        Stop = 1;
        return "Ошибка";
    }

}

/// форматирует изначальную строчку под читаемую
string formatstring(string a) {
    int skobka = 0;
    for (int i = 0; i < a.size(); ++i) {
        if (a[i] == '(')
            skobka++;
        if (a[i] == ')')
            skobka--;
        if (a[i] == ':')
            a[i] = '/';
        if (a[i] == ',')
            a[i] = '.';
    }

    if (skobka != 0) ///Проверка на одинаковое количество скобок открытия и закрытия
        code = 14;

    for (int i = 0; i < a.size(); ++i) {
        /// Преобразование заглавных букв в строчные
        if (a[i] <= 'Z' && a[i] >= 'A')
            a[i] = a[i] - ('Z' - 'z');
        /// добавление пробелы, где их нет
        if (a[i] != ' ') {
            if (a[i] == '+' || a[i] == '-' || a[i] == '*' || a[i] == '/' || a[i] == ':' || a[i] == '^' || a[i] == '(' ||
                a[i] == ')' || a[i] == 'x') {
                if (a[i - 1] != ' ')
                    a.insert(i, 1, ' ');
                if (a[i + 1] != ' ')
                    a.insert(i + 1, 1, ' ');
            }
            if (a[i] >= '0' && a[i] <= '9' && (a[i - 1] == 'n' || a[i - 1] == 's' || a[i - 1] == 'g' || a[i - 1] == 't' || a[i - 1] == 'd'))
                a.insert(i, 1, ' ');
        }
        ///убирает лишние пробелы
        if (a[i] == ' ')
            if (a[i - 1] == ' ' || a[i + 1] == ' ') {
                a.erase(i, 1);
                i--;
            }
    }


    if (a[1] == '-' && (a[3]>= '0' && a[3] <= '9')){
        a.erase(2, 1);
    }
    else if (a[1] == '-'){
        a.insert(0, " 0");
    }

    ///проверка на наличие отрицательных чисел
    for (int i = 1; i < a.size(); ++i) {
        if (a[i] == '-') {
            if (a[i + 2] == '-') {
                a[i] = '+';
                a.erase(i + 1, 2);
            } else if (a[i - 2] == '+') {
                a[i] = '-';
                a.erase(i - 2, 2);
            } else if (a[i - 2] == '*' || a[i - 2] == '/' || a[i - 2] == ':' || a[i - 2] == '^' || a[i - 2] == 'g' || a[i - 2] == 'n' ||
                       a[i - 2] == 's' || a[i - 2] == '(' || a[i - 2] == 't' || a[i - 2] == 'd') {
                a.erase(i + 1, 1);
            }
        }
    }
    stroke = a;
    return a;
}

/// Проверка на наличие некорректных операций
string CheckForCorrect() {
    if (outputList.size() == 0){
        code = 12;
        return "Ошибка";
    }
    try {
        if (stroke[0] == '+' || stroke[0] == '/' || stroke[0] == ':' || stroke[0] == '*' || stroke[0] == '^' ||
            stroke[0] == '=') {
            code = 6;
            return "Введены некорректные данные.";
        }


        for (int i = 1; i < stroke.size(); ++i) {
            if (stroke[i] == '-' || stroke[i] == '+' || stroke[i] == '/' || stroke[i] == '*' || stroke[i] == '^')
                if (stroke[i + 2] == '-' || stroke[i + 2] == '+' || stroke[i + 2] == '/' || stroke[i + 2] == ':' ||
                    stroke[i + 2] == '*' ||
                    stroke[i + 2] == '^') {
                    code = 7;
                    return "Введены некорректные данные.";
                }
            if (stroke[i] == '=' || stroke[i] == 'x' || stroke[i] == 'X' || stroke[i] == 'I') {
                code = 8;
                return "Введены некорректные данные.";
            }
        }


        for (int i = 1; i < outputList.size(); ++i) {
            if (!(outputList[i] == "-" || outputList[i] == "+" || outputList[i] == "/" || outputList[i] == ":" ||
                  outputList[i] == "*" ||
                  outputList[i] == "^" || outputList[i] == "sin" || outputList[i] == "cos" || outputList[i] == "tg" ||
                  outputList[i] == "ctg" || outputList[i] == "-sin" || outputList[i] == "-cos" ||
                  outputList[i] == "-tg" ||
                  outputList[i] == "-ctg" || outputList[i] == "root" || outputList[i] == "log" || outputList[i] == "mod")) {
                if (outputList[i][0] == '-') {
                    for (int j = 1; j < outputList[i].size(); ++j)
                        if (!((outputList[i][j] >= '0' && outputList[i][j] <= '9') || outputList[i][j] == '.')) {
                            code = 9;
                            return "Введены некорректные данные.";
                        }
                } else if (outputList[i][0] >= '0' && outputList[i][0] <= '9') {
                    for (int j = 1; j < outputList[i].size(); ++j)
                        if (!((outputList[i][j] >= '0' && outputList[i][j] <= '9') || outputList[i][j] == '.')) {
                            code = 10;
                            return "Введены некорректные данные.";
                        }
                } else {
                    code = 11;
                    return "Введены некорректные данные.";
                }
            }
        }
        return "1";
    }
    catch (const exception& e){
        printf("error: %s\n", e.what());
        Stop = 1;
        return "error";
    }
}
