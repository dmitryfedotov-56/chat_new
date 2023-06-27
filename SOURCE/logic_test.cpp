
	/***********************************************************/

	#define _CRT_SECURE_NO_WARNINGS

	#include <iostream>
	using namespace std;

	#include <time.h>
	#include <stdlib.h>
	#include <string>

	// #include "chat_class.h"
	// #include "chat_object.h"
	#include "chat_access.h"
	

	/***********************************************************/

	void success() { cout << " выполнено" << endl;  };

	void error_Message(int code) 
	{
		switch (code) 
		{
		case USER_NOT_FOUND: cout << " пользователь не найден";
			break;
		case ALREADY_EXISTS: cout << " пользователь уже существует";
			break;
		case WRONG_PASSWORD: cout << " неверный пароль";
			break;
		default:;
			cout << " ошибка доступа";
		};
		cout << endl;
	};

	/***********************************************************/

	void show_Bar()
	{
		cout << " ";
		for (unsigned i = 0; i < 80; i++)cout << "-";
		cout << endl;
	};

	/***********************************************************/

	void show_Message(Message_Collection_Access* message) 
	{
		char s[40];
		struct tm* u;

		const time_t time = message->message_Time();	// message time
		u = localtime(&time);
		strftime(s, 40, "%d.%m.%Y %H:%M:%S", u);
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

	void show_Messages(Session_Access* session) 
	{
		bool empty = true;
		Message_Collection_Access* collection = session->select_Message();
		// cout << " список сообщений" << endl;
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
		if (empty) cout << "сообщения отсутствуют" << endl;
	};

	/***********************************************************/

	Chat_Access* chat;
	Session_Access* session1 = nullptr;
	Session_Access* session2 = nullptr;
	Session_Access* session3 = nullptr;

	/***********************************************************/

	void show_Users() 
	{
		cout << endl;
		cout << " сообщения user1" << endl;
		show_Messages(session1);
		cout << endl;
		cout << " сообщения user2" << endl;
		show_Messages(session2);
		cout << endl;
		cout << " сообщения user3" << endl;
		show_Messages(session3);
	};

	/***********************************************************/

	int main() { 

		setlocale(LC_ALL, "");

		chat = open_Chat();

		try 
		{
			chat->start_Session("user1", "passx");
			success();
		}
		catch (int code) { error_Message(code); };

		try {
			session1 = chat->create_User("user1", "pass1");
			session2 = chat->create_User("user2", "pass2");
			session3 = chat->create_User("user3", "pass3");

			session3->broadcast_Message("третий-всем");
			session1->send_Message("user2", "первый-второму");
			session2->send_Message("user1", "второй-первому");
	
			show_Users();

			session1->send_Message("user2", "снова первый-второму");
			session2->send_Message("user1", "снова второй-первому");

			show_Users();
		}
		catch (int code) { error_Message(code); };

		close_Chat();

		return 0;
	}

	/***********************************************************/
