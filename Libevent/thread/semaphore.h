
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

namespace base{ namespace win32{ namespace thread{


struct Semaphore
{
	explicit Semaphore(long totalCount , long initCount = 0);
	Semaphore();
	~Semaphore();
	void initialize(long totalCount , long initCount = 0);
	int	getCount() const;
	void acquire();
	 
	bool tryAcquire(unsigned long dwMilliseconds);
	void release(long count = 1);
private:
	Semaphore(const Semaphore& other);
	Semaphore& operator=(const Semaphore& other);

	struct Private;
	Private* pData;
};





}}}
