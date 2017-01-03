////////////////////////////////////////////////////////////////////////////
//	File:		LogOperate.h
//	Version:	1.0.0.0
//	Created:	2010-06-10
//
//	Log file rename and backup
////////////////////////////////////////////////////////////////////////////

#ifndef _LOG_OPERATE_H_
#define _LOG_OPERATE_H_

#include <afx.h>
#include <io.h>
#include <direct.h>
#include <iostream>
#include <time.h>
#include <string>
#include <sstream>
using namespace std;

//////////////////////////////////////////////////////////////////////
// ��������

#define _MAX_FILE_PATH 1024                                // ֧�ֵ�����ļ�·����
#define MAX_MAINTENANCE_LOG_NUM 1024                       // ��ά������ļ���1024

char     g_szInitDir[_MAX_FILE_PATH];                           // ��־·����
char     g_szFileName[MAX_MAINTENANCE_LOG_NUM][_MAX_FILE_PATH]; // ����ļ�������
time_t   g_szFileCreateTime[MAX_MAINTENANCE_LOG_NUM];      // �ļ�����ʱ�����ļ���һһ��Ӧ

int      gnSaveIndex;     // ����������
int      gnMaxSaveNum;    // ��󱣴���

// �ļ�ת���ⲿ���������ļ�����ƥ����򣬱�����Ŀ
bool FileConveySave(const char *dir,const char *filespec,const int nSaveNum);

// �ļ�������������ԭ�ļ�����·����Ŀ���ļ��ļ�����
bool FileReName(const char *pszSrcFileName, const char* pszDesFileName);

//////////////////////////////////////////////////////////////////////////////
// ʵ�ֲ���

/************************ Function **********************************
* �� �� ����sort
* �������ܣ��������У������ļ�����ʱ���������
* ��    �ڣ�2007-4-10 13:03:39
********************************************************************/
void sort()
{
	time_t tem;
	char temfilename[_MAX_FILE_PATH];
	memset(temfilename, 0x00, _MAX_FILE_PATH);

	for(int i = 0; i < gnSaveIndex - 1; i++)
		for(int j = i + 1; j < gnSaveIndex; j++)
		{
			if(g_szFileCreateTime[i] > g_szFileCreateTime[j])
			{
				tem = g_szFileCreateTime[j];
				g_szFileCreateTime[j] = g_szFileCreateTime[i];
				g_szFileCreateTime[i] = tem;
				strncpy(temfilename, g_szFileName[j], _MAX_FILE_PATH);
				strncpy(g_szFileName[j],g_szFileName[i], _MAX_FILE_PATH);
				strncpy(g_szFileName[i], temfilename, _MAX_FILE_PATH);
			}
		}
}

/************************ Function **********************************
* �� �� ����SetInitDir
* �������ܣ����ñ���·��, ���·���Ϸ���
* ��    �ڣ�2007-4-10 11:20:25
********************************************************************/
bool SetInitDir(const char *dir)
{
	int nLen = 0;

	//�ж�Ŀ¼�Ƿ����
	if (_access(dir,0) != 0)
	{
		return false;
	}
	
	// ����Ŀ¼·��
	memset(g_szInitDir, 0x00, _MAX_FILE_PATH);
	memcpy(g_szInitDir, dir, strlen(dir));

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\'
	nLen=strlen(g_szInitDir);
	if (g_szInitDir[nLen - 1] != '\\')
	{
		strcat(g_szInitDir,"\\");
	}

	return true;
}


/************************ Function **********************************
* �� �� ����ProcessFile
* �������ܣ������������ļ�����
* ��    �ڣ�2007-4-10 13:09:33
********************************************************************/
bool ProcessFile(const char *filename,time_t createtime)
{
	// ���ļ������浽�����У��ж�����ļ����������뱣�������
	// �����ļ����ɵ�ʱ����ļ���������ɾ���������ɵ��ļ�
	// �������ص��������������ļ��������ﲻ��Ҫ��������
	// ���������ļ��Ĵ���ʱ��Ϊ���ݣ����ȴ��������ȷ���
	time_t filecreatetime;

	filecreatetime = createtime;
	// С�ڱ�����Ŀʱ��ɾ��
	//printf("%s\n",filename);
	if (gnSaveIndex < MAX_MAINTENANCE_LOG_NUM)
	{
		_snprintf(g_szFileName[gnSaveIndex],(MAX_MAINTENANCE_LOG_NUM - 1), "%s", filename);
		g_szFileCreateTime[gnSaveIndex] = filecreatetime;
		gnSaveIndex++;
	}
	else // �������ά������������ɾ�����紴������־
	{
		// ����
		sort();
		// ɾ���ļ�
		_unlink(g_szFileName[0]);
		_snprintf(g_szFileName[0], (MAX_MAINTENANCE_LOG_NUM - 1), "%s", filename);
		g_szFileCreateTime[0] = filecreatetime;
	}

	return true;
}

/************************ Function **********************************
* �� �� ����BrowseDir
* �������ܣ������ļ��Լ����ļ��У����ﲻ�������ļ���
* ����˵����const char *dir;          // �����ļ�·��  
            const char *filespec;     // ��������
* ��    �ڣ�2007-4-10 12:57:32
********************************************************************/
bool BrowseDir(const char *dir,const char *filespec)
{
	// �����ļ�
	CFileFind logFind;
	ostringstream ostrDir;
	ostrDir<<dir<<"\\"<<filespec;

	try
	{
		// ���ҵ�һ���ļ�
		BOOL IsFinded=(BOOL)logFind.FindFile(ostrDir.str().c_str()); 
		while(IsFinded) 
		{  
			IsFinded=(BOOL)logFind.FindNextFile();
			if(!logFind.IsDots()) 
			{ 
				char foundFileName[_MAX_FILE_PATH]; 
				memset(foundFileName, 0, _MAX_FILE_PATH);

				CString strFileName = logFind.GetFileName();
				strncpy(foundFileName, strFileName.GetBuffer(_MAX_FILE_PATH), (_MAX_FILE_PATH - 1)); 
				strFileName.ReleaseBuffer();

				if(logFind.IsDirectory()) // �����Ŀ¼������
				{ 
					continue;
				} 
				else // ���ҵ��ļ�
				{ 
					char filename[_MAX_FILE_PATH];
					memset(filename, 0x00, _MAX_FILE_PATH);
					strcpy(filename,dir);
					strcat(filename,foundFileName);

					CTime lastWriteTime;
					logFind.GetLastWriteTime(lastWriteTime);
					if (!ProcessFile(filename, (time_t)lastWriteTime.GetTime()))
						return false;
				} 
			} 
		} 
		logFind.Close(); 
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}

/************************ Function **********************************
* �� �� ����FileConveySave
* �������ܣ�ʵ���ļ�ת������
* ����˵����const char *dir;      // ת���ļ�Ŀ¼
            const char *filespec; // ת���ļ�����
			const int nSaveNum;   // �����ļ���Ŀ
* ��    �ڣ�2007-4-10 11:08:36
********************************************************************/
bool FileConveySave(const char *dir,const char *filespec,const int nSaveNum)
{
	// ��ʼ��
	memset(g_szFileName, 0x00, sizeof(g_szFileName));
	memset(g_szFileCreateTime, 0x00, sizeof(g_szFileCreateTime));
	gnSaveIndex = 0;

	bool bRes = false;

	gnMaxSaveNum = nSaveNum;

	// ���ñ���Ŀ¼
	bRes = SetInitDir(dir);
	if (!bRes)
	{
		// ת��ʧ��
		return false;
	}
	
	// �����ļ�,���ҽ�������ļ�ɾ����(�����ڱ��������ļ�)
	bRes = BrowseDir(g_szInitDir, filespec);
	if (!bRes)
	{
		// ����ʧ��
		return false;
	}

	// ����
	if (gnSaveIndex > nSaveNum)
	{
		sort();

		// ѭ��ɾ��������ļ�
		for(int i = 0; i < gnSaveIndex - nSaveNum; i++)
		{
			_unlink(g_szFileName[i]);
		}
	}

	return true;
}

/************************ Function **********************************
* �� �� ����FileReName
* �������ܣ��ļ�������
* ����˵����const char *pszSrcFileName; // Դ�ļ�����·�� 
            const char* pszDesFileName; // Ŀ���ļ��� 
* ��    �ڣ�2007-4-10 11:27:25
********************************************************************/
bool FileReName(const char *pszSrcFileName, const char* pszDesFileName)
{
	int  nLen = 0;
	char szPath[_MAX_FILE_PATH];
	char szDir[_MAX_FILE_PATH];
	char szDrive[_MAX_FILE_PATH];
	char szDesPath[_MAX_FILE_PATH];

	memset(szPath, 0x00, _MAX_FILE_PATH);
	memset(szDir, 0x00, _MAX_FILE_PATH);
	memset(szDrive, 0x00, _MAX_FILE_PATH);
	memset(szDesPath, 0x00, _MAX_FILE_PATH);

	// �����ļ�·��
	_splitpath(pszSrcFileName, szDrive, szDir, NULL, NULL);

	// ���Ŀ���ļ�·��
	_snprintf(szPath, (_MAX_FILE_PATH - 1), "%s%s",szDrive,szDir);

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\'
	nLen=strlen(szPath);
	if (szPath[nLen - 1] != '\\')
	{
		strcat(szPath,"\\");
	}

	// ��ȡ��ǰϵͳʱ��
	SYSTEMTIME oCurrTime;
	GetLocalTime(&oCurrTime);
	char szCurrTime[32];
	memset(szCurrTime, 0x00, 32);
	_snprintf(szCurrTime, 31, "%02d-%02d-%02d-%02d-%02d-%02d", oCurrTime.wYear, oCurrTime.wMonth,
		oCurrTime.wDay, oCurrTime.wHour, oCurrTime.wMinute, oCurrTime.wSecond);

	_snprintf(szDesPath, 1023, "%s%s.%s.txt", szPath, pszDesFileName,szCurrTime);

	// �������ļ���
	rename(pszSrcFileName, szDesPath);

	return true; 
}
#endif
 