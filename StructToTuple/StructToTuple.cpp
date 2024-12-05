﻿#include "StructToTuple.h"

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

static_assert(std::is_same_v<decltype(StructToTuple(Foo{})), std::tuple<int, float >> );
static_assert(std::is_same_v<decltype(StructToTuple(Bar{})), std::tuple<std::string, std::tuple<int, float>> > );
static_assert(std::is_same_v<decltype(StructToTuple(FooBar{})), std::tuple<std::tuple<bool, int>, std::tuple<bool, int>, std::vector<std::tuple<bool, int>> >> );

int main()
{
	// ===================================================================
	// ===================================================================
	//						TEST STRUCT TO TUPLE
	// ===================================================================
	// ===================================================================
	assert(std::get<0>(StructToTuple(Foo{ 42, 85.5415f })) == 42);
	assert(std::get<1>(StructToTuple(Foo{ 42, 85.5415f })) == 85.5415f);

	assert(std::get<0>(StructToTuple(Bar{ "Hello World!", Foo{15, 42.f} })) == "Hello World!");
	assert(std::get<0>(std::get<1>(StructToTuple(Bar{ "Hello World!", Foo{15, 42.f} }))) == 15);
	assert(std::get<1>(std::get<1>(StructToTuple(Bar{ "Hello World!", Foo{15, 42.f} }))) == 42.f);

	assert(std::get<0>(std::get<0>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))) == true);
	assert(std::get<1>(std::get<0>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))) == 3);
	assert(std::get<0>(std::get<1>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))) == false);
	assert(std::get<1>(std::get<1>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))) == 5);
	assert(std::get<0>(std::get<2>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))[0]) == true);
	assert(std::get<1>(std::get<2>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))[0]) == 6);
	assert(std::get<0>(std::get<2>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))[1]) == true);
	assert(std::get<1>(std::get<2>(StructToTuple(FooBar{ Baz{true, 3}, Baz{false, 5}, std::vector<Baz>{{true, 6}, {true,9} } }))[1]) == 9);



	// ===================================================================
	// ===================================================================
	//						TEST TUPLE TO STRUCT
	// ===================================================================
	// ===================================================================
	assert(TupleToStruct<Foo>(std::tuple<int, float>(42, 85.5415f)).A == 42);
	assert(TupleToStruct<Foo>(std::tuple<int, float>(42, 85.5415f)).B == 85.5415f);

	using BarTuple = std::tuple<std::string, std::tuple<int, float>>;

	assert(TupleToStruct<Bar>(BarTuple("Hello World!", { 15, 42.f })).A == "Hello World!");
	assert(TupleToStruct<Bar>(BarTuple("Hello World!", { 15, 42.f })).B.A == 15);
	assert(TupleToStruct<Bar>(BarTuple("Hello World!", { 15, 42.f })).B.B == 42.f);

	using FooBarTuple = std::tuple<std::tuple<bool, int>, std::tuple<bool, int>, std::vector<std::tuple<bool, int>>>;

	assert(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).A.A == true);
	assert(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).A.B == 3);
	assert(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).B.A == false);
	assert(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).B.B == 5);
	assert(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).C[0].A == true);
	assert(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).C[0].B == 6);
	assert(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).C[1].A == true);
	assert(TupleToStruct<FooBar>(FooBarTuple({ true, 3 }, { false, 5 }, { {true, 6}, { true, 9 } })).C[1].B == 9);

	return 0;
}
