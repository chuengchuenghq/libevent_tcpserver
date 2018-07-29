
//  ----------------------------------------------------------------------------
/// 
///  
/// 
//  ----------------------------------------------------------------------------
/// 
/// \file
/// \brief   Short description
/// 
//  ----------------------------------------------------------------------------
/// 
/// \par     Version Info:
///          $Source: v $\n
///          $Author: tzx $\n
///          $Revision: 1.0 $\n
///          $Date: 2017-02-25 $\n
///
//  ----------------------------------------------------------------------------
/// 
/// \par     Resources and environment
/// 
//  ----------------------------------------------------------------------------
/// 
/// \par     Documentation:
/// 
/// \par     Details:
/// 
/// \par     Usage:
///
//  ----------------------------------------------------------------------------
/// 
/// \par     Versions:
///          $Log: v $
///
///          Revision 1.0   2017-02-25 tzx
///          initial check-in
///          email:chuengchuenghq@yeah.net
///
//  ----------------------------------------------------------------------------
/// 

#pragma once

namespace base{

namespace ThisThread
{
	unsigned int current_thread_id();
	void sleep(unsigned int ms );
	int support_thread_count();
	void yield();
	void yield_processor();
}

 
 
 
 
struct Thread 
{
	enum{
		PriorityIdle,
		PriorityLowest,
		PriorityBlowNormal,
		PriorityNormal,
		PriorityAboveNormal,
		PriorityHighest,
		PriorityTimeCritical
	};

	Thread(); 
	Thread( void (*callback)(void*)  , void* param = 0 ); 
	virtual ~Thread(); 
	
	void start(); 
	void join(); 
	void stop(); 
	void stop(unsigned long dwMilliseconds);
	void abort(unsigned long dwMilliseconds = 1000); 

	void setAutoDelete(bool isAutoDelete); 
	unsigned int id()const; 
	bool isRunning()const;  

	void setStopFlag(); 
	bool setPriority(int priority); 
	int getPriority();
private:
	Thread(const Thread& other);
	Thread& operator=(const Thread& other);

	virtual void onRun();  
	void init();

	struct Private;
	Private* pData;	
};


}
