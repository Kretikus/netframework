#pragma once

#include <cstddef>
#include <string>

class StringLiteral {
private:
  const char *const p_;
  const std::size_t sz_;

public:
  template <std::size_t N>
  /* implicit */ constexpr StringLiteral(const char (&a)[N])
      : p_(a), sz_(N - 1) {}
  constexpr StringLiteral(const char *a, std::size_t N) : p_(a), sz_(N) {}

  constexpr std::size_t size() const { return sz_; }
  constexpr bool empty() const { return sz_ == 0; }

  constexpr char front() const {
    return (!empty()) ? p_[0]
                      : throw "Cannot call front() on an empty StringLiteral";
  }

  constexpr bool operator==(const StringLiteral &Other) const {
    return (size() == Other.size()) &&
           (empty() ||
            (front() == Other.front() && (drop_front() == Other.drop_front())));
  }

  constexpr StringLiteral drop_front(std::size_t count = 1) const {
    return (size() >= count)
               ? StringLiteral(p_ + count, sz_ - count)
               : throw "drop_front() cannot drop that many elements!";
  }

  constexpr StringLiteral drop_back(std::size_t count = 1) const {
    return (size() >= count)
               ? StringLiteral(p_, sz_ - count)
               : throw "drop_back() cannot drop that many elements!";
  }

  constexpr StringLiteral take_front(std::size_t count = 1) const {
    return (count == sz_) ? *this : drop_back(size() - count);
  }

  constexpr bool startswith(const StringLiteral &Other) const {
    return (size() >= Other.size()) && (take_front(Other.size()) == Other);
  }

  constexpr bool contains(char c) const {
    return !empty() && (front() == c || drop_front().contains(c));
  }

  constexpr bool contains(const StringLiteral &Other) const {
    return startswith(Other) || (!empty() && drop_front().contains(Other));
  }

  constexpr std::size_t find(const StringLiteral &Other) const {
    return !contains(Other)
               ? -1
               : (startswith(Other) ? 0 : 1 + drop_front().find(Other));
  }
};
