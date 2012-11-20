#include <windows.h>
#include <iostream>
#include <map>
#include <limits.h>
using namespace std;


#include <malloc.h>    
#include <stdio.h>

typedef BOOL (WINAPI *LPFN_GLPI)(
								 PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, 
								 PDWORD);
class HardwareProperties
{
public:
	int a;
public: 
	void foo(void )
	{
		BOOL done;
		BOOL rc;
		DWORD returnLength;
		DWORD procCoreCount;
		DWORD byteOffset;
		PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer, ptr;
		LPFN_GLPI Glpi;

		Glpi = (LPFN_GLPI) GetProcAddress(
			GetModuleHandle(TEXT("kernel32")),
			"GetLogicalProcessorInformation");
		if (NULL == Glpi) 
		{
			printf(
				("GetLogicalProcessorInformation is not supported.\n"));
			return;
		}

		done = FALSE;
		buffer = NULL;
		returnLength = 0;

		while (!done) 
		{
			rc = Glpi(buffer, &returnLength);

			if (FALSE == rc) 
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
				{
					if (buffer) 
						free(buffer);

					buffer=(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
						returnLength);

					if (NULL == buffer) 
					{
						printf(("Allocation failure\n"));
						return ;
					}
				} 
				else 
				{
					printf(("Error %d\n"), GetLastError());
					return ;
				}
			} 
			else done = TRUE;
		}

		procCoreCount = 0;
		byteOffset = 0;

		ptr=buffer;
		while (byteOffset < returnLength) 
		{
			switch (ptr->Relationship) 
			{
			case RelationProcessorCore:
				procCoreCount++;
				break;

			default:
				break;
			}
			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			ptr++;
		}

		printf(("Number of active processor cores: %d\n"), 
			procCoreCount);
		free (buffer);
	}
};

   

class EmptyClass
{
};
class Base
{
public: Base(int a)
		{
		};
		//a is passed by value and b is passed by reference 
		//The semantics [ Rules specifying the meaning of a syntactically correct construct ] of function
		//call is to pass a copy of the argument.  This copy operation is defined by the types copy constructor.
		double power(double a, unsigned int &b)
		{
			double ans=a;
			do{ ans *=a; b--;}while(b>1);

			return ans;
		}

};
class Derived :public Base
{
public: Derived() 
			: Base(1) //Base Initializer
		{
		};
		double fn()
		{
			unsigned int b=8;
			double ans = power(2,b);//Passing b by value
			unsigned int c= b;
			ans = power(2,b); //Passing b by reference
			c=b; //Since b is decremented in the consitional loop, we expect b==1;
			return ans;
		}
};

//The reinterpret_cast operator also allows any integral type to be converted into any pointer type and vice versa. 
//Misuse of the reinterpret_cast operator can easily be unsafe. Unless the desired conversion is inherently low-level, you should use one of the other cast operators.
//The reinterpret_cast operator can be used for conversions such as char* to int*, or One_class* to Unrelated_class*, which are inherently unsafe.
//The result of a reinterpret_cast cannot safely be used for anything other than being cast back to its original type. Other uses are, at best, nonportable.
//The reinterpret_cast operator cannot cast away the const, volatile, or __unaligned attributes. See const_cast Operator for information on removing these attributes.
//The reinterpret_cast operator converts a null pointer value to the null pointer value of the destination type.
//One practical use of reinterpret_cast is in a hash function, which maps a value to an index in such a way that two distinct values rarely end up with the same index. 
//This is not a 1:1 hash - so we expect collisions.
unsigned short Hash( void *p ) 
{
	//The reinterpret_cast allows the pointer to be treated as an integral type. 
	//The result is then bit-shifted and XORed with itself to produce a unique index (unique to a high degree of probability). 
	//The index is then truncated by a standard C-style cast to the return type of the function.

	//unsigned int val = reinterpret_cast<unsigned int>( p );  error C2440: 'reinterpret_cast' : cannot convert from 'void *' to 'unsigned int' on x86
	//At this moment there is no standard integer type that is guaranteed to
	//be 64-bits long in C++. __int64 and long long are non-standard
	//extensions. Neither one is guaranteed to be supported by all C++ compilers.
#ifdef _WIN64 
	__int64 val = reinterpret_cast<__int64>( p );
#else
	unsigned int val = reinterpret_cast<unsigned int>( p ); 
#endif
	long long val_ll = reinterpret_cast<long long>(p);

	return ( unsigned short )( val ^ (val >> 16));
}

//This is 1:1 for 32bit word size
unsigned int Hash32( void *p ) 
{
#ifdef _WIN64 
	__int64 val = reinterpret_cast<__int64>( p );
#else
	unsigned int val = reinterpret_cast<unsigned int>( p ); 
#endif
	long long val_ll = reinterpret_cast<long long>(p);

	return (unsigned int)( val ^ (val >> 16));
}

//Can_copy checks (at compile time) that a T1 can be assigned to a T2. 
//Can_copy<T,Shape*> checks that T is a Shape* or a pointer to a class publicly
//derived from Shape or a type with a user-defined conversion to Shape*. 
//Note that the definition is close to minimal: 
//one line to name the constraints to be checked and the types for which to check them 
//one line to list the specific constraints checked (the constraints() function) 
//one line to provide a way to trigger the check (the constructor) 
//Note also that the definition has the desirable properties that 
//You can express constraints without declaring or copying variables, 
//thus the writer of a constraint doesn't have to make assumptions about how a 
//type is initialized, whether objects can be copied, destroyed, etc. 
//(unless, of course, those are the properties being tested by the constraint) 
//No code is generated for a constraint using current compilers 
//No macros are needed to define or use constraints 
//Current compilers give acceptable error messages for a failed constraint, 
//including the word "constraints" (to give the reader a clue), the name of the constraints, 
//and the specific error that caused the failure (e.g. "cannot initialize Shape* by double*") 
template<class T1, class T2> struct Can_copy {
	static void constraints(T1 a, T2 b) { T2 c = a; b = a; }
	Can_copy() { void(*p)(T1,T2) = constraints; }
};
template<class T, class B> struct Derived_from {
	static void constraints(T* p) { B* pb = p; }
	Derived_from() { void(*p)(T*) = constraints; }
};

template<class T1, class T2 = T1> struct Can_compare {
	static void constraints(T1 a, T2 b) { a==b; a!=b; a<b; }
	Can_compare() { void(*p)(T1,T2) = constraints; }
};

template<class T1, class T2, class T3 = T1> struct Can_multiply {
	static void constraints(T1 a, T2 b, T3 c) { c = a*b; }
	Can_multiply() { void(*p)(T1,T2,T3) = constraints; }
};


struct B { };
struct D : B { };
struct DD : D { };
struct X { };

int main()
{
	HardwareProperties s;
	s.foo();

	Derived_from<D,B>();
	Derived_from<DD,B>();
	//Derived_from<X,B>();//error C2440: 'initializing' : cannot convert from 'X *' to 'B *'
	//Derived_from<int,B>();error C2440: 'initializing' : cannot convert from 'int *' to 'B *'
	//Derived_from<X,int>();//error C2440: 'initializing' : cannot convert from 'X *' to 'int *'	c:\kl\study\study\study.cpp	62	Study


	Can_compare<int,float>();
	//Can_compare<X,B>(); error C2676: binary '==' : 'X' does not define this operator or a conversion to a type acceptable to the predefined operator
	Can_multiply<int,float>();
	Can_multiply<int,float,double>();
	//Can_multiply<B,X>();//error C2676: binary '*' : 'B' does not define this operator or a conversion to a type acceptable to the predefined operator

	Can_copy<D*,B*>();
	//Can_copy<D,B*>();//error C2440: 'initializing' : cannot convert from 'D' to 'B *'
	//Can_copy<int,B*>();error C2440: '=' : cannot convert from 'int' to 'B *'.
	//Conversion from integral type to pointer type requires reinterpret_cast, C-style cast or function-style cast

	//Can_copy<reinterpret_cast<B*>(int),B*>;  //This does not work

	int* a ;B b1; B* pb=&b1;B b2; a=(int*)&b2;
	B* pb2 = reinterpret_cast<B*>(a); //Now a points to b2

	//int array[INT_MAX-1];//error C2148: total size of array must not exceed 0x7fffffff bytes
	//int array[0x7ffffffe/sizeof(int)];// fatal error C1126: automatic allocation exceeds 2G
	//int bigArraySize = 0x7ffffffe/(2*sizeof(int) );
	int bigArraySize = 1024*1024;
	//int* bigArray = new int[0x7ffffffe/sizeof(int)]; std::bad_alloc at memory location 0x0018f9a8
	int* bigArray = new int[bigArraySize];

	//int array[0x7ffffffe];//;_chkstk - check stack upon procedure entry
	//;       Provide stack checking on procedure entry. Method is to simply probe
	//;       each page of memory required for the stack in descending order. This
	//;       causes the necessary pages of memory to be allocated via the guard
	//;       page scheme, if possible. In the event of failure, the OS raises the
	//;       _XCPT_UNABLE_TO_GROW_STACK exception.

	int array[USHRT_MAX]; //sizeof(short) =2 , 2 bytes = 16bits 2^16 = 65535 = 0x FF FF
	int maxint = INT_MAX; //2147483647
	map<unsigned short, int> hashMap;
	typedef std::map<unsigned short, int> hashMapType;
	hashMap.insert(  pair<unsigned short,int>(Hash(array),0)   );		
	try
	{
		pair<unsigned short,int> duplicate(Hash(array),0);
		std::map<unsigned short, int>::iterator iter  =hashMap.find(duplicate.first);
		if (iter != hashMap.end() ) 
		{
			std::cout << "Value is: " << iter->second << '\n';
			cout<<"Houston, we have a hash collision"<<endl;
		}
		else
		{
			std::cout << "Key is not in map" << '\n';
		}		   
		hashMap.insert( duplicate    );
	}
	catch(std::exception e)
	{
		cout<<e.what()<<endl;
	}

	for ( int i = 0; i < USHRT_MAX; i++ )
	{
		//cout <<"Val = "<<array[i]<<"   "<<"Hash = "<< Hash( array + i ) << endl;
		hashMap.insert(  pair<unsigned short,int>(Hash(array+i),i)   );		    
	}
	//If we get here then there are no collisions and we are going to try and get one with a bigger set.
	//note the word size is 2x on x64 platform, so we greatly decrease the probability of a collision n 64bit build.
	hashMap.clear();
	for ( int i = 0; i < bigArraySize; i++ )
	{
		pair<unsigned short,int> nextValue(Hash(bigArray+i),i); 
		std::map<unsigned short, int>::iterator iter  =hashMap.find(nextValue.first);
		if (iter != hashMap.end() ) 
		{
			std::cout << "Value is: " << iter->second << '\n';
			cout<<"Houston, we have a hash collision"<<endl;
			break;
		}

		//cout <<"Val = "<<array[i]<<"   "<<"Hash = "<< Hash( array + i ) << endl;
		hashMap.insert(  pair<unsigned short,int>(Hash(bigArray+i),i)   );
		if(i%(bigArraySize/4)== 0)
			cout<<i%(bigArraySize/4)<<endl;

	}				
	hashMap.clear();		
	map<unsigned int,int> hashMap32;
	for ( int i = 0; i < bigArraySize; i++ )
	{
		pair<unsigned int,int> nextValue(Hash32(bigArray+i),i); 
		std::map<unsigned int, int>::iterator iter  =hashMap32.find(nextValue.first);
		if (iter != hashMap32.end() ) 
		{
			std::cout << "Value is: " << iter->second << '\n';
			cout<<"Houston, we have an x64 hash collision"<<endl;
		}
		hashMap32.insert( nextValue   );
	}        
	EmptyClass e;
	size_t sizeOfEmpty = sizeof(e);
	Derived d;
	d.fn();
	delete[] bigArray;
	return 0;
}

