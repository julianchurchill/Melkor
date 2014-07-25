// LogWriter.cpp: implementation of the LogWriter class.
//
//////////////////////////////////////////////////////////////////////

#include "LogWriter.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool LogWriter::forceStderr = false;

ostream* LogWriter::out = &cerr;

ofstream LogWriter::logFile;
string LogWriter::logFileName("");
bool LogWriter::useLogFile = false;

// print text as it
void LogWriter::print(string text)
{
//	if(forceStderr) return;
	// insert a '\n' after 80 continuous characters
	if(charCount>=80) {
		text+="\n";
		charCount = 0;
	}
	charCount++;
	if(forceStderr) 
		cerr << text;
	else
		*out << text;
	if(useLogFile)
		logFile << text;
}

// print text with a carriage return after it
void LogWriter::println(string text)
{
//	if(forceStderr) return;
	print(text);
	charCount = 0;
	if(forceStderr) 
		cerr << endl;
	else
		*out << endl;
	if(useLogFile)
		logFile << endl;
}

// output to whatever the text output object is
// and add the callers name to the front and 
// a carriage return after the string
void LogWriter::print(string text, string caller)
{
//	if(forceStderr) return;
	string message = caller;
	message+=": ";
	message+=text;
	print(message);
}

void LogWriter::println(string text, string caller)
{
//	if(forceStderr) return;
	print(text, caller);
	charCount = 0;
	if(forceStderr) 
		cerr << endl;
	else
		*out << endl;
	if(useLogFile)
		logFile << endl;
}

void LogWriter::printerr(string text)
{
	cerr << text;
	if(useLogFile)
		logFile << text;
}

void LogWriter::printerr(string text, string caller)
{
	string message = caller;
	message+=": ";
	message+=text;
	LogWriter::printerr(message);
}

void LogWriter::startLogFile(string& name)
{
	if(useLogFile)
		logFile.close();

	// open the file log and start outputting to it
	logFileName = name;
	useLogFile = true;

	// Make sure we append to the log file
	logFile.open(logFileName.c_str(), ios_base::app);

	// check for errors opening the file
	// fail() returns 0 if there are _no_ errors
	if(logFile.fail()!=0) {
		// if it doesn't open 
		logFileName = "";
		useLogFile = false;
	}
	else {
		// Time stamp the log
		time_t today;
		time(&today);
		struct tm *newToday;
		newToday = localtime(&today);
		string message = "\nLog started at ";
		message+=asctime(newToday);
		LogWriter::printerr(message);
	}
}