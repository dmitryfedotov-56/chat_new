	/***********************************************************/
	/*                     Chat Interface                      */
	/***********************************************************/

	#define _CRT_SECURE_NO_WARNINGS

	#include <iostream>
	using namespace std;

	#include <time.h>
	#include <conio.h>
	#include <stdlib.h>
	#include <string>

	#include "chat_access.h"

	/***********************************************************/
	/*
		locale data
	*/
	#define NOT_FOUND		"пользователь не найден"
	#define USER_EXISTS		"пользователь уже существует"
	#define WRONG_PASS		"неверный пароль"
	#define ACCESS_ERROR	"ошибка доступа"

	#define OPERATION		"операция"
	#define SIGN_IN			"вход"
	#define SIGN_UP			"регистрация пользователя"
	#define EXIT_APP		"выход из приложения"
	#define USERNAME		"имя"
	#define PASSWORD		"пароль"

	#define CHANGEPASS		"изменение пароля"
	#define PASSCHANGED		"пароль изменен" 
	#define ENDSESSION		"завершение сессии"
	#define SENDMESSAGE		"отправить сообщение"
	#define BROADCAST		"отправить всем"
	#define ALLMESSAGES		"все сообщения"
	#define SENTMESSAGES	"отправленные сообщения"
	#define RCVDMESSAGES	"полученные сообщения"
	#define CHATUSER		"пользователь (Enter - все)"
	#define NMESSAGE		"число сообщений (Enter - все)"
	#define MESSTEXT		"текст"
	#define ALLOFTHEM		"все"
	#define NOMESSAGES		"сообщения отсутствуют"


	/***********************************************************/

	Chat_Access* chat = nullptr;
	Session_Access* session = nullptr;

	/***********************************************************/

	void error_Message(int code)
	{
		switch (code)
		{
		case USER_NOT_FOUND: cout << " " << NOT_FOUND;
			break;
		case ALREADY_EXISTS: cout << " " << USER_EXISTS;
			break;
		case WRONG_PASSWORD: cout << " " << WRONG_PASS;
			break;
		default:;
			cout << " " << ACCESS_ERROR;
		};
		cout << endl;
	};

	/***********************************************************/

	void show_Bar()
	{
		// cout << " ";
		for (unsigned i = 0; i < 80; i++)cout << "-";
		cout << endl;
	};

	/***********************************************************/
	/*                  Session Operations                     */
	/***********************************************************/

	char get_Session_Operation()
	{
		show_Bar();
		cout << " 0 : " << ENDSESSION << endl;
		cout << " 1 : " << SENDMESSAGE << endl;
		cout << " 2 : " << BROADCAST << endl;
		cout << " 3 : " << ALLMESSAGES << endl;
		cout << " 4 : " << RCVDMESSAGES << endl;
		cout << " 5 : " << SENTMESSAGES << endl;
		cout << " 6 : " << CHANGEPASS << endl;

		show_Bar();

		cout << " " << session->user_Name() << " " << OPERATION << " : ";

		char c;
		while (true)	// get valid input
		{
			c = _getch();
			switch (c)
			{
			case '0':
				cout << ENDSESSION << endl;
				return c;
			case '1':
				cout << SENDMESSAGE << endl;
				return c;
			case '2':
				cout << BROADCAST << endl;
				return c;
			case '3':
				cout << ALLMESSAGES << endl;
				return c;
			case '4':
				cout << RCVDMESSAGES << endl;
				return c;
			case '5':
				cout << SENTMESSAGES << endl;
				return c;
			case '6':
				cout << CHANGEPASS << endl;
				return c;
			};
		};
	};

	/***********************************************************/

	void send_Message() 
	{
		string user;
		string text;
		cout << " " << CHATUSER << ": ";
		getline(cin, user);
		cout << " " << MESSTEXT << ": ";
		getline(cin, text);
	
		try {
			session->send_Message(user.c_str(), text.c_str());
		}
		catch (int code) { error_Message(code); };
	};

	/***********************************************************/

	void broadcast_Message() 
	{
		string text;
		cout << " " << MESSTEXT << ": ";
		getline(cin, text);
		try {
			session->broadcast_Message(text.c_str());
		}
		catch (int code) { error_Message(code); };
	};

	/***********************************************************/

	#define BUFFSIZE	40

	void show_Message(Message_Collection_Access* message)
	{

		struct tm* local;
		char s[BUFFSIZE];
		const time_t time = message->message_Time();	// message time
		local = localtime(&time);
		strftime(s, BUFFSIZE, "%d.%m.%Y %H:%M:%S", local);
		cout << " " << s << " ";

		if (message->message_Sent())					// sender and recipient
		{
			cout << " ";
			cout << message->sender_Name();
			cout << " -> ";
			cout << message->recipient_Name();
		};

		if (message->message_Received())				//  recipient and sender
		{
			cout << " ";
			cout << message->recipient_Name();
			cout << " <- ";
			cout << message->sender_Name();
		};

		cout << endl;

		cout << " ";
		cout << message->message_Text() << endl;		// message text
		show_Bar();
	};

	/***********************************************************/

	void show_Messages(const Condition* condition)
	{
		bool empty = true;
		Message_Collection_Access* collection = nullptr;
		collection = session->select_Messages(condition, Order::reverse);
		// cout << " " << ALLMESSAGES << endl;
		while (collection->has_Next())
		{
			if (empty)
			{
				show_Bar();
				empty = false;
			};
			show_Message(collection);
		};
		delete collection;
		if (empty) cout << " " << NOMESSAGES << endl;
	};

	/***********************************************************/

#define BS	8
#define CR	13

	unsigned get_line(char* s)
	{
		char c = 0;
		unsigned n = 0;
		while (n < BUFFSIZE - 1)
		{
			c = _getch();
			switch (c)
			{
			case 13:			// CR - end line
				s[n] = 0;
				return n;
			case BS:			// backspace - one symbol less
				if (n != 0) {
					_putch(BS);
					_putch(' ');
					_putch(BS);
					n--;
				};
				break;
			default:			// default - one more symbol
				_putch(c);
				s[n] = c;
				n++;
			};
		};
		s[n] = 0;
		return n;
	};
	
	/***********************************************************/

	void show_Messages(bool sent, bool received) 
	{
		unsigned n;
		Condition condition;
		char username[BUFFSIZE];

		condition.sent = sent;
		condition.received = received;

		cout << " " << CHATUSER << ": ";		// get user
		n = get_line(username);
		if (n == 0)
		{
			condition.user = nullptr;			// all users
			cout << ALLOFTHEM << endl;
		}
		else 
		{
			cout << endl;
			condition.user = username;			// some particular user
		};

		show_Messages(&condition);
	};

	/***********************************************************/

	void change_Password() 
	{
		string password;
		cout << " " << PASSWORD << ": ";
		getline(cin, password);
		session->change_Password(password.c_str());
		try {
			session->change_Password(password.c_str());
		}
		catch (int code) { error_Message(code); };
		cout << " " << PASSCHANGED << endl;
	};

	/***********************************************************/

	void start_User_Session(Session_Access* session) 
	{
		char c;
		while (true)
		{
			c = get_Session_Operation();
 			switch (c)
			{
			case '0':
				delete session;
				return;
			case '1':
				send_Message();					// message 
				break;
			case '2':
				broadcast_Message();
				break;
			case '3':
				show_Messages(false, false);	// all messages
				break;
			case '4':
				show_Messages(false, true);		// received mesages only
				break;
			case '5':
				show_Messages(true, false);		// sent messages only
				break;
			case '6':
				change_Password();
				break;
			};
		};
	};
	
	/***********************************************************/
	/*                    Сhat Operations                      */
	/***********************************************************/

	char get_Chat_Operation()
	{
		show_Bar();
		cout << " 0 : " << EXIT_APP << endl;
		cout << " 1 : " << SIGN_IN << endl;
		cout << " 2 : " << SIGN_UP << endl;
		show_Bar();

		cout << " " << OPERATION << " : ";

		char c;
		while (true)	// get valid input
		{
			c = _getch();
			switch (c)
			{
			case '0':
				cout << EXIT_APP << endl;
				return c;
			case '1':
				cout << SIGN_IN << endl;
				return c;
			case '2':
				cout << SIGN_UP << endl;
				return c;
			};
		};
	};

	/***********************************************************/

	void get_User_Data(string* username, string* password)
	{
		cout << " " << USERNAME << ": ";
		getline(cin, *username);
		cout << " " << PASSWORD << ": ";
		getline(cin, *password);
	};

	/***********************************************************/

	void sign_In() 
	{
		string username;
		string password;
		get_User_Data(&username, &password);
		try	{
			session = chat->start_Session(username.c_str(), password.c_str());
			start_User_Session(session);
		}catch (int code) { error_Message(code); };
	};

	/***********************************************************/

	void sign_Up()
	{
		string username;
		string password;
		get_User_Data(&username, &password);
		try	{
			session = chat->create_User(username.c_str(), password.c_str());
			start_User_Session(session);
		}catch (int code) { error_Message(code); };
	};

	/***********************************************************/

	void start_Chat() 
	{
		char c;
		while (true)
		{
			c = get_Chat_Operation();
			switch (c)
			{
			case '0': 
				return;
			case '1': sign_In();
				break;
			case '2': sign_Up();
				break;
			};
		};
	};

	/***********************************************************/
	/*                         Debugging                       */
	/***********************************************************/

	void test_Messages() 
	{
		Session_Access* session1 = nullptr;
		Session_Access* session2 = nullptr;
		Session_Access* session3 = nullptr;

		try {
			session1 = chat->create_User("user1", "pass1");
			session2 = chat->create_User("user2", "pass2");
			session3 = chat->create_User("user3", "pass3");

			session1->broadcast_Message("первый-всем");
			session2->broadcast_Message("второй-всем");
			session3->broadcast_Message("третий-всем");

			session1->send_Message("user2", "первый-второму");
			session2->send_Message("user1", "второй-первому");

			session3->broadcast_Message("снова третий-всем");

			session1->send_Message("user2", "снова первый-второму");
			session2->send_Message("user1", "снова второй-первому");

			session1->send_Message("user3", "первый-третьему");
			session2->send_Message("user3", "второй-третьему");

		}
		catch (int code) 
		{ 
			error_Message(code); 
			delete session1;
			delete session2;
			delete session3;
		};

		delete session1;
		delete session2;
		delete session3;
	};

	/***********************************************************/

	void open_Session(const char* name, const char* pass) 
	{
		Condition condition;
		session = chat->start_Session(name, pass);
		show_Messages(&condition);
		delete session;
	};

	/***********************************************************/

	void create_User(const char* name, const char* pass) 
	{
		Session_Access* session;
		session = chat->create_User(name, pass);
		session->broadcast_Message(name);
		delete session;
	};

	/***********************************************************/

	int main() 
	{
		// setlocale(LC_ALL, "");
		system("chcp 1251");

		try {		// generally speakig we can have errors here

			chat = open_Chat();
			// test_Messages();
		}
		catch (int code) 
		{
			error_Message(code);
			exit(0);
		}

		start_Chat();
		close_Chat();
		return 0;
	};


	/***********************************************************/



