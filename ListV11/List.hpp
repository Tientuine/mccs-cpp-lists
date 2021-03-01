#ifndef HPP_PRELUDE_LIST
#define HPP_PRELUDE_LIST

#include <stdexcept>

/**
 * Implementation of Haskell-like, functional programming idioms, particularly recursive Lists.
 */
namespace prelude {

template <typename A> class List;

/**
 * Family of recursively-defined homogeneous list types.
 * Lists create via constructor or the cons operator (overloaded |) use
 * shallow copy for the tail, which helps ease the storage burden for
 * memory-heavy element types.
 * 
 * @author Matthew A Johnson
 * @version 1.0
 */

template <typename A> class list;

template <typename A>
struct _list_base
{
    virtual auto head() const -> A const& = 0;
    virtual auto tail() const -> list<A>  = 0;
};

template <typename A>
struct _list_empty : public _list_base
{
    virtual auto head() const -> A const& { throw std::domain_error("prelude::head: empty list"); }
    virtual auto tail() const -> list<A>  { throw std::domain_error("prelude::tail: empty list"); }
};

template <typename A>
class list : public _list_base
{
    struct node;
    node* _rep;
public:
    /**
     * Internal node structure for a list.
     * @note required so that List can provide a distinct empty-list value
     */
    struct node {};

    class empty_node : public node
    {
        explicit empty_node() {}
    };
    struct nonempty_node : public node
    {
        /**
         * Make an internal list node from an element and a pointer to a node.
         * @param x  an element
         * @param xs pointer to an existing node (or nullptr)
         */
        explicit Node( A x, Node* xs ) : _refs( 0 ), _head( x ), _tail( acquire( xs ) ) {}
        /**
         * Make an internal list node from an element and a pointer to a node.
         * @param x  an element
         * @param xs pointer to an existing node (or nullptr)
         */
        explicit Node( A x )           : _refs( 0 ), _head( x ), _tail( nullptr )       {}

        Node( Node const& n )          : _refs( 0 ),       _head( n._head ),              _tail( acquire( n._tail ) ) {}
        Node( Node &&     n ) noexcept : _refs( n._refs ), _head( std::move( n._head ) ), _tail( n._tail )            { n._tail = nullptr; }

        /** Destroy this element and relinquish a claim on the tail. */
        ~Node() { release( _tail ); }

        /** Counter for tracking references to this element. */
        size_t  _refs;
        /** The value of an element. */
        A const _head;
        /** Node containing the next element. */
        Node*   _tail;
    };

    static node const EMPTY;

    List( A           x  )          : List( x, EMPTY )           {}
    List( List const& xs )          : _rep( acquire( xs._rep ) ) {}
    List( List &&     xs ) noexcept : _rep( xs._rep )            { xs._rep = nullptr; }
    
    ~List() { release( _rep ); }

    List& operator= ( List const& xs ) { if ( _rep != xs._rep ) { release( _rep ); _rep = acquire( xs._rep ); } return *this; }
    List& operator= ( List && xs ) { release( _rep ); _rep = xs._rep; xs._rep = nullptr; return *this; }

    operator bool() { return static_cast<bool>(_rep); }

//private:
    Node* _rep;

    List( A x, List const& xs ) : _rep( acquire( new Node( x, xs._rep ) ) ) {}
    List( A x, List &&     xs ) : _rep( acquire( new Node( std::move( x ) ) ) ) { _rep->_tail = xs._rep; }
    List( Node* n ) : _rep( acquire( n ) ) {}
    List()          : _rep( nullptr ) {}

    static Node* acquire( Node* n ) { if ( n ) { ++(n->_refs); } return n; }
    static Node* release( Node* n ) { if ( n ) { if ( !--(n->_refs) ) { delete n; } } return n; }

    List clone()
	{
  		auto zs = List<A>( this->_rep->_head );
	  	auto from = this->_rep;
	  	auto to = zs._rep;
  		while ( (from = from->_tail) ) {
    		to = to->_tail = List<A>::acquire( new typename List<A>::Node( from->_head ) );
	  	}
		return zs;
    }

    template <typename B> friend bool    null( List<B> );
    template <typename B> friend B       head( List<B> );
    template <typename B> friend List<B> tail( List<B> );
    template <typename B> friend List<B> cons( B x, List<B> );
    template <typename B> friend List<B> operator| ( B x, List<B> );
    template <typename B> friend std::ostream& operator<< ( std::ostream& os, List<B> const& xs );
    template <typename F, typename B> friend auto map( F f, List<B> const& xs ) -> List<decltype( f( head( xs ) ) )>;
    template <typename B> friend B sum( List<B> const& xs );
};

/** Typed empty list constant - class scoped. */
template <typename A>
List<A> const List<A>::EMPTY {};
/** Typed empty list constant - namespace scoped. */
template <typename A>
List<A> const& EMPTY = List<A>::EMPTY;

/**
 * Constructs a list by pre-pending an element to an existing list.
 * @param x an element
 * @param xs a list
 * @return a list with <var>x</var> at the head and <var>xs</var> as the tail
 */
template <typename A>
inline List<A> cons( A x, List<A> xs ) { return List<A>( x, xs ); }
/**
 * Constructs a list by pre-pending an element to an existing list (operator version).
 * @param x an element
 * @param xs a list
 * @return a list with <var>x</var> at the head and <var>xs</var> as the tail
 */
template <typename A>
inline List<A> operator| ( A x, List<A> xs ) { return List<A>( x, xs ); }

// List operations

/**
 * Obtains a new list by applying a function to each element of a list.
 * Converts a list of one type to that of another by applying a specified function.
 * @param f function that takes an element of <var>xs</var> and returns a different type
 * @param xs a list
 * @return a list of elements the same type as the return type of <var>f</var>
 */
template <typename F, typename A>
inline auto map( F f, List<A> const& xs ) -> List<decltype( f( head( xs ) ) )>
{
	using B = decltype( f( head(xs) ) );
  	using Node = typename List<B>::Node;

  	if ( null( xs ) ) { return EMPTY<B>; }

  	auto ys = List<B>( f( xs._rep->_head ) );
  	auto from = xs._rep;
  	auto to = ys._rep;
  	while ( (from = from->_tail) ) {
    	to = to->_tail = List<B>::acquire( new Node( f( from->_head ) ) );
  	}
  	return ys;
}

/**
 * Appends one list to another.
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of <var>xs</var> precede the elements of <var>ys</var>
 * @note element order is preserved
 */
template <typename A>
inline List<A> append( List<A> const& xs, List<A> const& ys )
{
  	using Node = typename List<A>::Node;

    if ( null( xs ) ) { return ys; }

  	auto zs = List<A>( xs._rep->_head );
  	auto from = xs._rep;
  	auto to = zs._rep;
  	while ( (from = from->_tail) ) {
    	to = to->_tail = List<A>::acquire( new Node( from->_head ) );
  	}
    to->_tail = ys._rep;
  	return zs;
}

/**
 * Appends one list to another (move-version).
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of <var>xs</var> precede the elements of <var>ys</var>
 * @note element order is preserved
 */
template <typename A>
inline List<A> append( List<A> && xs, List<A> const& ys )
{
    if ( null( xs ) ) { return ys; }

  	auto zs = std::move( xs );
  	auto to = zs._rep;
  	while ( (to = to->_tail) ) {}
    to->_tail = ys._rep;
  	return zs;
}

/**
 * Appends one list to another (operator variant).
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of <var>xs</var> precede the elements of <var>ys</var>
 * @note element order is preserved
 */
template <typename A>
inline List<A> operator+ ( List<A> xs, List<A> const& ys )
{
    return append( std::forward( xs ), ys );
}

/**
 * Extracts a sublist of those elements satisfying the given predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename P, typename A>
inline List<A> filter( P pred, List<A> xs )
{
  	using Node = typename List<A>::Node;

	List<A> ys;
	auto to = ys._rep;
  	auto from = xs._rep;

	while ( from ) {
    	if ( pred( from->_head ) ) {
			to = to->_tail = List<A>::acquire( new Node( from->_head ) );
    	} 
    	from = from->_tail;
  	} 
  	return ys;
}

/**
 * Extracts the first element of a list.
 * @param xs a non-empty list
 * @return the first element of <var>xs</var>
 */
template <typename A>
inline A head( List<A> xs )
{
  if ( null( xs ) ) { throw std::domain_error("prelude::head: empty list"); }
  return xs._rep->_head;
}

/**
 * Extracts the last element of a list.
 * @param xs a non-empty list
 * @return the last element of <var>xs</var>
 */
template <typename A>
inline A last( List<A> xs )
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
 * Extracts all elements after the head of a list.
 * @param xs a non-empty list
 * @return a list containing all but the first element of <var>xs</var>
 */
template <typename A>
inline List<A> tail( List<A> xs )
{
  if ( null( xs ) ) { throw std::domain_error("prelude::tail: empty list"); }
  return List<A>( xs._rep->_tail );
}

/**
 * Extracts all except the last element of a list.
 * @param xs a non-empty list
 * @return a list containing all but the last element of <var>xs</var>
 */
template <typename A>
inline List<A> init( List<A> xs )
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
 * Tests whether a list is empty.
 * @param xs a list
 * @return true if <var>xs</var> is empty, false otherwise
 */
template <typename A>
inline bool null( List<A> xs ) { return !xs; }

/**
 * Computes the length of a list.
 * @param xs a list
 * @return the number of elements in <var>xs</var>
 * @note This operation has time complexity O(n).
 */
template <typename A>
inline size_t length( List<A> xs )
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
 * @param xs a list
 * @return a list whose elements are the same as <var>xs</var> but in reverse order
 */
template <typename A>
inline List<A> reverse( List<A> const& xs )
{
  	using Node = typename List<A>::Node;

	auto from = xs._rep;

  	if ( !from ) return xs;

	auto to = new Node( from->_head );

  	while ( (from = from->_tail) ) {
		to = new Node( from->_head, to );
  	}
  	return List<A>( to );
}

// Special folds

/**
 * Extracts a sublist of those elements satisfying the given predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename A>
inline A sum( List<A> const& xs )
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
 * Gets from a list its leading sublist of a given size if one exists.
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of <var>xs</var> with at most <var>k</var> elements
 * @note returns an empty list if <var>xs</var> is empty
 */
template <typename A>
inline List<A> take( unsigned k, List<A> const& xs )
{
  	using Node = typename List<A>::Node;

  	auto from = xs._rep;
  	if ( k <= 0 || !from ) { return EMPTY<A>; }

	auto ys = List<A>( from->_head );
	auto to = ys._rep;
  	while ( --k > 0 && (from = from->_tail) ) {
		to = to->_tail = List<A>::acquire( new Node( from->_head ) );
	}
  	return ys;
}

/**
 * Gets what remains after removing a given number of elements from a list.
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of <var>xs</var> with at most <var>length-k</var> elements
 * @note returns an empty list if <var>xs</var> has fewer than <var>k</var> elements
 */
template <typename A>
inline List<A> drop( unsigned k, List<A> const& xs )
{
	if ( k <= 0 ) { return xs; }
	auto to = xs._rep;
	while ( k-- > 0 && to ) { to = to->_tail; }
  	return List<A>( to );
}

// Converting to and from strings

/**
 * Inserts a character string serialization of a list into an output stream.
 * @param os an output stream
 * @param xs a list
 * @return a reference to the output stream
 */
template <typename A>
std::ostream& operator<< ( std::ostream& os, List<A> const& xs )
{
  auto n = xs._rep;
  if ( n ) {
    os << '[' << n->_head;
    while ( (n = n->_tail) ) {
      os << ',' << n->_head;
    }
    os << ']';
  }
  return os;
}


} // end namespace prelude

#endif //HPP_PRELUDE_LIST
