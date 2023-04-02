#include<cstring>
#include<ctime>
#include<iostream>
#define MAX_QUEUE_LEN 100
using namespace std;

/* описание классов:
>Client: (57 строка)
    имя, номер телефона и время вхождения в список
    
    * конструктор по умолчанию ожидает ввод с клавиатуры
    void print_name() const; void print_phoneNum() const; void change_[name/phoneNum](), ввод с клавиатуры; 
    void change(const char* nm, const char* phn, bool change_entryTime), 1 - менять, 0 - нет.
>Node: (195 строка)
    Client* , Node* Next, Node* Prev. 
_________________________________________ методы:
    Node* next();     Node* prev();      void bind(Node* nd, bool orient), orient = 0 => this левее.
    
    Node();   Node(const char* nm, const char* phn);    Node(const Client& Cl);   Node(const Node& nd);
    ~Node(); 
    
    Node& operator = (const Node& nd);    bool operator != (const Node& nd);    
    
    Client* get_data()const;       void print() const, выводит данные клиента. 

>List: (295 строка)
    Node* First, Node *Last, static unsigned long int AoC_All, общее число клиентов
    unsigned int AoC = 0, клиенты в конкр. списке;   static unsigned int AoL, кол-во списков
    unsigned int ListNum = 0, номер списка в системе. (все нумерации идут с 1( -списков, -элементов списка)).
_________________________________________методы:

    unsigned int get_ListNum()const;    static unsigned int get_AoL();    static unsigned long int get_AoC_All();
    List();     List(const char* nm, const char* phn);    List(const Client& Cl);   ~List(), удаляет весь список.

    void push_front(const Client& Cl);     void push_back(const Client& Cl);
    bool insert(unsigned int p, const Client& Cl), перед позицией p => не вставляет после последнего, для этого push_back()

    Client* front()const;     Client* back()const;

    bool pop_front();     bool pop_back();      bool erase(unsigned int p);
    bool empty() const;
    
    unsigned int size() const;
    
    void print() const;     void print_backwards()const;

>Queue: List:    (550 строка)
    unsigned int Max_AoC// по умолчанию = 100
__________________методы: (все методы inline, вызывают методы от класса List)
    Queue(const unsigned int Mx = MAX_QUEUE_LEN);     Queue(const char* nm, const char* phn, long int Mx = MAX_QUEUE_LEN);
    Queue(const Client& Cl, long int Mx = MAX_QUEUE_LEN);  ~Queue(), сразу ~List();
    bool back(const Client& Cl);       Client* front() const;       bool pop();         bool empty() const;
    unsigned int size() const;        bool full() const;      void print() const;
*/


class Client{
    char* name = NULL;
    unsigned int nameLen;// name length
    char entryTime[30];
    char* phoneNum = NULL;
    int numLen;

public:
    Client();//default constructor
    Client(const char* nm, const char* phn);
    Client(const Client& Cl);
    ~Client();

    Client& operator = (const Client& Cl);
    
    void print_name() const{//inline
        if(name != NULL)
            cout << name << "\n";
    }

    void change_name();
    
    void print_phoneNum() const{//inline
        if(phoneNum != NULL)
            cout << "Phone number: " << phoneNum << "\n";
    }

    void change_phoneNum();

    void print_entryTime() const{//inline
        cout << "Entry time: " << entryTime;
    }
    
    void change(const char* nm, const char* phn, bool change_tm);
};

Client::Client(){
    change_name();
    change_phoneNum();

    time_t now = time(0);
    strcpy(entryTime,ctime(&now));

    cout << name << "'s default constructor\n";
}

Client::Client(const char* nm, const char* phn){
    name = new char[(nameLen = strlen(nm)) + 1];
    strcpy(name, nm);

    phoneNum = new char[(numLen = strlen(phn)) + 1];
    strcpy(phoneNum, phn);
    
    time_t now = time(0);
    strcpy(entryTime,ctime(&now));

    cout << name << "'s conversion constructor\n";
}

Client::Client(const Client& Cl){
    name = new char[(nameLen = Cl.nameLen) + 1];
    strcpy(name, Cl.name);
    
    phoneNum = new char[(numLen = Cl.numLen) + 1];
    strcpy(phoneNum, Cl.phoneNum);
    
    time_t now = time(0);
    strcpy(entryTime,ctime(&now));

    cout << name << "'s copier constructor\n";
}

Client::~Client(){
    
    delete[]phoneNum;
    cout << name << "'s destructor\n";
    delete[]name;
}

Client& Client::operator = (const Client& Cl){
    if(this == &Cl){
        return *this;
    }
    delete[]name;
    delete[]phoneNum;

    name = new char[(nameLen = Cl.nameLen) + 1];
    strcpy(name, Cl.name);
    
    phoneNum = new char[(numLen = Cl.numLen) + 1];
    strcpy(phoneNum, Cl.phoneNum);

    cout << name << "'s operation = \n";
    return *this;
}




void Client::change_name(){
    if(name != NULL)
        delete[]name;
    char tmp[20];
    cout << "Enter the name:\n";
    cin >> tmp;
    name = new char[(nameLen = strlen(tmp)) + 1];
    strcpy(name, tmp);
}

void Client::change_phoneNum(){
    if(phoneNum != NULL)
        delete[]phoneNum;
    char tmp[20];
    cout << "Enter the phone number of " << name << ":\n";
    cin >> tmp;
    phoneNum = new char[(numLen = strlen(tmp)) + 1]; 
    strcpy(phoneNum, tmp);
}



void Client::change(const char* nm, const char* phn, bool change_tm){
    delete[]name;
    delete[]phoneNum;
    name = new char[(nameLen = strlen(nm)) + 1];
    strcpy(name, nm);
    phoneNum = new char[(numLen = strlen(phn)) + 1];
    strcpy(phoneNum, phn);
    if(change_tm){
        time_t now = time(0);
        strcpy(entryTime,ctime(&now));
    }
}





//____________________________________________________________________________________________________

class Node{
    Client* data = NULL;// 0 Clients
    Node* Next = this;
    Node* Prev = this;
public:
    Node* next() const{//inline
        return Next;
    }

    Node* prev() const{//inline
        return Prev;
    }

    void bind(Node* nd, bool orient);

    Node(){cout << "Node\'s default constructor\n";}
    Node(const char* nm, const char* phn);// parameters of first Client
    Node(const Client& Cl);
    Node(const Node& nd);
    ~Node();
    Node& operator = (const Node& nd);
    bool operator != (const Node& nd);

    Client* get_data()const{
        return data;
    }
    void print()const;
};

void Node::bind(Node* nd, bool orient){
    if(!orient){
        Next = nd;
        nd->Prev = this;
        return;
    }
    nd->Next = this;
    Prev = nd;
}

Node::Node(const char* nm, const char* phn){

    data = new Client(nm, phn);

    cout << "Node\'s conversion constructor\n";
}

Node::Node(const Client& Cl){

    data = new Client(Cl);

    cout << "Node\'s conversion constructor\n";
}

Node::Node(const Node& nd){
    Next = nd.Next;
    Prev = nd.Prev;
    data = new Client(*(nd.data));
    cout << "Node\'s copier constructor\n";
}

Node::~Node(){
    if(data != NULL){// empty
        delete data;
    }

    cout << "Node\'s destructor\n";
}

Node& Node::operator = (const Node& nd){// unneccessary
    if(&nd == this)
        return *this;
    delete data;
    data = new Client(*(nd.data));
    Next = nd.Next;
    Prev = nd.Prev;
    cout << "Node\'s operator =\n";
    return *this;
}

bool Node::operator != (const Node& nd){
    if(data == nd.data && Next == nd.Next && Prev == nd.Prev)
        return false;
    return true;
}

void Node::print()const{
    if(data == NULL){
        cout << "empty node\n";
        return;
    }
    data->print_name();
    data->print_entryTime();
    data->print_phoneNum();
}



//_______________________________________________________________________________________

class List{
    unsigned int AoC = 0;
    Node* First = NULL;// 0 of Clients
    Node* Last = NULL;
    static unsigned long int AoC_All;// ammount of clients in all lists
    static unsigned int AoL; // ammount of lists
    unsigned int ListNum = 0;
    static void Incr_AoC_All(){
        ++AoC_All;
    }
    void Incr_AoC(){
        ++AoC;
    }
    static void Decr_AoC_All(){
        --AoC_All;
    }
    void Decr_AoC(){
        --AoC;
    }
    static void Decr_AoL(){
        --AoL;
    }
    void set_ListNum(){
        ++AoL;
        ListNum = AoL;
    }

public:
    unsigned int get_ListNum()const{
        return ListNum;
    }
    static unsigned int get_AoL() {return AoL;}
    static unsigned long int get_AoC_All() {return AoC_All;}
    List(){set_ListNum(); cout << "List\'s default constructor\n";}
    List(const char* nm, const char* phn);
    List(const Client& Cl);
    ~List();

    void push_front(const Client& Cl);// new first node of the list
    void push_back(const Client& Cl);
    
    Client* front()const;
    Client* back()const;

    bool pop_front(); // pop() = front(); pop_front();
    bool pop_back();
    bool insert(unsigned int p, const Client& Cl);
    bool erase(unsigned int p);
    bool empty() const{
        return !AoC;
    };
    
    unsigned int size() const{
        return AoC;
    }
    
    void print() const;
    void print_backwards()const;
};

List::List(const char* nm, const char* phn){
    First = new Node(nm, phn);
    Last = First;
    Incr_AoC();
    Incr_AoC_All();
    set_ListNum();
    cout << "List\'s conversion constructor";
}

List::List(const Client& Cl){
    First = new Node(Cl);
    Last = First;
    Incr_AoC();
    Incr_AoC_All();
    set_ListNum();
    cout << "List's conversion constructor \n";
}

List::~List(){
    if(First == NULL){
        return;
    }
    Node* ptr = First;
    while(ptr != Last){
        ptr = ptr->next();
        delete ptr->prev();
        Decr_AoC_All();
    }
    delete ptr;
    Decr_AoC_All();
    Decr_AoL();
    cout << "List\'s destructor\n";
}

void List::push_front(const Client& Cl){
    if(First == NULL){
        First = new Node(Cl);
        Last = First;
        Incr_AoC();
        Incr_AoC_All();
        return;
    }
    Node* nw = new Node(Cl);
    nw->bind(First,0);
    nw->bind(Last,1);
    First = nw;
    Incr_AoC();
    Incr_AoC_All();
}

void List::push_back(const Client& Cl){
    if(First == NULL){
        First = new Node(Cl);
        Last = First;
        Incr_AoC();
        Incr_AoC_All();
        return;
    }
    Node* nw = new Node(Cl);
    nw->bind(Last, 1);
    nw->bind(First, 0);
    Last = nw;
    Incr_AoC();
    Incr_AoC_All();
}


Client* List::front() const{//inline
    if(AoC != 0)
        return First->get_data();
    return NULL;  
}

Client* List::back() const{
    if(AoC != 0)
        return Last->get_data();
    return NULL;  
}

bool List::insert(unsigned int p, const Client& Cl){
    if(p == 0 || p > AoC){
        return false;
    }
    if(p == 1){
        push_front(Cl);
        return true;
    }
    Node* ptr = First;
    Node* nw = new Node(Cl);
    for(unsigned int i = 1; i<p; i++){
        ptr = ptr->next();
    }
    nw->bind(ptr->prev(), 1);
    nw->bind(ptr, 0);
    Incr_AoC();
    Incr_AoC_All();
    return true;
}

bool List::pop_front(){
    if(First == NULL){
        return false;
    }
    if(AoC == 1){
        delete First;
        First = Last = NULL;
        
        Decr_AoC();
        Decr_AoC_All();
        return true;
    }
    Node* ptr = First;
    First = First->next();
    First->bind(Last,1);
    delete ptr;
    Decr_AoC();
    Decr_AoC_All();
    return 1;
}

bool List::pop_back(){
    if(First == NULL){
        return false;
    }
    if(AoC == 1){
        delete First;
        First = Last = NULL;
        
        Decr_AoC();
        Decr_AoC_All();
        return true;
    }
    Node* ptr = Last;
    Last = Last->prev();
    Last->bind(First,0);
    delete ptr;
    Decr_AoC();
    Decr_AoC_All();
    return true;
}

bool List::erase(const unsigned int p){
    if(p==0 || p>AoC){return false;}
    if(p==1){
        pop_front();
        return true;
    }
    if(p==AoC){
        pop_back();
        return true;
    }
    Node* ptr = First;
    for(unsigned int i = 1; i<p; i++)
        ptr = ptr->next();
    ptr->next()->bind(ptr->prev(),1);
    delete ptr;
    Decr_AoC();
    Decr_AoC_All();

    return true;
}

void List::print() const{
    cout << "ID: " << ListNum << endl;
    if(First == NULL){cout << "is empty\n"; return;}
    Node* tmp = First;
    tmp->print();
    cout << endl;
    tmp = tmp->next();
    while(tmp != First){
        tmp->print();
        tmp = tmp->next();
        cout << "\n";
    }
}

void List::print_backwards() const{
    cout << "ID: " << ListNum << ", backwards" << endl;
    if(First == NULL){cout << "is empty\n"; return;}
    Node* tmp = Last;
    tmp->print();
    cout << endl;
    tmp = tmp->prev();
    while(tmp != Last){
        tmp->print();
        tmp = tmp->prev();
        cout << "\n";
    }
}

unsigned long int List::AoC_All = 0;
unsigned int List::AoL = 0;



//______________________________QUEUE___

class Queue: List{
    unsigned int Max_AoC;

public:
    Queue(const unsigned int Mx = MAX_QUEUE_LEN){
        Max_AoC = Mx;
        cout << "Queue's constructor 1\n";
    }
    
    Queue(const char* nm, const char* phn, long int Mx = MAX_QUEUE_LEN): List(nm, phn){
        if(Mx >= 0)
            Max_AoC = Mx;
        cout << "Queue's constructor 2\n";
    }
    
    Queue(const Client& Cl, long int Mx = MAX_QUEUE_LEN): List(Cl){
        if(Mx >= 0)
            Max_AoC = Mx;
        cout << "Queue's constructor 3\n";
    }
    
    ~Queue(){cout << "Queue's destructor\n";}
    
    bool back(const Client& Cl){// check Max_AoC
        if(size() < Max_AoC){
            push_back(Cl);
            return true;
        }
        return false;
    }

    Client* front() const{
        return List::front();
    }
    
    bool pop(){
        return pop_front();
    }

    bool empty() const{
        return List::empty();
    }

    unsigned int size() const{
        return List::size();
    }

    bool full() const{
        return size() == Max_AoC;
    }

    void print() const{
        List::print();
    }
};
//____________________________________


int main(){
    List L; // общие счётчики для List и для Queue (общий учёт клиентов и самих структур) => у Queue ID = 2.
    char s[2] = {'a', '\0'};
    Client Cl(s,"234");
    Queue Q(Cl, 3);
    cout << Q.empty() << ": Queue is not empty<<<<\n";
    while(!Q.full()){
        ++s[0];
        Cl.change(s, "234", 0);
        Q.back(Cl);
    }
    Q.print();
    cout << Q.back(Cl) << ": cannot accept a new Client\n" << endl;
    Q.pop();

    cout << "_____________________\n\n";
    Q.print();

    ++s[0];
    Cl.change(s, "234", 0);

    cout << "\n" << Q.back(Cl) << ": can accept a new Client\n" << endl;
    cout << "_____________________\n\n";
    Q.print();
    Cl = *(Q.front());
    cout << "_____________________\n\nfront.name: ";
    Cl.print_name();
    
/*
    List l("a", "123");
    Client Cl1("b","234");
    List l2(Cl1);
    Cl1.change("c", "345", 1);
    l2.push_front(Cl1);
    Cl1.change("d", "456", 1);
    l.push_back(Cl1);
// l: [a, d]. l2: [c,b]
    cout << "CHECK: ListNum " << l2.get_ListNum() << "; AoL " << l2.get_AoL() << "; AoC_All " << l2.get_AoC_All() << "; AoC " << l2.size() << endl;
    l.print();
    cout << "_____________________\n\n";
    l2.print();
    Cl1.change("e", "567", 1);
    l.insert(2,Cl1);
// l: [a, e, d].
    cout << "_____________________\n\n";
    l.print();
    cout << "CHECK: ListNum " << l.get_ListNum() << "; AoL " << l.get_AoL() << "; AoC_All " << l.get_AoC_All() << "; AoC " << l.size() << endl;

    cout << "_____________________\n\n";
    l.erase(2);
    l.print();
    cout << "CHECK: ListNum " << l2.get_ListNum() << "; AoL " << l2.get_AoL() << "; AoC_All " << l2.get_AoC_All() << "; AoC " << l2.size() << endl;
    l.print_backwards();
    cout << "CHECK: ListNum " << l.get_ListNum() << "; AoL " << l.get_AoL() << "; AoC_All " << l.get_AoC_All() << "; AoC " << l.size() << endl;

    l.erase(2);
    cout << "_____________________\n\n";
    l.print();
    cout << "CHECK: ListNum " << l.get_ListNum() << "; AoL " << l.get_AoL() << "; AoC_All " << l.get_AoC_All() << "; AoC " << l.size() << endl;

    l.pop_front();
    cout << "list 1 is empty: " << l.empty() << endl;
    l.print();
    l2.print();
*/
    return 0;
}