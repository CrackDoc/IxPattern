/**
 * @file      MessageQueue.h
 * @brief     ��װ��ƽ̨��msgqueue��vxworks��ʹ��ϵͳ��msgque
 *          
 * @note      
 *       
 * @author    
 * @date      2020/06/07
 * @version   1.0.0.0
 * @CopyRight IxLab
 */
#ifndef _MESSAGE_QUEUE_H_
#define _MESSAGE_QUEUE_H_
#include "IxPatternExport.h"
#include "ThreadMutex.h"
#include "Condition.h"
#include "Cache.h"

#ifdef VXWORKS
#include "msgQLib.h"
#else
typedef char* MSG_Q_ID;
#endif

/**
* @class CMessageQueue
* @brief ��װ��ƽ̨��msgqueue��vxworks��ʹ��ϵͳ��msgque
* @note  
*/
class PATTERN_EXPORT CMessageQueue
{
public:
	CMessageQueue();
	virtual ~CMessageQueue();

public:
	/**
	 * @fn       bool MsgQCreate(unsigned int unMaxMsgNum, unsigned int unMaxMsgLength)
	 * @brief    ������Ϣ����
	 * @param    unsigned int unMaxMsgNum   ������Ϣ��������
	 * @return   bool   �ɹ�����true��ʧ�ܷ���false
	*/
	bool MsgQCreate(unsigned int unMaxMsgNum, unsigned int unMaxMsgLength);

	/**
	 * @fn       bool MsgQReceive(char* pReceiveBuf, unsigned int unBufferLength, int nTimeout, int& nRecvLength)
	 * @brief    ����Ϣ������ȡ����
	 * @param    char * pReceiveBuf   ������Ϣ������
	 * @param    unsigned int unBufferLength   ������Ϣ�������ĳ���
	 * @param    int nTimeout   �ȴ���Ϣ���������ݵ�ʱ��
	 * @param    int & nRecvLength  �յ�����Ϣ����
	 * @return   bool   �ɹ�����true��ʧ�ܷ���false
	*/
	bool MsgQReceive(char* pReceiveBuf, unsigned int unBufferLength, int nTimeout, int& nRecvLength);

	/**
	 * @fn       bool MsgQSend(char* pMessage, unsigned int unMessageLength, int nTimeout)
	 * @brief    ������Ϣ
	 * @param    char * pMessage  ��Ϣ������
	 * @param    unsigned int unMessageLength  ��Ϣ����������
	 * @param    int nTimeout  ������ʱ��Ϣ�ȴ�ʱ��
	 * @return   bool   �ɹ�����true��ʧ�ܷ���false
	*/
	bool MsgQSend(char* pMessage, unsigned int& unMessageLength, int nTimeout);

	/**
	 * @fn       int MsgQDelete()
	 * @brief    ɾ����Ϣ����
	 * @return   bool   �ɹ�����true��ʧ�ܷ���false
	*/
	bool MsgQDelete();

protected:
	///��ʼ��
	void Clear();

public:
	CMessageQueue& operator=(const CMessageQueue& queue);

protected:
	MSG_Q_ID m_msgQueue; ///<��Ϣ����ID,windows��Ϊ����buffer��ָ��

#ifndef VXWORKS
	unsigned int m_nMaxQSize; ///<��������
	unsigned int m_nMsgSize; ///<��Ϣ����

	unsigned int m_nFront;	///<�����׵�����

	unsigned int m_nRear;	///<����β������

	bool m_bDestroy;		///<�����Ƿ��Ѿ�����

	unsigned int m_nTotalObjects;	///<��ǰ���е�Ԫ�ظ���

	CCondition m_enQueueCond;		///<��ӵ���������

	CCondition m_deQueueCond;		///<���ӵ���������

	CThreadMutex m_loopQueueLock;	///<���е���

	//std::list<CCache> m_ListMessage; /// ��Ϣ����

	void* m_ListMessage;
#endif
};
#endif
