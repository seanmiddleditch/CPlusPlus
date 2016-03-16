# Flat Containers Redux

- PNNNN
- LEWG, SG14
- Date: 2016-03-14
- Reply-to: Sean Middleditch <sean@seanmiddleditch.com>

## Introduction

This paper is a followup to the previous flat containers paper that confirms design decisions and proposes wording for the standard.

## Design

### Previously Open Questions

#### Standalone Container

Feedback from Kona indicted that the committee prefers standalone containers rather than an adaptor. This paper proposes the new containers flat_map, flat_set, flat_multimap, and flat_multiset.

#### Reference Proxies

For the flat_map containers, a design question is whether element access returns a pair<Key,Value> or a pair<Key&,Value&>, where pair may be a distinct type from std::pair. Returning a pair of references allows the keys and value to be stored separately in memory and keeping keys more tightly packed.

An alternative approach is to break with the std::flat_map-style interface and not return a pair at all, but rather for access to only return keys or values. This allows many of the performance advantages of reference proxy access without incuring the problems that C++ currently suffers with reference proxies.

#### Exception Guarantee

LEWG feedback accepted supporting only the basic exception guarantee. No alternative design to provide the strong guarantee has been discovered.

#### Delayed Insert and Sort

Delayed insertion is very difficult to make safe and performant. For example, with a flat_set (unique elements only) any insertion operation is required to reject duplicates. Using delayed insert, this would require O(N) searching of the unsorted delayed values.

The primary purpose of delayed insert is to support high-performance batch input, e.g. in deserialization. As high-performance deserialization will generally already work with batches of data, the range-based insertion operations for the container should offer sufficience performance. Relying on those higher-level primitives might also allow the implementation to more easily determine the insertion algorithm (insertion sort, quick sort, etc.) and make capacity growth decisions.

#### Sorted or Level-ordered

Early indications are that level-ordered storage of elements results in improved performance for search operations, impacting both find and insert. A potential pessimation with a level-ordered approach involved ordered operations like set_union.

Iteration must be ordered in either case to maintain one of the useful properties of flat containers, and interfaces for other operations have no dependency on the core algorithm. The only public consequence of choice of algorithm is whether the iterators are contiguous, as used by some of the hashing proposals. The use cases of contiguous iterators of a flat container are small enough that it's not necessary to restrict implementations to an algorithm based purely on iteration model.

### Proposed Design

We propse a new set of associative containers. These containers mandate contiguous allocation with growing capacity similar to vector. Iteration of element must be ordered. The underlying algorithm is implementation defined. The interface will be compatible with existing ordered associative containers where it makes sense to do so, but will break compatibility as necessary.

## Wording

    23.X Flat Containers [flat]
	
	23.X.1 In Generanl [flat.general]
	
	The header <flat_map> defines the class templates flat_map and flat_multimap. The header <flat_set> defines the class templates flat_set and flat_multiset.
	
	23.X.2 Header <flat_map> synopsis [flat.flat_map.syn]
	
	#include <initializer_list>

	namespace std {

		// 23.X.4, class template flat_map:
		template <class Key,
			class T,
			class Compare = less<Key>,
			class Alloc = std::allocator<std::pair<const Key, T> > >
		  class flat_map;

		// 23.X.5, class template flat_multimap:
		template <class Key,
			class T,
			class Compare = less<Key>
			class Alloc = std::allocator<std::pair<const Key, T> > >
		  class flat_multimap;

		template <class Key, class T, class Compare, class Alloc>
		  void swap(flat_map<Key, T, Compare, Alloc>& x,
					flat_map<Key, T, Compare, Alloc>& y);

		template <class Key, class T, class Compare, class Alloc>
		  void swap(flat_multimap<Key, T, Compare, Alloc>& x,
					flat_multimap<Key, T, Compare, Alloc>& y);

		template <class Key, class T, class Compare, class Alloc>
		  bool operator==(const flat_map<Key, T, Compare, Alloc>& a,
						  const flat_map<Key, T, Compare, Alloc>& b);

		template <class Key, class T, class Compare, class Alloc>
		  bool operator!=(const flat_map<Key, T, Compare, Alloc>& a,
						  const flat_map<Key, T, Compare, Alloc>& b);

		template <class Key, class T, class Compare, class Alloc>
		  bool operator==(const flat_multimap<Key, T, Compare, Alloc>& a,
						  const flat_multimap<Key, T, Compare, Alloc>& b);

		template <class Key, class T, class Compare, class Alloc>
		  bool operator!=(const flat_multimap<Key, T, Compare, Alloc>& a,
						  const flat_multimap<Key, T, Compare, Alloc>& b);
	} // namespace std
	
	23.X.3 Header <flat_set> synopsis [flat.set.syn]
	
	TODO
	
	23.X.4 Class template flat_map [flat.flat_map]
	
	23.X.4.1 Class template flat_map overview [flat.flat_map.overview]
	
	A flat_map is an associative container that supports unique keys (contains at most one of each key value) and provides for reasonably fast retrieval of values of another type T based on the keys while guaranting that keys and values are allocated contiguously. The flat_map class supports bidirectional iterators.
	
	A flat_map satisfies all of the requirements of a container, of an associative container (TODO: really?), and of an allocator-aware container.

	namespace std {
		template <class Key, class T, class Compare = less<Key>,
				  class Allocator = allocator<pair<const Key, T>>>
		class flat_map {
		public:
			// types:
			using key_type = Key;
			using mapped_type = T;
			using key_compare = Compare;
			using allocator_type = Allocator;
			using pair_type = pair<const Key, T>;
			using reference = T&;
			using const_reference = const T&;
			using iterator = implementation-defined; // See 23.2
			using const_iterator = implementation-defined; // See 23.2
			using size_type = implementation-defined; // See 23.2
			using difference_type = implementation-defined; // See 23.2
			using pointer = typename allocator_traits<Allocator>::pointer;
			using const_pointer = typename allocator_traits<Allocator>::const_pointer;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;
			
			explicit flat_map(const Compare& comp = Compare(),
							  const Allocator& = Allocator());
			template <class InputIterator>
			  flat_map(InputIterator first, InputIterator last,
					   const Compare& comp = Compare(), const Allocator& = Allocator());
			flat_map(const flat_map&);
			flat_map(flat_map&&);
			explicit flat_map(const Allocator&);
			flat_map(const flat_map&, const Allocator&);
			flat_map(flat_map&&, const Allocator&);
			flat_map(InputIterator first, InputIterator last, const Allocator& a)
			  : flat_map(first, last, Compare(), a) { }
			flat_map(initializer_list<pair_type> il, const Allocator& a)
			  : flat_map(il, Compare(), a) { }
			~flat_map();
			flat_map& operator=(const flat_map& x);
			flat_map& operator=(flat_map&& x);
			flat_map& operator=(initializer_list<pair_type>);
			allocator_type get_allocator() const noexcept;
			
			// iterators:
			iterator begin() noexcept;
			const_iterator begin() const noexcept;
			iterator end() noexcept;
			const_iterator end() const noexcept;
			reverse_iterator rbegin() noexcept;
			const_reverse_iterator rbegin() const noexcept;
			reverse_iterator rend() noexcept;
			const_reverse_iterator rend() const noexcept;
			const_iterator cbegin() const noexcept;
			const_iterator cend() const noexcept;
			const_reverse_iterator crbegin() const noexcept;
			const_reverse_iterator crend() const noexcept;
			// capacity:
			bool empty() const noexcept;
			size_type size() const noexcept;
			size_type max_size() const noexcept;
			
			// 23.X.4.3, capacity:
			size_type size() const noexcept;
			size_type max_size() const noexcept;
			size_type capacity() const noexcept;
			bool empty() const noexcept;
			void reserve(size_type n);
			void shrink_to_fit();

			// element access:
			reference operator[](size_type n);
			const_reference operator[](size_type n) const;
			const_reference at(size_type n) const;
			reference at(size_type n);
			reference front();
			const_reference front() const;
			reference back();
			const_reference back() const;
			
			// 23.X.4.4, modifiers:
			template <class... Args> pair<iterator, bool> emplace(Args&&... args);
			template <class... Args> iterator emplace_hint(const_iterator position, Args&&... args);
			  pair<iterator, bool> insert(const value_type& x);
			template <class P> pair<iterator, bool> insert(P&& x);
			  iterator insert(const_iterator position, const value_type& x);
			template <class P>
			  iterator insert(const_iterator position, P&&);
			template <class InputIterator>
			  void insert(InputIterator first, InputIterator last);
			void insert(initializer_list<pair_type>);
			iterator erase(const_iterator position);
			size_type erase(const key_type& x);
			iterator erase(const_iterator first, const_iterator last);
			void swap(flat_map&);
			void clear() noexcept;
			
			// observers:
			key_compare key_comp() const;
			
			// 23.X.4.5, flat_map operations:
			iterator find(const key_type& x);
			const_iterator find(const key_type& x) const;
			template <class K> iterator find(const K& x);
			template <class K> const_iterator find(const K& x) const;
			  size_type count(const key_type& x) const;
			template <class K> size_type count(const K& x) const;
			  iterator lower_bound(const key_type& x);
			const_iterator lower_bound(const key_type& x) const;
			template <class K> iterator lower_bound(const K& x);
			template <class K> const_iterator lower_bound(const K& x) const;
			  iterator upper_bound(const key_type& x);
			const_iterator upper_bound(const key_type& x) const;
			template <class K> iterator upper_bound(const K& x);
			template <class K> const_iterator upper_bound(const K& x) const;
			  pair<iterator,iterator>
			  equal_range(const key_type& x);
			pair<const_iterator,const_iterator>
			  equal_range(const key_type& x) const;
			template <class K>
			  pair<iterator, iterator> equal_range(const K& x);
			template <class K>
			  pair<const_iterator, const_iterator> equal_range(const K& x) const;
		};
		
		template <class Key, class T, class Compare, class Allocator>
		  bool operator==(const flat_map<Key,T,Compare,Allocator>& x,
						  const flat_map<Key,T,Compare,Allocator>& y);
		template <class Key, class T, class Compare, class Allocator>
		  bool operator< (const flat_map<Key,T,Compare,Allocator>& x,
						  const flat_map<Key,T,Compare,Allocator>& y);
		template <class Key, class T, class Compare, class Allocator>
		  bool operator!=(const flat_map<Key,T,Compare,Allocator>& x,
						  const flat_map<Key,T,Compare,Allocator>& y);
		template <class Key, class T, class Compare, class Allocator>
		  bool operator> (const flat_map<Key,T,Compare,Allocator>& x,
						  const flat_map<Key,T,Compare,Allocator>& y);
		template <class Key, class T, class Compare, class Allocator>
		  bool operator>=(const flat_map<Key,T,Compare,Allocator>& x,
						  const flat_map<Key,T,Compare,Allocator>& y);
		template <class Key, class T, class Compare, class Allocator>
		  bool operator<=(const flat_map<Key,T,Compare,Allocator>& x,
						  const flat_map<Key,T,Compare,Allocator>& y);
						  
		// specialized algorithms:
		template <class Key, class T, class Compare, class Allocator>
		  void swap(flat_map<Key,T,Compare,Allocator>& x,
					flat_map<Key,T,Compare,Allocator>& y);

	} // namespace std
	
	23.X.4.2 flat_map constructors, copy, and assignment [flat_map.cons]
	
	explicit flat_map(const Compare& comp = Compare(),
					  const Allocator& = Allocator());
					  
		Effects: Constructs an empty flat_map using the specified comparison object and allocator.
		Complexity: Constant.
	
	template <class InputIterator>
	  flat_map(InputIterator first, InputIterator last,
		  const Compare& comp = Compare(), const Allocator& = Allocator());
	
		Requires: If the iterator’s indirection operator returns an lvalue or a const rvalue pair<key_type,
				  mapped_type>, then both key_type and mapped_type shall be CopyInsertable into *this.
		Effects: Constructs an empty flat_map using the specified comparison object and allocator, and inserts
				 elements from the range [first,last).
		Complexity: Linear in N if the range [first,last) is already sorted using comp and otherwise N log N,
					where N is last - first.
					
	23.X.4.3 flat_map element access [flat_map.access]
	
	T& operator[](const key_type& x);
	
		Effects: If there is no key equivalent to x in the flat_map, inserts value_type(x, T()) into the flat_map.
		Requires: key_type shall be CopyInsertable and mapped_type shall be DefaultInsertable into
				  *this.
		Returns: A reference to the mapped_type corresponding to x in *this.
		Complexity: logarithmic.
	
	T& operator[](key_type&& x);
	
		Effects: If there is no key equivalent to x in the flat_map, inserts value_type(std::move(x), T()) into
				 the flat_map.
		Requires: mapped_type shall be DefaultInsertable into *this.
		Returns: A reference to the mapped_type corresponding to x in *this.
		Complexity: logarithmic.
	
	T& at(const key_type& x);
	const T& at(const key_type& x) const;
	
		Returns: A reference to the mapped_type corresponding to x in *this.
		Throws: An exception object of type out_of_range if no such element is present.
		Complexity: logarithmic.
	
	23.X.4.4 flat_map modifiers [flat_map.modifiers]
	
	template <class P> pair<iterator, bool> insert(P&& x);
	template <class P> iterator insert(const_iterator position, P&& x);
	template <class InputIterator>
	  void insert(InputIterator first, InputIterator last);
	
		Effects: The first form is equivalent to return emplace(std::forward<P>(x)). The second form is
				 equivalent to return emplace_hint(position, std::forward<P>(x)).
		Remarks: These signatures shall not participate in overload resolution unless std::is_constructible<value_-
				 type, P&&>::value is true.
	
	23.X.4.5 flat_map operations [flat_map.ops]
	
	iterator find(const key_type& x);
	const_iterator find(const key_type& x) const;
	iterator lower_bound(const key_type& x);
	const_iterator lower_bound(const key_type& x) const;
	iterator upper_bound(const key_type& x);
	const_iterator upper_bound(const key_type &x) const;
	pair<iterator, iterator>
	  equal_range(const key_type &x);
	pair<const_iterator, const_iterator>
	  equal_range(const key_type& x) const;
	
		The find, lower_bound, upper_bound and equal_range member functions each have two versions,
		one const and the other non-const. In each case the behavior of the two functions is identical except
		that the const version returns a const_iterator and the non-const version an iterator (23.2.4).
	
	23.X.4.6 flat_map specialized algorithms [flat_map.special]
	
	template <class Key, class T, class Compare, class Allocator>
	  void swap(flat_map<Key,T,Compare,Allocator>& x,
				flat_map<Key,T,Compare,Allocator>& y);
	
		Effects: x.swap(y);
	
## References

- Recent SG14 Reflector Discussions

 - https://groups.google.com/a/isocpp.org/forum/#!topic/sg14/WyYpdrWxj9Q
 - https://groups.google.com/a/isocpp.org/forum/#!topic/sg14/csLjKbLOArw

- Prior Art

 - [Loki AssocVector - Alexandrescu](http://loki-lib.sourceforge.net/html/a00645.html)
 - [Boost flat containers](http://www.boost.org/doc/libs/1_58_0/doc/html/container/non_standard_containers.html#container.non_standard_containers.flat_xxx)
 - ["Why you shouldn't use set (and what you should use instead)" - Matt Austern](http://lafstern.org/matt/col1.pdf)
 - ["Cache-friendly binary search" - Joaquín M López Muñoz](http://bannalia.blogspot.fr/2015/06/cache-friendly-binary-search.html)
 - ["Traversing a linearized tree" - Joaquín M López Muñoz](http://bannalia.blogspot.fr/2015/06/traversing-linearized-tree.html)

- Contributors & Thanks

 - SG14
 - J F Bastien
 - Matthew Bentley
 - Lawrence Crowl
 - Brian Ehlert
 - Brent Friedman
 - Ion Gaztañaga
 - Nicolas Guillemot
 - Joël Lamotte
 - Nevin Liber
 - Guillaume Matte
 - John McFarlane
 - Rene Rivera
 - Patrice Roy
 - Ville Voutilainen
 - Chuck Walbourn
 - Scott Wardle
 - Michael Wong
