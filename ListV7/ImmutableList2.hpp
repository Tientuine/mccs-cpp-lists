#ifndef HPP_IMMUTABLE_LIST
#define HPP_IMMUTABLE_LIST

#include <memory>
#include <functional>

template <typename A> class Node;

template <typename A>
using List = std::shared_ptr<Node<A>>;

template <typename B>
class Node
{
    friend List<B>;
public:
    Node(B x, List<B> const& xs) : _datum(x), _tail( xs ) {}
    Node(B x, List<B> && xs) : _datum(x), _tail( std::forward<List<B>>(xs) ) {}

private:
    B _datum;
    List<B> _tail;

    template <typename... Args> friend std::shared_ptr<Node> std::make_shared (Args&&...);
    template <typename A> friend bool    null( List<A> const& );
    template <typename A> friend A       head( List<A> const& );
    template <typename A> friend List<A> const& tail( List<A> const& );
    template <typename A> friend List<A> operator| ( A x, List<A> const& );
};

template <typename A>
List<A> EMPTY { nullptr };

template <typename A>
inline List<A> operator| ( A x, List<A> const& xs ) { return std::make_shared<Node<A>>( x, xs ); }

template <typename A>
inline List<A> operator| ( A x, List<A> && xs ) {
    return std::make_shared<Node<A>>( x, std::forward<List<A>>(xs) );
}

template <typename A>
inline A head( List<A> const& xs )
{
    if ( null( xs ) ) { throw xs; }
    return xs->_datum;
}

template <typename A>
inline List<A> const& tail( List<A> const& xs )
{
    if ( null( xs ) ) { throw xs; }
    return xs->_tail;
}

template <typename A>
inline bool null( List<A> const& xs ) { return !xs; }

template <typename A>
inline unsigned length( List<A> const& xs )
{
  if ( null( xs ) ) {
    return 0;
  } else {
    return 1 + length( tail( xs ) );
  }
}

#endif //HPP_PRELUDE_LIST

