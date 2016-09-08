#include "csv_export.h"
#include <iostream>

void WriteFormatted (FILE * stream, const char * format, ...)
{
  va_list args;
  va_start (args, format);
  vfprintf (stream, format, args);
  va_end (args);
}

int main()
{
	std::cout << "Input file name: ";
	std::string name;
	getline(std::cin, name);
	
	int n_headers;
	std::cout << "Input number of headers: ";
	std::cin >> n_headers;

	std::string temp;
	std::vector <std::string> headers;
	std::cout << "Input headernames: " << std::endl;

	for(int i = 0; i < n_headers; i++)
	{
		std::cin >> temp;
		headers.push_back(temp);
	}

	csv_export test(name, n_headers);
	test.create_headers_v(headers, ',');
}