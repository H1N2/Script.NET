//----------------------------------------------------------------------------
//Name            : RCResult.h
//Function        : RemoteControl��������ֵ�����ļ�
//Date            : 2002-08-10
//Author          : 
//Modification    :
//----------------------------------------------------------------------------

#ifndef RC_RESULT_H
#define RC_RESULT_H

#define      ENGLISH_STRINGTABLE             0
#define      CHINESE_STRINGTABLE             1

//RemoteControl��������ֵ
enum ERCResult
{
    trOk             = 0,                       //ִ�гɹ�
    trUnconnected       ,                       //����δ����
    trUnlogin           ,                       //δ��¼
    trNetFail           ,	                    //���粻ͨ
    trUserNotFound      ,	                    //δ�ҵ��û�
    trPassWordErr       ,	                    //�����
	trRemoteDeviceOpenErr,                      //Զ���豸��ʧ��
	trReceiveTimeOut    ,                       //�������ݳ�ʱ

    //����ǰ����������ķ�����
    trLast                                     //���ķ���ֵ
};

const TCHAR* GetResultString(ERCResult eResult, UINT iLanguage = ENGLISH_STRINGTABLE);


#endif // end of RC_RESULT_H