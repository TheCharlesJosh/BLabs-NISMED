#ifndef GET_SETTINGS
#define GET_SETTINGS

#include "get_settings.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace get_settings
{
	int get_variable(const char *file_name, const char* variable_name)
	{
		char buffer[100] = {};

		FILE* fp = 	fopen(file_name, "r");

		if(fp == NULL)
			return -1;

		int result = -2;
		while(fgets(buffer, 100, fp) != NULL)
		{
			if(strncmp(variable_name, buffer, strlen(variable_name)) == 0)
			{
				result = atoi(buffer + strlen(variable_name) + 1);
				break; 
			}

		}

		fclose(fp);
		return result;
	}

	int store_variable(const char *file_name, const char *variable_name, int value)
	{
		char buffer[100] = {};

		FILE* fp = 	fopen(file_name, "r");
		FILE* fp2 = fopen("temp", "w+");	

		if(fp == NULL || fp2 == NULL)
			return -1;

		int result = -2;
		while(fgets(buffer, 100, fp) != NULL)
		{
			printf("%s", buffer);
			if(strncmp(variable_name, buffer, strlen(variable_name)) == 0)
			{
				sprintf(buffer, "%s=%d\n", variable_name, value);
				fputs(buffer, fp2);
			}
			else
			{
				fputs(buffer, fp2);
			}
		}

		fclose(fp);
		rewind(fp2);

		//copy contents of fp2 to fp1
		fp = fopen(file_name, "w");
		while(fgets(buffer, 100, fp2) != NULL)
			fputs(buffer, fp);

		fclose(fp);
		fclose(fp2);
		return result;
	}
}

#endif