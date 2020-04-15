// Headers {{{
   #include <string>
   #include <vector>
   #include <utility>
   #include <iostream>
   #include <iterator>
   #include <type_traits>
   #include <unordered_map>
   #include <unordered_set>
   #include <optional>
// Headers }}}
// Macros {{{
   // #define for_ipair(i,e,v)   for ( auto [i,e]: IPair(v) )
// Macros }}}
// Aliases {{{
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
   // string:
                               using CStr      = char *;
                               using Str       = std::string;
   // size type:
                               using Size      = size_t;
   // function type:
   template <class R, class... Args>
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
// Aliases }}}
// Global Constants {{{
   // TODO: Decide on how to handle logging verbosity levels.
   #ifdef _DEBUG
      Bool constexpr g_isDebugging = true;
   #else
      Bool constexpr g_isDebugging = false;
   #endif
// Global Constants }}}
// Implementations {{{
   // String Hashing {{{
   namespace falk {
      constexpr inline I64 hash( char const *c_str, I32 offset=0 ) noexcept {
         return c_str[offset]? (hash(c_str,offset+1)*33) xor c_str[offset]  :  5381;
      }
      inline I64 hash( std::string const &s ) noexcept {
         return hash( s.c_str() );
      }
   }
   // hash string literal operator
   constexpr inline I64 operator ""_h( char const *c_str, size_t ) noexcept {
      return falk::hash( c_str );
   }
   // String Hashing }}}
   // Random Access IPair Iteration {{{
   template <class T_RandomAccessCollection>
   class IPairIterator {
   // TODO: static_assert that T_RandomAccessCollection complies to the concept
   public:
       using iterator_category = std::random_access_iterator_tag;
       using value_type        = typename T_RandomAccessCollection::value_type;
       using size_type         = typename T_RandomAccessCollection::size_type;
       using difference_type   = typename T_RandomAccessCollection::difference_type;
       using pointer           = typename T_RandomAccessCollection::pointer;
       using const_pointer     = typename T_RandomAccessCollection::const_pointer;
       using reference         = typename T_RandomAccessCollection::reference;
       using const_reference   = typename T_RandomAccessCollection::const_reference;

       IPairIterator( T_RandomAccessCollection &c, size_type index ):
           m_collection ( c ),
           m_index      ( index )
       {}
       
       std::pair<
           size_type,
           std::conditional_t< 
               std::is_const_v<T_RandomAccessCollection>,
                   const_reference,
                   reference
           >
       >
       operator*() {
           return { m_index, m_collection[m_index] };
       }
       
       IPairIterator & operator+=( difference_type offset ) {
           m_index += offset;
           return *this;
       }
       
       IPairIterator & operator-=( difference_type offset ) {
           m_index -= offset;
           return *this;
       }
       
       IPairIterator & operator++() {
           ++m_index;
           return *this;
       }
       
       IPairIterator operator+( difference_type offset ) const {
           return { m_collection, m_index + offset };
       }
       
       IPairIterator & operator--() {
           --m_index;
           return *this;
       }
       
       difference_type operator-( IPairIterator const &other ) const {
           return m_index - other.m_index;
       }
       
       // TODO: n + it (is it really necesssary though? bigThonk)
       
       IPairIterator operator-( difference_type offset ) const {
           return { m_collection, m_index - offset };
       }
       
       reference operator[] ( size_type index ) {
           return m_collection[index];
       }
       
       bool operator< ( IPairIterator const &other ) const {
           return m_index < other.m_index;
       }
       
       bool operator<= ( IPairIterator const &other ) const {
           return m_index <= other.m_index;
       }
       
       bool operator> ( IPairIterator const &other ) const {
           return m_index > other.m_index;
       }
       
       bool operator>= ( IPairIterator const &other ) const {
           return m_index >= other.m_index;
       }
       
       bool operator!= ( IPairIterator const &other ) const {
           return m_index != other.m_index;
       }

   private:
       T_RandomAccessCollection &m_collection;
       size_type                 m_index;
   };

   template <class T_RandomAccessCollection>
   class IPair  {
   public:
       IPair( T_RandomAccessCollection &c ):
           m_collection ( c )
       {}
       
       IPairIterator<T_RandomAccessCollection> begin() {
           return { m_collection, 0 };
       }
       
       IPairIterator<T_RandomAccessCollection> end() {
           return { m_collection, m_collection.size() };
       }
       
   private:
       T_RandomAccessCollection &m_collection;
   };
   // Random Access IPair Iteration }}}
// Implementations }}}
// EOF
