#include <iostream>
#include <functional>
#include <memory>

class classA
{
typedef std::function<void(int i)> callback_t;

public:
	classA() {}
	~classA() {}
	
	void handle(int i)
	{
		std::cout << "classA::handle" << std::endl;
		
		cbHandle(i);
	}

	void registCb(callback_t func)
	{cbHandle = std::move(func);}
private:
	callback_t cbHandle;
};

class classB
{
public:
	classB(classA& cA) 
	{		
		//cA.registCb([this](int i){classB::handle(i);});
		cA.registCb(std::bind(&classB::handle, this, std::placeholders::_1));
	}
	~classB() {}
	
	void handle(int i)
	{
		std::cout << "classB, handle message" << i << std::endl;
	}
};

int main()
{
	classA testa;
	classB testb(testa);

	testa.handle(10);
}

