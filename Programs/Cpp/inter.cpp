#include <iostream>
#include <string>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <vector>
#include <stack>

using namespace std;

const char *Razdelitel[] = {"@", ";", ",", ":", "=", "(", ")", "==", "<", ">", "+", "-",
							"*", "/", "<=", "!=", ">=", "{", "}", ".", NULL};

enum type_of_lex
{
	LEX_NULL,
	LEX_STRUCT,
	LEX_AND,
	LEX_BOOL,
	LEX_DO,
	LEX_ELSE,
	LEX_IF,
	LEX_FALSE,
	LEX_INT,
	LEX_NOT,
	LEX_OR,
	LEX_PROGRAM,
	LEX_READ,
	LEX_TRUE,
	LEX_WHILE,
	LEX_WRITE,
	LEX_STRING,
	LEX_GOTO,
	LEX_BREAK,
	LEX_FOR,
	LEX_FIN,

	LEX_SEMICOLON,
	LEX_COMMA,
	LEX_COLON,
	LEX_ASSIGN,
	LEX_LPAREN,
	LEX_RPAREN,
	LEX_EQ,
	LEX_LSS,
	LEX_GTR,
	LEX_PLUS,
	LEX_MINUS,
	LEX_TIMES,
	LEX_SLASH,
	LEX_LEQ,
	LEX_NEQ,
	LEX_GEQ,
	LEX_LFIG,
	LEX_RFIG,
	LEX_DOT,
	LEX_NUM,
	LEX_ID,
	POLIZ_LABEL,
	POLIZ_ADDRESS,
	POLIZ_GO,
	POLIZ_FGO,
	LEX_STRUCT_TYPE, // типы, определяемые пользователем
	LEX_STRUCT_FIELD
};

const char *Slujeb[] = {"", "struct", "and", "bool", "do", "else", "if", "false", "int", "not", "or", "program", "read",
						"true", "while", "write", "string", "goto", "break", "for", NULL};

class Ident
{
	string name;
	bool declare;
	type_of_lex type;
	bool assign;
	int int_value;
	bool bool_value;
	string str_value;
	vector<Ident> struct_value;

public:
	Ident()
	{
		declare = false;
		assign = false;
	}
	bool operator==(const string &s) const
	{
		return name == s;
	}
	Ident(const string n)
	{
		name = n;
		declare = false;
		assign = false;
	}
	string get_name() const
	{
		return name;
	}
	bool get_declare() const
	{
		return declare;
	}
	void put_declare()
	{
		declare = true;
	}
	type_of_lex get_type() const
	{
		return type;
	}
	void put_type(type_of_lex t)
	{
		type = t;
	}
	bool get_assign() const
	{
		return assign;
	}
	void put_assign()
	{
		assign = true;
	}
	int get_int_value() const
	{
		return int_value;
	}
	void put_int_value(int v)
	{
		int_value = v;
	}
	bool get_bool_value() const
	{
		return bool_value;
	}
	void put_bool_value(bool v)
	{
		bool_value = v;
	}
	string get_str_value() const
	{
		return str_value;
	}
	void put_str_value(string v)
	{
		str_value = v;
	}
};

// есть структура со всей информацией об идентификаторе

vector<Ident> TID;

int find(vector<Ident> vec, string buf)
{
	vector<Ident>::iterator k;
	int i = 0;
	for (k = vec.begin(); k != vec.end(); k++)
	{
		if ((*k).get_name() == buf)
		{
			return i;
		}
		i++;
	}
	return i;
}

int put(const string &buf, vector<Ident> &vec)
{
	int k;

	if ((k = find(vec, buf)) != vec.size())
		return k;
	vec.push_back(Ident(buf));
	return k;
}

vector<Ident> TSTRUCT_ID;
vector<vector<Ident>> TSTRUCT_FIELDS;

class Lex
{
	type_of_lex t_lex;
	int numInT;
	string field_name; // для обращения к полю структуры. имена полей не вношу в TID, буду получать при синтаксическом анализе

public:
	Lex(type_of_lex t = LEX_NULL, int v = 0, string fld_nm = "_") : t_lex(t), numInT(v)
	{
		field_name = fld_nm;
	}
	type_of_lex get_type() const
	{
		return t_lex;
	}
	int get_value() const
	{
		return numInT;
	}
	// при переборе слов в тексте мы будем возвращать Lex - тип последней лексемы и номер в соотв таблице
};

class Scanner
{
	FILE *fp;
	int c;
	int look(const string buf, const char **list)
	{
		int i = 0;
		while (list[i])
		{
			if (buf == list[i])
				return i;
			++i;
		}
		return 0;
	}
	void gc()
	{
		c = fgetc(fp);
	}

public:
	Scanner(const char *program)
	{
		if (!(fp = fopen(program, "r")))
			throw "can\'t open file";
	}
	Lex get_lex();
};

int str = 0;
string tmp;
Lex Scanner::get_lex()
{
	enum state
	{
		H, IDENT, NUMB, DIV_COM, COM, ALE, NEQ, STRING
	};
	int d, j;
	string buf;
	state CS = H;
	do
	{
		gc();
		switch (CS)
		{

		case H:
			if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
				; // skip
			else if (c == '.')
			{
				str = 2;
				cout << "razdelitel: .\n";
				return Lex(LEX_DOT, (int)(LEX_DOT - LEX_FIN), ".");
			}
			else if (isalpha(c))
			{
				buf.push_back(c);
				CS = IDENT;
			}
			else if (isdigit(c))
			{
				d = c - '0';
				CS = NUMB;
			}
			else if (c == '/')
			{
				CS = DIV_COM;
			}
			else if (c == '=' || c == '<' || c == '>')
			{
				buf.push_back(c);
				CS = ALE;
			}
			else if (c == EOF)
			{
				cout << "end of program\n";
				return Lex(LEX_FIN);
			}
			else if (c == '!')
			{
				buf.push_back(c);
				CS = NEQ;
			}
			else if (c == '\"')
			{
				CS = STRING;
			}
			else
			{
				buf.push_back(c);
				if ((j = look(buf, Razdelitel)))
				{
					cout << "razdelitel: " << buf << endl;
					return Lex((type_of_lex)(j + (int)LEX_FIN), j, buf);
				}
				else
					throw c;
			}
			break;

		case STRING:
			if (c != EOF && c != '\"')
				buf.push_back(c);
			else if (c == EOF)
				throw c;
			else
			{
				cout << "string: " << buf << endl;
				return Lex(LEX_STRING, int(LEX_STRING), buf);
			}
			break;

		case IDENT:
			if (isalpha(c) || isdigit(c))
				buf.push_back(c);
			else
			{
				while (c == ' ')
					gc();
				if (c == '.')
					tmp = buf;
				ungetc(c, fp);
				if (buf == "struct")
				{
					str = 1;
					cout << "Slujeb: " << buf << endl;
					return Lex(LEX_STRUCT, (int)(LEX_STRUCT), buf);
				}
				else if ((j = look(buf, Slujeb)))
				{
					cout << "Slujeb: " << buf << endl;
					return Lex((type_of_lex)j, j, buf);
				}
				else
				{
					if (str == 1)
					{ // имя после слова struct
						str = 0;
						j = put(buf, TSTRUCT_ID);
						cout << "user type: " << buf << endl;
						return Lex(LEX_STRUCT_TYPE, j, buf);
					}
					else if (str == 2)
					{ // поле структуры (после точки)
						str = 0;
						cout << "field: " << tmp << '.' << buf << endl; // добавить проверку того, что имя поля не равно имени пользовательского типа (семантика)
						return Lex(LEX_STRUCT_FIELD, find(TID, tmp), buf);
					}
					j = put(buf, TID);
					cout << "ident: " << buf << endl;
					return Lex(LEX_ID, j, buf);
				}
			}
			break;

		case NUMB:
			if (isdigit(c))
			{
				d = d * 10 + (c - '0');
			}
			else
			{
				ungetc(c, fp);
				cout << "NUMB: " << d << endl;
				return Lex(LEX_NUM, d);
			}
			break;

		case DIV_COM:
			if (c == '*')
				CS = COM;
			else
			{
				ungetc(c, fp); // division
				cout << "razdelitel: " << '/' << endl;
				return Lex(LEX_SLASH, look("/", Razdelitel));
			}

		case COM:
			if (c == '*')
			{
				gc();
				if (c == '/')
				{
					cout << "comm\n";
					CS = H;
				}
			}
			else if (c == EOF)
				throw c;
			else if (c = '/')
			{
				gc();
				if (c == '*')
					throw c;
			}
			break;

		case ALE:
			if (c == '=')
			{
				buf.push_back(c);
				j = look(buf, Razdelitel);
				cout << "razdelitel: " << buf << endl;
				return Lex((type_of_lex)(j + (int)LEX_FIN), j);
			}
			else
			{
				ungetc(c, fp);
				j = look(buf, Razdelitel);
				cout << "razdelitel: " << buf << endl;
				return Lex((type_of_lex)(j + (int)LEX_FIN), j);
			}
			break;

		case NEQ:
			if (c == '=')
			{
				buf.push_back(c);
				cout << "razdelitel: " << buf << endl;
				return Lex(LEX_NEQ, look(buf, Razdelitel));
			}
			else
				throw(int)('!');
			break;
		} // end switch
	} while (true);
}

int main(int argc, char *argv[])
{
	Scanner s("prog.ml");
	Lex L;
	try
	{
		for (int i = 0; i < 100; ++i)
		{
			L = s.get_lex();
			if (L.get_type() == LEX_FIN)
			{
				cout << "end of file\n";
				break;
			}
		}
	}
	catch (int c)
	{
		cout << "err on char: " << (char)c << endl;
	}
	cout << TSTRUCT_ID[0].get_name() << ' ' << TID[0].get_name() << ' ' << TID[1].get_name() << ' ' << TID[2].get_name() << endl;
}
