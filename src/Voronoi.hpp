#pragma once

#include "falk/defs.hpp"
#include "falk/RNG.hpp"

#include <cassert>
#include <cstdio>
#include <cmath>
#include <limits>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../stb_image.h"
#include "../../stb_image_write.h"

template <class T>
class RandomAccessHashSet {
public:
   void insert( T const &e ) {
      if ( not m_set.contains(e) ) {
         m_vec.push_back(e);
         m_set.insert(e);
      }
   }
   
   void remove( T const &e ) {
      if ( m_set.contains(e) ) {
         m_set.erase(e);
         m_vec.erase( std::remove(m_vec.begin(), m_vec.end(), e), m_vec.end() );
      }
   }

   Bool contains( T const &e ) const {
      return m_set.contains(e);
   }

   Bool empty() const {
      return m_vec.empty();
   }

   Size size() const {
      return m_vec.size();
   }

   T& operator[]( Idx index ) {
      return m_vec[index];
   }

   T const& operator[]( Idx index ) const {
      return m_vec[index];
   }
   
   auto begin() {
      return m_vec.begin();
   }
   
   auto end() {
      return m_vec.end();
   }

private:
   HashSet<T>  m_set;
   Vec<T>      m_vec;
};

namespace Direction {
   static Idx constexpr N  = 0,
                        NE = 1,
                        E  = 2,
                        SE = 3,
                        S  = 4,
                        SW = 5,
                        W  = 6,
                        NW = 7;
};

template <typename T>
struct Map {
   struct InContextValue {
      T         &value;
      V2u const  pos;
   };

   struct InContextIterator {
      InContextIterator( Map &map, V2u pos ):
         m_map ( map ),
         m_pos ( pos )
      {}

      InContextValue operator*() {
         assert( m_pos != m_map.dimensions() );
         return { m_map(m_pos), m_pos }; 
      }
      
      InContextValue const operator*() const {
         assert( m_pos != m_map.dimensions() );
         return { m_map(m_pos), m_pos }; 
      }

      void operator++() {
         if ( m_pos.x < m_map.width()-1 )
            ++m_pos.x;
         else {
            ++m_pos.y;
            m_pos.x = 0;
         }
         assert( m_pos.x < m_map.width() );
         assert( ( m_pos.y < m_map.height() ) or ( m_pos == V2u(0, m_map.height()) ) );
      }

      bool operator!=( InContextIterator const &other ) const {
         return m_pos != other.m_pos;
      }

   private:
      Map &m_map;
      V2u  m_pos;
   };

   struct InContext {
      InContext( Map &map ):
         m_map ( map )
      {}

      InContextIterator begin() {
         return InContextIterator( m_map, {0U,0U} );
      }
     
      InContextIterator const begin() const {
         return InContextIterator( m_map, {0U,0U} );
      }
     
      InContextIterator end() {
         return InContextIterator( m_map, V2u(0, m_map.height()) );
      }

      InContextIterator const end() const {
         return InContextIterator( m_map, V2u(0, m_map.height()) );  
      }

   private:
      Map &m_map;
   };

   struct Iterator {
      Iterator( Map &map, V2u pos ):
         m_map ( map ),
         m_pos ( pos )
      {}

      T& operator*() {
         assert( m_pos != m_map.dimensions() );
         return m_map( m_pos ); 
      }
      
      T const& operator*() const {
         assert( m_pos != m_map.dimensions() );
         return m_map( m_pos ); 
      }

      void operator++() {
         if ( m_pos.x < m_map.width()-1 )
            ++m_pos.x;
         else {
            ++m_pos.y;
            m_pos.x = 0;
         }
      }

      bool operator!=( Iterator const &other ) const {
         return m_pos != other.m_pos;
      }

   private:
      Map &m_map;
      V2u  m_pos;
   };

   Map( Size width, Size height, T default_value = T() ):
      m_map ( width * height, default_value ),
      m_dim ( width, height )
   {}

   Map( V2u dim, T default_value = T() ):
      m_map ( dim.x * dim.y, default_value ),
      m_dim ( dim )
   {}

   Iterator begin() {
      return Iterator( *this, {0U, 0U} );
   }

   Iterator const begin() const {
      return Iterator( *this, {0U, 0U} );
   }

   Iterator end() {
      return Iterator( *this, V2u(0, height()) );
   }

   Iterator const end() const {
      return Iterator( *this, V2u(0, height()) );
   }

   InContext in_context() {
      return InContext( *this );
   }

   InContext const in_context() const {
      return InContext( *const_cast<Map*>(this) );
   }

   T& operator()( Size x, Size y ) {
      assert( (x < m_dim.x) and (y < m_dim.y) );
      return m_map[ index(x,y) ];
   }

   T const& operator()( Size x, Size y ) const {
      assert( (x < m_dim.x) and (y < m_dim.y) );
      return m_map[ index(x,y) ];
   }

   T& operator()( V2u pos ) {
      assert( (pos.x < m_dim.x) and (pos.y < m_dim.y) );
      return m_map[ index(pos) ];
   }

   T const& operator()( V2u pos ) const {
      assert( (pos.x < m_dim.x) and (pos.y < m_dim.y) );
      return m_map[ index(pos) ];
   }

   template <Bool T_is_wrapped=true>
   Arr<T,8> get_neighbours( V2u pos ) const {
     Arr<T,8> neighbours;
     neighbours[Direction::N ] = m_map[ index( pos.x,              (pos.y-1) % m_dim.y) ];
     neighbours[Direction::NE] = m_map[ index((pos.x+1) % m_dim.x, (pos.y-1) % m_dim.y) ];
     neighbours[Direction::E ] = m_map[ index((pos.x+1) % m_dim.x,  pos.y             ) ];
     neighbours[Direction::SE] = m_map[ index((pos.x+1) % m_dim.x, (pos.y+1) % m_dim.y) ];
     neighbours[Direction::S ] = m_map[ index( pos.x,              (pos.y+1) % m_dim.y) ];
     neighbours[Direction::SW] = m_map[ index((pos.x-1) % m_dim.x, (pos.y+1) % m_dim.y) ];
     neighbours[Direction::W ] = m_map[ index((pos.x-1) % m_dim.x,  pos.y             ) ];
     neighbours[Direction::NW] = m_map[ index((pos.x-1) % m_dim.x, (pos.y-1) % m_dim.y) ];
     return neighbours;
   }

   inline Size width() const {
      return m_dim.x;
   }

   inline Size height() const {
      return m_dim.y;
   }

   inline V2u dimensions() const {
      return m_dim;
   }
   
   inline Size index( V2u pos ) const {
      return index( pos.x, pos.y );
   }

   inline Size index( Size x, Size y ) const {
      return y * m_dim.x + x;
   }

private:
   Vec<T>  m_map; // map data
   V2u     m_dim; // map dimensions
};

// TODO: DistanceFunction Concept
enum class DistanceFunction { manhattan, euclidean, chebychev, weirdness };

struct ManhattanDistance {
   F32 operator()( V2f p1, V2f p2 ) const {
      return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
   }                   
};

struct EuclideanDistance {
   F32 operator()( V2f p1, V2f p2 ) const {
      return std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2);  
   }                   
};

struct ChebychevDistance {
   F32 operator()( V2f p1, V2f p2 ) const {
      return std::max( std::abs(p1.x - p2.x),  std::abs(p1.y - p2.y) );
   }
};

struct WeirdnessDistance {
   F32 operator()( V2f p1, V2f p2 ) const {
      return std::abs(std::pow(p1.x - p2.x, 3)) + std::abs(std::pow(p1.y - p2.y, 3));
   }
};

// TODO: make function names conformant
template <Bool T_is_tiled = false, U8 T_threshold_percentage=10>
struct Voronoi {
   struct Centre {
      V2f  pos    = { .0f, .0f };
      F32  weight = 1.0f;
      Idx  index  = invalid_idx;
   };

   Voronoi( V2f dimensions, Opt<Size> maybe_reserve_count={} ):
      m_dim ( dimensions )
   {
      if ( maybe_reserve_count )
         m_centres.reserve( maybe_reserve_count.value() );
   }

   void addCentre( V2f pos, F32 weight=1.0f ) {
      assert( pos.x <= m_dim.x and pos.y <= m_dim.y );
      m_centres.push_back( Centre{ pos, weight, m_next_idx++ } );
      if constexpr ( T_is_tiled )
         tileCopyCentre( m_centres.back() );
   }

   Size size() const {
      return m_next_idx;
   }

   // NOTE: Unoptimized! TODO
   template <typename T_DistanceFunction = EuclideanDistance>
   Map<Idx> toMap( V2u dimensions, V2f offset={.0f,.0f} ) const {
      T_DistanceFunction distance_between;
      Map<Idx> map { dimensions };
      for ( auto point : map.in_context() ) {
         F32  shortestDistance = std::numeric_limits<F32>::max();
         Size closestCentreIdx = -1;
         for ( auto const &centre : m_centres ) {
            F32 distance = distance_between( offset+point.pos, centre.pos ) * (1.0f / centre.weight);
            if ( distance < shortestDistance ) {
               shortestDistance = distance;
               closestCentreIdx = centre.index;
            }
         }
         point.value = closestCentreIdx;
      }
      return map;
   }

private:
   Size         m_next_idx = 0;
   V2f          m_dim;
   Vec<Centre>  m_centres;

   void tileCopyCentre( Centre const &c ) {
      static constexpr F32 threshold = .01f * T_threshold_percentage;

      static F32 const  x_lower_threshold =         threshold  * m_dim.x,
                        x_upper_threshold = (1.0f - threshold) * m_dim.x,
                        y_lower_threshold =         threshold  * m_dim.y,
                        y_upper_threshold = (1.0f - threshold) * m_dim.y;
 
      Bool n = c.pos.y > y_upper_threshold,
           s = c.pos.y < y_lower_threshold,
           w = c.pos.x > x_upper_threshold,
           e = c.pos.x < x_lower_threshold;

      if (n and w) m_centres.push_back( Centre{c.pos + V2f{-m_dim.x, -m_dim.y}, c.weight, c.index } );
      if (n      ) m_centres.push_back( Centre{c.pos + V2f{     .0f, -m_dim.y}, c.weight, c.index } );
      if (n and e) m_centres.push_back( Centre{c.pos + V2f{ m_dim.x, -m_dim.y}, c.weight, c.index } );
      if (      e) m_centres.push_back( Centre{c.pos + V2f{ m_dim.x,      .0f}, c.weight, c.index } );
      if (s and e) m_centres.push_back( Centre{c.pos + V2f{ m_dim.x,  m_dim.y}, c.weight, c.index } );
      if (s      ) m_centres.push_back( Centre{c.pos + V2f{     .0f,  m_dim.y}, c.weight, c.index } );
      if (s and w) m_centres.push_back( Centre{c.pos + V2f{-m_dim.x,  m_dim.y}, c.weight, c.index } );
      if (      w) m_centres.push_back( Centre{c.pos + V2f{-m_dim.x,      .0f}, c.weight, c.index } );
   }
};

using CellNeighbourMap = HashMap<Idx,RandomAccessHashSet<Idx>>;
CellNeighbourMap  generate_neighbour_map( Map<Idx> const &map ) {
   CellNeighbourMap  neighbour_map;
   for ( auto const &e : map.in_context() )
      for ( auto const &neighbour : map.get_neighbours(e.pos) )
         if ( neighbour != e.value )
            neighbour_map[e.value].insert(neighbour);
   return neighbour_map;
}

void map2png( Map<Idx> const &map, Str path ) {
   Size const TEX_WIDTH  { map.width()  },
              TEX_HEIGHT { map.height() };
   Vec<RGBA>  pixels( TEX_WIDTH * TEX_HEIGHT );

   for ( auto e: map.in_context() )
      pixels[ map.index(e.pos) ] = 0xFF'000000 + (((U32)std::pow(e.value, 2)) & 0x00'FFFFFFu);
   
   stbi_write_png( path.c_str(), static_cast<I32>(TEX_WIDTH), static_cast<I32>(TEX_HEIGHT), 4, pixels.data(), TEX_WIDTH * 4 );
}

void neighbours_map2png( Map<Size> const &map, CellNeighbourMap const &neighbours_map, Str path ) {
   Size const TEX_WIDTH  { map.width()  },
              TEX_HEIGHT { map.height() };
   Vec<RGBA>  pixels( TEX_WIDTH * TEX_HEIGHT );

   for ( auto e: map.in_context() )
      pixels[ map.index(e.pos) ] = 0xFF'000000 + (((U32)std::pow(neighbours_map.at(e.value).size(), 13)) & 0x00'FFFFFFu);
   
   stbi_write_png( path.c_str(), static_cast<I32>(TEX_WIDTH), static_cast<I32>(TEX_HEIGHT), 4, pixels.data(), TEX_WIDTH * 4 );
   
}

struct CellGrowth {
    Idx  index;
    Size max_remaining_growth;
};

template <Bool T_is_tiled = false, U8 T_threshold_percentage=10>
Vec<CellGrowth> generate_growth_targets( Voronoi<T_is_tiled, T_threshold_percentage> const &voronoi_diagram, 
                                         F32                                                percentage_of_growth_targets,
                                         U8                                                 min_target_growth,
                                         U8                                                 max_target_growth,
                                         RNG::Engine                                       &rng_engine )
{
   Idx                largest_index     = voronoi_diagram.size() - 1;
   Size               num_targets       = percentage_of_growth_targets * voronoi_diagram.size();
   RNG::Int<Idx>      index_rng         = { rng_engine, 0, largest_index };
   RNG::Int<U8>       target_growth_rng = { rng_engine, min_target_growth, max_target_growth };
   
   Vec<CellGrowth> growth_targets = {};
   growth_targets.reserve( num_targets );
  
   HashMap<Idx,Bool> claimed = {};
   for ( auto index = voronoi_diagram.size();  index --> 0; )
      claimed[index] = false;

   while ( --num_targets ) {
      Idx index; 
      while ( claimed[index=index_rng()] );

      growth_targets.push_back({ index, target_growth_rng()  });
      claimed[index] = true;
   }
   return growth_targets;
}


// TODO: categorize map border (W,E,N,S)
// for pole/ocean generation

template <Bool T_is_tiled = false, U8 T_threshold_percentage=10>
void grow_regions( Voronoi<T_is_tiled, T_threshold_percentage> const &voronoi_diagram,
                   Map<Idx>                                          &map,
                   Vec<CellGrowth>                                   &growth_targets,
                   RNG::Engine                                       &rng_engine ) 
{
   // TODO: grow big areas first?
   
   auto neighbour_map = generate_neighbour_map( map );

   // remove growth target cells from neighbour sets
   for ( auto &[key,set] : neighbour_map )
      for ( auto target : growth_targets )
         if ( set.contains(target.index) )
            set.remove( target.index );

   // initialize cells' lowest assimilated index map
   HashMap<Idx,Idx>  lowest_assimilated_index;
   for ( auto index = voronoi_diagram.size();  index --> 0; )
      lowest_assimilated_index[index] = index;

   // initialize cell ownership map
   HashMap<Idx,Idx>  cell_owner;
   for ( auto index = voronoi_diagram.size();  index --> 0; )
      cell_owner[index] = index;

   RNG::Real<>  rng( rng_engine, .0f, 1.0f );
   while ( not growth_targets.empty() ) {
      for ( auto &growth_target : growth_targets ) {
         auto &neighbour_set = neighbour_map[growth_target.index];
         // remove growth_target if growth is no longer possible:
         if ( (growth_target.max_remaining_growth < 1) or neighbour_set.empty() ) {
            growth_targets.erase(
               std::remove_if(   growth_targets.begin(),
                                 growth_targets.end(),
                                 [&growth_target]( CellGrowth const &e ) {
                                    return e.index == growth_target.index;
                                 } ),
               growth_targets.end()
            );
         }
         else { // otherwise grow by one cell:
            // select random neighbour
            Idx  random_set_index       = rng() * neighbour_set.size();
            Idx  random_neighbour_index = neighbour_set[random_set_index];
            neighbour_set.remove( random_neighbour_index );
            // remove from all neighbour lists:
            for ( auto &[key,set] : neighbour_map )
               set.remove( random_neighbour_index );
            
            // assimilate the random neighbour:
            
            // assimilate all neighbours of the random cell_owner
            for ( auto &neighbour : neighbour_map[random_neighbour_index] )
               neighbour_set.insert( neighbour );
            // update lowest assimilated index if necessary:
            cell_owner[random_neighbour_index] = growth_target.index;
            if ( random_neighbour_index < growth_target.index )
               lowest_assimilated_index[growth_target.index] = random_neighbour_index;

            growth_target.max_remaining_growth--;
         }
      }
   }

   // update map: // TODO: remove later and instead just keep the cell owner map
   for ( auto &index : map )
      index = lowest_assimilated_index[cell_owner[index]];
      // TODO: update Voronoi diagram centres 
}

// EOF
