#pragma once 

#include <tuple>
#include <type_traits>
#include <vector>

namespace StT
{
	template<typename T, template<typename...> typename Template>
	constexpr bool IS_SPECIALISATION = false;

	template<template<typename...> typename Template, typename... Args>
	constexpr bool IS_SPECIALISATION<Template<Args...>, Template> = true;

	template<typename T, template<typename...> typename Template>
	concept IsSpecialisation = IS_SPECIALISATION<T, Template>;

	struct Any
	{
		template<typename T>
		constexpr operator T() const;
	};

	template<typename T>
	concept Aggregate = std::is_aggregate_v<T>;

	template<typename T, typename... Args>
	concept AggregateInitializable = requires { T{ std::declval<Args>()... }; };

	template<std::size_t I>
	using IndexedAny = Any;
	template<Aggregate T, typename Indices>
	struct AggregateInitializableFromIndices;
	template<Aggregate T, std::size_t... Is>
	struct AggregateInitializableFromIndices<T, std::index_sequence<Is...>> : std::bool_constant<AggregateInitializable<T, IndexedAny<Is>...>>
	{
	};

	template<typename T, size_t N>
	concept AggregateInitializibleWithNArgs = Aggregate<T> && AggregateInitializableFromIndices<T, std::make_index_sequence<N>>::value;

	template<Aggregate T, size_t N, bool CanInit>
	struct FieldCount : FieldCount<T, N + 1, AggregateInitializibleWithNArgs<T, N + 1>>
	{
	};

	template<Aggregate T, size_t N>
	struct FieldCount<T, N, false> : std::integral_constant<size_t, N - 1>
	{
	};

	template<Aggregate T>
	constexpr size_t FieldCount_v = FieldCount<T, 0, true>::value;

	template<typename T>
	concept IsStruct = std::is_class_v<T> && Aggregate<T>;

	#define CONCAT(a, b) a##b
	#define GENERATE_VAR(n) CONCAT(t, n)
	#define STRUCT_BIND_START auto [
	#define STRUCT_BIND_END ] = Struct;

	#define STRUCT_BIND_1 t1
	#define STRUCT_BIND_2 t1, t2
	#define STRUCT_BIND_3 t1, t2, t3
	#define STRUCT_BIND_4 t1, t2, t3, t4
	#define STRUCT_BIND_5 t1, t2, t3, t4, t5
	#define STRUCT_BIND_6 t1, t2, t3, t4, t5, t6
	#define STRUCT_BIND_7 t1, t2, t3, t4, t5, t6, t7
	#define STRUCT_BIND_8 t1, t2, t3, t4, t5, t6, t7, t8

	#define GET_BIND_MACRO(n) CONCAT(STRUCT_BIND_, n)
	#define STRUCT_BIND(N)                                                                                                                                         \
  STRUCT_BIND_START GET_BIND_MACRO(N)                                                                                                                          \
  STRUCT_BIND_END                                                                                                                                              \
  return std::make_tuple(GET_BIND_MACRO(N));

	#define GET_NTH_ELEMENT(I, N)                                                                                                                                  \
  STRUCT_BIND_START GET_BIND_MACRO(N)                                                                                                                          \
  STRUCT_BIND_END                                                                                                                                              \
  return std::get<I>(std::make_tuple(GET_BIND_MACRO(N)));

// This will make a struct that looks like:
/*
template<typename T>
struct StructUnpacker<T, N>
{
		// The amount of t1, t2, ... depends on N which is given as a parameter to the macro
		constexpr auto operator()(T Struct) { auto [t1, t2] = Struct; return std::make_tuple(t1, t2); }
};
*/
	#define STRUCT_UNPACKER(N)                                                                                                                                     \
  template<typename T>                                                                                                                                         \
  struct StructUnpacker<T, N>                                                                                                                                  \
  {                                                                                                                                                            \
    constexpr auto operator()(T Struct)                                                                                                                        \
    {                                                                                                                                                          \
      STRUCT_BIND(N)                                                                                                                                           \
    }                                                                                                                                                          \
    template<int I>                                                                                                                                            \
    static constexpr auto GetNthElement(T Struct)                                                                                                              \
    {                                                                                                                                                          \
      GET_NTH_ELEMENT(I, N)                                                                                                                                    \
    }                                                                                                                                                          \
  };

	template<typename T, size_t N>
	struct StructUnpacker
	{
		static_assert(N != 0, "Can't have 0 elements in a struct");
		// static_assert(false, "A new STRUCT_UNPACKER is necessary!");
	};

	STRUCT_UNPACKER(1)
		STRUCT_UNPACKER(2)
		STRUCT_UNPACKER(3)
		STRUCT_UNPACKER(4)
		STRUCT_UNPACKER(5)
		STRUCT_UNPACKER(6)
		STRUCT_UNPACKER(7)
		STRUCT_UNPACKER(8)

		template<IsStruct T>
	constexpr auto StructToTuple(T const& struct_);

	template<IsStruct T, typename NewType>
	constexpr std::vector<NewType> StructToTupleVectorImpl(std::vector<T> const& vec)
	{
		std::vector<NewType> newVec;
		newVec.reserve(vec.size());

		for (auto const& val : vec)
		{
			newVec.emplace_back(StructToTuple(val));
		}

		return newVec;
	}

	template<typename... Args, size_t... Is>
	constexpr auto StructToTupleImpl(std::tuple<Args...> const& tuple, std::index_sequence<Is...> const&)
	{
		#define GET_TYPE(x) std::remove_cvref_t<decltype(x)>
		#define CURR_ELEM std::get<Is>(tuple)
		#define CURR_ELEM_TYPE GET_TYPE(CURR_ELEM)

		return std::make_tuple(
			[&]()
			{
				if constexpr (IsStruct<CURR_ELEM_TYPE>)
				{
					return StructToTupleImpl(StructUnpacker<CURR_ELEM_TYPE, FieldCount_v<CURR_ELEM_TYPE>> {}(CURR_ELEM),
						std::make_index_sequence<FieldCount_v<CURR_ELEM_TYPE>> {});
				}
				else if constexpr (IsSpecialisation<CURR_ELEM_TYPE, std::vector>)
				{
					#define VECTOR_TYPE typename CURR_ELEM_TYPE::value_type
					if constexpr (IsStruct<VECTOR_TYPE>)
					{
						return StructToTupleVectorImpl < VECTOR_TYPE,
							decltype(StructToTupleImpl(StructUnpacker<VECTOR_TYPE, FieldCount_v<VECTOR_TYPE>> {}(std::declval<VECTOR_TYPE>()),
								std::make_index_sequence<FieldCount_v<VECTOR_TYPE>> {})) > (CURR_ELEM);
					}
					else
					{
						return CURR_ELEM;
					}
					#undef VECTOR_TYPE
				}
				else
				{
					return CURR_ELEM;
				}
			}()...);

		#undef CURR_ELEM
		#undef CURR_ELEM_TYPE
	}

	template<IsStruct T>
	constexpr auto StructToTuple(T const& struct_)
	{
		constexpr size_t fieldCount = FieldCount_v<T>;
		return StructToTupleImpl(StructUnpacker<T, fieldCount> {}(struct_), std::make_index_sequence<fieldCount> {});
	}

	template<IsStruct T>
	constexpr auto StructToTuple(std::vector<T> const& vec)
	{
		return StructToTupleVectorImpl<T, decltype(StructToTuple(std::declval<T>()))>(vec);
	}

	template<typename T, typename... Ts>
	constexpr T TupleToStruct(std::tuple<Ts...> const& tuple);

	template<typename T, typename... Ts>
	constexpr std::vector<T> TupleToStructVectorImpl(std::vector<std::tuple<Ts...>> const& vec)
	{
		std::vector<T> newVec;
		newVec.reserve(vec.size());

		for (auto const& tuple : vec)
		{
			newVec.push_back(TupleToStruct<T>(tuple));
		}

		return newVec;
	}

	template<typename T, typename... Ts, size_t... Is>
	constexpr T TupleToStructImpl(std::tuple<Ts...> const& tuple, std::index_sequence<Is...>)
	{
		#define CURR_ELEM std::get<Is>(tuple)
		#define CURR_TYPE std::remove_cvref_t<decltype(CURR_ELEM)>
		#define CURR_STRUCT_MEMBER_TYPE std::remove_cvref_t<decltype(StructUnpacker<T, FieldCount_v<T>>::template GetNthElement<Is>(std::declval<T>()))>
		return T{ [&]()
				  {
					if constexpr (IsSpecialisation<CURR_TYPE, std::tuple>)
					{
					  // Nested struct, we need to get the nested struct's type
					  // We can assume that T is our 'parent' struct, and that Is is the 'index' of the member in the 'parent' struct
					  return TupleToStructImpl<CURR_STRUCT_MEMBER_TYPE>(CURR_ELEM, std::make_index_sequence<FieldCount_v<CURR_STRUCT_MEMBER_TYPE>> {});
					}
					else if constexpr (IsSpecialisation<CURR_STRUCT_MEMBER_TYPE, std::vector>)
					{
						return TupleToStructVectorImpl<CURR_STRUCT_MEMBER_TYPE::value_type>(CURR_ELEM);
					}
					else
					{
					  return std::get<Is>(tuple);
					}
				  }()... };
		#undef CURR_ELEM
		#undef CURR_TYPE
		#undef CURR_STRUCT_MEMBER_TYPE
	}

	template<typename T, typename... Ts>
	constexpr T TupleToStruct(std::tuple<Ts...> const& tuple)
	{
		return TupleToStructImpl<T>(tuple, std::make_index_sequence<sizeof...(Ts)> {});
	}

	template<typename T, typename... Ts>
	constexpr std::vector<T> TupleToStruct(std::vector<std::tuple<Ts...>> const& vec)
	{
		std::vector<T> newVec;
		newVec.reserve(vec.size());

		for (auto const& tuple : vec)
		{
			newVec.push_back(TupleToStructImpl<T>(tuple, std::make_index_sequence<sizeof...(Ts)> {}));
		}

		return newVec;
	}
} // namespace StT