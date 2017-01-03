////////////////////////////////////////////////////////////////////////////
//	File:		Timer.h
//	Version:	1.0.0.0
//	Created:	2007-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Timer Interface
////////////////////////////////////////////////////////////////////////////
#ifndef __TIMER_H_
#define __TIMER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ��ʱ��״̬
enum {
	TIMER_STATE_STOP,		// δ����
	TIMER_STATE_UNACTIVE,	// δ����
	TIMER_STATE_ACTIVE,		// �Ѽ���
	TIMER_STATE_END,		// �ѽ���
};

// �������Ƿ��ͷ�
enum {
	TIMER_INTERP_DONTRELEASE,	// ���ͷ�
	TIMER_INTERP_REFRELEASE,	// �����ü����ͷ�
	TIMER_INTERP_FOUCERELEASE,	// ǿ���ͷ�
};

// ��������
#define TIMER_TYPE_NORMAL		0x00000000	// ��ͨ����
#define TIMER_TYPE_STARTTIME	0x00000001	// ����ʼʱ��
#define TIMER_TYPE_ENDTIME		0x00000002	// �н���ʱ��
#define TIMER_TYPE_TRIGGER		0x00000004	// �д�����

//
// ��ʱ���������
//
class CTimerTask : public CObject
{
public:
	CTimerTask();
	CTimerTask(CString strTaskName, CString strInterpId, CString strInterpName, CString strCommand);
	CTimerTask(CString strTaskName, LPVOID lpFunction);
	~CTimerTask();

	CTimerTask(const CTimerTask& other)
	{
		m_strTaskName	= other.m_strTaskName;
		m_strInterpId	= other.m_strInterpId;
		m_strInterpName	= other.m_strInterpName;
		m_strCommand	= other.m_strCommand;
		m_lpInterpObj	= other.m_lpInterpObj;
		m_lpFunction	= other.m_lpFunction;
		m_nReleaseInterp= other.m_nReleaseInterp;
		m_nTaskType		= other.m_nTaskType;
		m_nSeconds		= other.m_nSeconds;
		m_tStartTime	= other.m_tStartTime;
		m_tEndTime		= other.m_tEndTime;
		m_strTrigger	= other.m_strTrigger;
		m_nExecCount	= other.m_nExecCount;
		m_nCurTimer		= other.m_nCurTimer;
		m_nCurCount		= other.m_nCurCount;
		m_nCurStatus	= other.m_nCurStatus;
	}

	CTimerTask& operator = (const CTimerTask& other)
	{
		m_strTaskName	= other.m_strTaskName;
		m_strInterpId	= other.m_strInterpId;
		m_strInterpName	= other.m_strInterpName;
		m_strCommand	= other.m_strCommand;
		m_lpInterpObj	= other.m_lpInterpObj;
		m_lpFunction	= other.m_lpFunction;
		m_nReleaseInterp= other.m_nReleaseInterp;
		m_nTaskType		= other.m_nTaskType;
		m_nSeconds		= other.m_nSeconds;
		m_tStartTime	= other.m_tStartTime;
		m_tEndTime		= other.m_tEndTime;
		m_strTrigger	= other.m_strTrigger;
		m_nExecCount	= other.m_nExecCount;
		m_nCurTimer		= other.m_nCurTimer;
		m_nCurCount		= other.m_nCurCount;
		m_nCurStatus	= other.m_nCurStatus;
		return *this;
	}

	void StartTask() {m_nCurStatus = TIMER_STATE_UNACTIVE;}
	void SetTaskType(int nType) {m_nTaskType = nType;}
	void SetTaskSeconds(int nSeconds) {m_nSeconds = nSeconds;}
	void SetStartTime(CTime tTime);
	void SetEndTime(CTime tTime);
	void SetTrigger(LPCTSTR lpszTrigger);
	void SetExecCount(int nCount);
	int GetStatus() {return m_nCurStatus;}
	int CheckStatus();		// �������״̬
	int	RunTask();			// ��������

public:
	CString	m_strTaskName;	// ������
	CString	m_strInterpId;	// ������ID
	CString	m_strInterpName;// ������ʵ����
	CString	m_strCommand;	// ����ִ�е�����(�����<file>��ͷ,���ʾִ���ļ�)
	LPVOID	m_lpInterpObj;	// ����������ָ��
	LPVOID	m_lpFunction;	// ����ִ�к���ָ��
	int		m_nReleaseInterp;// �Ƿ��ͷŽ�����(0=���ͷ�,1=�����ü���,2=ǿ���ͷ�)
	int		m_nTaskType;	// ��������
	int		m_nSeconds;		// ��������
	CTime	m_tStartTime;	// ����ʼʱ��
	CTime	m_tEndTime;		// �������ʱ��
	CString	m_strTrigger;	// ���񴥷�����
	int		m_nExecCount;	// ִ�д���
	int		m_nCurTimer;	// ��ǰ����
	int		m_nCurCount;	// ��ǰ����
	int		m_nCurStatus;	// ��ǰ״̬(δ����,������,�ѽ���)
};

typedef int (TIMERTASK_FUNC)(CTimerTask* pTimerTask);


#endif //__TIMER_H_

