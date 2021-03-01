#include <memory>

class Foo {
 private:
   struct this_is_private {};

   struct deleter {
       void operator()(Foo* ptr) const {
           delete ptr;
       }
   };

 public:
   explicit Foo(const this_is_private &, int x) : Foo(x) {}

   static ::std::shared_ptr<Foo> getSharedPointer() {
      return ::std::make_shared<Foo>(this_is_private{}, 5);
   }

    ~Foo(){}
 protected:
    Foo(int x){}
};


