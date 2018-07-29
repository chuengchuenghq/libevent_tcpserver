#include "query_timer.h"
#include <Windows.h>

namespace base{  namespace time{



query_timer::query_timer(void)
{
	LARGE_INTEGER *pFreq =  (LARGE_INTEGER *)&m_freq;
	QueryPerformanceFrequency(pFreq);
	restart();
}

query_timer::~query_timer()
{

}

void query_timer::restart()
{
	LARGE_INTEGER *pStartCount =(LARGE_INTEGER *)&m_startCount ;
	QueryPerformanceCounter(pStartCount);
	m_endCount = m_startCount ;
}

double query_timer::elaspe()
{
	LARGE_INTEGER *pEndCount = (LARGE_INTEGER *)&m_endCount ;
	QueryPerformanceCounter(pEndCount);
	return (double)(m_endCount - m_startCount) / m_freq;
}



}}


