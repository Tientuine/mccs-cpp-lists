#ifndef HPP_PRELUDE_LIST
#define HPP_PRELUDE_LIST

#include <memory>
#include <functional>

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
template <typename A>
class List
{
    class Node;
public:
    static List EMPTY;

    List( A x ) : List( x, EMPTY ) {}
    List( List const& xs ) : _rep( acquire( xs._rep ) ) { /*std::cerr << "List::copy-ctor new " << this << " (from " << &xs << ")\n";*/ }
    List( List && xs ) noexcept : _rep( xs._rep ) { xs._rep = nullptr; /*std::cerr << "List::move-ctor new " << this << " (from " << &xs << ")\n";*/ }
    ~List() { release( _rep ); /*std::cerr << "List::dtor destroy " << this << "\n";*/ }

    //List& operator= ( List xs ) { swap( xs ); std::cerr << "assigned to " << this << "\n"; return *this; }
    List& operator= ( List const& xs ) { if ( _rep != xs._rep ) { release( _rep ); _rep = acquire( xs._rep ); /*std::cerr << "copy-assigned " << &xs << " to " << this << "\n";*/ return *this; } else { /*std::cerr << "copy-assignment skipped from " << &xs << " to " << this << "\n";*/ } return *this; }
    List& operator= ( List && xs ) { release( _rep ); _rep = xs._rep; xs._rep = nullptr; /*std::cerr << "move-assigned " << &xs << " to " << this << "\n";*/ return *this; }

    operator bool() { return static_cast<bool>(_rep); }

private:
    List( A x, List xs ) : _rep( acquire( new Node( x, xs._rep ) ) ) { /*std::cerr << "List::cons-ctor new " << this << "(from " << x << " and " << &xs << ")\n";*/ }
    List( Node* n ) : _rep( acquire( n ) ) { /*std::cerr << "List::raw-ctor new " << this << " (from Node " << n << ")\n";*/ }
    List() : _rep( nullptr ) { /*std::cerr << "List::null-ctor new " << this << "\n";*/ }

    void swap( List& xs ) { std::swap( _rep, xs._rep ); /*std::cerr << "swapped " << this << " and " << &xs << "\n";*/ }

    Node* _rep;

    static Node* acquire( Node* n ) { if ( n ) { ++(n->_refs); } return n; }
    static Node* release( Node* n ) { if ( n ) { if ( !--(n->_refs) ) { delete n; } } return n; }
    //Node* acquire( Node* n ) { std::cerr << this << " acquired " << n; if ( n ) { ++(n->refs); std::cerr << " (refs=" << n->refs << ")"; } std::cerr << "\n"; return n; }
    //Node* release( Node* n ) { std::cerr << this << " released " << n; if (n) { std::cerr << " (refs=" << (n->refs - 1) << ")"; if ( !--(n->refs) ) { std::cerr << "\n   delete " << n << "!\n"; delete n; } } std::cerr << "\n"; return n; }

    /**
     * Internal node structure for a list.
     * @note required so that List can provide a distinct empty-list value
     */
    class Node
    {
        friend class List;
    public:
    /** . */
        ~Node() { release( _tail ); /*std::cerr << "deleted " << this << "\n";*/ }
    private:
        /**
         * Make an internal list node from an element and a pointer to a node.
         * @param x  an element
         * @param xs pointer to an existing node (or nullptr)
         */
        explicit Node( A x, Node* xs ) : _refs( 0 ), _head( x ), _tail( acquire( xs ) ) { /*std::cerr << "new " << this << "\n";*/ }

        Node( Node const& n ) : _refs( 0 ), _head( n._head ), _tail( acquire( n._tail ) ) { /*std::cerr << "new " << this << " (copy of " << &n << ")\n";*/ }

        Node( Node && n ) noexcept : _refs( n._refs ), _head( std::move( n._head ) ), _tail( nullptr ) { std::swap( _tail, n._tail ); /*std::cerr << "new " << this << " (moved from " << &n << ")\n";*/ }

        /** . */
        size_t  _refs;
        /** The value of an element. */
        A const _head;
        /** Node containing the next element. */
        Node*   _tail;

        template <typename B> friend B       head( List<B> );
        template <typename B> friend List<B> tail( List<B> );
        template <typename F, typename B> friend auto map( F f, List<B> const& xs ) -> List<decltype( f( head( xs ) ) )>;
        template <typename B> friend B sum( List<B> const& xs );
    };

    template <typename B> friend bool    null( List<B> );
    template <typename B> friend B       head( List<B> );
    template <typename B> friend List<B> tail( List<B> );
    template <typename B> friend List<B> cons( B x, List<B> );
    //template <typename B> friend List<B> cons( B x, List<B> const& );
    //template <typename B> friend List<B> cons( B x, List<B> && );
    //template <typename B> friend List<B> operator| ( B x, List<B> );
    template <typename B> friend std::ostream& operator<< ( std::ostream& os, List<B> const& xs );
    template <typename F, typename B> friend auto map( F f, List<B> const& xs ) -> List<decltype( f( head( xs ) ) )>;
    template <typename B> friend B sum( List<B> const& xs );
};

/** Typed empty list constant - could implement as a method instead. */
template <typename A>
List<A> List<A>::EMPTY {};
template <typename A>
List<A> const& EMPTY = List<A>::EMPTY;

/**
 * Constructs a list by pre-pending an element to an existing list.
 * @param x an element
 * @param xs a list
 * @return a list with <var>x</var> at the head and <var>xs</var> as the tail
 */
template <typename A>
inline List<A> cons      ( A x, List<A> xs ) { return std::move( List<A>( x, xs ) ); }
//template <typename A>
//inline List<A> cons      ( A x, List<A> const& xs ) { return std::move( List<A>( x, xs ) ); }
//template <typename A>
//inline List<A> cons      ( A x, List<A> && xs ) { return std::move( List<A>( x, std::forward( xs ) ) ); }
//template <typename A>
//inline List<A> operator| ( A x, List<A> xs ) { return List<A>( x, xs ); }

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

  	if ( null( xs ) ) { return EMPTY<B>; }

  	auto ys = List<B>( f( head( xs ) ) );
  	auto from = xs._rep;
  	auto to = ys._rep;
  	while ( (from = from->_tail) ) {
    	//to = to->tail = Node<B>::acquire( cons( f( head( from ) ), EMPTY<B> ) );
    	to = to->_tail = List<B>::acquire( new typename List<B>::Node( f( from->_head ), nullptr ) );
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
inline List<A> append( List<A> xs, List<A> ys )
{
  if ( null( xs ) ) { return ys; }
  return head( xs ) | append( tail( xs ), ys );
}
//template <typename A>
//inline List<A> operator+ ( List<A> xs, List<A> ys )
//{
//  if ( null( xs ) ) { return ys; }
//  return head( xs ) | tail( xs ) + ys;
//}

/**
 * Extracts a sublist of those elements satisfying the given predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename P, typename A>
inline List<A> filter( P pred, List<A> xs )
{
  if ( null( xs ) ) { return EMPTY<A>; }
  auto ys = filter( pred, tail( xs ) );
  auto x = head( xs );
  return pred( x ) ? ( x | ys ) : ys;
}

/**
 * Extracts the first element of a list.
 * @param xs a non-empty list
 * @return the first element of <var>xs</var>
 */
template <typename A>
inline A head( List<A> xs )
{
  if ( null( xs ) ) { throw xs; }
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
  if ( null( xs ) ) { throw xs; }

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
  if ( null( xs ) ) { throw xs; }
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
  if ( null( xs ) ) { throw xs; }

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
  if ( null( xs ) ) {
    return 0;
  } else {
    return 1 + length( tail( xs ) );
  }
}

/**
 * Reverses a list.
 * @param xs a list
 * @return a list whose elements are the same as <var>xs</var> but in reverse order
 */
template <typename A>
inline List<A> reverse( List<A> const& xs )
{
  std::function<List<A>(List<A>, List<A>)> rev;
  rev = [&rev](List<A> const& xs, List<A> ys)->List<A>{
    if ( null( xs ) ) { return ys; }
    return rev( tail( xs ), head( xs ) | ys );
  };
  return rev( xs, EMPTY<A> );
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
  if ( k == 0 || null( xs ) ) { return EMPTY<A>; }

  return head( xs ) | take( k - 1, tail( xs ) );
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
  if ( k == 0 || null( xs ) ) { return xs; }

  return drop( k - 1, tail( xs ) );
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
  auto n = xs;
  if ( n ) {
    os << '[' << n._rep->_head;
    while ( (n = n._rep->_tail) ) {
      os << ',' << n._rep->_head;
    }
    os << ']';
  }
  return os;
}


} // end namespace prelude

#endif //HPP_PRELUDE_LIST
