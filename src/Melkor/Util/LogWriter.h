// LogWriter.h: interface for the LogWriter class.
//
//////////////////////////////////////////////////////////////////////

#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;

class LogWriter  
{
public:
	static bool forceStderr;
	
	LogWriter(ostream &o = cerr) : charCount(0) { out = &o; };
	virtual ~LogWriter() { };

	static void setOutputStream(ostream &o) { out = &o; };
	static ostream& getOutputStream() { return *out; };

	static ostream& getErrorStream() { return cerr; };

	void print(string text);
	void println(string text);
	void print(string text, string caller);
	void println(string text, string caller);

	// these functions guarantee printing a message to the stderr stream
	static void printerr(string text);
	static void printerr(string text, string caller);

	static void startLogFile(string& name);
	static void stopLogFile() 
	{ 
		// write a time stamp to the log
		time_t today;
		time(&today);
		struct tm *newToday;
		newToday = localtime(&today);
		string message = "Log stopped at ";
		message+=asctime(newToday);
		LogWriter::printerr(message);
		if(useLogFile)
			logFile.close();
		useLogFile = false;
	};

private:
	// special function to allow special handling
	// of text for multi-thread programs
	void sendTextMessage(string text);

	int charCount;

	// ANSI doesn't have ostream with overloaded '='
	// so we can't copy the stream in setOutpuStream() above.
	// so lets use a pointer...
	static ostream* out;

	static string logFileName;
	static bool useLogFile;
	static ofstream logFile;
};

#endif
