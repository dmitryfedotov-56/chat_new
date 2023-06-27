	#pragma once
	/***********************************************************/
	/*                  Chat Access Definition                 */
	/***********************************************************/
	
	#include <time.h>

	#define ID			size_t			// object ID

	/***********************************************************/

	class Message_Collection_Access {
	public:
		virtual bool has_Next() = 0;
		virtual const char* message_Text() const = 0;
		virtual const time_t message_Time() const = 0;
		virtual bool message_Sent() const = 0;
		virtual bool message_Received() const = 0;
		virtual const char* sender_Name() const = 0;
		virtual const char* recipient_Name() const = 0;
		virtual ID message_ID() const = 0;

		virtual ~Message_Collection_Access() {};
	};

	/***********************************************************/

	struct Condition
	{
		bool sent = false;				// include sent messages only
		bool received = false;			// include received messages only
		unsigned number = 0;			// number of messages, 0 - no restriction
		const char* user = nullptr;		// user name
	};

	/***********************************************************/

	enum class Order { direct, reverse };

	class Session_Access {
	public:

		virtual const char* user_Name() const = 0;

		virtual void send_Message(const char* recipient, const char* text) = 0;
/*
			send a message to some particular recipient
*/
		virtual void broadcast_Message(const char* text) = 0;	
/*
			broadcast a message to all users
*/
		virtual Message_Collection_Access* select_Messages(const Condition* condition,
			Order order) = 0;
/*
			select messages by condition
*/
		virtual void change_Password(const char* password) = 0;
/*
			change user password
*/
		virtual ~Session_Access() {};
	};

	/***********************************************************/

	class Chat_Access {
	public:

		virtual Session_Access* create_User(const char* name, const char* password) = 0;
/*
			create a user and start user session
*/
		virtual Session_Access* start_Session(const char* name, const char* password) = 0;
/*
			start user session
*/
 		virtual ~Chat_Access(){};
	};

	/***********************************************************/

	Chat_Access* open_Chat();

	void close_Chat();
	
	/***********************************************************/
/*
	list of errors
*/
	#define USER_NOT_FOUND	1
	#define ALREADY_EXISTS	2
	#define WRONG_PASSWORD	3

	/***********************************************************/


