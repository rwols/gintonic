#define BOOST_TEST_MODULE Casting test
#include "Casting.hpp"
#include <algorithm>
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <iostream>
#include <vector>

using namespace gintonic;

class A
{
  public:
    enum class Kind
    {
        Ax,
        B,
        C,
        D
    };

  private:
    const Kind mKind;

  public:
    A(const Kind kind) : mKind(kind) {}
    virtual ~A() = default;
    Kind getKind() const noexcept { return mKind; }
};

class B : public A
{
  private:
  protected:
    B(const Kind derived) : A(derived) {}

  public:
    B() : B(Kind::B) {}
    ~B() override = default;
    static bool classOf(const A* ptr) noexcept
    {
        return Kind::B <= ptr->getKind() && ptr->getKind() <= Kind::C;
    }
};

class C : public B
{
  private:
  public:
    C() : B(Kind::C) {}
    ~C() override = default;
    static bool classOf(const A* ptr) noexcept
    {
        return ptr->getKind() == Kind::C;
    }
};

class D : public A
{
  private:
  public:
    D() : A(Kind::D) {}
    ~D() override = default;
    static bool classOf(const A* ptr) noexcept
    {
        return ptr->getKind() == Kind::D;
    }
};

BOOST_AUTO_TEST_CASE(cast_various_things)
{
    auto ptr = std::unique_ptr<A>(new B());
    BOOST_CHECK(dynCast<B>(ptr.get()));
    ptr = std::unique_ptr<A>(new C());
    BOOST_CHECK(dynCast<C>(ptr.get()));
    BOOST_CHECK(dynCast<B>(ptr.get()));
    BOOST_CHECK(nullptr == dynCast<D>(ptr.get()));
}
