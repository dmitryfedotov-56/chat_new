	/***********************************************************/
	/*       Hash Table based Chat Object Implementation      */
	/***********************************************************/

	#include <string.h>
	#include "chat_object.h"
	#include "storage_access.h"
	#include "sha1.h"

	#include "hash_table.h"

	#define DEBUG	0

	#if DEBUG
		#include <iostream>
		using namespace std;
	#endif

	static Storage_Access* storage = nullptr;

	/***********************************************************/
	/*                        Object Class                     */
	/***********************************************************/

	void Object_Class::open(ID id) { object_block->open(id); };

	void Object_Class::save() { object_block->save(); };

	Object_Class::Object_Class(Object_Block* block):object_block(block) {};

	List_Access::List_Access(Object_Block* block) : Object_Class(block) {};

	User_Access::User_Access(Object_Block* block) : Object_Class(block) {};

	User_List_Access::User_List_Access(Object_Block* block) : Object_Class(block) {};

	Message_Access::Message_Access(Object_Block* block) : Object_Class(block) {};

	Message_Link_Access::Message_Link_Access(Object_Block* block) : Object_Class(block) {};

	Group_Access::Group_Access(Object_Block* block) : Object_Class(block) {};

	Header_Access::Header_Access(Object_Block* block) : Object_Class(block) {};
	
	/***********************************************************/
	/*                        Block Template                   */
	/***********************************************************/

	template <typename Data>
	class Block_Template : public Object_Block {
	public:

		virtual void open(ID id) 
		{
			object_id = id;
			storage->get_Data(object_id, sizeof(Data), &data);
		};

		virtual void save() 
		{ 
			storage->put_Data(object_id, sizeof(Data), &data);
		};

		virtual ID create() 
		{ 
			object_id = storage-> new_Data(sizeof(Data), &data); 
			return object_id;
		};

		ID object_id = NOTANID;
		Data data;
	};

	/***********************************************************/
	/*                        Link Class                       */
	/***********************************************************/
/*
	Supplementary class used by List_Class
*/
	class Link_Data {
	public:
		ID next_id = NOTANID;		// next element id
		ID elem_id = NOTANID;		// element id
		ID prev_id = NOTANID;		// prev element id
	};

	/***********************************************************/

	class Link_Class : public Object_Class
	{
	public:
		ID create(ID init_id, ID element_id)
		{
			block.data.prev_id = NOTANID;		// no previous
			block.data.next_id = init_id;		// next link
			block.data.elem_id = element_id;	// element id
			ID id = block.create();
#if DEBUG
			cout << " link created id = " << id << endl;
			cout << " prev = " << block.data.prev_id << endl;
			cout << " elem = " << block.data.elem_id << endl;
			cout << " next = " << block.data.next_id << endl;
			cout << endl;
#endif
			return id;
		};

		Block_Template<Link_Data> block;
		Link_Class() : Object_Class(&block) {};
	};

	/***********************************************************/
	/*                        List Class                       */
	/***********************************************************/

	class List_Data {
	public:
		ID init_id = NOTANID;		// init element id
		ID last_id = NOTANID;		// last element id
		size_t size = 0;			// number of elements
	};

	/***********************************************************/

	class List_Class : public List_Access {
	public:

		virtual ID create() override;	// create list object

		virtual unsigned Size() override;

		virtual ID init_Element() override;
/*
			init list element, returns NOTANID if the list is empty
*/
		virtual ID last_Element() override;
/*
			last list element, returns NOTANID if the list is empty
*/
		virtual ID next_Element() override;
/*
			next list element, can return NOTANID
*/
		virtual ID prev_Element() override;
/*
			previous list element can return NOTANID
*/
		virtual void include_Element(ID element) override;
/*
			include element to the beginning
*/
		// virtual void exclude_Element(ID element) ;
/*
			exclude element from the list
*/
		List_Class() :List_Access(&list) {};

	private:
		ID link_id = NOTANID;
		Link_Class link;
		Block_Template<List_Data> list;
	};

	/***********************************************************/

	ID List_Class::create()
	{
		ID id = list.create();
#if DEBUG
		cout << " list created id = " << id << endl;
		cout << " size = " << list.data.size << endl;
		cout << " init = " << list.data.init_id << endl;
		cout << " last = " << list.data.last_id << endl;
		cout << endl;
#endif
		return id;
	};

	/***********************************************************/

	unsigned List_Class::Size() { return list.data.size; };

	/***********************************************************/

	ID List_Class::init_Element()
	{
		if (list.data.init_id == NOTANID)return NOTANID;	// list is empty?
		link_id = list.data.init_id;						// link_id is init_id
		link.open(link_id);									// open link
		return link.block.data.elem_id;						// return element id
	};

	/***********************************************************/

	ID List_Class::last_Element()
	{
		if (list.data.last_id == NOTANID)return NOTANID;	// list is empty?
		link_id = list.data.last_id;						// link_id is last_id
		link.open(link_id);									// open link
		return link.block.data.elem_id;						// return element id
	};

	/***********************************************************/

	ID List_Class::next_Element()
	{
		link_id = link.block.data.next_id;					// link_id is next_id
		if (link_id == NOTANID)return NOTANID;				// no link_id?
		link.open(link_id);									// open link				
		return link.block.data.elem_id;						// return element id
	};

	/***********************************************************/

	ID List_Class::prev_Element()
	{
		link_id = link.block.data.prev_id;					// link_id is prev_id
		if (link_id == NOTANID)return NOTANID;				// no link_id?
		link.open(link_id);									// open link				
		return link.block.data.elem_id;						// return element id
	};

	/***********************************************************/

	void List_Class::include_Element(ID element_id)
	{
		link_id = link.create(list.data.init_id, element_id);

		// create new link containing the element id

		if(list.data.init_id != NOTANID )			// init elment avaialable ?
		{
			link.open(list.data.init_id);			// open init element
			link.block.data.prev_id = link_id;		// update previous
			link.save();							// save element block
		};
		list.data.init_id = link_id;				// init list element
		if (list.data.size == 0) {					// if list is empty
			list.data.last_id = link_id;			// it is also the last element
		};
		list.data.size++;							// update list size
		list.save();								// save list block
	};

	/***********************************************************/
	/*                         User Class                      */
	/***********************************************************/

	class User_Data {
	public:
		ID name_id = NOTANID;			// name block id
		ID group_list_id = NOTANID;		// group list id
		ID message_list_id = NOTANID;	// message list object id
		Digest digest;					// password digest 
	};

	/***********************************************************/

	class User_Class : public User_Access {
	public:
		virtual ID create(const char* name, const char* pass) override;	// create a user

		virtual const char* allocate_Name() const override;			// user name
		virtual bool valid_Password(const char* pass) const;		// check password
		virtual ID message_Link_List() const override;				// message link list ID
		virtual ID group_List() const override;						// group list

		virtual void change_Password(const char* pass) override;	// change password

		User_Class():User_Access(&user){};

	private:
		Block_Template<User_Data> user;
	};

	/***********************************************************/

	const char* User_Class::allocate_Name() const 
	{
		return storage-> allocate_Line(user.data.name_id);
	};

	bool User_Class::valid_Password(const char* pass) const
	{
		Digest digest;
		sha1(pass, strlen(pass), digest);
		for (unsigned i = 0; i < 5; i++)
			if (user.data.digest[i] != digest[i])return false;
		return true;
	};

	ID User_Class::group_List() const { return user.data.group_list_id;	};

	ID User_Class::message_Link_List() const { return user.data.message_list_id; };

	/***********************************************************/

	void User_Class::change_Password(const char* pass) 
	{
		sha1(pass, strlen(pass), user.data.digest);
		user.save();
	};

	/***********************************************************/

	ID User_Class::create(const char* name, const char* pass)
	{
		List_Class message_link_list;
		user.data.name_id = storage-> new_Line(name);
		sha1(pass, strlen(pass), user.data.digest);
		user.data.message_list_id = message_link_list.create();
		ID id = user.create();
#if DEBUG
		cout << " user created id = " << id << endl;
		cout << " name = " << name << endl;
		cout << " pass = " << pass << endl;
		for (int i = 0; i < 5; i++) 
		{
			cout << " " << user.data.digest[i];
		};
		cout << endl;
		cout << " group list = " << user.data.group_list_id << endl;
		cout << " message list = " << user.data.message_list_id << endl;
		cout << endl;
#endif
		return id;
	};

	/***********************************************************/
	/*                     ID Table Class                      */
	/***********************************************************/

	const char* allocate_Key(size_t n) 
	{
		User_Class user;
		user.open(n);
		return user.allocate_Name();
	};

	/***********************************************************/
	/*                    User List Class                      */
	/***********************************************************/

	class User_List_Data 
	{
	public:
		unsigned size = 0;
		unsigned cntr = 0;
		ID table_id = NOTANID;
	};

	/***********************************************************/

	class User_List : public User_List_Access
	{
	public:

		// open and save methods are overriden
		virtual void open(ID id)override;
		virtual void save() override;

		virtual ID create();
		virtual ID find_User(const char* name) override;
		virtual ID init_User(unsigned* nptr) override;
		virtual ID next_User(unsigned* nptr) override;
		virtual void new_User(ID user_id, const char* name) override ;
		
		User_List();
		
		virtual ~User_List();

	private:

		Block_Template<User_List_Data> user_list;
		Hash_Table* table = nullptr;
		unsigned init = 0;
	};

	/***********************************************************/

	User_List::User_List() : User_List_Access(&user_list) 
	{
		table = new Hash_Table();	// create an empty table
	};

	User_List::~User_List() 
	{ 
		delete table; 
	};

	/***********************************************************/

	void User_List::open(ID id) 
	{
		user_list.open(id);

		unsigned buff_size = table->memory_Size(user_list.data.size);
		void* tptr = malloc(buff_size);
		storage->get_Data(user_list.data.table_id, buff_size, tptr);
		table->set_Data(user_list.data.size, user_list.data.cntr, tptr);
	};

	/***********************************************************/

	void User_List::save() 
	{
		unsigned size = user_list.data.size;
		unsigned buff_size = table->memory_Size(size);
		void* tptr = table->table_Pointer();

		if (init == size)	// is it the same table?
		{
			storage->put_Data(user_list.data.table_id, buff_size, tptr);	// save
		}
		else				// not the same table
		{
			user_list.data.table_id = storage->new_Data(buff_size, tptr);	// save table
		};

		user_list.save();
	};

	/***********************************************************/

	ID User_List::find_User(const char* name) 
	{
		return table->find_Link(name);
	};

	/***********************************************************/

	ID User_List::init_User(unsigned* nptr) 
	{
		return table->init_Link(nptr);
	};

	/***********************************************************/

	ID User_List::next_User(unsigned* nptr) 
	{
		return table->next_Link(nptr);
	};

	/***********************************************************/

	void User_List::new_User(ID user_id, const char* name) 
	{
		table->add_Link(user_id, name);
		user_list.data.size = table->table_Size();		// table size
		user_list.data.cntr = table->table_Cntr();		// table counter
		save();											// save data
	};

	/***********************************************************/

	ID User_List::create() 
	{
		unsigned size = table->table_Size();
		user_list.data.size = size;
		user_list.data.cntr = table->table_Cntr();
		void* tptr = table->table_Pointer();
		unsigned buff_size = table->memory_Size(size);
		user_list.data.table_id = storage->new_Data(buff_size, tptr);
		ID id = user_list.create();
#if DEBUG
		cout << " user list created id = " << id << endl;
		cout << " table_id = " << user_list.data.table_id << endl;
		cout << endl;
#endif
		return id;
	};

	/***********************************************************/
	/*                         Message Class                   */
	/***********************************************************/

	class Message_Data {
	public:
		ID text_id = NOTANID;			// text block id
		time_t time = 0;				// timestamp
		ID sender_id = NOTANID;			// sender object id
		ID recipient_id = NOTANID;		// recipient object id	
		ID group_id = NOTANID;			// group object id
	};

	/***********************************************************/

	class Message_Class : public Message_Access {
	public:
		virtual ID create(const char* text, 
			ID sender_id, 
			ID recipient_id,
			ID group_id) override;	// create a message

		virtual const char* allocate_Text() const override;		// message text
		virtual time_t message_Time() const override;			// message timestamp
		virtual ID sender_ID() const override;					// message sender ID
		virtual ID recipient_ID()const override;				// message recipient ID
		virtual ID group_ID() const override;					// message group id
	
		Message_Class() : Message_Access(&message) {};

	private:
		Block_Template<Message_Data> message;
	};

	/***********************************************************/

	const char* Message_Class::allocate_Text() const 
	{
		return storage->allocate_Line(message.data.text_id);
	};

	time_t Message_Class::message_Time() const { return message.data.time; };

	ID Message_Class::sender_ID() const { return message.data.sender_id; };

	ID Message_Class::recipient_ID()const { return message.data.recipient_id; };

	ID Message_Class::group_ID()const { return message.data.group_id; };

	/***********************************************************/

	ID Message_Class::create(const char* text, ID sender_id, ID recipient_id, ID group_id)
	{
		message.data.text_id = storage->new_Line(text);
		message.data.time = time(nullptr);
		message.data.sender_id = sender_id;
		message.data.recipient_id = recipient_id;
		message.data.group_id = group_id;
		ID id = message.create();
#if DEBUG
		cout << " message created id = " << id << endl;
		cout << " text = " << text << endl;
		cout << " sender = " << sender_id << endl;
		cout << " recipient = " << recipient_id << endl;
		cout << " group = " << group_id << endl;
		cout << endl;
#endif
		return id;
	};

	/***********************************************************/
	/*                    Message Link Class                   */
	/***********************************************************/

	class Message_Link_Data {
	public:
		bool sent = false;				// sent
		bool received = false;			// received
		ID message_id = NOTANID;		// message object id
	};

	/***********************************************************/

	class Message_Link_Class : public Message_Link_Access {
	public:
		virtual ID create(bool sent, bool received, ID message_id) override;	// create a link

		virtual bool message_Sent() const override;				// message is sent
		virtual bool message_Received() const override;			// message is received
		virtual ID message_ID() const override;					// message ID

		Message_Link_Class() :Message_Link_Access(&message_link) {};

	private:
		Block_Template<Message_Link_Data> message_link;
	};

	/***********************************************************/

	ID Message_Link_Class::create(bool sent, bool received, ID message_id) 
	{
		message_link.data.sent = sent;
		message_link.data.received = received;
		message_link.data.message_id = message_id;
		ID id =  message_link.create();
#if DEBUG
		cout << " message link created id = " << id << endl;
		if (message_link.data.sent) cout << " sent" << endl;
		if (message_link.data.received) cout << " received" << endl;
		cout << " mesage = " << message_link.data.message_id << endl;
		cout << endl;
#endif
		return id;
	};

	/***********************************************************/

	bool Message_Link_Class::message_Sent() const { return message_link.data.sent; };

	bool Message_Link_Class::message_Received() const { return message_link.data.received; };

	ID Message_Link_Class::message_ID() const { return message_link.data.message_id; };

	/***********************************************************/
/*                        Group Class                      */
/***********************************************************/

	class Group_Data {
	public:
		ID name_id = NOTANID;			// group name
		ID user_list_id = NOTANID;		// user list id
	};

	/***********************************************************/

	class Group_Class : public Group_Access {
	public:
		virtual ID create(const char* name) override;			// create gruop

		virtual const char* allocate_Name() const override;		// group name
		virtual ID user_List_ID() const override;				// group user list

		Group_Class() :Group_Access(&group) {};

	private:
		Block_Template<Group_Data> group;
	};

	/***********************************************************/

	const char* Group_Class::allocate_Name() const
	{
		return storage->allocate_Line(group.data.name_id);
	};

	/***********************************************************/

	ID Group_Class::user_List_ID() const
	{
		return group.data.user_list_id;
	};

	/***********************************************************/

	ID Group_Class::create(const char* name)
	{
		List_Class user_list;
		group.data.name_id = storage->new_Line(name);
		group.data.user_list_id = user_list.create();
		ID id = group.create();
#if DEBUG
		cout << " group created id = " << id << endl;
		cout << " name = " << name << endl;
		cout << " user list = " << group.data.user_list_id << endl;
		cout << endl;
#endif
		return id;
	};

	/***********************************************************/
	/*                        Header Class                     */
	/***********************************************************/

	class Header_Data {
	public:
		ID user_list_id = NOTANID;		// user list id
		ID group_list_id = NOTANID;		// group list id
	};

	/***********************************************************/

	class Header_Class : public Header_Access {
	public:
		virtual ID create() override;					// create header
		virtual ID user_List_ID() const override;		// user list id
		virtual ID group_List_ID()const override;		// group list id

		Header_Class() :Header_Access(&header) {};

	private:
		Block_Template<Header_Data> header;
	};

	/***********************************************************/

	ID Header_Class::user_List_ID() const { return header.data.user_list_id; };

	ID Header_Class::group_List_ID() const { return header.data.group_list_id; };

	/***********************************************************/

	ID Header_Class::create()
	{
		List_Class list;
		User_List user_list;
		ID id = header.create();
		header.data.user_list_id = user_list.create();
		header.data.group_list_id = list.create();
		header.save();
#if DEBUG
		cout << " header created id = " << id << endl;
		cout << " user list = " << header.data.user_list_id << endl;
		cout << " group list = " << header.data.group_list_id << endl;
		cout << endl;
#endif
		return id;
	};

	/***********************************************************/
	/*                        Chat Class                       */
	/***********************************************************/

	class Chat_Class : public Chat_Class_Access
	{
	public:
		virtual User_Access* create_User_Access() override
		{ 
			return new User_Class(); 
		};

		virtual Message_Access* create_Message_Access() override
		{ 
			return new Message_Class(); 
		};

		virtual Message_Link_Access* create_Message_Link_Access() override
		{ 
			return new Message_Link_Class(); 
		};

		virtual Group_Access* create_Group_Access() override 
		{
			return new Group_Class();
		};
		
		virtual Header_Access* create_Header_Access() override 
		{
			return new Header_Class();
		};

		virtual List_Access* create_List_Access() override
		{
			return new List_Class();
		};

		virtual User_List_Access* create_User_List_Access() override
		{
			return new User_List();
		};

		virtual ID user_List_ID() override { return user_list_id; };

		virtual void close_Chat_Class() override { storage-> close_Storage(); };

		virtual void set_User_List_ID(ID id) { user_list_id = id; };

		virtual void set_Group_List_ID(ID id) { group_list_id = id; };

	private:
		ID user_list_id = NOTANID;
		ID group_list_id = NOTANID;
	};

	/***********************************************************/

	static Chat_Class chat_class;	// static object

	Chat_Class_Access* get_Chat_Class() 
	{
		Header_Class header;
		if(storage == nullptr)storage = open_Storage();	// open storage

		if (storage->empty_Storage())					// if storage is empty
		{
			ID header_id = header.create();				// create header
		}
		else 
		{
			header.open(0);								// open existing header
		};
		
		ID user_list_id = header.user_List_ID();		// user_list_id
		ID group_list_id = header.group_List_ID();		// group list id
		chat_class.set_User_List_ID(user_list_id);		// set user list id
		chat_class.set_Group_List_ID(group_list_id);	// set group list id

		return &chat_class;								// return chat_class
	};

	/***********************************************************/

