#include "MessageQueue.h"
#include <list>

CMessageQueue::CMessageQueue():
	m_nTotalObjects(0)
	, m_ListMessage(new std::list<CCache*>())
{

}
CMessageQueue& CMessageQueue::operator=(const CMessageQueue& queue)
{
	m_loopQueueLock.Acquire();
	MsgQDelete();
	Clear();
	m_nMaxQSize = queue.m_nMaxQSize;
	m_nMsgSize = queue.m_nMsgSize;
	m_ListMessage = queue.m_ListMessage;
	m_loopQueueLock.Release();
	return *this;
}

CMessageQueue::~CMessageQueue()
{

}

bool CMessageQueue::MsgQCreate( unsigned int unMaxMsgNum, unsigned int unMaxMsgLength )
{
	m_nMaxQSize = unMaxMsgNum;
	m_nMsgSize = unMaxMsgLength;
	return false;
}

bool CMessageQueue::MsgQReceive( char* pReceiveBuf, unsigned int unBufferLength, int nTimeout, int& nRecvLength )
{
	bool bSucc = false;
	bSucc = m_deQueueCond.Wait(nTimeout);

	m_loopQueueLock.Acquire();

	std::list<CCache*>& listQueue = *static_cast<std::list<CCache*>*>(m_ListMessage);

	if (listQueue.size() > m_nMaxQSize)
	{
		CCache* cache = listQueue.front();
		cache->ManualDeleteBuffer();
		listQueue.pop_front();
		m_nTotalObjects--;
	}
	CCache* pCache = new CCache(pReceiveBuf, unBufferLength);
	listQueue.push_back(pCache);
	m_nTotalObjects++;
	m_loopQueueLock.Release();
	return bSucc;
}

bool CMessageQueue::MsgQSend( char* pMessage, unsigned int& unMessageLength, int nTimeout )
{
	bool bSucc = false;
	bSucc = m_enQueueCond.Wait(nTimeout);

	m_loopQueueLock.Acquire();
	std::list<CCache*>& listQueue = *static_cast<std::list<CCache*>*>(m_ListMessage);
	if (listQueue.empty())
	{
		return false;
	}
	CCache* cache = listQueue.back();
	if (cache && pMessage)
	{
		memcpy(pMessage, cache->GetBuffer(), cache->length());
		unMessageLength = cache->length();
		cache->ManualDeleteBuffer();
		listQueue.pop_back();
		m_nTotalObjects--;
	}
	m_loopQueueLock.Release();

	return bSucc;
}

bool CMessageQueue::MsgQDelete()
{
	m_loopQueueLock.Acquire();
	std::list<CCache*>& listQueue = *static_cast<std::list<CCache*>*>(m_ListMessage);

	std::list<CCache*>::iterator it0 = listQueue.begin();

	for(;it0 != listQueue.end();++it0)
	{
		CCache* cache = *it0;
		cache->ManualDeleteBuffer();
	}
	listQueue.clear();
	m_loopQueueLock.Release();
	return true;
}

void CMessageQueue::Clear()
{
	MsgQDelete();
}

