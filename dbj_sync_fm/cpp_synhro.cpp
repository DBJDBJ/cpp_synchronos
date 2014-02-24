
#include "dbj_synhro_fm.h"

int _test(int argc = 0, _TCHAR* argv[] = 0 )
{
	typedef dbj::fm::lock_block<dbj::fm::SRWLock> lock_type ;
	lock_type lb ;
	dbj::fm::blocking_queue<std::wstring> bq;

	bq.push(L"one") ;
	std::wcout << bq.pop() ;
	return 0;
}

