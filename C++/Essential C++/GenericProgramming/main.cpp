#include <stdlib.h>
#include <vector>


int* find(const vector<int> &vec, int value)
{
	for( int ix = 0 ; ix < vec.size(); ++ix)
	{
		if (vec[ ix ] == value)
		{
			return &vex[ ix ];
		}
	}

	return std::nullptr;
}

template<typename elemType>
elemType* find(const vector<elemType> &vec, 
				const elemType& value)
{
	for( int ix = 0 ; ix < vec.size() ; ++ix)
	{
		if ( vec[ix] == value)
		{
			return &vec[ix];
		}
	}

	return std::nullptr
}

template<typename elemType>
elemType* find(const elemType *array, int size,
				const elemType& value)
{
	if ( ! array || size < 1 )
	{
		return 0;
	}

	for ( int ix = 0 ; ix < size ; ++ix )
	{
		if ( vec[ix] == value )
		{
			return &vec[ix]
		}
	}

	return std::nullptr;
}

template<typename IteratorType, typename elemType >
IteratorType find(IteratorType first, IteratorType last, 
				const elemtType &value)
{
	for (; first != last; ++first)
	{
		if(*first == value)
		{
			return first;
		}
	}
	return last;
}


std::vector<std::string> svec;
std::vector<std::string>::iterator iter = svec.begin();


vector<int> less_than_10(const vector<int>& vec)
{
	vector<int> nvec;

	for(int ix = 0 ; ix < vec.size(); ix++)
	{
		if(vec[ix] < 10)
		{
			nvec.push_back(vec[ix]);

		}
	}
	return nvec;
}

vector<int> filter_ver1(const vector<int>& vec, 
					int filter_value,
					bool (*pred)( int, int))
{
	vector<int> nvec;

	for( int ix = 0 ; ix < vec.size(); ix++)
	{
		if(pred(vec[ix], filter_value))
		{
			nvec.push_back(vec[ix]);
		}
	}

	return nvec;
}


int count_occurs(const vector<int>& vec, int val)
{
	vector<int>::const_iterator iter = vec.begin();

	int occurs_count = 0;
	while((iter = find(iter, vec.end, val)) != vec.end())
	{
		++occurs_count;
		++iter;
	}

	return occurs_count;
}

std::vector<int> less_than_10(std::vector<int>& vec)
{
	std::vector<int> nvec;
	for( int ix = 0; ix < vec.size(); ++ix)
	{
		if(vec[ix] < 10)
		{
			nvec.push_back(vec[ix]);
		}
	}
	return nvec;
}

std::vector<int> less_than(std::vector<int>& vec, int level)
{

}


void main()
{
	const inst asize = 8;
	int ia[ asize ] = { 1, 1, 2, 3, 5, 8, 13, 21};

	int* pia = find( ia, ia+asize, 1024);
	
	vector<int> ivec;
	vector<int>::iterator it(ia, ia + asize); 
	it = find(ivec.begin, ivec.end(), 1024);

	list<int> ilist;
	list<int>::iterator iter(ia, ia + asize);
	iter = find(ilist.begin(), ilist.end(), 1024);
	
}


