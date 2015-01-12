Using Namespace Aliases
=======================

 - Document Number : NXXXX - 01-2014
 - Date            : 2015-01-XX
 - Project         : Programming Language C++, Evolution Working Group
 - Reply-to        : Sean Middleditch <sean@seanmiddleditch.com>

***

Abstract
--------

This proposal seeks to fix a small inconsistency in the language. Today, the `using` keyword introduces
aliases for both types and type templates. Unfortunately, namespace aliases have a completely different
syntax for introducing aliases. This difference is both unnecessary and can lead to confusion.

Motivation and Use Cases
------------------------

`using` introduces aliases in C++11 onward. Examples:

	// type alias
	using type = my_namespace::other_type;
	
	// alias template
	template <typename T>
	using dynamic_array = std::vector<T>;
	
	// dependent alias
	template <typename T> struct type {
	  using dependent = typename T::type;
	};
	
Namespace aliases are introduced with an entirely different syntax.

	// namespace alias (old)
	namespace foo = my_namespace::inner_namespace;
	
This is inconsistent. Type aliases are not introduced with `class` or `typename`. Alias templates are
not introduced with `template`. Worse, the `using` keyword originally hails from the namespace feature
of C++.

The paper [N1489] introduces the same feature as proposed here, in section 2.2.

Alternatives
------------

The proposed syntax has no ambiguities. However, should it be desired that namespace aliases via the
`using` keyword keep the explicit `namespace` reference in place, this should be done on the right side
of the `=` sign in order to be consistent with dependent type syntax as well as the modern recommendations
for `auto` which keep types on the right.

The alternative verbose syntax would look like:

	// namespace alias (alternative)
	using foo = namespace my_namespace::inner_namespace;
	
This is both more verbose than the current inconsistent syntax and unnecessary as there are no ambiguities.
	
Example Syntax
--------------

The proposed syntax looks like:

	// namespace alias (proposed)
	using foo = my_namespace::inner_namespace;
	
Standard Document Changes
-------------------------

Modify the grammar rules in **7.3.2p1** to insert the new syntax:

	namespace-alias-definition:  
		using identifier = qualified-namespace-specifier |  
		namespace identifier = qualified-namespace-specifier ;

Modify the examples in **7.3.2p3** to use the new, consistent syntax while also keeping an example of
the old syntax:

	namespace Company_with_very_long_name { /∗ ... ∗/ }
	using CWVLN = Company_with_very_long_name;
	using CWVLN = Company_with_very_long_name; // OK: duplicate
	using CWVLN = CWVLN;
	namespace CWVLN = Company_with_very_long_name; // OK: namespace may be used in place of using

Acknowledgements
----------------

The paper [N1489] by Bjarne Stroustrup and Gabriel dos Reis originally introduced this exact feature.

Gabriel dos Reis explains that the `using` namespace alias is not in the standard today mostly because
it got overlooked due to timing and the committee's workload on C++11:
	https://groups.google.com/a/isocpp.org/d/msg/std-proposals/xErw8Zwg2so/FayA-IKe2NEJ

	
  [N1489]: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2003/n1489.pdf "Templates aliases for C++"
