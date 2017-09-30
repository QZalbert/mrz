#include "param.h"
#include "iniwrite.hh"
#include "string.h"
#include <stdlib.h>
#include <QDir>


Param::Param(const char* machineIni, const char* projectDir,
             const char* secName)
{
    strcpy(prjName, "test");
    strcpy(m_machineIni, machineIni);
    strcpy(m_prjSaveDir, projectDir);
    if(m_prjSaveDir[strlen(m_prjSaveDir)-1]=='/')
        m_prjSaveDir[strlen(m_prjSaveDir)-1]=0;

	m_qSaveDir= new QDir(m_prjSaveDir);

    if(secName)
        strcpy(m_secName, secName);
    else
        strcpy(m_secName, "MARK");

   // paramInit();
}

int Param::loadMachineIni(){
    if (m_iniFile.Open(m_machineIni) == false) {
        printf("can not open machine ini file %s\n",m_machineIni);
        return -1;
    }
    RWParam(true, prjName);
    rwMachineParam(true);
    m_iniFile.Close();
    return 0;
}

void  Param::saveMachineIni(){

    char tmp[256];
    strcpy(tmp, m_iniPath);
    strcpy(m_iniPath, m_machineIni);
    rwMachineParam(false);
    RWParam(false, prjName);
    strcpy(m_iniPath, tmp);
}


int Param::loadProject(const char* projectName){
    if(projectName!=NULL)
        strcpy(prjName, projectName);
	printf("Load project: %s\n", prjName);
    savePorjectName();
    sprintf(m_prjDir, "%s/%s", m_prjSaveDir, prjName);
    sprintf(m_iniPath, "%s/mark.ini", m_prjDir);

    if (m_iniFile.Open(m_iniPath) == false) {
        printf("can not open project ini file %s\n",m_iniPath);
        return -1;
    }
    rwProjectParam(true);
    m_iniFile.Close();
    return 0;
}

void Param::saveProject(const char* projectName){
    if(projectName!=NULL)
        strcpy(prjName, projectName);
    sprintf(m_prjDir, "%s/%s", m_prjSaveDir, prjName);
    sprintf(m_iniPath, "%s/mark.ini", m_prjDir);
    m_qSaveDir->mkdir(prjName);
    rwProjectParam(false);
    savePorjectName();
}

void Param::deleteProject(const char *name){
	QString path=QString::fromUtf8(m_prjSaveDir)+QDir::separator()+name;
	QDir dir(path);
    dir.rmdir(path);
}

int Param::loadParam(double &value, const char *name){
    return ini_get_double_param(&m_iniFile, m_secName, name, &value, value);
}

int Param::loadParam(float &value, const char *name){
    double val=value;
    int res=loadParam(val, name);
    value=val;
    return res;
}


int Param::loadParam(int &value, const char *name){
   return  ini_get_int_param(&m_iniFile, m_secName, name, &value, value);
}

int  Param::loadParam(bool &value, const char *name){
    int val=value;
    int res=loadParam(val, name);
    value=val;
    return res;
}

int Param::loadParam(char* str, const char *name){
    return ini_get_string_param(&m_iniFile, m_secName, name, str, str);
}

void Param::saveParam(double value, const char *name){
    write_profile_double(m_secName, name, value, m_iniPath);
}

void Param::saveParam(int value, const char *name){
    write_profile_int(m_secName, name, value, m_iniPath);
}

void Param::saveParam(const char* str, const char *name){
    write_profile_string(m_secName, name, str, m_iniPath);
}

void Param::savePorjectName(){
    write_profile_string(m_secName, "prjName", prjName, m_machineIni);
}


const char* Param::nameConvet(const char* name, int index0, int index1, int index2){
	const char* pSrc=name;
	char* pDst=m_convertName;
	bool inArray=false;
	while(*pSrc){
		if(*pSrc=='.'||*pSrc=='>'){
			pDst=m_convertName;
		}
		else if(*pSrc=='['){
			inArray=true;
		}
		else if(*pSrc==' '){
			;
		}
		else if(inArray){
			if(*pSrc==']')
				inArray=false;
		}
		else{
			*pDst=*pSrc;
			pDst++;
		}
		pSrc++;
	}
	*pDst=0;
	char indexbuf[32];
	if(index0>=0){
		sprintf(indexbuf, "_%d", index0);
		strcat(m_convertName, indexbuf);
		if(index1>=0){
			sprintf(indexbuf, "_%d", index1);
			strcat(m_convertName, indexbuf);
			if(index2>=0){
				sprintf(indexbuf, "_%d", index2);
				strcat(m_convertName, indexbuf);
			}
		}
	}

	return m_convertName;
}


int Param::rwParam(bool r, int& value, const char* name){
    if(r)
        return loadParam(value, name);
    else
        saveParam(value, name);
    return 0;
}
int Param::rwParam(bool r, bool& value, const char* name){
    if(r)
        return loadParam(value, name);
    else
        saveParam(value, name);
    return 0;
}
int Param::rwParam(bool r, double& value, const char* name){
    if(r)
        return loadParam(value, name);
    else
        saveParam(value, name);
    return 0;
}
int Param::rwParam(bool r, float& value, const char* name){
    if(r)
        return loadParam(value, name);
    else
        saveParam(value, name);
    return 0;
}
int Param::rwParam(bool r, char* str, const char *name){
    if(r)
        return loadParam(str, name);
    else
        saveParam(str, name);
    return 0;
}

QStringList Param::getProjectNames()
{
	return m_qSaveDir->entryList(QDir::Dirs|QDir::NoDotAndDotDot);
}

bool Param::projectExist( const char* name )
{
	QStringList strlist=getProjectNames();
	for(QStringList::Iterator it=strlist.begin();it!=strlist.end();it++){
		if(QString::fromUtf8(name)==*it)
			return true;
	}
	return false;
}





