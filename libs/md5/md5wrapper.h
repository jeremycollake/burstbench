/*
 *	This is my wrapper-class to create
 *	a MD5 Hash from a string and a file.
 *
 *	This code is completly free, you
 *	can copy it, modify it, or do
 *	what ever you want with it.
 *
 *	Feb. 2005
 *	Benjamin Gr�delbach
 */

 //include protection
#ifndef MD5WRAPPER_H
#define MD5WRAPPER_H

//basic includes
#include <string>
#include <atlstr.h>

using namespace ATL;

//forwards
class MD5;

class md5wrapper
{
private:
	MD5* md5;

	/*
	 * internal hash function, calling
	 * the basic methods from md5.h
	 */
	CString hashit(CString text);

	/*
	 * converts the numeric giets to
	 * a valid CString
	 */
	CString convToString(unsigned char* bytes);
public:
	//constructor
	md5wrapper();

	//destructor
	~md5wrapper();

	/*
	 * creates a MD5 hash from
	 * "text" and returns it as
	 * string
	 */
	CString getHashFromString(CString text);

	/*
	 * creates a MD5 hash from
	 * a file specified in "filename" and
	 * returns it as string
	 */
	CString getHashFromFile(CString filename);
};


//include protection
#endif

/*
 * EOF
 */
