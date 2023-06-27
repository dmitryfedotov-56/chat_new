	/***********************************************************/
	/*                   Chat Implementation                   */
	/***********************************************************/

	#include <stdlib.h>
	#include <string>
	#include "chat_class.h"
	#include "chat_access.h"
	#include "chat_object.h"

	using namespace std;

	#define ALL "chat"

	Chat_Class_Access* chat_class = nullptr;
	
	/***********************************************************/
	/*                Chat Class Implementation                */
	/***********************************************************/

	void Chat::open_Chat(ID id) 
	{
		user_list_id = id;
		user = chat_class->create_User_Access();
		user_list = chat_class->create_User_List_Access();
		user_list->open(user_list_id);
	};

	/***********************************************************/

	void Chat::close_Chat() 
	{
		delete user;
		delete user_list;
		chat_class->close_Chat_Class();
	};

	/***********************************************************/

	ID Chat::new_User(const char* name, const char* password) 
	{
		ID user_id = user->create(name, password);
		user_list->new_User(user_id, name);
		return user_id;
	};

	/***********************************************************/

	ID Chat::user_ID(const char* name)	// find user by name 
	{
	
		return user_list->find_User(name);
	};

	/***********************************************************/

	ID Chat::find_ID(const char* name) 
	{
		ID user_id = user_ID(name);
		if (user_id == NOTANID) 
		{
			throw USER_NOT_FOUND;
		};
		return user_id;
	};

	/***********************************************************/

	static Chat chat;

	Chat_Access* open_Chat()
	{
		if (chat_class == nullptr)chat_class = get_Chat_Class();	// check object layer
		ID user_list_id = chat_class->user_List_ID();				// user list id
		chat.open_Chat(user_list_id);								// open chat
		return &chat;
	};

	/***********************************************************/

	void close_Chat() 
	{ 
		chat.close_Chat(); 
	};

	/***********************************************************/

	Session_Access* Chat::start_Session(const char* name, const char* password)
	{
		ID user_id = find_ID(name);	// in case of failure throws exception 

		user->open(user_id);		// open object

		if( !(user->valid_Password(password)))
		{
			throw WRONG_PASSWORD;
		};

		Session* session = new Session(this, user_id);		// create session

		try {												// try to open
			session->open();								
		}
		catch (int code)									// in case of failure
		{
			delete session;									// delete session
			throw code;
		};
		return session;
	};

	/***********************************************************/

	Session_Access* Chat::create_User(const char* name, const char* password)
	{
		if (strcmp(name, ALL) == 0)	// special user?
		{
			throw ALREADY_EXISTS;
			// return nullptr;
		};

		ID user_id = user_ID(name);
		if (user_id != NOTANID)
		{
			throw ALREADY_EXISTS;
			// return nullptr;
		};
		user_id = new_User(name, password);
		Session* session = new Session(this, user_id);
		session->open();
		return session;
	};

	/***********************************************************/
	/*                     Session Implementation              */
	/***********************************************************/

	Session::Session(Chat* _chat, ID _user_id)
	{
		chat = _chat;
		user_id = _user_id;
		user = chat_class->create_User_Access();
		peer = chat_class->create_User_Access();
		message = chat_class->create_Message_Access();
		message_link = chat_class->create_Message_Link_Access();
		message_link_list = chat_class->create_List_Access();
	};

	/***********************************************************/

	Session::~Session()
	{
		delete user;
		delete peer;
		delete message;
		delete message_link;
		delete message_link_list;
		free((void*)user_name);
	};

	/***********************************************************/

	void Session::open() 
	{
		user->open(user_id); 						// open user access 
		list_id = user->message_Link_List();		// message link list id
		user_name = user->allocate_Name();			// session user name
	};

	/***********************************************************/

	const char* Session::user_Name() const { return user_name; };

	/***********************************************************/

	void Session::to_User_List(ID message_id)
	{
		ID link_id = message_link->create(true, false, message_id);	// create link
		message_link_list->open(list_id);							// open own list
		message_link_list->include_Element(link_id);				
	};

	/***********************************************************/

	void Session::to_Peer_List(ID message_id, ID peer_id)
	{
		peer->open(peer_id);										// open user
		ID list_id = peer->message_Link_List();						// user list id
		ID link_id = message_link->create(false, true, message_id);	// create link
		message_link_list->open(list_id);							// open peer list
		message_link_list->include_Element(link_id);				// include link
	};

	/***********************************************************/
/*
	first of all, we will find the recipient, in case of an error we do nothing at all
*/
	void Session::send_Message(const char* recipient, const char* text) 
	{
		if (strcmp(recipient, ALL) == 0) 
		{
			broadcast_Message(text);
			return;
		};
		ID peer_id = chat->find_ID(recipient);						// find the recipient
		ID message_id = message->create(text, user_id, peer_id, NOTANID);	// create message
		to_User_List(message_id);									// to user list
		to_Peer_List(message_id, peer_id);							// to peer list
	};

	/***********************************************************/

	void Session::broadcast_Message(const char* text) 
	{
		unsigned n;
		ID peer_id;
		ID message_id;
		message_id = message->create(text, user_id, NOTANID, NOTANID);	// create message
		to_User_List(message_id);										// to user list
		peer_id = chat->user_list->init_User(&n);					
		while (peer_id != NOTANID)										// for all peers
		{																
			if (peer_id != user_id)										// but not to the user
			{ 
				to_Peer_List(message_id, peer_id);						// to peer list
			};	
			peer_id = chat->user_list->next_User(&n);
		};
	};

	/***********************************************************/

	Message_Collection_Access* Session::select_Messages(const Condition* condition, 
		Order order) 
	{ 
		Message_Collection* collection = nullptr;
		collection = new Message_Collection(this, list_id, condition, order);

		try {						// try to open
			collection->open(); 
		}
		catch (int code)			// in case if failure 
		{
			delete collection;		// delete collection
			throw code;
		};
		return collection;
	};

	/***********************************************************/

	void Session::change_Password(const char* password) 
	{
		user->change_Password(password);
	};

	/***********************************************************/
	/*              Message Collection Implementation          */
	/***********************************************************/

	const char* Message_Collection::message_Text() const { return text; };

	const time_t Message_Collection::message_Time() const { return time; };

	bool Message_Collection::message_Sent() const { return sent; };

	bool Message_Collection::message_Received() const { return received; };

	/***********************************************************/

	const char* Message_Collection::get_Name(const char* name, ID id)const
	{
		if (id == NOTANID)return ALL;
		return name;
	};

	/***********************************************************/

	const char* Message_Collection::sender_Name() const 
	{ 
		const char* name = nullptr;
		if (sent) name = session->user_Name();
		if (received) name = peer_name;
		return name;
	};

	/***********************************************************/

	const char* Message_Collection::recipient_Name() const 
	{ 
		const char* name = nullptr;
		if (sent) name = get_Name(peer_name, recipient_id);
		if (received)name = get_Name(session->user_Name(), recipient_id);
		return name;
	};

	/***********************************************************/

	ID Message_Collection::message_ID() const { return message_id; };

	/***********************************************************/
/*
	message a collection shares access objects with it's session
*/
	Message_Collection::Message_Collection(Session* session, 
		ID id, 
		const Condition* condition,
		Order order)
	{
		list_id = id;
		this->order = order;
		this->condition = condition;
		this->session = session;
/*
		access objects
*/
		this->peer = session->user;
		this->message = session->message;
		this->message_link = session->message_link;
		this->message_link_list = session->message_link_list;
	};

	/***********************************************************/

	Message_Collection::~Message_Collection() {	clear(); };

	/***********************************************************/

	void Message_Collection::open() 
	{
		message_link_list->open(list_id);
		message_id = message_link_list->init_Element();
	};

	/***********************************************************/

	ID Message_Collection::get_Init() 
	{
		switch (order)		// depending on the order
		{
		case Order::direct:
			return message_link_list->init_Element();	// init message
			break;
		case Order::reverse:
			return message_link_list->last_Element();	// last message
			break;
		};
		return NOTANID;
	};

	/***********************************************************/

	ID Message_Collection::get_Next() 
	{
		switch (order)		// depending on the order
		{
		case Order::direct:
			return message_link_list->next_Element();	// next message
			break;
		case Order::reverse:
			return message_link_list->prev_Element();	// prev message
			break;
		};
		return NOTANID;
	};

	/***********************************************************/

	void Message_Collection::clear() 
	{
		free((void*)text);
		text = nullptr;
		free((void*)peer_name);
		peer_name = nullptr;
	};

	/***********************************************************/

	void Message_Collection::open_Message(ID message_link_id)
	{
		ID peer_id = NOTANID;

		message_link->open(message_link_id);			// open message link object
		sent = message_link->message_Sent();			// sent
		received = message_link->message_Received();	// received
		message_id = message_link->message_ID();		// get message id	
		message->open(message_id);						// open message object
		sender_id = message->sender_ID();				// sender id
		recipient_id = message->recipient_ID();			// recipient_id

		text = message->allocate_Text();				// message text
		time = message->message_Time();					// message time

		if (sent)peer_id = recipient_id;				// if sent peer is the recipient
		if (received) peer_id = sender_id;				// if received peer is the sebder

		if (peer_id != NOTANID)							// if peer_id available
		{
			peer->open(peer_id);						// open peer
			peer_name = peer->allocate_Name();			// peer name
		};
	};

	/***********************************************************/

	bool Message_Collection::suitable_Message() 
	{
		if (condition->sent)if (!sent)return false;

		if (condition->received)if (!received)return false;

		if (condition->user != nullptr)						// user filter?
		{
			if (received)
			{
				if (strcmp(condition->user, this->sender_Name()) != 0)return false;
			};

			if (sent) 
			{
				if (strcmp(condition->user, recipient_Name()) != 0)return false;
			};
		};

		return true;
	};

	/***********************************************************/

	bool Message_Collection::has_Next()
	{
		// ID peer_id = NOTANID;
		ID message_link_id = NOTANID;
		// ID sender_id = NOTANID;
		// ID recipient_id = NOTANID;

		if (!started)						// not started? 
		{
			message_link_id = get_Init();	// get initial message
			started = true;					// it is started
		}
		else 
		{
			message_link_id = get_Next();	// get next message
		};

		if (message_link_id == NOTANID) 	// no message ?
		{
			clear();
			return false;
		};

		do	// look for a suitable message					
		{
			clear();									// cltar data
			open_Message(message_link_id);				// open message
			if(suitable_Message()) return true;			// is it suitable?
			message_link_id = get_Next();				// get the next one
		} while (message_link_id != NOTANID);			// while message is available
	
		clear();
		return false;
	};

	/***********************************************************/


