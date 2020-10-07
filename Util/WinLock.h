#pragma once

class CriticalSection
{
public:
	CRITICAL_SECTION m_cs;

	CriticalSection()
	{
		InitializeCriticalSectionAndSpinCount( &m_cs, 4000 );
	}

	~CriticalSection()
	{
		DeleteCriticalSection( &m_cs );
	}

	struct Lock
	{
		CriticalSection& m_cs;

		Lock( CriticalSection& cs );

		~Lock();
	};
};

#ifdef _DEBUG
extern LONG CriticalSectionLockCount();
#endif
