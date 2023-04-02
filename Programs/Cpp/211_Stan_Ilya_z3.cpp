#include<iostream>
#include<cstring>
#define PENT_PRISE 1000 // цена дома
#define PENT_INCOME 100 // доход после каждого хода
#define PENT_PROFIT 1500 // цена, по которой можно продать
#define CAFE_PRISE 3000
#define CAFE_MAX_INCOME 400 // доход в диапазоне от 50 до 400
#define CAFE_PROFIT 4000
#define NIGHT_PRISE 5000
#define NIGHT_INCOME 500
#define NIGHT_PROFIT 6500
#define BUSI_PRISE 10000
#define BUSI_MUL 0.1 // цена businesscenter растёт при его продаже на BUSI_PRISE * BUSI_MUL
#define BUSI_INCOME 1000
#define BUSI_PROFIT 14000
#define START_BUDGET 5000

using namespace std;

//Определение основных классов для игры.
//Порядок следования важен, чтобы определение типа было до инициализации пременной типа.

class Wallet{
    int wallet = START_BUDGET;
public:
    Wallet(){}
    ~Wallet(){}
    int get_wallet(){return wallet;}
    friend class Cafe;
    friend class Businesscenter;
    friend class Penthouse;
    friend class Nightclub;
};

class Object{
    bool is_sold = false;
    unsigned int income;
    unsigned int prise;
    int sell_profit;
public:
    bool sold() const{
        return is_sold;
    }
    unsigned int get_prise() const{
        return prise;
    }
    void add_to_prise(unsigned int add){
        prise += add;
    }
    void change_is_sold(){
        is_sold = !is_sold;
    }
    Object(unsigned int profit, unsigned long int cost, int sllprft);
    ~Object(){}

    virtual void sell(Wallet& Wall) = 0;
    virtual bool buy(Wallet& Wall) = 0;
    virtual void profit(Wallet& Wall) = 0;
    virtual void info() const = 0;
};

class Player{// независимый класс с массивом
    Wallet* w;
    Object* Houses[4];
    void inform();
    void income();
    void all_sold(bool& end);
    void mov(char move[4]);
    bool mov_check(char move[4]);
public:
    Wallet* get_w(){return w;}
    Player();
    ~Player();
    void game();
};



// Определение классов от Object

class Penthouse: public Object{
public:
    Penthouse():Object(PENT_INCOME, PENT_PRISE, PENT_PROFIT){}
    ~Penthouse(){}
    void sell(Wallet& Pl);
    bool buy(Wallet& Pl);
    void profit(Wallet& Pl);
    void info() const;
};

class Nightclub: public Object{
    bool visit = 0;
public:
    void change_visit(){
        visit = !visit;
    }
    Nightclub():Object(NIGHT_INCOME, NIGHT_PRISE, NIGHT_PROFIT){}
    ~Nightclub(){}
    void sell(Wallet& Wall);
    bool buy(Wallet& Wall);
    void profit(Wallet& Wall);
    void info() const;
};

class Cafe: public Object{
public:
    Cafe():Object(0, CAFE_PRISE, CAFE_PROFIT){}
    ~Cafe(){}
    void sell(Wallet& Wall);
    bool buy(Wallet& Wall);
    void profit(Wallet& Wall);
    void info() const;
};

class Businesscenter: public Object{
public:
    Businesscenter():Object(BUSI_INCOME, BUSI_PRISE, BUSI_PROFIT){}
    ~Businesscenter(){}
    void sell(Wallet& Wall);
    bool buy(Wallet& Wall);
    void profit(Wallet& Wall);
    void info() const;
};


// Определения функций, которые не inline. Порядок соответствует порядку определения классов.

Object::Object(unsigned int profit, unsigned long int cost, int sllprft){
    income = profit;
    prise = cost;
    sell_profit = sllprft;
}


//_____________________________Player_

void Player::inform(){
    for(int i = 0; i<4; ++i){
        Houses[i]->info();
        cout << endl;
    }
}
void Player::income(){
    for(int i = 0; i<4; ++i){
        if(Houses[i]->sold()){
            Houses[i]->profit(*(get_w()));
            cout << endl;
        }
    }
}
void Player::all_sold(bool& end){
    end = true;
    for(int i = 0; i<4; ++i){
        end = end && Houses[i]->sold();
    }
}
void Player::mov(char move[4]){
    if(move[0] != 'W'){
        if(move[0] == 'B'){
            Houses[move[1]-'0'-1]->buy(*(get_w()));
        }
        else{
            Houses[move[1]-'0'-1]->sell(*(get_w()));
        }
    }
}
bool Player::mov_check(char move[4]){
    char c;
Metka: scanf("%3s", move);
    while(getchar() != '\n'); // игнорирование следующих символов в istream

    if(!strcmp(move, "end") || !strcmp(move, "End")){
        cout << "Конец по спец-слову.\n";
        return true;
    }
    if(move[0] != 'W' && move[0] != 'B' && move[0] != 'S'){
        cout << "Неправильный ход. Попробуйте ещё раз.\n";
        goto Metka;
    }
    else if(move[0] != 'W' && (move[1] < '1' || move[1] > '4')){
        cout << "Неправильный номер объекта. Попробуйте ещё раз.\n";
        goto Metka;
    }
    return false;
}

Player::Player(){
    w = new Wallet;
    Penthouse* p = new Penthouse;
    Cafe* c = new Cafe;
    Nightclub* n = new Nightclub;
    Businesscenter* b = new Businesscenter;
    Houses[0] = p; Houses[1] = n; Houses[2] = c; Houses[3] = b;
}

Player::~Player(){
    for(int i = 0; i < 4; ++i){
        delete Houses[i];
    }
    delete w;
}

// Сама игра. 

void Player::game(){
    char move[4];
    short count = 0;
    bool end = 0;
    cout << "Добро пожаловать!\nСмысл игры - купить по дому каждого типа за минимальное число шагов.\n";
    cout << "Результат, который необходимо побить: 12 шагов.\n";
    cout << "Делайте ход правильно: первый символ ввода: B, S или W (купить, продать или ждать).\n";
    cout << "Дальше без лишних символов: число от 1 до 4 - номер интересующего вас объекта.\n";
    cout << "Во время хода можно написать \"End\", чтобы закончить игру.\nУдачи!\n\n";
    while(!end){
        cout << "\nТекущее состояние недвижимости:\n\n";
        inform();
        cout << "Хотите что-то купить, продать? Или соберёте доход с купленного?\n";
        cout << "Текущее состояние кошелька: " << get_w()->get_wallet() << "." << endl;
        if(mov_check(move))
            return;
        mov(move);
        cout << "\nКонец хода " << ++count << ".\n\n";
        income();
        all_sold(end);
    }
    cout << "Победа!\n";
    inform();
    cout << "Количество затраченных ходов: " << count << endl;
}


//_____________________________________Penthouse____

void Penthouse::sell(Wallet& Wall){
    if(sold()){
        Wall.wallet += PENT_PROFIT;
        change_is_sold();
        cout << "Penthouse продан.\n";
        return;
    }
    cout << "У вас ещё нет penthouse на продажу.\n";
}

bool Penthouse::buy(Wallet& Wall){
    if(!sold()){
        if(Wall.wallet >= PENT_PRISE){
            Wall.wallet -= PENT_PRISE;
            change_is_sold();
            cout << "Вы купили penthouse!\n";
            return true;
        }
        cout << "Не достаточно средств для покупки penthouse.\n";
        return false;
    }
    cout << "У вас уже есть penthouse.\n";
    return false;
}

void Penthouse::profit(Wallet& Wall){
    if(sold()){
        short chance;
        chance = rand() % 20 - 10;
        if(chance >= 0){
            Wall.wallet += PENT_INCOME;
            cout << "Вы заработали " << PENT_INCOME << " монет с penthouse.\n";
        }
        else{
            cout << "У penthouse не было съёмщиков, дохода нет.\n";
        }
    }
}
void Penthouse::info() const{
    if(sold()){
        cout << "Penthouse: (куплено)\nДоходность: " << PENT_INCOME << " монет/ход (если есть съёмщики).\nМожно продать за " << PENT_PROFIT << " монет.\n";
        return;
    }
    cout << "Penthouse:\nЦена: " << PENT_PRISE <<". Доходность: " << PENT_INCOME << ".\nМожно продать за " << PENT_PROFIT << " монет."<<endl;
}


//_____________________________________Nightclub__

void Nightclub::sell(Wallet& Wall){
    if(sold()){
        Wall.wallet += NIGHT_PROFIT;
        change_is_sold();
        cout << "Nightclub продан.\n";
        return;
    }
    cout << "У вас ещё нет Nightclub на продажу.\n";
}

istream& operator << (istream& in, bool& b){ //если стрелки в другую сторону, то, неоднозначность выбора функции
// есть такая же системная, поэтому компилятор не может выбрать => стрелки, как для ostream.
    char word[4];
    scanf("%3s", word);
    while(getchar() != '\n'); // игнорирование следующих символов в istream
    if(!strcmp(word,"yes") || !strcmp(word,"Yes")){
        b = true;
        return in;
    }
    b = false;
    return in;
}

bool Nightclub::buy(Wallet& Wall){
// Здесь используется перегрузка оператора <<.
    bool bargain;
    int sale;
    if(!sold()){
        cout << "- Хотите начать торговаться? (yes = да, no = нет): ";
        cin << bargain;
        if(!bargain){
            cout << "- Значит, хотите за полную цену.\n";
            goto FULL_PRISE;
        }
        if(bargain && visit){
            cout << "- Уже торговались, хватит.\nХотите купить за полную цену? (yes = да, no = нет)\n";
            cin << bargain;
            if(bargain)
                goto FULL_PRISE;
            cout << "Сделка не состоялась.\n";
            return false;
        }
        if(bargain && !visit){
            change_visit();
            sale = get_prise() - (rand() % NIGHT_PRISE);
            if(sale < NIGHT_PRISE / 2)
                cout << "(Хозяин нездоров).\n";
            cout << "- На " << sale << " согласны? (yes = да, no = нет): ";
            cin << bargain;
            if(bargain){
                if(Wall.wallet >= sale){
                    Wall.wallet -= sale;
                    change_is_sold();
                    cout << "Вы купили Nightclub!\n";
                    return true;
                }
                cout << "Не хватает средств для совершения сделки по nightclub.\n";
                return false;
            }
            cout << "- Нет, так нет.\n";
            return false;
        }
FULL_PRISE:if(Wall.wallet >= NIGHT_PRISE){
            Wall.wallet -= NIGHT_PRISE;
            change_is_sold();
            cout << "Вы купили nightclub!\n";
            return true;
        }
        cout << "Не достаточно средств для покупки nightclub за полную цену.\n";
        return false;
    }
    cout << "У вас уже есть nightclub.\n";
    return false;
}

void Nightclub::profit(Wallet& Wall){
    if(sold()){
        Wall.wallet += NIGHT_INCOME;
        cout << "Вы заработали "<< NIGHT_INCOME << " монет с Nightclub.\n";
    }
}

void Nightclub::info() const{
    if(sold()){
        cout << "Nightclub: (куплено)\nДоходность: " << NIGHT_INCOME << " монет/ход.\nМожно продать за " << NIGHT_PROFIT << " монет.\n";
        return;
    }

    cout << "Nightclub:\nЦена: " << NIGHT_PRISE << ". Возможностей для торга: " << !visit << ". Доходность: " << NIGHT_INCOME << ".\nМожно продать за " << NIGHT_PROFIT << " монет."<<endl;
}


//________________________________Cafe___

void Cafe::sell(Wallet& Wall){
    if(sold()){
        Wall.wallet += CAFE_PROFIT;
        change_is_sold();
        cout << "Cafe продан.\n";
        return;
    }
    cout << "У вас ещё нет cafe на продажу.\n";
}

bool Cafe::buy(Wallet& Wall){
    if(!sold()){
        if(Wall.wallet >= CAFE_PRISE){
            Wall.wallet -= CAFE_PRISE;
            change_is_sold();
            cout << "Вы купили cafe!\n";
            return true;
        }
        cout << "Не достаточно средств для покупки cafe.\n";
    return false;
    }
    cout << "У вас уже есть cafe.\n";
    return false;
}

void Cafe::profit(Wallet& Wall){
    if(sold()){
        int in = rand() % (CAFE_MAX_INCOME-50) + 50;
        Wall.wallet += in;
        cout << "Вы заработали " << in << " монет с cafe.\n";
    }
}

void Cafe::info() const{
    if(sold()){
        cout << "Cafe: (куплено)\nДоходность: зависит от числа клиентов.\nМожно продать за " << CAFE_PROFIT << " монет.\n";
        return;
    }
    cout << "Cafe:\nЦена: " << CAFE_PRISE << ". Доходность: зависит от числа клиентов.\nМожно продать за " << CAFE_PROFIT << " монет."<<endl;
}


//__________________________________Businesscenter__

void Businesscenter::sell(Wallet& Wall){
    if(sold()){
        Wall.wallet += BUSI_PROFIT;
        change_is_sold();
        add_to_prise(BUSI_PRISE * BUSI_MUL);
        cout << "Businesscenter продан.\n";
        return;
    }
    cout << "У вас ещё нет businesscenter на продажу.\n";
}
bool Businesscenter::buy(Wallet& Wall){
    if(!sold()){
        if(Wall.wallet >= BUSI_PRISE){
            Wall.wallet -= BUSI_PRISE;
            change_is_sold();
            cout << "Вы купили businesscenter!\n";
            return true;
        }
        cout << "Не достаточно средств для покупки businesscenter.\n";
        return false;
    }
    cout << "У вас уже есть businesscenter.\n";
    return false;
}
void Businesscenter::profit(Wallet& Wall){
    if(sold()){
        Wall.wallet += BUSI_INCOME;
        cout << "Вы заработали " << BUSI_INCOME << " монет с businesscenter.\n";
    }
}
void Businesscenter::info() const{
    if(sold()){
        cout << "Businesscenter: (куплено)\nДоходность: " << BUSI_INCOME << " монет/ход.\nМожно продать за " << BUSI_PROFIT << " монет.\n";
        return;
    }
    cout << "Businesscenter:\nЦена: " << get_prise() << ". Доходность: " << BUSI_INCOME << ".\nМожно продать за " << BUSI_PROFIT << " монет."<<endl;
}

// Finish.

// Ввод контроллируется. При неправильном вводе будет предложено сделать ход заново.

int main(){
    Player Pl;
    Pl.game();
    return 0;
}

/* рекорд - 8. */
