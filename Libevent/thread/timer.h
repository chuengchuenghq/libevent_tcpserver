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

struct timer_event
{
	virtual void onTimeEvent() = 0;
	virtual ~timer_event(){}
};


struct timer_event;

struct timer
{
	timer();
	timer(timer_event* pcall);
	timer(void(*callback)(void*) , void* param);
	timer( void(*callback)(void*) , void* param , int intervalMillSeconds , bool isSingleShot);
	bool start();
	void stop();
	void setCallBack(timer_event* pcall);
	void setCallBack(void(*callback)(void*) , void* param = 0);
	void setInterval(int intervalMillSeconds);
	void setSingleShot(bool isSingleShot);
	~timer();
private:
	struct Private;
	Private *pData;
};

}

