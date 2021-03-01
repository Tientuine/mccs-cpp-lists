#ifndef HPP_PRELUDE_LIST
#define HPP_PRELUDE_LIST

#include <ostream>
#include <stdexcept>

/**
 * Implementation of Haskell-like, functional programming idioms, particularly recursive lists.
 */
namespace prelude {

/**
 * Family of immutable, recursively-defined, homogeneous list types.
 * Lists create via constructor or the cons operator (overloaded |) use
 * shallow copy for the tail, which helps ease the storage burden for
 * memory-heavy element types.
 * 
 * @author Matthew A Johnson
 * @version 1.0
 */
template <typename A>
class list
{
    struct node;
    /** Class-scoped constant provided for empty list. */
    static list const EMPTY;
public:
    /**
     * Constructs a singleton list containing the specified element.
     * @param x the element to be stored in this list
     */
    list( A x ) : list( x, EMPTY )  {}
    /**
     * Makes a shallow copy of the specified list. This version shares ownership of
     * the internal node structure, increasing the reference count.
     * Note that deep copies are not permitted via the public interface for lists.
     * @param xs the existing list to be copied
     */
    list( list const& xs ) : _rep( acquire( xs._rep ) ) {}
    /**
     * Move-constructor for lists. This version swaps the internal pointers while avoiding
     * unnecessary net-zero changes to the reference counts.
     * @param xs the r-value list to be moved
     */
    list( list && xs ) noexcept : _rep( xs._rep ) { xs._rep = nullptr; }
    /**
     * Constructs a finite list using standard uniform initialization.
     * @param xs a comma-separated list of values that will be elements of this list
     */
    list( std::initializer_list<A> xs ) : _rep( nullptr )
        {
            for (auto i = xs.end(); i-- != xs.begin();) {
                _rep = acquire( new node(*i, _rep) );
            }
        }
    /**
     * Destroys this list and any referenced nodes for which this list was sole owner.
     */
    ~list() { release( _rep ); }
    /**
     * Copy-assignment of lists. Performs a shallow copy while sharing ownership of the
     * internal node structure and incrementing its reference count.
     * @param xs the existing list to be copied
     */
    list& operator= ( list const& xs )
        { if ( _rep != xs._rep ) { release( _rep ); _rep = acquire( xs._rep ); } return *this; }
    /**
     * Move-assignment of lists. This version effectively swaps the internal node pointers,
     * decrementing the LHS reference count without incrementing the RHS reference count.
     * @param xs the r-value list to be moved
     */
    list& operator= ( list && xs )
        { release( _rep ); _rep = xs._rep; xs._rep = nullptr; return *this; }
    /**
     * Casting operator permits type-conversion from list to boolean for use in test expressions.
     */
    explicit operator bool() { return static_cast<bool>(_rep); }

private:
    // Pointer to internal, reference-counting node structure representation.
    node* _rep;
    /**
     * `Cons`-constructor, copying version, for internal use only.
     */
    list( A x, list const& xs ) : _rep( acquire( new node( x, xs._rep ) ) ) {}
    /**
     * `Cons`-constructor, move version, for internal use only.
     */
    list( A x, list && xs ) : _rep( acquire( new node( std::move( x ) ) ) ) { _rep->_tail = xs._rep; xs._rep = nullptr; }
    /**
     * Constructs a list from a raw pointer to a node structure. For internal use only.
     */
    list( node* n ) : _rep( acquire( n ) ) {}
    /**
     * Constructs an empty list (internal pointer is null). For internal use only;
     * the global {@code empty} function is provided for general usage.
     */
    list() : _rep( nullptr ) {}

    /**
     * Auxiliary function for incrementing a pointed-to node's reference count.
     */
    static node* acquire( node* n ) { if ( n ) { ++(n->_refs); } return n; }
    /**
     * Auxiliary function for decrementing a node's reference count and deleting if necessary.
     */
    static node* release( node* n ) { if ( n ) { if ( !--(n->_refs) ) { delete n; } } return n; }

    /**
     * Internal reference-counting node structure for a list.
     * Required so that list can provide a distinct empty-list value (encapsulated nullptr).
     */
    struct node
    {
        /**
         * Make an internal list node from an element and a pointer to a node.
         * @param x  an element
         * @param xs pointer to an existing node (or nullptr)
         */
        explicit node( A x, node* xs ) : _refs( 0 ), _head( x ), _tail( acquire( xs ) ) {}
        /**
         * Make an internal list node from an element and a pointer to a node.
         * @param x  an element
         * @param xs pointer to an existing node (or nullptr)
         */
        explicit node( A x ) : _refs( 0 ), _head( x ), _tail( nullptr ) {}
        /**
         * Constructs a shallow copy of the given node, copying the head but sharing
         * ownership of the tail with the original node.
         * @param n an existing node to be copied
         */
        node( node const& n ) : _refs( 0 ), _head( n._head ), _tail( acquire( n._tail ) ) {}
        /**
         * Moves the given node to this one, transferring ownership of all resources.
         * @param n an existing node to be moved
         */
        node( node && n ) noexcept
            : _refs( n._refs ), _head( std::move( n._head ) ), _tail( n._tail ) { n._tail = nullptr; }

        /** Destroy this element and relinquish a claim on the tail. */
        ~node() { release( _tail ); }

        /** Counter for tracking references to this element. */
        size_t  _refs;
        /** The value of an element. */
        A const _head;
        /** node containing the next element. */
        node*   _tail;
    };

    /**
     * @deprecated This function is not currently used and is likely to be removed in a future version.
     * Creates and returns a deep copy of this list.
     */
    list clone()
	{
  		auto zs = list<A>( this->_rep->_head );
	  	auto from = this->_rep;
	  	auto to = zs._rep;
  		while ( (from = from->_tail) ) {
    		to = to->_tail = list<A>::acquire( new typename list<A>::node( from->_head ) );
	  	}
		return zs;
    }

    // Friend privileges provided for optimal performance of core functions.

    A& operator[] ( size_t i )
    {
        if ( !this->_rep ) { throw std::domain_error("prelude::[]: index too large"); }
	    if ( i <  0 ) { throw std::domain_error("prelude::[]: negative index"); }
        if ( i == 0 ) { return this->_rep->_head; }
    	auto to = xs._rep;
	    while ( k-- ) { // 0 -> skip and return head (but haven't checked null), 1 -> check null
            if ( !to ) { throw std::domain_error("prelude::[]: index too large"); }
            to = to->_tail;
        }
      	return to->_head;
    }

    template <typename B> friend list<B> const& empty();
    template <typename B> friend bool    null( list<B> );
    template <typename B> friend B       head( list<B> );
    template <typename B> friend list<B> tail( list<B> );
    template <typename B> friend list<B> cons( B, list<B> );
    template <typename B> friend list<B> operator| ( B, list<B> );
    template <typename B> friend list<B> operator+ ( list<B> const&, list<B> const& );
    template <typename B> friend list<B> operator+ ( list<B> &&, list<B> const& );
    template <typename B> friend list<B> operator+ ( list<B> &&, list<B> && );
    template <typename B> friend std::ostream& operator<< ( std::ostream& , list<B> const& );
    template <typename F, typename B> friend auto map( F f, list<B> const& xs ) -> list<decltype( f( head( xs ) ) )>;
    template <typename P, typename B> friend list<B> filter( P, list<B> const& );
    template <typename B> friend B sum( list<B> const& );
    template <typename B> friend size_t length( list<B> const& );
    template <typename B> friend list<B> take( unsigned, list<B> const& );
    template <typename B> friend list<B> drop( unsigned, list<B> const& );
    template <typename B> friend bool operator== ( list<B> const&, list<B> const& );
    template <typename B> friend list<B> reverse( list<B> const& );
};

/** Typed empty list constant - class scoped. */
template <typename A> list<A> const list<A>::EMPTY {};
/** Typed empty list constant - namespace scoped. */
template <typename A> list<A> const& empty() { return list<A>::EMPTY; }

/**
 * Constructs a list by pre-pending an element to an existing list.
 * @param x an element
 * @param xs a list
 * @return a list with {@code x} at the head and {@code xs} as the tail
 */
template <typename A>
inline list<A> cons( A x, list<A> xs ) { return list<A>( x, xs ); }
/**
 * Constructs a list by pre-pending an element to an existing list (operator version).
 * @param x an element
 * @param xs a list
 * @return a list with {@code x} at the head and {@code xs} as the tail
 */
template <typename A>
inline list<A> operator| ( A x, list<A> xs ) { return list<A>( x, xs ); }

// Fundamental list operations

/**
 * Converts a list of one type to that of another by applying a specified function.
 * {@code map(f,xs)} is the list obtained by applying {@code f} to each element
 * {@code xs}, i.e.,
 *     {@code map(f, list(x1, x2, ..., xn)) == list(f(x1), f(x2), ..., f(xn)}
 *     {@code map(f, list(x1, x2, ...)) == list(f(x1), f(x2), ...)}
 * @param f function that takes an element of {@code xs} and returns a different type
 * @param xs a list
 * @return a list of elements the same type as the return type of {@code f}
 */
template <typename F, typename A>
inline auto map( F f, list<A> const& xs ) -> list<decltype( f( head( xs ) ) )>
{
	using B = decltype( f( head(xs) ) );
  	using node = typename list<B>::node;

  	if ( null( xs ) ) { return empty<B>(); }

  	auto ys = list<B>( f( xs._rep->_head ) );
  	auto from = xs._rep;
  	auto to = ys._rep;
  	while ( (from = from->_tail) ) {
    	to = to->_tail = list<B>::acquire( new node( f( from->_head ) ) );
  	}
  	return ys;
}

/**
 * Appends one list to another (move-version).
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of {@code xs} precede the elements of {@code ys}
 * @note element order is preserved
 */
template <typename A>
inline list<A> append( list<A> const& xs, list<A> const& ys )
{
    return xs + ys;
}

/**
 * Appends one list to another.
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of {@code xs} precede the elements of {@code ys}
 * @note element order is preserved
 */
template <typename A>
inline list<A> operator+ ( list<A> const& xs, list<A> const& ys )
{
  	using node = typename list<A>::node;

    if ( null( xs ) ) { return ys; }
    // TODO Fix this implementation! It crashes!
  	auto zs = list<A>( xs._rep->_head );
  	auto from = xs._rep;
  	auto to = zs._rep;
  	while ( (from = from->_tail) ) {
    	to = to->_tail = list<A>::acquire( new node( from->_head ) );
  	}
    to->_tail = list<A>::acquire( ys._rep );
  	return zs;
}

/**
 * Appends one list to another (operator variant).
 *     list(x1, ..., xm) + list(y1, ..., yn) == list(x1, ..., xm, y1, ..., yn)
 *     list(x1, ..., xm) + list(y1, ...) == list(x1, ..., xm, y1, ...)
 * If the first list is not finite, the result is the first list. Element order is preserved.
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of {@code xs} precede the elements of {@code ys}
 */
template <typename A>
inline list<A> operator+ ( list<A> && xs, list<A> const& ys )
{
    if ( null( xs ) ) { return ys; }

  	auto zs = std::move( xs );
  	auto to = zs._rep;
  	while ( to->_tail ) { to = to->_tail; }
    to->_tail = list<A>::acquire( ys._rep );
  	return zs;
}

/**
 * Appends one list to another (operator variant).
 *     list(x1, ..., xm) + list(y1, ..., yn) == list(x1, ..., xm, y1, ..., yn)
 *     list(x1, ..., xm) + list(y1, ...) == list(x1, ..., xm, y1, ...)
 * If the first list is not finite, the result is the first list. Element order is preserved.
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of {@code xs} precede the elements of {@code ys}
 */
template <typename A>
inline list<A> operator+ ( list<A> && xs, list<A> && ys )
{
    if ( null( xs ) ) { return ys; }

  	auto zs = std::move( xs );
  	auto to = zs._rep;
  	while ( to->_tail ) { to = to->_tail; }
    to->_tail = ys._rep;
    ys._rep = nullptr;
  	return zs;
}

/**
 * {@code filter}, applied to a predicate and a list, returns the list of
 * those elements that satisfy the predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of {@code xs} satisfying {@code pred}
 */
template <typename P, typename A>
inline list<A> filter( P pred, list<A> const& xs )
{
  	using node = typename list<A>::node;

	list<A> ys;
	auto to = ys._rep;
  	auto from = xs._rep;

	while ( from ) {
    	if ( pred( from->_head ) ) {
            if ( to ) {
    			to = to->_tail = list<A>::acquire( new node( from->_head ) );
            } else {
                to = ys._rep = list<A>::acquire( new node( from->_head ) );
            }
    	} 
    	from = from->_tail;
  	} 
  	return ys;
}

/**
 * Extract the first element of a list, which must be non-empty.
 * @param xs a non-empty list
 * @return the first element of {@code xs}
 * @throws std::domain_error if {@code xs} is empty
 */
template <typename A>
inline A head( list<A> xs )
{
  if ( null( xs ) ) { throw std::domain_error("prelude::head: empty list"); }
  return xs._rep->_head;
}

/**
 * Extract the elements after the head of a list, which must be non-empty.
 * @param xs a non-empty list
 * @return a list containing all but the first element of {@code xs}
 * @throws std::domain_error if {@code xs} is empty
 */
template <typename A>
inline list<A> tail( list<A> xs )
{
  if ( null( xs ) ) { throw std::domain_error("prelude::tail: empty list"); }
  return list<A>( xs._rep->_tail );
}

/**
 * Extract the last element of a list, which must be finite and non-empty.
 * @param xs a non-empty list
 * @return the last element of {@code xs}
 * @throws std::domain_error if {@code xs} is empty
 */
template <typename A>
inline A last( list<A> xs )
{
  if ( null( xs ) ) { throw std::domain_error("prelude::last: empty list"); }

  auto ys = tail( xs );
  if ( null( ys ) ) {
    return head( xs );
  } else {
    return last( ys );
  }
}

/**
 * Return all the elements of a list except the last one.
 * The list must be non-empty.
 * @param xs a non-empty list
 * @return a list containing all but the last element of {@code xs}
 * @throws std::domain_error if {@code xs} is empty
 */
template <typename A>
inline list<A> init( list<A> xs )
{
  if ( null( xs ) ) { throw std::domain_error("prelude::init: empty list"); }

  auto ys = tail( xs );
  if ( null( ys ) ) {
    return ys;
  } else {
    return head( xs ) | init( ys );
  }
}

/**
 * Test whether a list is empty.
 * @param xs a list
 * @return true if {@code xs} is empty, false otherwise
 */
template <typename A>
inline bool null( list<A> xs ) { return !xs; }

/**
 * Returns the length of a finite list as an int.
 * This operation has time complexity O(n).
 * @param xs a list
 * @return the number of elements in {@code xs}
 */
template <typename A>
inline size_t length( list<A> const& xs )
{
    size_t n = 0;
    auto e = xs._rep;
    while ( e ) {
        ++n;
        e = e->_tail;
    }
    return n;
}

/**
 * Reverses a list.
 * {@code reverse(xs)} returns the elements of {@code xs} in reverse order.
 * Note that {@code xs} must be finite.
 * @param xs a finite list
 * @return a list whose elements are the same as {@code xs} but in reverse order
 */
template <typename A>
inline list<A> reverse( list<A> const& xs )
{
  	using node = typename list<A>::node;

	auto from = xs._rep;

  	if ( !from ) { return xs; }

	auto to = new node( from->_head );

  	while ( (from = from->_tail) ) {
		to = new node( from->_head, to );
  	}
  	return list<A>( to );
}

// Special folds

/**
 * The {@code sum} function computes the sum of a finite list of numbers.
 * @param xs a list
 * @return the sum of each element of {@code xs}
 */
template <typename A>
inline A sum( list<A> const& xs )
{
    A result = 0;
    auto e = xs._rep;
    while ( e ) {
        result += e->_head;
        e = e->_tail;
    }
    return result;
}

// Sublists

/**
 * Gets from any list the leading sublist up to a given length.
 * {@code std::bind(take,n,_1)}, applied to a list {@code xs}, returns the prefix of {@code xs}
 * of length {@code n}, or {@code xs} itself if {@code n > 'length' xs}:
 *     {@code take(5,"Hello World!") == "Hello"}
 *     {@code take(3,list(1,2,3,4,5)) == list(1,2,3)}
 *     {@code take(3,list(1,2)) == list(1,2)}
 *     {@code take(3,empty) == empty}
 *     {@code take(-1,list(1,2)) == empty}
 *     {@code take(0,list(1,2)) == empty}
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of {@code xs} with at most {@code k} elements
 * @note returns an empty list if {@code xs} is empty
 */
template <typename A>
inline list<A> take( unsigned k, list<A> const& xs )
{
  	using node = typename list<A>::node;

  	auto from = xs._rep;
  	if ( k <= 0 || !from ) { return empty<A>(); }

	auto ys = list<A>( from->_head );
	auto to = ys._rep;
  	while ( --k > 0 && (from = from->_tail) ) {
		to = to->_tail = list<A>::acquire( new node( from->_head ) );
	}
  	return ys;
}

/**
 * Gets from a finite list the trailing sublist up to the specified length.
 * {@code drop} @n xs@ returns the suffix of {@code xs}
 * after the first {@code n} elements, or {@code empty} if {@code n > 'length' xs}:
 *     {@code drop(6,"Hello World!") == "World!"}
 *     {@code drop(3,list(1,2,3,4,5)) == list(4,5)}
 *     {@code drop(3,list(1,2)) == empty}
 *     {@code drop(3,empty)) == empty}
 *     {@code drop(-1,list(1,2)) == list(1,2)}
 *     {@code drop(0,list(1,2)) == list(1,2)}
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of {@code xs} with at most {@code k} elements
 * @note returns an empty list if {@code xs} has fewer than {@code k} elements
 */
template <typename A>
inline list<A> drop( unsigned k, list<A> const& xs )
{
	if ( k <= 0 ) { return xs; }
	auto to = xs._rep;
	while ( k-- > 0 && to ) { to = to->_tail; }
  	return list<A>( to );
}

template <typename B>
inline bool operator== ( list<B> const& xs, list<B> const& ys )
{
    auto xs_ = xs._rep, ys_ = ys._rep;
    
    if ( xs_ == ys_ ) { return true; }


    while ( xs_ && ys_ ) {
        if ( xs_->_head != ys_->_head ) { return false; }
        xs_ = xs_->_tail;
        ys_ = ys_->_tail;
    }
    return xs_ == ys_;
}

// Converting to and from strings

/**
 * Inserts a character string serialization of a list into an output stream.
 * @param os an output stream
 * @param xs a list
 * @return a reference to the output stream
 */
template <typename A>
std::ostream& operator<< ( std::ostream& os, list<A> const& xs )
{
    auto n = xs._rep;
    os << '[';
    if ( n ) {
        os << n->_head;
        while ( (n = n->_tail) ) {
            os << ',' << n->_head;
        }
    }
    return os << ']';
}


} // end namespace prelude

#endif //HPP_PRELUDE_LIST

