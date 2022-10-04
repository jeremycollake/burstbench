/*
 *	This is part of my wrapper-class to create
 *	a MD5 Hash from a string and a file.
 *
 *	This code is completly free, you
 *	can copy it, modify it, or do
 *	what ever you want with it.
 *
 *	Feb. 2005
 *	Benjamin Grüdelbach
 *
 *  modified JC
 *
 */

 //---------------------------------------------------------------------- 
#include "md5wrapper.h"
#include "md5.h"
#include <fstream>
#include <iostream>

//---------privates--------------------------

/*
 * internal hash function, calling
 * the basic methods from md5.h
 */
CString md5wrapper::hashit(CString text)
{
	MD5_CTX ctx;

	//init md5
	md5->MD5Init(&ctx);
	//update with our string
	md5->MD5Update(&ctx,
		(unsigned char*)CT2A(text.GetBuffer()).m_psz,
		text.GetLength());

	//create the hash
	unsigned char buff[16] = "";
	md5->MD5Final((unsigned char*)buff, &ctx);

	//converte the hash to a string and return it
	return convToString(buff);
}

/*
 * converts the numeric hash to
 * a valid CString.
 * (based on Jim Howard's code;
 * http://www.codeproject.com/cpp/cmd5.asp)
 */
CString md5wrapper::convToString(unsigned char* bytes)
{
	char asciihash[33];

	int p = 0;
	for (int i = 0; i < 16; i++)
	{
		sprintf_s(&asciihash[p], 3, "%02x", bytes[i]);
		p += 2;
	}
	asciihash[32] = '\0';
	return CString(asciihash);
}

//---------publics--------------------------

//constructor
md5wrapper::md5wrapper()
{
	md5 = new MD5();
}


//destructor
md5wrapper::~md5wrapper()
{
	delete md5;
}

/*
 * creates a MD5 hash from
 * "text" and returns it as
 * string
 */
CString md5wrapper::getHashFromString(CString text)
{
	return this->hashit(text);
}
