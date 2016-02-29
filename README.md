# About

This repository contains an implementation of `<tuple>`, using concepts (mostly implemented in the oldschool SFINAE style), that attempts to fix bugs in the current standard, provide additional features to catch more bugs at compile time and make `tuple` feel more complete. Additionally, care was taken to avoid recursive templates resulting in improved compile times.

*Note: Unless specified otherwise, declarations live in the namespace `fcc` (e.g. if you see `tuple_size<T>` assume the README is referring to `::fcc::tuple_size<T>`).*

# Usage

**Currently requires gcc-6.0 or later**.

```sh
$ mkdir build && cd ./build
$ cmake -G"Unix Makefiles" ..
$ make -j8
$ ./src/main
```

The output contains a series of tables indicating whether a cv qualified r/l-value `tuple` is constructible/assignable from either the type contained in the `tuple`, or another instance of the `tuple` type with different r/l-value and cv qualifications. Highlighted in red are the tables for which the assignability/constructibility is not the same for all elements in that table *(red does not mean error)*.

The actual implementation of `tuple` is header only. To use `fcc::tuple` (not thoroughly tested) in your projects simply add the [include](include/) directory to your header search paths, and use it as you would `std::tuple`:

```cpp
#include <fcc/tuple.hpp>

// optional, makes the tuple operators work with all TupleLike types
using namespace fcc::operators;
// optional, makes tup[1_c] syntax work. ripped off from boost::hana
using namespace fcc::literals;

int main() {
    fcc::tuple<int, float> tup = fcc::make_tuple(5, 8.f);
    return 0;
}
```

# Differences with std::tuple

0. [TupleLike concept](#tuplelike-concept)

1. [Copy and Move Constructors](#copy-and-move-constructors)

2. [Braced-Init Constructors](#braced-init-constructors)

3. [Perfect Forwarding Assignment](#perfect-forwarding-assignment)

4. [Perfect Forwarding Swap](#perfect-forwarding-swap)

5. [Piecewise Constructors](#piecewise-constructors)

6. [noexcept and constexpr](#noexcept-and-constexpr)

7. [uses_allocator Constructors](#uses_allocator-constructors)

8. [size()](#size)

9. [get\<I\>](#geti)

10. [ignore, piecewise_construct, allocator_arg](#ignore-piecewise_construct-allocator_arg)

11. [Overload Resolution](#overload-resolution)

12. [Tuple Algorithms](#tuple-algorithms)

## TupleLike concept

Defining a `TupleLike` concept enabled `fcc::tuple` to be rewritten agnostic of other related types (pair, tuple, array, etc).

For a type, `T`, to be considered `TupleLike`, it must support the following:
 - `tuple_size<std::remove_reference_t<T>>::value` where `value` is of type `std::size_t`. This will be called `TSize` in the rest of the README.
 - `tuple_element<I, std::remove_reference_t<T>>::type` for every `std::size_t I` in the range `[0, TSize)`.
 - `get<I>(std::declval<T>())` must be `noexcept` and return a value of the type [`get_type<I, T>`](#get_typei-tuple) for every `std::size_t I` in the range `[0, TSize)`. Additionally, `get<I>` must be findable via ADL from an unspecified namespace. This roughly lines up with the current standard's idea of `TupleLike`.

*Note: The `noexcept` spec might change; one major reason it's a part of the concept is to simplify the definitions of construction, assignment and swap.*
 
### get_type\<I, Tuple\>

The definition of the type alias `get_type<I, Tuple>` matches the type of `decltype(get<I>(std::declval<Tuple>()))` according to the current standard, but adds support for `volatile tuple`'s and `const volatile tuple`'s. It is mentioned several times in this README so the definition is here for reference:

```cpp
template<typename T, typename As>
using as_same_const = std::conditional_t<
    std::is_const<As>::value,
    std::add_const_t<T>,
    T>;

template<typename T, typename As>
using as_same_volatile = std::conditional_t<
    std::is_volatile<As>::value,
    std::add_volatile_t<T>,
    T>;

template<typename T, typename As>
using as_same_cv = as_same_const<as_same_volatile<T, As>, As>;

// no-ref=rvalue ref
template<typename T, typename As>
using as_same_ref = std::conditional_t<
    std::is_lvalue_reference<As>::value,
    std::add_lvalue_reference_t<T>,
    std::add_rvalue_reference_t<T>>;
    
template<typename T, typename As>
using as_same_cvref =
    as_same_ref<
        as_same_cv<T, std::remove_reference_t<As>>,
        As>;

template<std::size_t I, typename T>
using get_type = as_same_cvref<tuple_element_t<I, std::remove_reference_t<T>>, T>;
```
 
### get\<T\> can be defined once

With this definition of `TupleLike` types, where `T` is some type in the `TupleLike` object, `get<T>` can be defined in one location for *all* `TupleLike` objects with relative ease as (roughly) follows:
```cpp
template<typename T, TupleLike Tup>
    requires detail::unique_in_tuple<T, Tup>::value // bool inidicating whether T, is unique in the
                                                    // tuple_element_t<I, Tup> typelist
// ::index represents the I specifiying where T is in the typelist created by tuple_element_t
constexpr get_type<detail::unique_in_tuple<T, Tup>::index, Tup>
get(Tup&& tup) noexcept
{ return get<detail::unique_in_tuple<T, Tup>::index>(std::forward<Tup>(tup)); }
```
 
### volatile tuples are TupleLike

It would be surprising to discover that a `volatile tuple<int>` is not `TupleLike`, but a `tuple<volatile int>` is `TupleLike`, as they should represent the same type (see [wandbox](http://melpon.org/wandbox/permlink/IcTq1KCAgXwFPYqz)).

```cpp
tuple<volatile int> tupleLike{5};
auto x = get<0>(tupleLike); // ok

volatile tuple<int> notTupleLike{5};
auto y = get<0>(notTupleLike); // oops (for current standard)
```

As a result, `volatile tuple`'s (and `const volatile tuple`'s) are fully supported in this implementation, and have `volatile`ness applied to certain operations in the same way that `const tuple<T>`'s have `const`ness applied to certain operations in the current standard (e.g. `get<I>`).

**This is a deeper concept.** For more on the deepitude, see Louis Dionne's blog post about [variants and tuples](http://ldionne.com/2015/07/14/empty-variants-and-tuples/). It's also somewhat interesting that a `const fcc::tuple<>` is assignable.

*Note: Embracing `volatile`, simplified most constructors, `get<I>` and `get<T>`, but slightly complicated assignment and member `swap`.*

### Implementing your own TupleLike types

Implementing a `TupleLike` type has a lot of benefits. `TupleLike` types have full use of [tuple algorithms](include/fcc/tuple/tuple_algorithms.hpp), [tuple operators](include/fcc/tuple/tuple_operators.hpp), [iostream input/output](include/fcc/tuple/tuple_io.hpp), `tuple_cat`, and seamless interaction with `fcc::tuple` (construction, assignment, etc).

*All* POD's are excellent candidates for `TupleLike` types, and if C++ get's reflection, `fcc` will make them `TupleLike` by default. Until then, here's some sample code to demonstrate how to turn your POD (or other, similar enough, type) into a `TupleLike` conformist.

```cpp
namespace a {
    struct my_struct { int x; double y; };
    
    template<std::size_t I, typename T>
        requires std::is_same<meta::uncvref<T>, my_struct>::value &&
                 (I < 2)
    constexpr fcc::get_type<I, T> get(T&& t) noexcept
    { return static_cast<fcc::get_type<I, T>>(get<I>(fcc::tie(t.x, t.y))); }
}

namespace fcc {
    template<>
    struct tuple_size<a::my_struct> : meta::size_t<2> {};
    
    template<>
    struct tuple_element<0, a::my_struct> : meta::id<int> {};
    
    template<>
    struct tuple_element<1, a::my_struct> : meta::id<double> {};
}
```

## Copy and Move Constructors

**Forward:** Copy and move construction, from the same `tuple` type, is `default`ed in the current standard. `fcc::tuple` removes those constructors from overload resolution *only* to simplify reasoning about which constructor gets called for a given set of arguments:

```cpp
tuple(const tuple& rhs) requires false = delete;
tuple(tuple&& rhs)      requires false = delete;
```

**Problem:** There are bugs in the current standard with regards to copy and move construction from `tuple`'s and `pair`'s of a similar type.

The current `TupleLike` copy/move constructors in the standard:

```cpp
tuple(const tuple<Us...>& rhs); // 0
tuple(tuple<Us...>&& rhs);      // 1
tuple(const pair<U0, U1>& rhs); // 2
tuple(pair<U0, U1>&& rhs);      // 3
```

Consider the following code in which an `int&` gets bound to a prvalue:
```cpp
// should compile according to current standard
std::tuple<int&> x{std::forward_as_tuple(9)}; // lvalue ref bound to literal
```

To understand why an lvalue ref winds up bound to a prvalue, start with the remarks clause [§20.4.2.1.20](http://eel.is/c++draft/tuple.cnstr#20) about the move constructor with comment `// 1`, which according the standard:
> This constructor shall not participate in overload resolution unless `std::is_constructible<Ti, Ui&&>::value` is `true` for all `i`

`std::is_constructible<int&, int&&>::value` is `false` so the constructor does not participate in overload resolution, but `// 0` is still a candidate because `tuple<Us...>&&` binds to `const tuple<Us...>&`.

The relevant part of the remarks clause [`§20.4.2.1.17`](http://eel.is/c++draft/tuple.cnstr#17) is as follows:
> This constructor shall not participate in overload resolution unless `std::is_constructible<Ti, const Ui&>::value` is `true` for all `i`.

`std::is_constructible<int&, std::add_rvalue_reference_t<std::add_const_t<int&>>>::value` is `true`, and the requires clause simply verifies that the two tuples have the same size. This constructor is selected, and what should be invalid code compiles. *Note: libc++ does the right thing here*

**Solution:** `fcc::tuple` resolves this by having a single perfectly forwarded constructor (actually two to maintain *perfect initialization*) for `TupleLike` types.

> `template<TupleLike Tup0> tuple(Tup0&& rhs);`

The constructor participates in overload resolution if, and only if:

> `std::is_constructible<tuple_element_t<I, TupleBeingConstructed>, get_type<I, RHSTuple&&>>::value` is `true` for all `std::size_t I` in the range `[0, TSize)`

**Alternate Solution:** `fcc::tuple` could have resolved this with minimal changes to the current standard by only *adding* constructors as follows:
```cpp
tuple(const tuple<Us...>& rhs); // 0
tuple(tuple<Us...>&& rhs);      // 1
tuple(const pair<U0, U1>& rhs); // 2
tuple(pair<U0, U1>&& rhs);      // 3

// new
tuple(tuple<Us...>&& rhs) = delete; // 4
tuple(pair<U0, U1>&& rhs) = delete; // 5
tuple(const tuple<Us...>&& rhs) = delete; // 6
tuple(const pair<U0, U1>&& rhs) = delete; // 7
```

Constructor 4 and constructor 5 have an opposite condition in the remarks clause from [`§20.4.2.1.20`](http://eel.is/c++draft/tuple.cnstr#20):
> This constructor shall not participate in overload resolution unless `std::is_constructible<Ti, Ui&&>::value` is **`false`** for **any** `i`

This would be sufficient to stop compilation before an rvalue reference to `tuple` has a chance to bind to a `const tuple&`.

*Note: In this example, constructors 6 and 7 always participate in overload resolution, though something smarter could be done. In many scenarios constructing a `tuple` from a `const tuple&&` should be valid (think construction of `tuple<int>` from `const tuple<int>&&`). However, if that's the path the standard wants to take, the solution used by `fcc::tuple` is a lot easier to write, and more complete as it handles [`volatile tuple`](#volatile-tuples-are-tuplelike)'s. Even if the standard didn't want to handle `volatile tuple`'s, perfect forwarding with a 'participates in overload resolution' clause that forbids `volatile` types would be simpler.*

## Braced-Init Constructors

**Forward:** The *Braced-Init Constructors* are the constructors having no template parameters (excluding possibly an allocator). These constructors allow for a much more terse syntax where values may be constructed in place without specifying types as in the following example:

```cpp
tuple<std::vector<int>, double> x{{0, 0, 0, 0}, 9.0};

tuple<std::vector<int>, double> foo()
{ return {{0, 0, 0, 0}, 0.0}; }
```

**Problem:** The current standard defines one such constructor (excluding allocator and `explicit` variants) which has very poor support for `tuple`'s containing references, and no support for move only types [`§20.4.2.1.6`](http://eel.is/c++draft/tuple#tuple.cnstr-6):

```cpp
// constructor in standard
EXPLICIT constexpr tuple(const Types&...);
```

Which means, according to the standard, this code is invalid ([wandbox](http://melpon.org/wandbox/permlink/F5VYhzObecbcWmYh)):

```cpp
// invalid
int x = 9;
tuple<std::vector<int>, int&&> a{{0, 0, 0, 0}, std::move(x)};
tuple<std::unique_ptr<int>> b{{nullptr}};
```

*Worse still*, any use of the current *Braced-Init Constructor* for `tuple`'s containing any non-reference types results in a guaranteed copy.

**Solution:** `fcc::tuple` defines a constructor taking `std::remove_cv_t<Types>...` with no added cv/ref qualifications.

```cpp
// constructor in fcc::tuple
EXPLICIT constexpr tuple(std::remove_cv_t<Types>...);
```

This constructor accepts move-only types, handles `tuple`'s of references, *and* it's very efficient! It should be noted that it's also very unlikely to be called, unless a user writes code including a non-explicitly typed braced-init list. The perfect forwarding constructors will almost always be preferred unless the arguments match `std::remove_cv_t<Types>...` exactly (modulo rvalue references). The only scenarios in which `std::remove_cv_t<Types>...` match the arguments exactly result in either moves of values or binding of references.

## Perfect Forwarding Assignment

**Problem:** Consider the following code in which the value of the literal `5` is apparently changed ([wandbox](http://melpon.org/wandbox/permlink/8CrRMJ0BmaEXv3Nw)):

```cpp
std::forward_as_tuple(5) = std::make_tuple(8);
```

libc++, stdc++, and Microsoft's STL accept this code which is undefined behavior.

The relevant part of the standard [`§20.4.2.2`](http://eel.is/c++draft/tuple.tuple#tuple.assign) treats all `tuple` assignment operations as having lvalue left hand side operands, roughly summarized as this:

> *Requires:* `is_assignable<Ti&, UTuple_i>::value == true` for all `i`.

The standard does not currently have any ref qualified assignment operators, though this was considered during C++11's standardization. `fcc::tuple` recognizes that `std::tuple/pair` is worthy of an exception, due to rvalue `tuple`'s being recommended as LHS operands in example code similar to the following:

```cpp
void my_class::some_function(int x, int y) {
    std::tie(my_x, my_y) = std::tie(x, y);
}
```

**Solution:** `fcc::tuple` accomplishes perfect forwarding assignment by first removing the default assignment operators from overload resolution:

```cpp
tuple& operator=(const tuple& rhs) requires false = delete;
tuple& operator=(tuple&& rhs)      requires false = delete;
```

Then by having a perfect forwarding assignment operator for every ref and cv qualification (play around with this here: [wandbox](http://melpon.org/wandbox/permlink/6XnfHGcmWtUe23ZC)):

```cpp
tuple& operator=(UTuple&& u) &;
tuple&& operator=(UTuple&& u) &&;
const Tuple& operator=(UTuple&& u) const &;
...
const volatile tuple&& operator=(UTuple&& u) const volatile &&;
```

These assignment operators participate in overload resolution if, and only if:

> `std::is_assignable<get_type<I, CVRefQualifiedLhsTuple>, get_type<I, UTuple&&>>::value` is `true` for all `std::size_t I` in the range `[0, TSize)`.

Where `CVRefQualifiedLhsTuple` is the type of the left hand side operand in the assignment operation.

To prevent promotion of, among other things, rvalues to lvalues (`T&&` to `const T&`), there are duplicate `delete`d assignment operators with the exact opposite overload resolution condition:

```cpp
tuple& operator=(UTuple&& u) & = delete;
tuple&& operator=(UTuple&& u) && = delete;
const Tuple& operator=(UTuple&& u) const & = delete;
...
const volatile tuple&& operator=(UTuple&& u) const volatile && = delete;
```

>  `std::is_assignable<get_type<I, CVRefQualifiedLhsTuple>, get_type<I, UTuple&&>>::value` is **`false`** for **any** `std::size_t I` in the range `[0, TSize)`.

Playing around with the [wandbox](http://melpon.org/wandbox/permlink/6XnfHGcmWtUe23ZC) link to understand this behavior is highly recommended.

*Note: The verbosity of this might be a motivation to consider non-member assignment operators post concepts-lite.*

## Perfect Forwarding Swap

**Problem:** In this example, `my_class` wants to expose a few of it's members to the world for modification, and `foo()` wants to take ownership of these members. Consider the following code:

```cpp
// cough up my_class's guts with this function
auto my_class::get_mem_pair()
{ return std::tie(m_vec, m_i); }

void foo()
{
    my_class a{...};
    
    std::vector<int> x{...};
    int y = ...;

    auto mem_pair = a.get_mem_pair();
    auto tied_xy = std::tie(x, y);
    std::swap(tied_xy, mem_pair);                // 0: ok
    std::swap(std::tie(x, y), a.get_mem_pair()); // 1: oops (for current standard)
    std::tie(x, y).swap(a.get_mem_pair());       // 2: oops (for current standard)
}
```

The reason the line marked `// 0` in the function `foo` is `ok`, but `// 1` and `//2` are an `oops` is because the current standard requires non-`const` lvalue references for both arguments to `swap` [`§20.4.2.9`](http://eel.is/c++draft/tuple.tuple#tuple.special):

> `template <class... Types> void swap(tuple<Types...>& x, tuple<Types...>& y)`

If `std::tie` solves the multiple return value assignment operation, why can't it also solve the multiple return value `swap` operation? It is arbitrary for `swap` to only support lvalue `tuple`'s. The `tuple swap` operation only depends on whether the [`get_type`](#get_typei-tuple)'s of both `tuple`'s satisfy the `Swappable` concept.

**Solution:** `fcc::tuple` properly handles `swap`'s by implementing a perfectly forwarded `swap` non-member function, as well as a perfectly forwarded `swap` member function (see [Perfect Forwarding Assignment](#perfect-forwarding-assignment) for the way the member function behaves).

Both the member, and non-member, `swap` functions participate in overload resolution if, and only if:

> `std::swap(std::declval<get_type<I, LhsTuple>>(), std::declval<get_type<I, RhsTuple>>())` is valid for all `std::size_t I` in the range `[0, TSize)`.

*Note: Code for the member `swap` could be completely removed with the unified call syntax in [N4474](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4474.pdf) resulting in much less code.*

## Piecewise Constructors

**Motivation:** The missing piecewise construction of `tuple`'s is a substantial functional difference in the standard between `std::tuple` and `std::pair`, and it's absence prevents construction of `tuple`'s containing unmovable, uncopyable types.

**Implementation:** `fcc::tuple` contains piecewise constructors similar to `std::pair` except they are perfectly forwarded, and use a modified *perfect-initialization*.

*Note: See [`uses_allocator` Constructors](#uses_allocator-constructors) for more on perfect-initialization when a type is being passed anything other than one argument.*

```cpp
// 0
template<typename... UTuples>
    requires (is_tuple_like<UTuples>::value && ...)
EXPLICIT constexpr tuple(piecewise_construct_t, UTuples&&... utuples);

// 1
template<Allocator Alloc, typename... UTuples>
    requires (is_tuple_like<UTuples>::value && ...)
EXPLICIT constexpr
tuple(piecewise_construct_t, allocator_arg_t, const Alloc& alloc, UTuples&&... utuples);
```

Constructor `0` participates in overload resolution if, and only if:

> `fcc::is_constructible<tuple_element_t<I, tuple>, get_type<Js, UTupleI>...>::value` is `true` for all `std::size_t I` in the range `[0, TSize)` where `Js` is equivalent to the parameter pack such that `std::is_same<std::integer_sequence<std::size_t, Js...>, std::make_index_sequence<tuple_size<UTupleI>::value>>::value` is `true`

Constructor `1` is identical to `0` except it uses [`uses_allocator`](#uses_allocator-constructors) construction.

## noexcept and constexpr

`fcc::tuple` deduces `noexcept` for all operations. Including the [uses_allocator Constructors](#uses_allocator-constructors).

`fcc::tuple` also uses `constexpr` for every member and non-member function in the library.

## uses_allocator Constructors

**Problem:** The `uses_allocator` constructors don't currently have much of an explanation in the standard. For 15 constructors it simply has two sentences [`§20.4.2.1.28`](http://eel.is/c++draft/tuple#tuple.cnstr-28):

> Requires: Alloc shall meet the requirements for an Allocator (17.6.3.5).
> Effects: Equivalent to the preceding constructors except that each element is constructed with uses-allocator construction.

Does this mean they are explicit under the same circumstances? Participate in overload resolution under the same conditions? Are they `noexcept` with the same parameters?

**Solution:** The core, functional, difference between `fcc::tuple` and the standard is that `noexcept` and *perfect-initialization* are explicitly stated as being based on the constructor selected by `uses_allocator` construction.

The exception specification is `noexcept`, if and only if:

> The constructors selected by `uses_allocator` construction are `noexcept` for all elements of the `tuple`.

*Perfect-initialization* for constructors taking anying other than one parameter is not defined in the current standard (and it's technically impossible to get 100% correct AFAIK). `fcc::tuple` considers such a constructor (`sizeof...(Ai) != 1`, where `Ai` are the types of the arguments to the constructor) to be implicit if and only if the following code is valid:

```cpp
template<typename T> void f(T) {}
f<T>({std::declval<Ai>()...});
```

and:

> `std::is_constructible<T, Ai...>::value` is `true`

Where `T` is the type being constructed and `Ai` is a parameter pack of the types of the arguments provided to the constructor of `T`.

***Bolded Note: This still fails to detect explicit constructors correctly due to some constructors overlapping with `std::initializer_list` constructors (e.g. `std::vector`). See [`generic_traits.hpp`](include/fcc/detail/generic_traits.hpp)***.

## size()

`size()` is a `constexpr` and `noexcept` [`const volatile`](#volatile-tuples-are-tuplelike) method returning the same value as `tuple_size<TupleType>::value`.

This was mostly added for convenience:

```cpp
decltype(auto) foo(auto tup)
{
    std::cout << "Calling 'bar' with "
              << tup.size()
              << " arguments." << std::endl;

    return apply(&bar, tup);
}
```

## get\<I\>

`get<I>(tuple)` is roughly identical to the current standard, except it is perfectly forwarded, and with that comes full support of `volatile tuple`'s.

```cpp
template<std::size_t I, FccTuple Tup>
    requires (I < tuple_size<std::remove_reference_t<Tup>>::value)
constexpr get_type<I, Tup> get(Tup&& tup) noexcept;
```

## ignore, piecewise_construct, allocator_arg

**Problem:** While looking at the following code, try to think what would be the most reasonable value for the float in the tuple ([wandbox](http://melpon.org/wandbox/permlink/kkpFt9AoWsUSScj6)):

```cpp
// though the standard doesn't support default initialization of the third parameter,
// both libc++ and libstdc++ do
tuple<double, float, double> x{{}, 8.0};
```

The reality is... it depends. libc++ says 0 (having used a `double` as an allocator) and libstdc++ fails to compile it.

The standard states that `piecewise_construct_t` and `allocator_arg_t` are essentially `struct {};`. The latter of which is inadvertently being created in the above example. libc++ does exactly what's written in the standard, and libstdc++ tries to avoid ambiguities and runtime errors.

**Solution:** `fcc::tuple` takes the libstdc++ approach and marks `explicit` the default constructors for `ignore`, `piecewise_constructor_t` and `allocator_arg_t`. This prevents ambiguities, and makes it more obvious that constructor calls using braced-init-list arguments, *actually* use the [Braced-Init Constructors](#braced-init-constructors).

## Overload Resolution

`fcc::tuple` properly constrains all `struct`'s, all `class`'s, all member functions, all non-member functions, all constructors, etc. This makes `fcc::tuple` totally SFINAE, and concept friendly. The `Allocator` concept is a bit looser than what the standard requires.

The [Tuple Algorithms](#tuple-algorithms) are still missing some concepts, but they are still constrained in the oldschool SFINAE fashion.

## Tuple Algorithms

**Motivation:** There's a ton of power in having algorithms to manipulate product types. The combinator `pretty`, which creates a pretty printable version of any `TupleLike` type, is defined in one line.

```cpp
tuple_cat(make_tuple((const char*)"("),
          intersperse((const char*)", ", std::forward<Tup>(tup)),
          make_tuple((const char*)")"))
```

The [iostream operators](include/fcc/tuple/tuple_io.hpp) themselves, are defined simply as `foldl`'s.

**Implementation:** The implementation for these algorithms is currently under heavy development.

Below is a basic overview of the current algorithms:

 - Each algorithm is implemented as a function object. This means some function objects must be template variables (e.g. `replicate<I>(val)`).
 - Each algorithm always forwards every argument atleast once (including `Callable`'s), typically the last time in which the argument is read. This increases support for move only types, and catches more r/lvalue errors.
 - Each algorithm works on any `TupleLike` type.
 
`fcc` currently implements the following algorithms:

 - `assign` - useful for implementing [Perfect Forwarding Assignment](#perfect-forwarding-assignment) on your own `TupleLike` types
 - `swap`
 - `head`
 - `tail`
 - `apply`
 - `join`
 - `replicate`
 - `for_each_elem`
 - `zip_with`
 - `zip`
 - `intersperse`
 - `foldl`
 - `foldr`
 - `pretty`
