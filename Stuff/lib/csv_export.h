#ifndef CSV_EXPORT_H
#define CSV_EXPORT_H

#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>

//change to another type as needed
typedef double export_type;

#define CARD_DIRECTORY "./"

class csv_export
{
private:
	std::string export_arg = "%lf";
	std::string directory = CARD_DIRECTORY;
	std::string filename;
	std::string file_path;
 
 	int n_args;
public:
	//string is filename, integer is number of columns
	csv_export(const std::string, int);
	~csv_export();

	/* data */

	//char is delimiter character, returns 1 if successful file open, 0 if not
	//create_headers is always string regardless of export_type, and will truncate the file
	int create_headers(char, ...);
	int export_values(char, ...);

	//vector version of above functions:
	int create_headers_v(std::vector<std::string>, char);
	int export_values_v(std::vector<export_type>, char);
};

#endif