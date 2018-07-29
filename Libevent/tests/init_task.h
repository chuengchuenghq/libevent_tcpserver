
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
 

#include <string>
#include <vector>
#include <ctime>
#include <iostream>
#include <iomanip>

#ifndef TOSTR
#define TOSTR(x)	#x  
#define TOSTRDEEP(x)	TOSTR(x)
#endif


#define TASK_INIT(tag , name)	void tag##_##name(bool&,bool&);  \
								bool tag##_##name##__LINE__ = \
								base::job_holder::instance().add( TOSTRDEEP(tag##_##name) , tag##_##name );	\
								void tag##_##name(bool& test_state , bool& need_rethrow)


#define JOB_TEST(bool_test)	\
	do { \
		if(!(bool_test))\
		{ 	\
			test_state=false; \
			std::cout <<"Error!!!\n\t[File]:"<<  __FILE__ \
				<< "\n\t[Line]:" << __LINE__ \
				<< "\n\t[Func]:" << __FUNCTION__ \
				<< "\n\t[problem]:" << (#bool_test) << std::endl; } \
	}while(0)


#define JOB_RUN(argc , argv)	base::job_holder_run(argc , argv)



namespace base{  


class job_holder
{
public:
	static job_holder& instance()
	{
		static job_holder ins;
		return ins;
	}

	int size() const
	{
		return m_jobs.size();
	}

	bool contains( const std::string& name ) const
	{
		for(int i = 0 ; i< size(); ++i)
			if( name == m_jobs[i].m_name )
				return true;
		return false;
	}

	bool add( const char* name , void (*func)(bool&,bool&) )
	{
		m_jobs.push_back( func_wrapper( name , func) );
		return true;
	}

	bool run( const std::string& name )
	{
		int i ;
		for( i = 0 ; i< size(); ++i)
			if( name == m_jobs[i].m_name )
				break;
		return run(i);
	}

	bool run( int index )
	{
		if( index>=0 &&  index < size() )
			return m_jobs[index]();
		return true;
	}

	bool run_all()
	{
		bool state = true;
		for(int i = 0 ; i< size(); ++i)
		{
			state &= m_jobs[i]();
		}
		return state;
	}

	void show()
	{
		std::cout << "Welcome back to libev program library=>";
		std::cout << "\nthere are "<< m_jobs.size() << " jobs here:\n";
		int width = 1; 
		int isize = m_jobs.size();
		while(isize/=10)	
			width += 1;

		for(unsigned int i = 0 ; i< m_jobs.size(); ++i)
			std::cout <<"\t<" << std::setw(width) << i << ">: " << m_jobs[i].m_name << "\n";
	}

private:
	job_holder(){}

	~job_holder()
	{}
	
	struct func_wrapper  
	{
		func_wrapper( const char* name , void (*func)(bool&,bool&) ) 
			:m_name(name),m_func(func)
		{}

		bool operator()()
		{
			std::cout << "=====<" << m_name << ">begin\n"; 

			 
			std::clock_t _start_time = std::clock(); 
			bool result_state = true; 
			bool need_rethrow = false;
			try{
				m_func(result_state , need_rethrow); 
			}catch(std::exception& e)
			{
				std::cout << "exception here ....:" << e.what() << std::endl;
				if(need_rethrow)  throw e;
			}catch(...)
			{
				std::cout << "exception here ....!!!!\n";
				if(need_rethrow)  throw;
			}
			
			 
			double elapsed_t =  double(std::clock() - _start_time) / CLOCKS_PER_SEC;

			std::cout << "=====<" << m_name 
				<< ">end [State:"<< (result_state?"OK":"Fail") 
				<<"] [Times:" << elapsed_t <<"s]\n\n"; 
			return result_state ;
		}
		const char* m_name;
		void (*m_func)(bool&,bool&);
	};

	std::vector<func_wrapper> m_jobs;
};


static bool job_holder_run(int argc , char** argv)
{
	 
	if( 0 == job_holder::instance().size() )
	{
		std::cout << "there is no job here!\n";
		return false;
	}

	 
	if( 1 == job_holder::instance().size())
	{
		job_holder::instance().run_all();
		return true;
	}

	 
	do
	{
		job_holder::instance().show();
		int select = job_holder::instance().size()-1 ;
		std::cin.sync();
		std::cin.clear();
		std::cout << "select (-1 to escape , -2 to run all):";
		std::cin >> select;
		if( select == -1)
			break;
		else if(select == -2)
			job_holder::instance().run_all();			
		else
			job_holder::instance().run(select);
	} while (true);
	return true;
}




} 


#ifdef AUTO_RUN_JOB
int main(int argc , char* argv[])
{
	std::cout << std::boolalpha;
	return JOB_RUN(argc , argv)? EXIT_SUCCESS : EXIT_FAILURE ;
}
#endif