Type List Indexing
==================

 - Document Number : NXXXX - 01-2014
 - Date            : 2015-01-XX
 - Project         : Programming Language C++, Evolution Working Group
 - Reply-to        : Sean Middleditch <sean@seanmiddleditch.com>

***

FIXME
-----

The example use case actually depends on more than just this feature. Either we need a different example,
this proposal needs to be expanded to make the current example feasible, or a separate paper needs to
be written.

Abstract
--------

This proposal is in response to feedback from [N3761]. This proposal introduces a language change that
allows for easy and direct access to any member of a parameter pack using a constant expression integral
value.

Motivation and Use Cases
------------------------

Various algorithms over parameter packs currently must be implemented as a recursive series of function
or template invocations. This is often not the most natural or obvious way of implementing the algorithm.

By allowing direct access to the element of a parameter pack by index, algorithms that need access to
specific elements or ranges of elements become easier to write and reason about.

Consider this contrived example:

    // empty list case
    template <typename Value>
    constexpr bool binary_search(Value& value) {
      // empty list
      return false;
    }
    
    // single element case
    template <typename Value, typename T>
    constepxr bool binary_search(Value& value, T& t) {
      return value == t;
    }

    // remaining cases
    template <typename Value, typename ...Ts> 
    constexpr bool binary_search(Value& value, Ts&... ts) { 
      constexpr auto pivot = sizeof...(ts) / 2; 
    
      if (value < ts...[pivot]) 
        return binary_search(value, ts...[std::make_index_sequence<pivot>]...); 
      else 
        return binary_search(value, ts...[pivot + std::make_index_sequence<sizeof(ts)... - pivot>]...); 
    }
    
Libraries such as the proposed [Boost.Hana](https://github.com/ldionne/hana) should also be easier to write
and maintain with this feature.

This facility combined with `sizeof...`, `std::integer_sequence`, and `constexpr` functions would also allow
for simpler transformations of parameter packs and algorithms without requiring the use of recursion where
iteration may be the preferred implementation approach.

Alternatives
------------

The alternatives include both the library-only options and more intricate language options.

### std::tuple

This option is library-only and does not require any additions to the standard.

A user can convert a parameter pack into a tuple and then use `std::get<>` to access the nth element.

This technique is a little verbose and is not immediately obvious to users of the language.

### type_at

A small library addition would provide `std::type_at<N, Ts...>::type` and `std::value_at<N>(pack...)`.
These can of course have C++14 style aliases such as `type_at_t`.

The original comments included a note by Bjarne Stroustrup that the sample implementation provided would
execute in O(N) compile time. `std::integer_sequence` provides some implementation insight on how to
reduce this algorithmic complexity. A library-only solution could at best achieve O(log(N)) complexity.

### Vendor intrinsic

Rather than introducing new user-facing syntax, a library-only interface like type_at could could
offered as part of the standard and vendors can implement that facility using a vendor-specific builtin
to achieve O(1) complexity.

This puts an ability into the vendor-provided library that cannot be portability reimplemented as
part of third-party STL alternatives such as Boost, EASTL, STLPort, or others, however.
	
Example Syntax
--------------

The proposed syntax looks like:

    template <typename ...Ts> struct test {
      using third_type = Ts...[2];
	};
    
    template <typename ...Ts>
	auto fourth_value(Ts... ts) {
      return ts...[3];
	}
	
Standard Document Additions
---------------------------

**help needed**

Acknowledgements
----------------

In comments [N3761], Richard Smith presented a strawman syntax proposing a language approach as
alternative to the library approach presented. I used that as a trampoline for this paper.

Bjarne Stroustrup illustrated the computational complexity problems with a pure library approach
compared to a core language approach.

Matheus Izvekov provided some key feedback on the original draft, including pointing out a bad
example in the code and mentioning Boost.Hana.
	
	
  [N3761]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3761.html "Proposing type_at<>"
