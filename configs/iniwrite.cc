/**
 * @file
 * @brief initialization file read and write API implementation
 * @author Deng Yangjun
 * @date 2007-1-9
 * @modifyed by huangbocai
 * @date 2011-7-6
 * @version 0.2
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>

#include "inifile.hh"		

#include "iniwrite.hh"

#ifdef __cplusplus
extern "C"
{
#endif

#define MAX_FILE_SIZE 1024*64
#define LEFT_BRACE '['
#define RIGHT_BRACE ']'

static int load_ini_file(const char *file, char *buf,int *file_size)
{
	FILE *in = NULL;
	int i=0;
	*file_size =0;

	assert(file !=NULL);
	assert(buf !=NULL);

	in = fopen(file,"r");
	if( NULL == in) {
		return 0;
	}

	buf[i]=fgetc(in);
	
	//load initialization file
	while( buf[i]!= (char)EOF) {
		i++;
		assert( i < MAX_FILE_SIZE ); //file too big, you can redefine MAX_FILE_SIZE to fit the big file 
		buf[i]=fgetc(in);
	}
	
	buf[i]='\0';
	*file_size = i;

	fclose(in);
	return 1;
}

static int newline(char c)
{
	return ('\n' == c ||  '\r' == c )? 1 : 0;
}
static int end_of_string(char c)
{
	return '\0'==c? 1 : 0;
}
static int left_barce(char c)
{
	return LEFT_BRACE == c? 1 : 0;
}
static int isright_brace(char c )
{
	return RIGHT_BRACE == c? 1 : 0;
}
static int parse_file(const char *section, const char *key, const char *buf,int *sec_s,int *sec_e,
					  int *key_s,int *key_e, int *value_s, int *value_e)
{
	const char *p = buf;
	int i=0;

	assert(buf!=NULL);
	assert(section != NULL && strlen(section));
	assert(key != NULL && strlen(key));

	*sec_e = *sec_s = *key_e = *key_s = *value_s = *value_e = -1;

	while( !end_of_string(p[i]) ) {
		//find the section
		if( ( 0==i ||  newline(p[i-1]) ) && left_barce(p[i]) )
		{
			int section_start=i+1;

			//find the ']'
			do {
				i++;
			} while( !isright_brace(p[i]) && !end_of_string(p[i]));

			if( strlen(section)==(unsigned)(i-section_start)
				&& 0 == strncmp(p+section_start,section, i-section_start)) {
				int newline_start=0;

				i++;

				//Skip over space char after ']'
				while(isspace(p[i])) {
					i++;
				}

				//find the section
				*sec_s = section_start;
				*sec_e = i;

				while( ! (newline(p[i-1]) && left_barce(p[i])) 
				&& !end_of_string(p[i]) ) {
					int j=0;
					//get a new line
					newline_start = i;

					while( !newline(p[i]) &&  !end_of_string(p[i]) ) {
						i++;
					}
					
					//now i  is equal to end of the line
					j = newline_start;

					if(';' != p[j] && '#'!=p[j]) //skip over comment
					{
						while(j < i && p[j]!='=') {
							j++;
							if('=' == p[j]) {	
								unsigned int keyEnd=j-1;
								while(isspace(p[keyEnd]))
									keyEnd--;							
								if(strlen(key)==keyEnd-newline_start+1
									&& strncmp(key,p+newline_start,strlen(key))==0)
								{
									//find the key ok
									*key_s = newline_start;
									*key_e = j-1;

									*value_s = j+1;
									*value_e = i;

									return 1;
								}
							}
						}
					}

					i++;
				}
			}
		}
		else
		{
			i++;
		}
	}
	return 0;
}

/**
*@brief read string in initialization file\n
* retrieves a string from the specified section in an initialization file
*@param section [in] name of the section containing the key name
*@param key [in] name of the key pairs to value 
*@param value [in] pointer to the buffer that receives the retrieved string
*@param size [in] size of result's buffer 
*@param default_value [in] default value of result
*@param file [in] path of the initialization file
*@return 1 : read success; \n 0 : read fail
*/
int read_profile_string( const char *section, const char *key,char *value, 
		 int size, const char *default_value, const char *file)
{
	char buf[MAX_FILE_SIZE]={0};
	int file_size;
	int sec_s,sec_e,key_s,key_e, value_s, value_e;

	//check parameters
	assert(section != NULL && strlen(section));
	assert(key != NULL && strlen(key));
	assert(value != NULL);
	assert(size > 0);
	assert(file !=NULL &&strlen(key));

	if(!load_ini_file(file,buf,&file_size))
	{
		if(default_value!=NULL)
		{
			strncpy(value,default_value, size);
		}
		return 0;
	}

	if(!parse_file(section,key,buf,&sec_s,&sec_e,&key_s,&key_e,&value_s,&value_e))
	{
		if(default_value!=NULL)
		{
			strncpy(value,default_value, size);
		}
		return 0; //not find the key
	}
	else
	{
		int cpcount = value_e -value_s;

		if( size-1 < cpcount)
		{
			cpcount =  size-1;
		}
	
		memset(value, 0, size);
		memcpy(value,buf+value_s, cpcount );
		value[cpcount] = '\0';

		return 1;
	}
}

/**
*@brief read int value in initialization file\n
* retrieves int value from the specified section in an initialization file
*@param section [in] name of the section containing the key name
*@param key [in] name of the key pairs to value 
*@param default_value [in] default value of result
*@param file [in] path of the initialization file
*@return profile int value,if read fail, return default value
*/
int read_profile_int( const char *section, const char *key,int default_value, 
				const char *file)
{
	char value[32] = {0};
	if(!read_profile_string(section,key,value, sizeof(value),NULL,file))
	{
		return default_value;
	}
	else
	{
		return atoi(value);
	}
}

/**
 * @brief write a profile string to a ini file
 * @param section [in] name of the section,can't be NULL and empty string
 * @param key [in] name of the key pairs to value, can't be NULL and empty string
 * @param value [in] profile string value
 * @param file [in] path of ini file
 * @return 1 : success\n 0 : failure
 */
int write_profile_string(const char *section, const char *key,
					const char *value, const char *file)
{
	char buf[MAX_FILE_SIZE]={0};
	char w_buf[MAX_FILE_SIZE]={0};
	int sec_s,sec_e,key_s,key_e, value_s, value_e;
	int value_len = (int)strlen(value);
	int file_size;
	FILE *out;

	//check parameters
	assert(section != NULL && strlen(section));
	assert(key != NULL && strlen(key));
	assert(value != NULL);
	assert(file !=NULL &&strlen(key));

	if(!load_ini_file(file,buf,&file_size))
	{
		sec_s = -1;
	}
	else
	{
		parse_file(section,key,buf,&sec_s,&sec_e,&key_s,&key_e,&value_s,&value_e);
	}

	if( -1 == sec_s)
	{
		if(0==file_size)
		{
			sprintf(w_buf+file_size,"[%s]\n%s=%s\n",section,key,value);
		}
		else
		{
			//not find the section, then add the new section at end of the file
			memcpy(w_buf,buf,file_size);
			sprintf(w_buf+file_size,"\n[%s]\n%s=%s\n",section,key,value);
		}
	}
	else if(-1 == key_s)
	{
		//not find the key, then add the new key=value at end of the section
		memcpy(w_buf,buf,sec_e);
		sprintf(w_buf+sec_e,"%s=%s\n",key,value);
		//???
		memcpy(w_buf+sec_e+strlen(key)+strlen(value)+2,buf+sec_e, file_size - sec_e);
	}
	else
	{
		//update value with new value
		memcpy(w_buf,buf,value_s);
		memcpy(w_buf+value_s,value, value_len);
		memcpy(w_buf+value_s+value_len, buf+value_e, file_size - value_e);
	}
	
	out = fopen(file,"w");
	if(NULL == out)
	{
		return 0;
	}
	
	if(-1 == fputs(w_buf,out) )
	{
		fclose(out);
		return 0;
	}

	fclose(out);
	return 1;
}

int write_profile_int(const char *section, const char *key,
					int value, const char *file)
{
	int res;
	char buf[32];
	sprintf(buf, "%d", value);
	res= write_profile_string(section,key, buf, file);
	return res;
}

int write_profile_double(const char *section, const char *key,
					double value, const char *file)
{
	int res;
	char buf[32];
	sprintf(buf, "%.12lf", value);
	res= write_profile_string(section,key, buf, file);
	return res;
}

int get_varfile_parameters(const char *filename, int begin, int end, double valArr[])   //!< name of parameter file to read  
{
  FILE *infile;
  char line[256];
  int variable;
  double value;
  int index;                    // index into _required_parameters
  int k;

  // it's OK if the parameter file doesn't exist yet
  // it'll be created in due course with some default values
  if(access(filename, F_OK) == -1){
	printf("var file %s does not exit\n", filename);
     return -1;
  }
	
  // open original for reading
  infile = fopen(filename, "r");
  if(infile==NULL){
  	printf("Unable to open parameter file: '%s'", filename);
	return -1;
  }
	
  k = begin;
  index = 0;

	/*assert the variables of var file is in order*/
  while (feof(infile) == 0 && k<=end) {
    if (fgets(line, 256, infile) == NULL) {
      break;
    }
    // try for a variable-value match in the file
    if (sscanf(line, "%d %lf", &variable, &value) == 2) {
    	if( variable==k){
			valArr[index]=value;
			k++;
			index++;
    	}
		else if(variable>k){
			index+=variable-k;
			k=variable;
			if(k<=end){
				valArr[index]=value;
				index++;
				k++;
			}
			else
				break;
		}
		//if <, next line
    }
  }
  fclose(infile);
  return 0;
}

int ini_get_double_param(IniFile* inifile, const char* section, const char*key, double* val, double defaultVal, bool silent)
{
	double dfVal;
	IniFile::ErrorCode errorCode;
	errorCode=inifile->Find(&dfVal, key, section);
	if(errorCode==IniFile::ERR_NONE)
		*val=dfVal;
	else
	{
        if(!silent)
            printf("not set %s in %s, use default value: %lf\n", key, section, defaultVal);
		*val=defaultVal;
		return -1;
	}
	return 0;
}

int ini_get_int_param(IniFile* inifile, const char* section, const char*key, int* val, int defaultVal, bool silent)
{
	int intVal;
	IniFile::ErrorCode errorCode;
	errorCode=inifile->Find(&intVal, key, section);
	if(errorCode==IniFile::ERR_NONE)
	 	*val=intVal;
	else
	{
		if(!silent)
			printf("not set %s in %s, use default value: %d\n", key, section, defaultVal);
		*val=defaultVal;
		return -1;
	}
	return 0;
}

int ini_get_string_param(IniFile* inifile, const char* section, const char*key,
                         char* val, const char* defaultVal, bool silent)
{
	const char* strVal;
	strVal=inifile->Find(key, section);
	if(strVal)
		strcpy(val, strVal);
	else
	{
        if(!silent)
            printf("not set %s in %s, use default value: %s\n", key, section, defaultVal);
		strcpy(val, defaultVal);
		return -1;
	}
	return 0;
}




#ifdef __cplusplus
}; //end of extern "C" {
#endif
