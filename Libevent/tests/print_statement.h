

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
 
#include <iostream>
#include <iomanip>
#include <ctime>
#include <string>
#include <cstdio>
#include <bitset>
#include <sstream>
#include <map>
#include <set>
#include <vector>

using std::cout;
using std::endl;
using std::left;
using std::right;
using std::setw;
using std::setfill;
using std::boolalpha;
using std::string;

typedef std::map<string,string> str_map_type;
typedef std::vector<string> str_list_type;
typedef std::set<string> str_set_type;

#ifndef CAT
#define CAT(x,y)	x##y
#define CAT_DEEP(x,y)	CAT(x,y)
#endif

 
#ifndef forever
#define forever							for(;;)
#endif

#define static_init_once()				do{static bool is_init = false; if(is_init) return ; is_init = true;}while(0)
#define fortimes(cnt)					for(int CAT_DEEP(i,__LINE__) = 0 ; CAT_DEEP(i,__LINE__) < cnt ; ++ CAT_DEEP(i,__LINE__) )
#define wait_exit()						base::detail::wait_exit_detail()
#define CloseWaitExit()					base::detail::console_wait_exit console_wait_exit_value				
#define GetConsoleString(description)	base::detail::GetConsoleString_detail(description)		
#define GetConsoleInt(description)		base::detail::GetConsoleT_detail<int>(description)	
#define GetConsoleLongLong(description)	base::detail::GetConsoleT_detail<long long>(description)	
#define GetConsoleFloat(description)	base::detail::GetConsoleT_detail<double>(description)	

#define OUTPUT(x)							std::cout << std::left << std::setw(10) << (#x)<<" \":\" " << std::left << std::setw(10)<< (x) << std::endl
#define PSEndl()						std::cout << std::endl;
#define PSBool(x)						std::cout << std::left << std::setw(10) << (#x)<<" \":\" " << std::left << std::setw(10)<< ((x)?"OK":"Fail") << std::endl
#define PSName(x)						std::cout << std::left << std::setw(100) << (#x)<< std::endl
#define PSVoid(x)						do{ x;std::cout << std::left << std::setw(100) << (#x)<< std::endl;}while(0)
#define PSType(t)						PSTypeImpl(t)						
#define PSTypeImpl(t)					std::cout << std::left << std::setw(10) << (#t)<<" \":\" " << std::left << std::setw(10)<< typeid(t).name() << std::endl
#define PSSize(t)						PS(sizeof(t))
#define PSHex(val)						std::printf("%#.02x\n",val)
#define PSBytes(addr,size)				base::detail::print_hex_values(addr , size)
#define PSBinary(x)						std::cout << (#x ":") << std::bitset<sizeof(x)*8>(x) << std::endl
#define PSSplit()						do { fortimes(50) std::cout << "=";std::cout<<std::endl; }while(0)
#define PSFunc()						std::cout << __FUNCTION__ << std::endl
#define PSCodeInfo(x)					std::cout <<"File:" << __FILE__  << " Func:" << __FUNCTION__ << " Line:" << __LINE__ << std::endl
#define PSMsgInOut(msg)					base::in_out_printer CAT_DEEP(iop,__LINE__)(msg)
#define PSFuncInOut()					PSMsgInOut(__FUNCTION__) 
#define PSFucTime()						base::in_out_timer  CAT_DEEP(print_lt,__LINE__)(__FUNCTION__)
#define PSIterable(IterBeg,IterEnd)		base::detail::print_iterable(IterBeg , IterEnd)
 
 
 
namespace base { 

 
template<class T>
class big3_tracer 
{
public:  
	big3_tracer() 
	{ 
		std::cout << typeid(T).name() << " call construction\n";  
	}  

	big3_tracer(const big3_tracer& other) 
	{ 
		std::cout << typeid(T).name() << " call copy-construction\n";  
	}  

	big3_tracer& operator=(const big3_tracer& other) 
	{ 
		std::cout << typeid(T).name() << " call operator=()\n";  
		return *this;  
	}  

	~big3_tracer() 
	{    
		std::cout << typeid(T).name() << " call deconstruction\n";
	}  
};


 
struct in_out_printer
{
	template<typename tag>
	struct CountSingleton{ static int count;};

	in_out_printer(const std::string& msg):m_msg(msg)
	{	
		CountSingleton<in_out_printer>::count++;

		std::cout <<"[in ] ";
		for (int i = 0; i < CountSingleton<in_out_printer>::count+2; ++i)
			std::cout << "===";
		std::cout <<"("<< CountSingleton<in_out_printer>::count <<")"<< m_msg << std::endl;
	}

	~in_out_printer()
	{	
		std::cout << "[out] ";
		for (int i = 0; i < CountSingleton<in_out_printer>::count+2; ++i)
			std::cout << "===";
		std::cout <<"("<< CountSingleton<in_out_printer>::count <<")"<< m_msg << std::endl;
		CountSingleton<in_out_printer>::count--;

	}

	std::string m_msg;
};

template<typename tag>
int in_out_printer::CountSingleton<tag>::count ;

 
struct in_out_timer
{
	in_out_timer(const char* name = "")
		:m_name(name)
	{ 
		_start_time = std::clock(); 
	}

	~in_out_timer()
	{
		double elapsed_t =  double(std::clock() - _start_time) / CLOCKS_PER_SEC;   
		std::cout << m_name << " take total seconds :" << elapsed_t << std::endl;
	}
private:
	std::clock_t _start_time;
	std::string m_name;
};

 
template<class T>
struct object_counter
{
	template<class ObejctType>
	friend int get_object_count();
protected:
	object_counter() {  ++m_count ;  }
	~object_counter(){ --m_count ;   } 
private:
	static int m_count;
};

template<class ObejctType>
int get_object_count()
{
	return object_counter<ObejctType>::m_count;
}

template<class T>
int object_counter<T>::m_count = 0;


 
namespace detail{


inline void  print_hex_values(void* valAddr ,int valBytes , int lineCountBytes = 25)
{
	unsigned char* pVal = (unsigned char*)valAddr;
	int i ;
	for( i = 0 ; i< valBytes ; ++i)
	{
		std::printf("%02x " , pVal[i] );	
		if( (i+1)%lineCountBytes == 0)	
			std::printf("\n");
	}
	if( (i+1) % lineCountBytes != 0)	
		std::printf("\n");
}

template<typename K , typename V>
std::ostream& operator<<(std::ostream& out , std::pair<K,V>& kv)
{
	out << kv.first << "-" << kv.second ;
	return out;
}

template<typename IterType>
inline void print_iterable(IterType beg , IterType end)
{
	for(int i = 0;beg!=end; ++beg)
		std::cout << i++ << ":"<< (*beg) << std::endl;
}

inline void wait_exit_detail()
{
	int i = 0;
	do 
	{
		std::cout << "Enter -1 to exit: \n";
		std::cin >> i;
		if(i == -1)
			std::exit(0);
	} while (true);
}

struct console_wait_exit
{
	~console_wait_exit()
	{
		std::atexit(wait_exit_detail);
	}
};

inline std::string GetConsoleString_detail(const char* msg)
{
	std::cin.sync();
	std::cin.clear();
	std::string str;
	std::cout << msg ;
	std::getline(std::cin , str);
	std::cout << "You input:" << str << std::endl;
	return str;
}

template<typename T>
inline T GetConsoleT_detail(const char* msg)
{
	std::stringstream ss;
	ss << GetConsoleString_detail(msg);
	T val = T();
	ss >> val;
	return val;
}


} 


} 
 