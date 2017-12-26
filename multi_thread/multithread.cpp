#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>

std::mutex g_display_mutex;

class foo 
{
public:
	foo();
	~foo();
	
private:
	std::thread t;
};

foo::foo()
{
	t = std::thread([] {
		std::thread::id this_id = std::this_thread::get_id();
		std::cout << "thread: sleep for 3s! " << std::endl;
		sleep(3);
		g_display_mutex.lock();
		std::cout << "thread id = " << this_id << std::endl;
		g_display_mutex.unlock();
	});
}

foo::~foo()
{
	std::cout << "destructor called" << std::endl;
	
	if (t.joinable())
	{
		std::cout << "joinable" << std::endl;

		t.join();
	}
}

class node{
	public:
	int a; 
	int b;
};

class thread_passparam
{
public:
	thread_passparam()
	{
		std::cout << "main: Address of node: " << &num <<  std::endl;

		t = std::thread(&thread_passparam::th_func, this, num);
	}
	~thread_passparam(){
		if (t.joinable())
		{
			t.join();
		}
	}

	void th_func(int i) 
	{		
		std::thread::id this_id = std::this_thread::get_id();
		g_display_mutex.lock();
		std::cout << "thread: Address of node: " << &i <<  std::endl;
		g_display_mutex.unlock();
	}
private:
	std::thread t;
	int num;
};


int main()
{
	thread_passparam m_th;
	
	getchar();
	return 0;
}