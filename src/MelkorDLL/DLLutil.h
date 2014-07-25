/////////////////////////////////////////////////
// Support file for interfacing a C++ DLL with //
// a Visual Basic application.				   //
// By Nicholas Skapura						   //
// 17 February 2002							   //
/////////////////////////////////////////////////

#include <wtypes.h>

////////// Useful Typedefs //////////
/*
typedef unsigned char Byte;
typedef short Integer;
typedef long Long;
typedef float Single;
typedef double Double;
typedef __int64 Currency;
typedef BSTR String;
typedef LPSTR cString;
*/

////////// Length of a BSTR String //////////
int BSTRLen(BSTR String)
{
	int Len = 0;
	int Pos = 0;
	bool Finish = false;
	if (String == NULL)
		return(0);
	while (!Finish)
	{
		unsigned int part = String[Pos];
		if (part == 0)
		{
			return(Len);
		}
		else
		{
			Len++;
		}
		Pos++;
	}
	return(0);
}


////////// Convert BSTR String to a Character Array //////////
char* BSTRToChar(BSTR String)
{
	int i,Len = BSTRLen(String);
	char* FinalStr = new char[Len + 1];
	strcpy(FinalStr, "");
	for (i = 0;i < Len;i++)
	{
		unsigned int Part = String[i];
		FinalStr[i] = Part;
		FinalStr[i + 1] = '\0';
	}
	return(FinalStr);
}


////////// Convert a Character Array to a BSTR String //////////
BSTR CharToBSTR(const char* String)
{
	int i,Len = strlen(String);
	unsigned short* FinalStr = new unsigned short[Len + 1];
	for (i = 0; i < Len;i++)
	{
		char Part = String[i];
		FinalStr[i] = Part;
		FinalStr[i + 1] = '\0';
	}
	return(FinalStr);
}
