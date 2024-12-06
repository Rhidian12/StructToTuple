#include "StructToTuple.h"

#include <assert.h>
#include <string>

using namespace StT;

struct Foo
{
	int   A;
	float B;
};

struct Bar
{
	std::string A;
	Foo         B;
};

struct Baz
{
	bool A;
	int  B;
};

struct FooBar
{
	Baz              A;
	Baz              B;
	std::vector<Baz> C;
};

#define ASSERT(expr) static_assert(expr); assert(expr);

int main()
{
	// ===================================================================
	// ===================================================================
	//						TEST STRUCT TO TUPLE
	// ===================================================================
	// ===================================================================
	ASSERT(std::get<0>(StructToTuple(Foo{ 42, 85.5415f })) == 42);
	ASSERT(std::get<1>(StructToTuple(Foo{ 42, 85.5415f })) == 85.5415f);

	ASSERT(std::get<0>(StructToTuple(Bar{ "Hello World!", Foo{15, 42.f} })) == "Hello World!");
	ASSERT(std::get<0>(std::get<1>(StructToTuple(Bar{ "Hello World!", Foo{15, 42.f} }))) == 15);
	ASSERT(std::get<1>(std::get<1>(StructToTuple(Bar{ "Hello World!", Foo{15, 42.f} }))) == 42.f);

	ASSERT(std::get<0>(std::get<0>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))) == true);
	ASSERT(std::get<1>(std::get<0>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))) == 3);
	ASSERT(std::get<0>(std::get<1>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))) == false);
	ASSERT(std::get<1>(std::get<1>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))) == 5);
	ASSERT(std::get<0>(std::get<2>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))[0]) == true);
	ASSERT(std::get<1>(std::get<2>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))[0]) == 6);
	ASSERT(std::get<0>(std::get<2>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))[1]) == true);
	ASSERT(std::get<1>(std::get<2>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))[1]) == 9);



	// ===================================================================
	// ===================================================================
	//						TEST TUPLE TO STRUCT
	// ===================================================================
	// ===================================================================
	ASSERT(TupleToStruct<Foo>(std::tuple<int, float>(42, 85.5415f)).A == 42);
	ASSERT(TupleToStruct<Foo>(std::tuple<int, float>(42, 85.5415f)).B == 85.5415f);

	using BarTuple = std::tuple<std::string, std::tuple<int, float>>;

	ASSERT(TupleToStruct<Bar>(BarTuple("Hello World!", { 15, 42.f })).A == "Hello World!");
	ASSERT(TupleToStruct<Bar>(BarTuple("Hello World!", { 15, 42.f })).B.A == 15);
	ASSERT(TupleToStruct<Bar>(BarTuple("Hello World!", { 15, 42.f })).B.B == 42.f);

	using FooBarTuple = std::tuple<std::tuple<bool, int>, std::tuple<bool, int>, std::vector<std::tuple<bool, int>>>;

	ASSERT(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).A.A == true);
	ASSERT(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).A.B == 3);
	ASSERT(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).B.A == false);
	ASSERT(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).B.B == 5);
	ASSERT(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).C[0].A == true);
	ASSERT(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).C[0].B == 6);
	ASSERT(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).C[1].A == true);
	ASSERT(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).C[1].B == 9);

	return 0;
}
