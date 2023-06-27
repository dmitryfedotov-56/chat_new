	/***********************************************************/
	/*                         FILE Storage                     */
	/***********************************************************/

	#define _CRT_SECURE_NO_WARNINGS

	#include "storage_access.h"

	/***********************************************************/
/*
	CAUTION: this code uses fstream and filesystem libraries
*/
	/***********************************************************/
/*
	#include <io.h>
*/
	#include <fstream>
	#include <stdio.h>
	#include <sys\types.h> 
	#include <sys\stat.h> 
	#include <string.h>
	using namespace std;

	#define DEBUG	1

	#if DEBUG
	#include <iostream>
	#endif

	/***********************************************************/

	#define DATAFILE	"chat_data"
	

	/***********************************************************/
	/*                   Filesystem Operations                 */
	/***********************************************************/

	#define FILESYSTEM	1		// <filesystem usage>

	#if FILESYSTEM

	#define	EXECFILE	"CHAT.exe"		

	// caution: EXECFILE should be exactly the name of the executable file!!!
	// e.g. "CHAT.exe" for Windows
	// e.g. "chat" for Linux

	#include <filesystem>
	namespace fs = std::filesystem;

	// set permissions for some particular user type
	// the user can read and write DATAFILE if and only if he has execute rights.

	void set_User_Perms(fs::perms p, fs::perms exec_perm, fs::perms read_perm, fs::perms write_perm)
	{
		if ((p & exec_perm) != fs::perms::none)
			fs::permissions(DATAFILE, read_perm | write_perm, fs::perm_options::add);
		else
			fs::permissions(DATAFILE, read_perm | write_perm, fs::perm_options::remove);
	};

	void set_Perms() 
	{
		fs::perms p = fs::status(EXECFILE).permissions();	// executable file permissions
		set_User_Perms(p, fs::perms::owner_exec, fs::perms::owner_read, fs::perms::owner_write);
		set_User_Perms(p, fs::perms::group_exec, fs::perms::group_read, fs::perms::group_write);
		set_User_Perms(p, fs::perms::others_exec, fs::perms::others_read, fs::perms::others_write);
	};

	#endif

	/***********************************************************/
	/*                   FILE_Storage Class                    */
	/***********************************************************/

	class FILE_Storage : public Storage_Access {
	public:

		virtual BLOCK new_Data(unsigned size, void* src);

		virtual void del_Data(BLOCK id);

		virtual void put_Data(BLOCK id, unsigned size, void* src);

		virtual void get_Data(BLOCK id, unsigned size, void* dst);

		virtual BLOCK new_Line(const char* s);

		virtual const char* allocate_Line(BLOCK id);

		virtual void open_Storage();

		virtual void close_Storage();

		virtual bool empty_Storage();

	private:

		void check(BLOCK id, unsigned size) 
		{
			if (id + size > offs)					// out of space?
			{
/*
				cout << " blck = " << id;
				cout << " offs = " << offs;
				cout << " size = " << size;
				cout << endl;
				exit(0);
*/
				throw STORAGE_ERROR;
			};
		};

		void read(BLOCK id, unsigned size, char* dst)
		{
			check(id, size);							// check
			data_file->seekg(id, ios_base::beg);		// take positon
			data_file->read(dst, size);					// read data
		};

		void write(BLOCK id, unsigned size, const char* src)
		{
			check(id, size);							// check
			data_file->seekp(id, ios_base::beg);		// take positon
			data_file->write((const char*)src, size);	// write data
		};

		fstream* data_file;
		size_t offs = 0;
		bool empty_storage = false;
	};

	/***********************************************************/
	/*
		data operations
	*/
	BLOCK FILE_Storage::new_Data(unsigned size, void* src)
	{
		BLOCK id = offs;
		data_file->seekp(offs, ios_base::beg);		// to the end of file
		data_file->write((const char*)src, size);	// write data
		offs += size;
		return id;									// return positiom
	};

	void FILE_Storage::del_Data(BLOCK id) {};

	void FILE_Storage::put_Data(BLOCK id, unsigned size, void* src)
	{
		check(id, size);							// check
		data_file->seekg(id, ios_base::beg);		// take positon
		data_file->write((const char*)src, size);	// read data	
	};

	void FILE_Storage::get_Data(BLOCK id, unsigned size, void* dst)
	{
		check(id, size);							// check
		data_file->seekg(id, ios_base::beg);		// take positon
		data_file->read((char*)dst, size);			// read data
	};

	/***********************************************************/
	/*
		line operations
	*/
	BLOCK FILE_Storage::new_Line(const char* s)
	{
		BLOCK id = offs;
		unsigned size = strlen(s) + 1;							// line sise
		data_file->seekp(offs, ios_base::beg);					// to the end of file
		data_file->write((const char*)&size, sizeof(unsigned));	// write size
		offs += sizeof(size_t);
		data_file->write(s, size);								// write line
		offs += size;
		return id;
	};

	const char* FILE_Storage::allocate_Line(BLOCK id)
	{
		unsigned size = 0;
		get_Data(id, sizeof(unsigned), &size);					// read line size
		char* ptr = (char*)malloc(size);						// allocate memory
		if (ptr == nullptr)throw STORAGE_ERROR;					// check error
		get_Data(id + sizeof(unsigned), size, ptr);				// read line
		return (const char*)ptr;
	};

	/***********************************************************/
	/*
		open and close
	*/
	void FILE_Storage::open_Storage()
	{
		ifstream ifstr(DATAFILE);
		if (ifstr.is_open())					// existing file?
		{
			empty_storage = false;				// storage is not empty
			ifstr.seekg(0, ios_base::end);		// get file size
			offs = ifstr.tellg();				// set offs
		}
		else
		{
			empty_storage = true;				// storage is empty
			offs = 0;							// zero offset
		};

		ifstr.close();

		if (empty_storage)										// empty storage?
		{
			fstream new_file;
			new_file.open(DATAFILE, ios::binary | ios::out);	// create a new file
			if (!new_file.is_open())throw STORAGE_ERROR;		// check it
			new_file.close();

#if FILESYSTEM
			set_Perms();										// set permissions
#endif
		};

		// now file is ready, open file for reading and writing;

		data_file = 
			new fstream(DATAFILE, ios::ate | ios::binary | ios::in | ios::out | ios::_Nocreate);
		if(!data_file->is_open())throw STORAGE_ERROR;
	};

	void FILE_Storage::close_Storage()
	{
		data_file->close();
		delete data_file;
		data_file = nullptr;
	};

	bool FILE_Storage::empty_Storage() { return empty_storage; };

	/***********************************************************/

	FILE_Storage storage;				// it is a single static object

	Storage_Access* open_Storage() 
	{
		storage.open_Storage();
		return &storage;
	};

	/***********************************************************/
/*
	int main() 
	{
		size_t n = 0, m = 0;
		const char* s = nullptr;
		BLOCK b1, b2, b3, b4;
		Storage_Access* storage = open_Storage();

		if (storage->empty_Storage()) 
		{
			n = 1;
			b1 = storage->new_Data(4, &n);

			n = 2;
			b2 = storage->new_Data(4, &n);

			n = 3;
			b3 = storage->new_Data(4, &n);

			b4 = storage->new_Line("text");

			return 0;

		};

		storage->get_Data(0, 4, &n);
		cout << n << endl;

		storage->get_Data(4, 4, &n);
		cout << n << endl;

		storage->get_Data(8, 4, &n);
		cout << n << endl;

		s = storage->allocate_Line(12);
		cout << s << endl;

		n = 4;
		storage->put_Data(4, 4, &n);

		storage->get_Data(0, 4, &n);
		cout << n << endl;

		storage->get_Data(4, 4, &n);
		cout << n << endl;

		storage->get_Data(8, 4, &n);
		cout << n << endl;

		storage->close_Storage();
		return 0;
	};
*/
	/***********************************************************/