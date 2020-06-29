#include "hello_imgui/hello_imgui.h"

#ifdef HELLOIMGUI_USE_SDL_OPENGL3
   #include <SDL.h>
#endif

#include "falk/Voronoi.hpp"

I32 _main( I32 const argc, char const *argv[] ) {
   DistanceFunction distance_function = DistanceFunction::euclidean;
   if ( argc != 1 ) { // if decided through command-line arg:
      Str distance_function_arg { argv[1] };
      if ( distance_function_arg =="Euclidean" or distance_function_arg =="euclidean" )
         distance_function = DistanceFunction::euclidean;
      else if ( distance_function_arg =="Manhattan" or distance_function_arg =="manhattan" )
         distance_function = DistanceFunction::manhattan;
      else if ( distance_function_arg =="Chebychev" or distance_function_arg =="chebychev" )
         distance_function = DistanceFunction::chebychev;
      else if ( distance_function_arg =="Weirdness" or distance_function_arg =="weirdness" )
         distance_function = DistanceFunction::weirdness;
      else assert( false and "Valid distance functions are: Euclidean, Manhattan, Chebychev, Weirdness" );
   }
                                               // default values:
   Str  filename    = argc >2?           argv[2]  : "output.png";
   U32  side        = argc >3? std::stoi(argv[3]) :          512;
   U32  cell_count  = argc >4? std::stoi(argv[4]) :         1024;
   F32  weight_min  = argc >5? std::stof(argv[5]) :         1.0f;
   F32  weight_max  = argc >6? std::stof(argv[6]) :         1.0f;

   RNG::Engine rng_engine;
   if ( argc >7 )
      rng_engine = RNG::Engine { std::stoul(argv[4]) };
   assert( argc < 9 );

   RNG::Real<F32>  rng_weight   { rng_engine, weight_min, weight_max };
   RNG::Real<F32>  rng_axis_pos { rng_engine, .0f, F32(side) };
   Voronoi<true> v { V2f( side, side ), cell_count };
   for ( auto i = 0;  i < cell_count;  ++i ) {
      V2f pos    = { rng_axis_pos(), rng_axis_pos() };
      F32 weight = rng_weight();
      if constexpr ( isDebugging )
         printf( "Creating centre node i %d at (%3.3f,%3.3f) with weight: %2.1f\n", i, pos.x, pos.y, weight );
      v.addCentre( pos, weight );
   }
   V2u dimensions{ side, side };
   auto map = [&]() {
      switch ( distance_function ) {
         default: // make Euclidean distance the default
         case DistanceFunction::euclidean: return v.toMap<EuclideanDistance>( dimensions );
         case DistanceFunction::manhattan: return v.toMap<ManhattanDistance>( dimensions );
         case DistanceFunction::chebychev: return v.toMap<ChebychevDistance>( dimensions );
         case DistanceFunction::weirdness: return v.toMap<WeirdnessDistance>( dimensions );
      }
   }();
   auto growth_targets = generate_growth_targets(v, .33f, 2, 13, rng_engine );
   grow_regions<true>( v, map, growth_targets, rng_engine );

   map2png( map, filename );
   return 0;
}


I32 main( I32, char** )
{
   HelloImGui::Run(
      [] {             
         // app code start    
         ImGui::Text("Hello, world!");
         // app code end
      },               
      { 640.f, 480.f }, // window dimensions
      "Map Generator!"  // title
     );
}

