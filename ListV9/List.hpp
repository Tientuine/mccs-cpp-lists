#ifndef HPP_PRELUDE_LIST
#define HPP_PRELUDE_LIST

#include <memory>
#include <functional>

/**
 * Implementation of Haskell-like, functional programming idioms, particularly recursive Lists.
 */
namespace prelude {

template <typename A> class Node;

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
using List = Node<A>*;

/** Typed empty list constant - could implement as a method instead. */
template <typename A>
List<A> EMPTY = nullptr;

/**
* Internal node structure for a list.
* @note required so that List can provide a distinct empty-list value
*/
template <typename A>
class Node
{
    friend List<A>;
public:
    /** . */
    ~Node() { release( tail ); /*std::cerr << "deleted " << this << "\n";*/ }
private:
    /**
     * Make an internal list node from an element and a pointer to a node.
     * @param x  an element
     * @param xs pointer to an existing node (or nullptr)
     */
    explicit Node( A x, Node* xs = EMPTY<A> ) : refs( 0 ), head( x ), tail( acquire( xs ) ) { /*std::cerr << "new " << this << "\n";*/ }

    Node( Node const& n ) : refs( 0 ), head( n.head ), tail( acquire( head.tail ) ) { /*std::cerr << "new " << this << " (copy of " << &n << ")\n";*/ }

    Node( Node && n ) noexcept : refs( n.refs ), head( std::move( n.head ) ), tail( nullptr ) { std::swap( tail, n.tail ); /*std::cerr << "new " << this << " (moved from " << &n << ")\n";*/ }

    Node& operator= ( Node n ) { swap( n ); /*std::cerr << "assigned to " << this << "\n";*/ return *this; }

    void swap( Node& n ) { std::swap( refs, n.refs ); std::swap( head, n.head ); std::swap( tail, n.tail ); /*std::cerr << "swapped " << this << " and " << &n << "\n";*/ }

    /** . */
    size_t  refs;
    /** The value of an element. */
    A const head;
    /** Node containing the next element. */
    Node*   tail;

    //static Node* acquire( Node* n ) { if ( n ) { ++(n->refs); } return n; }
    //static Node* release( Node* n ) { if (n) { if ( !--(n->refs) ) { delete n; } } return n; }
    Node* acquire( Node* n ) { /*std::cerr << this << " acquired " << n;*/ if ( n ) { ++(n->refs); /*std::cerr << " (refs=" << n->refs << ")";*/ } /*std::cerr << "\n";*/ return n; }
    Node* release( Node* n ) { /*std::cerr << this << " released " << n;*/ if (n) { /*std::cerr << " (refs=" << (n->refs - 1) << ")";*/ if ( !--(n->refs) ) { /*std::cerr << "\n   delete " << n << "!\n";*/ delete n; } } /*std::cerr << "\n";*/ return n; }

    template <typename B> friend bool    null( List<B> );
    template <typename B> friend B       head( List<B> );
    template <typename B> friend List<B> tail( List<B> );
    template <typename B> friend List<B> cons( B x, List<B> );
    //template <typename B> friend List<B> operator| ( B x, List<B> );
    template <typename B> friend std::ostream& operator<< ( std::ostream& os, Node<B> const& xs );
    template <typename F, typename B> friend auto map( F f, Node<B>* xs ) -> List<decltype(f(xs->head))>;
	template <typename B> friend B sum( List<B> const& xs );
};

/**
 * Constructs a list by pre-pending an element to an existing list.
 * @param x an element
 * @param xs a list
 * @return a list with <var>x</var> at the head and <var>xs</var> as the tail
 */
template <typename A>
inline List<A> cons( A x, List<A> xs ) { return new Node<A>( x, xs ); }
//inline List<A> operator| ( A x, List<A> xs ) { return new Node<A>( x, xs ); }

// List operations

/**
 * Obtains a new list by applying a function to each element of a list.
 * Converts a list of one type to that of another by applying a specified function.
 * @param f function that takes an element of <var>xs</var> and returns a different type
 * @param xs a list
 * @return a list of elements the same type as the return type of <var>f</var>
 */
template <typename F, typename A>
inline auto map( F f, List<A> xs ) -> List<decltype( f( xs->head ) )>
{
	using B = decltype( f( head(xs) ) );

  	if ( null( xs ) ) { return EMPTY<B>; }

  	auto ys = cons( f( head( xs ) ), EMPTY<B> );
  	auto from = xs;
  	auto to = ys;
  	while ( (from = tail( from ) ) ) {
    	//to = to->tail = Node<B>::acquire( cons( f( head( from ) ), EMPTY<B> ) );
    	to = to->tail = to->acquire( cons( f( head( from ) ), EMPTY<B> ) );
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
  return xs->head;
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
  return xs->tail;
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
    auto e = xs;
    while ( e ) {
        result += e->head;
        e = e->tail;
    }
    return result;

  	//if ( null( xs ) ) { return 0; }
  	//return head( xs ) + sum( tail( xs ) );
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
std::ostream& operator<< ( std::ostream& os, Node<A> const& xs )
{
  auto n = xs;
  if ( n ) {
    os << '[' << n->head;
    while ( (n = n->tail) ) {
      os << ',' << n->head;
    }
    os << ']';
  }
  return os;
}


} // end namespace prelude

#endif //HPP_PRELUDE_LIST
