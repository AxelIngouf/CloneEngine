#include "NonRegressionTest.h"

#include <cstdlib>
#include <iostream>

#include "Core/reflection/StructMeta.h"
#include "Core/reflection/EnumMeta.h"
#include "Vector/Vector.h"

namespace ReflectionTest
{
	using namespace Core;

	STRUCT(Integer,
		FIELD(int, number)
	);

	STRUCT(Boolean,
		FIELD(bool, toggle)
	);

	STRUCT(Light,
		FIELD(char, red),
		FIELD(char, green),
		FIELD(char, blue)
	);

	// inheritance death diamond
	STRUCT(DiaT,
		FIELD(void*, voidPtr)
	);

	STRUCT(DiaL,
		BASE(Integer),
		OTHER_BASE(DiaT),
		FIELD(Light, light)
	);

	STRUCT(DiaR,
		BASE(Boolean),
		OTHER_BASE(DiaT),
		FIELD(float, amount)
	);

	STRUCT(DiaB,
		BASE(DiaL),
		OTHER_BASE(DiaR),
		FIELD(void*, intPtr) // todo: change to int* when ptr are supported
	);

	// straight deep inheritance
	STRUCT(First,
		FIELD(float, f),
		FIELD(int, i),
		FIELD(bool, b)
	);

	STRUCT(Second,
		BASE(First),
		FIELD(char, c)
	);

	STRUCT(Third,
		BASE(Second),
		FIELD(short, s)
	);

	STRUCT(Fourth,
		BASE(Third),
		FIELD(long long, l)
	);
	//STRUCT(First,
	//	FIELD(float, f = -1.2f),
	//	FIELD(int, i = 1 << 20),
	//	FIELD(bool, b = true)
	//);

	//STRUCT(Second,
	//	BASE(First),
	//	FIELD(char, c = ' ')
	//);

	//STRUCT(Third,
	//	BASE(Second),
	//	FIELD(short, s = 2)
	//);

	//STRUCT(Fourth,
	//	BASE(Third),
	//	FIELD(long long, l = ConstexprCustomHash("Hello World!"))
	//);

	// inheritance frenzy
	STRUCT(Test,
		FIELD(bool, isSet),
		FIELD(int, value)
	);

	STRUCT(Skip,
		BASE(Test)
	);

	STRUCT(Triple,
		BASE(First),
		OTHER_BASE(Skip),
		OTHER_BASE(DiaL),
		FIELD(int, count),
		FIELD(long long, hash),
		FIELD(int, other)
	);

	STRUCT(Full,
		BASE(Triple),
		FIELD(Third, third),
		FIELD(short, court),
		FIELD(DiaB, diab)
	);
}


int offset(void* object, void* field)
{
	return (int)((unsigned long long)field - (unsigned long long)object);
}

#define CHECK_SIZE(type)\
if (type::MetaData->size != sizeof(type))\
{\
	std::cout << "ERROR Reflection size for class " << # type << " is : " << type::MetaData->size << std::endl\
		<< "\tshould be : " << sizeof(type) << std::endl;\
}
#define CHECK_OFFSET(instance, instanceField)\
if (instance .MetaData->FindField(# instanceField).offset != offset(& instance, & instance. ## instanceField))\
{\
	std::cout << "ERROR Reflection offset for field " << instance.MetaData->name.text << "::" << # instanceField << " is : " << instance .MetaData->FindField(# instanceField).offset << std::endl\
		<< "\tshould be : " << offset(& instance, & instance. ## instanceField) << std::endl;\
}
#define CHECK_FIELD_COUNT(type, expected)\
{\
	int count = 0;\
	for (const StructureField& field : type::MetaData->GetAllFields())\
	{\
		(void)field;\
		count++;\
	}\
	if (count != expected)\
	{\
		std::cout << "ERROR Reflection number of field for class " << # type << " is : " << count << std::endl\
		<< "\tshould be : " << std::to_string(expected) << std::endl;\
	}\
}

namespace Core
{
	using namespace ReflectionTest;

	void CheckSize()
	{
		CHECK_SIZE(Integer);
		CHECK_SIZE(Boolean);
		CHECK_SIZE(Light);
		CHECK_SIZE(DiaT);
		CHECK_SIZE(DiaL);
		CHECK_SIZE(DiaR);
		CHECK_SIZE(DiaB);
		CHECK_SIZE(First);
		CHECK_SIZE(Second);
		CHECK_SIZE(Third);
		CHECK_SIZE(Fourth);
		CHECK_SIZE(Test);
		CHECK_SIZE(Skip);
		CHECK_SIZE(Triple);
		CHECK_SIZE(Full);
	}

	void CheckOffset()
	{
		Integer integer;
		CHECK_OFFSET(integer, number);

		Boolean boolean;
		CHECK_OFFSET(boolean, toggle);

		Light light;
		CHECK_OFFSET(light, red);
		CHECK_OFFSET(light, green);
		CHECK_OFFSET(light, blue);



		DiaT diat;
		CHECK_OFFSET(diat, voidPtr);

		DiaL dial;
		CHECK_OFFSET(dial, number);
		CHECK_OFFSET(dial, voidPtr);
		CHECK_OFFSET(dial, light);

		DiaR diar;
		CHECK_OFFSET(diar, toggle);
		CHECK_OFFSET(diar, voidPtr);
		CHECK_OFFSET(diar, amount);

		DiaB diab;
		CHECK_OFFSET(diab, number);
		//CHECK_OFFSET(diab, voidPtr);
		CHECK_OFFSET(diab, light);
		CHECK_OFFSET(diab, toggle);
		//CHECK_OFFSET(diab, voidPtr);
		CHECK_OFFSET(diab, amount);
		CHECK_OFFSET(diab, intPtr);



		First first;
		CHECK_OFFSET(first, f);
		CHECK_OFFSET(first, i);
		CHECK_OFFSET(first, b);

		Second second;
		CHECK_OFFSET(second, f);
		CHECK_OFFSET(second, i);
		CHECK_OFFSET(second, b);
		CHECK_OFFSET(second, c);

		Third third;
		CHECK_OFFSET(third, f);
		CHECK_OFFSET(third, i);
		CHECK_OFFSET(third, b);
		CHECK_OFFSET(third, c);
		CHECK_OFFSET(third, s);

		Fourth fourth;
		CHECK_OFFSET(fourth, f);
		CHECK_OFFSET(fourth, i);
		CHECK_OFFSET(fourth, b);
		CHECK_OFFSET(fourth, c);
		CHECK_OFFSET(fourth, s);
		CHECK_OFFSET(fourth, l);



		Test test;
		CHECK_OFFSET(test, isSet);
		CHECK_OFFSET(test, value);

		Skip skip;
		CHECK_OFFSET(skip, isSet);
		CHECK_OFFSET(skip, value);

		Triple triple;
		CHECK_OFFSET(triple, f);
		CHECK_OFFSET(triple, i);
		CHECK_OFFSET(triple, b);
		CHECK_OFFSET(triple, isSet);
		CHECK_OFFSET(triple, value);
		CHECK_OFFSET(triple, number);
		CHECK_OFFSET(triple, voidPtr);
		CHECK_OFFSET(triple, light);

		Full full;
		CHECK_OFFSET(full, f);
		CHECK_OFFSET(full, i);
		CHECK_OFFSET(full, b);
		CHECK_OFFSET(full, isSet);
		CHECK_OFFSET(full, value);
		CHECK_OFFSET(full, number);
		CHECK_OFFSET(full, voidPtr);
		CHECK_OFFSET(full, light);
		CHECK_OFFSET(full, third);
		CHECK_OFFSET(full, diab);
	}

	void CheckFieldCount()
	{
		CHECK_FIELD_COUNT(Integer, 1);
		CHECK_FIELD_COUNT(Boolean, 1);
		CHECK_FIELD_COUNT(Light, 3);
		CHECK_FIELD_COUNT(DiaT, 1);
		CHECK_FIELD_COUNT(DiaL, 3);
		CHECK_FIELD_COUNT(DiaR, 3);
		CHECK_FIELD_COUNT(DiaB, 7);
		CHECK_FIELD_COUNT(First, 3);
		CHECK_FIELD_COUNT(Second, 4);
		CHECK_FIELD_COUNT(Third, 5);
		CHECK_FIELD_COUNT(Fourth, 6);
		CHECK_FIELD_COUNT(Test, 2);
		CHECK_FIELD_COUNT(Skip, 2);
		CHECK_FIELD_COUNT(Triple, 11);
		CHECK_FIELD_COUNT(Full, 14);
	}

	void CheckDefault()
	{
		// todo: add default value to reflection
		//Fourth test;
		//if (test.b != true
		//	|| test.c != ' '
		//	|| test.s != 2
		//	|| test.i != 1 << 20
		//	|| test.f != -1.2f
		//	|| test.l != ConstexprCustomHash("Hello World!"))
		//{
		//	std::cout << "ERROR in Reflection default value" << std::endl;
		//}
	}

	void CheckFieldAccess()
	{
		Full full;
		void* fullPtr = &full;

		full.court = 12;
		short court = *(short*)Full::MetaData->FindField("court").GetFrom(&full);
		if (court != 12)
		{
			std::cout << "ERROR : StructureField::GetFrom(...) test failed" << std::endl;
		}

		full.court = 6;
		if (6 != Full::MetaData->FindField("court").GetValue<short>(full))
		{
			std::cout << "ERROR : StructureField::GetValue(...) test failed" << std::endl;
		}

		full.court = 8;
		if (8 != Full::MetaData->FindField("court").GetValue<short>(fullPtr))
		{
			std::cout << "ERROR : StructureField::GetValue(...) test 2 failed" << std::endl;
		}



		int value = 15;
		Full::MetaData->FindField("court").SetFor(&full, &value);
		if (full.court != value)
		{
			std::cout << "ERROR : StructureField::SetFor(...) test failed" << std::endl;
		}

		Full::MetaData->FindField("court").SetValue(full, -27);
		if (full.court != -27)
		{
			std::cout << "ERROR : StructureField::SetValue(...) test failed" << std::endl;
		}

		Full::MetaData->FindField("court").SetValue(fullPtr, 69);
		if (full.court != 69)
		{
			std::cout << "ERROR : StructureField::SetValue(...) test 2 failed" << std::endl;
		}



		full.light.red = '\0';
		full.light.green = '\127';
		full.light.blue = '\255';

		Light& light = Full::MetaData->FindField("light").GetValue<Light>(full);
		char green = Light::MetaData->FindField("green").GetValue<char>(light);
		if (green != '\127')
		{
			std::cout << "ERROR : StructureField::GetValue(...) test 3 failed" << std::endl;
		}

		((Structure*)Full::MetaData->FindField("light").type)->FindField("red").SetValue(*(Light*)Full::MetaData->FindField("light").GetFrom(&full), full.light.blue);
		if (full.light.blue != full.light.red
			|| full.light.blue != '\255')
		{
			std::cout << "ERROR : StructureField::SetValue(...) test 3 failed" << std::endl;
		}
	}

	void CheckFunction()
	{
		// todo: todo
	}

	void NonRegressionReflectionTest()
	{
		Core::InitReflection();

		CheckSize();

		if (Test::MetaData->FindField("random").IsValid())
		{
			std::cout << "ERROR Reflection finding non-existing field" << std::endl;
		}

		CheckOffset();
		CheckFieldCount();
		CheckDefault();
		CheckFieldAccess();

		CheckFunction();

		std::cout << "    >> finish reflection non regression testing" << std::endl;
	}
}
