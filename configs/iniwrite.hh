/**
 * @file
 * @brief initialization file read and write API
 *	-size of the ini file must less than 16K
 *	-after '=' in key value pair, can not support empty char. this would not like WIN32 API
 *	-support comment using ';' prefix
 *	-can not support multi line
 * @author Deng Yangjun
 * @date 2007-1-9
 * @modifyed by huangbocai
 * @date 2011-7-6
 * @version 0.2
 */
 
#ifndef INI_WRITE_H_
#define INI_WRITE_H_

#include "inifile.hh"		


#ifdef __cplusplus
extern "C"
{

#endif

int read_profile_string( const char *section, const char *key,char *value, int size,const char *default_value, const char *file);
int read_profile_int( const char *section, const char *key,int default_value, const char *file);
int write_profile_string( const char *section, const char *key,const char *value, const char *file);
int write_profile_int(const char *section, const char *key,int value, const char *file);
int write_profile_double(const char *section, const char *key, double value, const char *file);
int get_varfile_parameters(const char *filename, int begin, int end, double valArr[]) ;

int ini_get_int_param(IniFile* inifile, const char* section, const char*key, int* val, int defaultVal, bool silent=false);
int ini_get_double_param(IniFile* inifile, const char* section, const char*key, double* val, double defaultVal=0, bool silent=false);
int ini_get_string_param(IniFile* inifile, const char* section, const char*key, char* val, const char* defaultVal="", bool silent=false);

#ifdef __cplusplus
}; //end of extern "C" {
#endif

#endif //end of INI_FILE_H_

