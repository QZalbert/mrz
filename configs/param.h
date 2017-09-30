#ifndef TBJPARAM_H
#define TBJPARAM_H
#include <QDir>
#include "inifile.hh"

#define RWParam(r,value) rwParam(r,value,nameConvet(#value))
#define RWParam1(r,value,index0) rwParam(r,value,nameConvet(#value,index0))
#define RWParam2(r,value,index0, index1) rwParam(r,value,nameConvet(#value,index0,index1))
#define RWParam3(r,value,index0, index1, index2) rwParam(r,value,nameConvet(#value,index0,index1,index2))


class Param
{
public:
    Param(const char* machinIni, const char* projectDir,
          const char* secName=NULL);      //
	QStringList getProjectNames(); 
	bool projectExist(const char* name);

    int loadMachineIni();
    int loadProject(const char* name=NULL);

    void saveMachineIni();
    void saveProject(const char* name=NULL);

    void deleteProject(const char* name);

    int rwParam(bool r, int& value, const char* name);
    int rwParam(bool r, bool& value, const char* name);
    int rwParam(bool r, double& value, const char* name);
    int rwParam(bool r, float& value, const char* name);
    int rwParam(bool r, char* str, const char *name);

    char prjName[64];

protected:
    virtual void paramInit()=0;
    virtual void rwMachineParam(bool r)=0;
    virtual void rwProjectParam(bool r)=0;

    void savePorjectName();

    void saveParam(int value, const char* name);
    void saveParam(double value, const char* name);
    void saveParam(const char* value, const char* name);

    int loadParam(int& value, const char* name);
    int loadParam(bool& value, const char* name);
    int loadParam(double& value, const char* name);
    int loadParam(float& value, const char* name);
    int loadParam(char* str, const char *name);
	const char* nameConvet(const char* name, int index0=-1, int index1=-1, int index2=-1);

    char m_machineIni[256];
    char m_prjSaveDir[256];
    char m_prjDir[256];
    char m_secName[64];
    char m_iniPath[256];
	char m_convertName[256];
	QDir* m_qSaveDir;

    IniFile m_iniFile;
};

#endif // TBJPARAM_H
