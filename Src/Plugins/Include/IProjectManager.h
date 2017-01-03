#if !defined(__VCI_INTERFACE_IProjectManager_H_INCLUDED__)
#define __VCI_INTERFACE_IProjectManager_H_INCLUDED__



#define IID_IProjectManager "IProjectManager"

interface IProjectManager : public IUnknown
{
	//{{ ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!
	// ��ȡ�ӿڵĶ�����Ϣ
	virtual LPVOID __stdcall GetExtraInfo(LPVOID lpRefData) = 0;
	//}} ע�⣺!!!�����Ǳ�������Ҫ����������ɾ��!!!

	//{{VCI_INTERFACE_BEGIN
	// �����½�������
	virtual int __stdcall NewProjectWizard(CString& strPrjVci, CString& strPrjFile) = 0;
	// �����½��ļ���
	virtual int __stdcall NewFileWizard() = 0;
	//}}VCI_INTERFACE_END
};


#endif // !defined(__VCI_INTERFACE_IProjectManager_H_INCLUDED__)
