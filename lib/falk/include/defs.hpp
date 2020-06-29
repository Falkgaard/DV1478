#pragma once

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <utility>
#include <functional>
#include <algorithm>
#include <utility>
#include <memory>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>

// unsigned integer types:
                            using U8        = uint8_t;
                            using U16       = uint16_t;
                            using U32       = uint32_t;
                            using U64       = uint64_t;
// signed integer types:
                            using I8        = int8_t;
                            using I16       = int16_t;
                            using I32       = int32_t;
                            using I64       = int64_t;
// byte type:
                            using Byte      = U8;
// bool type:
                            using Bool      = bool;
// floating point types:
                            using F32       = float;
                            using F64       = double;
// string types:
                            using Str       = std::basic_string<char>;
// size type:
                            using Size      = size_t;
// index type:
                            using Idx       = size_t;
// void type:
                            using Void      = void;
// function type:
template< class R, class... Args >
                            using Fun       = std::function<R(Args...)>;
// array types:
template <class T>          using Vec       = std::vector<T>;
template <class T, Size N>  using Arr       = std::array<T,N>;
// hash map type:
template <class K, class V> using HashMap   = std::unordered_map<K,V>;
// hash set type:
template <class T>          using HashSet   = std::unordered_set<T>;
// smart pointer types:
template <class T>          using SPtr      = std::shared_ptr<T>;
template <class T>          using UPtr      = std::unique_ptr<T>;
template <class T>          using WPtr      = std::weak_ptr<T>;
// container types:
template <class T>          using Opt       = std::optional<T>;

static inline constexpr Idx         invalid_idx = -1;
static inline constexpr char const *invalid_str = "ERROR";

using RGBA = U32;

#ifdef _DEBUG
   Bool constexpr isDebugging {  true };
#else
   Bool constexpr isDebugging { false };
#endif

// vectors (TODO: padding alignment if sizeof(T)%4!=0?)
template <typename T> union V2 {
	T  data[2];
	struct { T  x, y; }; // anon
	struct { T  r, g; }; // anon

   V2(                      ): x(T()), y(T())         {}
   V2( T const x, T const y ): x(x), y(y)             {}
   V2( V2<T> const  &other  ): x(other.x), y(other.y) {}

   inline Bool operator==( V2<T> const &other ) const noexcept {
      return (x == other.x) && (y == other.y);
   }

   inline V2<T> &operator=( V2<T> const &other ) noexcept {
      if ( &other != this ) {
         x = other.x;
         y = other.y;
      }
      return *this;
   }

   template <typename T2>
   inline V2<T> operator+( V2<T2> const &other ) const noexcept {
      return { x + other.x,   y + other.y };
   }

   inline Bool operator!=( V2 const &other ) const {
      return (x != other.x) or (y != other.y);
   }
};
using V2f = V2<F32>;
using V2u = V2<U32>;
using V2i = V2<I32>;


template <typename T> union V3 {
   T  data[3] { T() };
	struct     { T  x, y, z; }; // anon
	struct     { T  r, g, b; }; // anon

   V3(                                 ): x(T()), y(T()), z(T())             {}
   V3( T const x, T const y, T const z ): x(x), y(y), z(z)                   {}
   V3( V3<T> const &other              ): x(other.x), y(other.y), z(other.z) {}

   inline Bool operator==( V3<T> const &other ) const noexcept {
      return (x == other.x) && (y == other.y) && (z == other.z);
   }

   inline V3<T> &operator=( V3<T> const &other ) noexcept {
      if ( &other != this ) {
         x = other.x;
         y = other.y;
         z = other.z;
      }
      return *this;
   }

   template <typename T2>
	inline V3<T> operator+( V3<T2> const &other ) const noexcept {
      return { x + other.x,   y + other.y,   z + other.z };
   }
   
   inline Bool operator!=( V3 const &other ) const {
      return (x != other.x) or (y != other.y) or (z != other.z);
   }
};
using V3f = V3<F32>;
using V3u = V3<U32>;
using V3i = V3<I32>;

template <typename T> union V4 {
	T  data[4];
	struct { T  x, y, z, w; }; // anon
	struct { T  r, g, b, a; }; // anon

   V4(                                            ): x(T()), y(T()), z(T()), w(T())                 {}
   V4( T const x, T const y, T const z, T const w ): x(x), y(y), z(z), w(w)                         {}
   V4( V4<T> const  &other                        ): x(other.x), y(other.y), z(other.z), w(other.w) {}

   inline Bool operator==( V4<T> const &other ) const noexcept {
      return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w);
   }

   inline V4<T> &operator=( V4<T> const &other ) noexcept {
      if ( &other != this ) {
         x = other.x;
         y = other.y;
         z = other.z;
         w = other.w;
      }
      return *this;
   }

   template <typename T2>
	inline V4<T> operator+( V4<T2> const &other ) const noexcept {
      return { x + other.x,   y + other.y,   z + other.z,   w + other.w };
   }
   
   inline Bool operator!=( V4 const &other ) const {
      return (x != other.x) or (y != other.y) or (z != other.z) or (w != other.w);
   }
};
using V4f = V4<F32>;
using V4u = V4<U32>;
using V4i = V4<I32>;

struct Bounds { V2u min, max; };

// EOF
