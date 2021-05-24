/**
 * @file      MessageQueue.h
 * @brief     封装跨平台的msgqueue，vxworks下使用系统的msgque
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
* @brief 封装跨平台的msgqueue，vxworks下使用系统的msgque
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
	 * @brief    创建消息队列
	 * @param    unsigned int unMaxMsgNum   队列消息的最大个数
	 * @return   bool   成功返回true，失败返回false
	*/
	bool MsgQCreate(unsigned int unMaxMsgNum, unsigned int unMaxMsgLength);

	/**
	 * @fn       bool MsgQReceive(char* pReceiveBuf, unsigned int unBufferLength, int nTimeout, int& nRecvLength)
	 * @brief    从消息队列中取数据
	 * @param    char * pReceiveBuf   接收消息缓冲区
	 * @param    unsigned int unBufferLength   接收消息缓冲区的长度
	 * @param    int nTimeout   等待消息队列有数据的时间
	 * @param    int & nRecvLength  收到的消息长度
	 * @return   bool   成功返回true，失败返回false
	*/
	bool MsgQReceive(char* pReceiveBuf, unsigned int unBufferLength, int nTimeout, int& nRecvLength);

	/**
	 * @fn       bool MsgQSend(char* pMessage, unsigned int unMessageLength, int nTimeout)
	 * @brief    发送消息
	 * @param    char * pMessage  消息缓冲区
	 * @param    unsigned int unMessageLength  消息缓冲区长度
	 * @param    int nTimeout  队列满时消息等待时间
	 * @return   bool   成功返回true，失败返回false
	*/
	bool MsgQSend(char* pMessage, unsigned int& unMessageLength, int nTimeout);

	/**
	 * @fn       int MsgQDelete()
	 * @brief    删除消息队列
	 * @return   bool   成功返回true，失败返回false
	*/
	bool MsgQDelete();

protected:
	///初始化
	void Clear();

public:
	CMessageQueue& operator=(const CMessageQueue& queue);

protected:
	MSG_Q_ID m_msgQueue; ///<消息队列ID,windows下为队列buffer首指针

#ifndef VXWORKS
	unsigned int m_nMaxQSize; ///<队列容量
	unsigned int m_nMsgSize; ///<消息容量

	unsigned int m_nFront;	///<队列首的索引

	unsigned int m_nRear;	///<队列尾的索引

	bool m_bDestroy;		///<队列是否已经销毁

	unsigned int m_nTotalObjects;	///<当前队列的元素个数

	CCondition m_enQueueCond;		///<入队的条件变量

	CCondition m_deQueueCond;		///<出队的条件变量

	CThreadMutex m_loopQueueLock;	///<队列的锁

	//std::list<CCache> m_ListMessage; /// 消息内容

	void* m_ListMessage;
#endif
};
#endif
