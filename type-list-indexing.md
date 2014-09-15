Type List Indexing
==================

Document Number : NXXXX - 09-2014
Date            : 2014-09-XX
Project         : Programming Language C++, Evolution Working Group
Reply-to        : Sean Middleditch <sean@seanmiddleditch.com>

# Type List Indexing

## Summary

This proposal is in response to feedback from [N3761]. This proposal introduces a language change that
allows for easy and direct access to any member of a parameter pack using a constant expression integral
value.

This facility combined with `sizeof...`, `std::integer_sequence`, and `constexpr` functions would allow
for simpler transformations of parameter packs and algorithms without requiring the use of recursion
where iteration may be the preferred implementation approach.

## Motivation and Use Cases

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
	
## Example Syntax

The proposed syntax looks like:

	// context: template <typename ...Ts>
	using third_type = Ts...[2];
	
	// context: template <typename ...Ts> void function(Ts... ts)
	auto fourth_value = ts...[3];
	
## Standard Additions

**help needed**

## Acknowledgements

In comments on Richard Smith, a strawman syntax proposing a language approach as alternative to
N3761 was mentioned.

**help needed** This same syntax was floated in response to another paper submitted to the committee
sometime in the last year, but I have completely forgotten which paper or who made the suggestion,
and I'd like to give credit.

	
	
  [N3761]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3761.html "Proposing type_at<>"