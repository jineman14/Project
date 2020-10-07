
#include "stdafx.h"
#include "WinLock.h"

#ifdef _DEBUG
volatile LONG g_lockCount = 0;

LONG CriticalSectionLockCount()
{
	return g_lockCount;
}

#endif

CriticalSection::Lock::Lock( CriticalSection& cs ) : m_cs(cs) 
{
	EnterCriticalSection( &m_cs.m_cs ); 

//#ifdef _DEBUG
//	::InterlockedIncrement(&g_lockCount);
//#endif
}

CriticalSection::Lock::~Lock()
{ 
	LeaveCriticalSection( &m_cs.m_cs ); 

//#ifdef _DEBUG
//	::InterlockedDecrement(&g_lockCount);
//#endif
}
