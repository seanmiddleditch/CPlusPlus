Type List Indexing
==================

 - Document Number : NXXXX - 09-2014
 - Date            : 2014-09-XX
 - Project         : Programming Language C++, Evolution Working Group
 - Reply-to        : Sean Middleditch <sean@seanmiddleditch.com>

***

Summary
-------

This proposal is in response to feedback from [N3761]. This proposal introduces a language change that
allows for easy and direct access to any member of a parameter pack using a constant expression integral
value.

This facility combined with `sizeof...`, `std::integer_sequence`, and `constexpr` functions would allow
for simpler transformations of parameter packs and algorithms without requiring the use of recursion
where iteration may be the preferred implementation approach.

Motivation and Use Cases
------------------------

Various algorithms over parameter packs currently must be implemented as a recursive series of function
or template invocations. This is often not the most natural or obvious way of implementing the algorithm.

By allowing direct access to the element of a parameter pack by index, loops may be written over the
pack. Example:

    template <typename ...Ts>
    constexpr bool all(Ts&&... ts)
    {
      // check if any member of ts evaluates to false
      for (auto index = 0U; index != sizeof...(ts); ++index)
        if (!ts...[index])
    	  return false;
    	  
      // either all members evaluated to true or there were no members; only succeed in former case
      return sizeof...(ts) != 0;
    }

There are also cases where a user may only wish to iterate over part of a parameter pack such as when
performing a binary search. This case is far more difficult to do with recursive template expansion
than it would be if direct index access were allowed.

Alternatives
------------

The alternatives are the option presented in [N3761] or its alternatives.

### std::tuple

A user can convert a parameter pack into a tuple and then use `std::get<>` to access the nth element.

This technique is a little verbose and is not immediately obvious. This will contribute to the general
consensus that C++ template programming is difficult and only for experts.

### type_at

A small library addition would provide `std::type_at<N, Ts...>::type` and `std::value_at<N>(pack...)`.
These can of course have C++14 style aliases such as `type_at_t`.

The original comments included a note by Bjarne Stroustrup that the sample implementation provided would
execute in O(N) compile time. `std::integer_sequence` provides some implementation insight on how to
reduce this to O(log(N)) or that an implementation could use vendor-specific built-ins to implement this
in O(1).

This proposal provides O(1) access using a standard syntax in place of an implementation-specific
feature backing a library interface.
	
Example Syntax
--------------

The proposed syntax looks like:

    // context: template <typename ...Ts>
    using third_type = Ts...[2];
    
    // context: template <typename ...Ts> void function(Ts... ts)
    auto fourth_value = ts...[3];
	
Standard Document Additions
---------------------------

**help needed**

Acknowledgements
----------------

In comments [N3761], Richard Smith presented a strawman syntax proposing a language approach as
alternative to the library approach presented. I used that as a trampoline for this paper.

Bjarne Stroustrup illustrated the computational complexity problems with a pure library approach
compared to a core language approach.

**help needed** This same syntax was floated in response to another paper submitted to the committee
sometime in the last year, but I have completely forgotten which paper or who made the suggestion,
and I'd like to give credit.

	
	
  [N3761]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3761.html "Proposing type_at<>"
