////////////////////////////////////////////////////////////////////////////
//	File:		Temsp.h
//	Version:	1.0.0.0
//	Created:	2002-06-20
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Comm define of wait object and wait list.
////////////////////////////////////////////////////////////////////////////
#ifndef __WAIT_H_
#define __WAIT_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CWait  
{
private:
	BOOL	m_bWait;	// �Ƿ���Wait״̬
	int		m_nID;		// Wait����ID
//	DWORD	m_dwThreadID;// ��¼�߳�ID
//	HANDLE	m_hWait;	// �ȴ��¼������
	
public:
	CWait(int nID = 0);
	virtual ~CWait();

public:
	void Awake();						// ���ѵȴ�
	int  Wait(DWORD dwTimeOut = 0);		//�ȴ�ָ��ʱ��
	BOOL IsWaiting() { return m_bWait; };	//�Ƿ����ڵȴ�
	int  GetWaitID() { return m_nID; }	// ��ȡ����ID
};


void DeleteAllWaitObject();
CWait* CreateWaitObject(int nID = 0);
BOOL DeleteWaitObject(int nID);
BOOL AwakeWaitObject(int nID);
CWait* GetWaitObject(int nID);


#endif	// __TEMS_H_
