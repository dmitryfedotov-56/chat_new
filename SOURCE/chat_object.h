	#pragma once
	/***********************************************************/
	/*            Hash Table based Chat Object Access          */
	/***********************************************************/

	#include <time.h>
	#include <cstdint>

	#define ID			size_t			// object ID
	#define NOTANID		SIZE_MAX		// reserved ID

	class Object_Block {
	public:
		virtual void open(ID id) = 0;
		virtual void save() = 0;
		virtual ID create() = 0;

		virtual ~Object_Block() {};
	};

	/***********************************************************/

	class Object_Class { 
	public:
		virtual void open(ID id);				// open object

		virtual void save();					// save object

		Object_Class(Object_Block* block);

		virtual ~Object_Class() {};

	protected:
		Object_Block* object_block = nullptr;
	};

	/***********************************************************/
	/*                     Object List Access                  */
	/***********************************************************/

	class List_Access : public Object_Class {
	public:

		virtual ID create() = 0;		// create list object

		virtual unsigned Size() = 0;	// list size

		virtual ID init_Element() = 0;
/*
			init list element, returns NOTANID if the list is empty
*/
		virtual ID last_Element() = 0;
/*
			last list element, returns NOTANID if the list is empty
*/
		virtual ID next_Element() = 0;
/*
			next list element, can return NOTANID
*/
		virtual ID prev_Element() = 0;
/*
			previous list element, can return NOTANID
*/
		virtual void include_Element(ID element) = 0;
/*
			include element to the beginning
*/
		// virtual void exclude_Element(ID element) = 0;
/*
			exclude element from the list
*/
		List_Access(Object_Block* block);

		virtual ~List_Access() {};
	};

	/***********************************************************/

	class User_Access : public Object_Class {
	public:
		virtual ID create(const char* name, const char* pass) = 0;	// create a user

		virtual const char* allocate_Name() const = 0;				// user name
		virtual bool valid_Password(const char* pass) const = 0;	// user password
		virtual ID message_Link_List() const = 0;					// message link list ID
		virtual ID group_List() const = 0;							// group list

		virtual void change_Password(const char* pass) = 0;			// change password
		
		User_Access(Object_Block* block);

		virtual ~User_Access() {};
	};

	/***********************************************************/

	class User_List_Access : public Object_Class 
	{
	public:
		virtual ID find_User(const char* name) = 0;
		virtual ID init_User(unsigned* nptr) = 0;
		virtual ID next_User(unsigned* nptr) = 0;
		virtual void new_User(ID user_id, const char* name) = 0;
		User_List_Access(Object_Block* block);
		virtual ~User_List_Access() {};
	};

	/***********************************************************/

	class Message_Link_Access : public Object_Class {
	public:
		virtual ID create(bool sent, bool received, ID message_id) = 0;	// create a link

		virtual bool message_Sent() const = 0;				// message is sent
		virtual bool message_Received() const = 0;			// message is received
		virtual ID message_ID() const = 0;					// message ID

		Message_Link_Access(Object_Block* block);

		virtual ~Message_Link_Access() {};
	};

	/***********************************************************/

	class Message_Access : public Object_Class {
	public:
		virtual ID create(const char* text, ID sender_id, ID recipient_id, ID group_id) = 0; // create

		virtual const char* allocate_Text() const = 0;		// message text
		virtual time_t message_Time() const = 0;			// message timestamp
		virtual ID sender_ID() const = 0;					// message sender ID
		virtual ID recipient_ID() const = 0;				// message recipient ID
		virtual ID group_ID() const = 0;					// message group id
		
		Message_Access(Object_Block* block);

		virtual ~Message_Access() {};
	};

	/***********************************************************/

	class Group_Access : public Object_Class {
	public:
		virtual ID create(const char* name) = 0;			// create group

		virtual const char* allocate_Name() const = 0;		// group name
		virtual ID user_List_ID() const = 0;				// group user list

		~Group_Access() {};

		Group_Access(Object_Block* block);
	};

	/***********************************************************/

	class Header_Access : public Object_Class {
	public:
		virtual ID create() = 0;							// create header
		virtual ID user_List_ID() const = 0;				// user list id
		virtual ID group_List_ID() const = 0;				// group list id

		~Header_Access() {};

		Header_Access(Object_Block* block);
	};

	/***********************************************************/

	class Chat_Class_Access {
	public:
		virtual User_Access* create_User_Access() = 0;
		virtual Message_Access* create_Message_Access() = 0;
		virtual Message_Link_Access* create_Message_Link_Access() = 0;
		virtual Group_Access* create_Group_Access() = 0;
		virtual Header_Access* create_Header_Access() = 0;
		virtual List_Access* create_List_Access() = 0;
		virtual User_List_Access* create_User_List_Access() = 0;
		virtual ID user_List_ID() = 0;
		virtual void close_Chat_Class() = 0;

		virtual ~Chat_Class_Access() {};
	};

	Chat_Class_Access* get_Chat_Class();

	/***********************************************************/

	


