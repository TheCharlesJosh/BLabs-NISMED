#include "csv_export.h"

csv_export::csv_export(const std::string fname, int argc)
{
	filename = (std::string) fname;
	file_path = directory + filename;

	n_args = argc;
}

csv_export::~csv_export()
{
	//	to save overhead we can put the fopen and fclose
	//	in the construcor and destructor respectively, but
	//	it might not be *that* safe if the fp is left open
	//	for weeks at a time
	//	if needed we can put the fclose here
}

int csv_export::create_headers(char delim, ...)
{
	FILE *fp = fopen(file_path.c_str(), "w");

	if(fp == NULL)
		return 0;

 	va_list vargs;
	va_start(vargs, delim);
	for(int i = 0; i < n_args; i++)
	{
		if(i > 0)
			fprintf(fp, "%c", delim);

		std::string val = std::string(va_arg(vargs, const char*));
		int x = fprintf(fp, "%s", val.c_str());
		// printf("\t\t%d\n", x);
	}
	va_end(vargs);

	fprintf(fp, "\n");
	fclose(fp);
	// printf("ASDDSA\n");
	return 1;
}

int csv_export::export_values(char delim, ...)
{
	FILE *fp = fopen(file_path.c_str(), "a");

	if(fp == NULL)
		return 0;

	std::string format = "";
	for(int i = 0; i < n_args; i++)
	{
		if(i > 0)
			format = format + delim;
		format = format + export_arg;	
	}
	format = format + "\n";

	// printf("%s\n", format.c_str());
 	va_list vargs;
	va_start(vargs, delim);
	// int x = fprintf(fp, "ASDASD%f %f", 1.1, 2.1);
	int x = vfprintf(fp, format.c_str(), vargs);
	// printf("\t%d\n", x);
	va_end(vargs);
	fclose(fp);

	return 1;
}

int csv_export::export_values_v(std::vector<export_type> entry, char delim)
{
	FILE *fp = fopen(file_path.c_str(), "a");

	if(fp == NULL)
		return 0;

	std::string format = export_arg;
	for(int i = 0; i < entry.size(); i++)
	{
		if(fprintf(fp, format.c_str(), entry[i]) < 0)
		{
			return 0;
		}

		if(i == 0)
			format +=  delim;
	}

	if(fprintf(fp, "\n") < 0)
	{
		return 0;
	}

	return 1;
}

int csv_export::create_headers_v(std::vector<std::string> headers, char delim)
{
	FILE *fp = fopen(file_path.c_str(), "a");

	if(fp == NULL)
		return 0;

	for(int i = 0; i < headers.size(); i++)
	{
		if(fprintf(fp, "%s", headers[i].c_str()) >= 0)
		{
			if(i != headers.size() - 1)
			{
				if(fprintf(fp, "%c", delim) >= 0)
					continue;
			}
			else
				continue;
		}
		return 0;
	}

	if(fprintf(fp, "\n") < 0)
	{
		return 0;
	}

	return 1;
}