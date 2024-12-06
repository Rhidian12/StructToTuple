# StructToTuple
A C++20 header-only library to convert Structs to Tuples, and Tuples to Structs!
Nested structs and tuples are also fully supported!

## Usage

Converting any Struct to a std::tuple is as simple as calling `StT::StructToTuple(struct)`
```cpp
struct FooBar
{
  int a;
  int b;
  int c;
};
auto [a, b, c] = StT::StructToTuple(FooBar{});
std::tuple<int, int, int> = StT::StructToTuple(FooBar{});
```

Converting any tuple to a struct is as simple as calling `StT::TupleToStruct<Struct>(tuple)`. Note that the Struct must be given as a template parameter
```cpp
struct FooBar
{
  int a;
  int b;
  int c;
};
FooBar foo = StT::TupleToStruct<FooBar>(std::tuple<int, int, int>());
```
