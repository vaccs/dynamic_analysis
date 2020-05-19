#ifndef VACCS_ERROR_MESSAGE_H
#define VACCS_ERROR_MESSAGE_H

#include <string>
#include "pin.H"
#include <iostream>

using namespace std;

#define DELIMITER ":"
#define VACCS_ERROR_LEVEL_WARNING "warning"
#define VACCS_ERROR_LEVEL_ERROR "error"

class vaccs_error_message
{
private:
	string file_name;
	int line;
	string severity;
	string message;

public:
	vaccs_error_message();
	~vaccs_error_message();

	string get_file_name() const
	{
		return file_name;
	}

	vaccs_error_message* add_file_name(string file_name)
	{
		this->file_name = file_name;
		return this;
	}

	int get_line() const
	{
		return line;
	}

	vaccs_error_message* add_line(int line)
	{
		this->line = line;
		return this;
	}

	string get_severity() const
	{
		return severity;
	}

	vaccs_error_message* add_severity(string severity)
	{
		this->severity = severity;
		return this;
	}

	string get_message() const
	{
		return message;
	}

	vaccs_error_message* add_message(string message)
	{
		this->message = message;
		return this;
	}

	string get_vaccs_formatted_error_message() {
		return "["+file_name + DELIMITER + decstr(line) + "]" + DELIMITER + " (" + severity + ") " + message;
	}

	void emit_vaccs_formatted_error_message() {
		cerr << get_vaccs_formatted_error_message() << "\n";
	}
};

#endif