////////////////////////////////////////////////////////////////////////////
//	File:		IPlatUI.h
//	Version:	1.0.0.0
//	Created:	2008-07-08
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Interface define of VCI message queue
////////////////////////////////////////////////////////////////////////////
#if !defined(__VCI_INTERFACE_IMessageQueue_H_INCLUDED__)
#define __VCI_INTERFACE_IMessageQueue_H_INCLUDED__


#define IID_IMessageQueue "IMessageQueue"

interface IMessageQueue : public IUnknown
{
	//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	// ��ȡ�ӿڵĶ�����Ϣ
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	//{{VCI_INTERFACE_BEGIN
	// ���õ�ǰ���̵Ľ��̱�ʶ
	virtual int __stdcall SetAppProcessId(LPCTSTR lpszProcess) = 0;
	// ��VCI��Ϣ�����Ӧ�������Ϣ����
	virtual int __stdcall PostMessage(CVciMessage* pIn, LPCTSTR lpszQueueName) = 0;
	//}}VCI_INTERFACE_END
};


#endif // !defined(__VCI_INTERFACE_IMessageQueue_H_INCLUDED__)
