// Headers {{{
   // C {{{
      #include <cstdlib>
      #include <cassert>
      #include <cstdint>
   // C }}}
   // C++ {{{
      #include <iostream>
      #include <stdexcept>
      #include <functional>
      #include <vector>
      #include <set>
      #include <optional>
   // C++ }}}
   // GLFW & VK {{{
      #define GLFW_INCLUDE_VULKAN
      #include <GLFW/glfw3.h>
   // GLFW & VK }}}
   // {fmt} {{{
      #include <fmt/core.h>
      #include <fmt/format.h>
   // {fmt} }}}
   // Falk {{{
      #include "falk.hpp"
   // Falk }}}
// Headers }}}
// Namespaces {{{
   // Ur {{{
      // UR: Project namespace
      namespace ur {
         void init();
         // Log {{{
            // WHAT: Namespace for logging stuff.
            namespace log {
            // Structs {{{
               // FG {{{
                  // WHAT: Wraps the ANSI escape code for 256-bit foreground colours.
                  struct FG {
                     U8 code;
                     explicit FG( U8 code=255 ) noexcept: //{{{
                        code ( code ) // default 255 = white
                     {} //}}}
                     Str to_string() const noexcept { //{{{
                        return fmt::format("38;6;{}", code);
                     } //}}}
                  };
               // FG }}}
               // BG {{{
                  // WHAT: Wraps the ANSI escape code for 256-bit background colours.
                  struct BG {
                     U8 code;
                     explicit BG( U8 code=0 ) noexcept: //{{{
                        code ( code ) // default 0 = black
                     {} //}}}
                     Str to_string() const noexcept { //{{{
                        return fmt::format("48;6;{}", code);
                     } //}}}
                  };
               // BG }}}
               // Mod {{{
                  // WHAT: Wraps the ANSI escape modifier codes.
                  struct Mod {
                     Mod() noexcept: //{{{
                        m_data ( 0 ) // default plain
                     {} //}}}
                     Str to_string () const noexcept { //{{{
                        Str s {};
                        if ( m_data &       bold.m_data ) s += "1;";
                        if ( m_data &      faint.m_data ) s += "2;";
                        if ( m_data &     italic.m_data ) s += "3;";
                        if ( m_data &  underline.m_data ) s += "4;";
                        if ( m_data & slow_blink.m_data ) s += "5;";
                        if ( m_data & fast_blink.m_data ) s += "6;";
                        return s;
                     } //}}}
                     Mod operator& ( Mod other ) const noexcept { //{{{
                        if ( (m_data & Mod::clear.m_data) or (other.m_data & Mod::clear.m_data) )
                           return Mod::clear;
                        else return Mod( m_data | other.m_data );
                     } //}}}
                     // Static Modifier declarations {{{
                        static Mod const none       ;
                        static Mod const bold       ;
                        static Mod const faint      ;
                        static Mod const italic     ;
                        static Mod const underline  ;
                        static Mod const fast_blink ;
                        static Mod const slow_blink ;
                        static Mod const clear      ;
                     // Static Modifier declarations }}}
                  private:
                     explicit Mod( U8 bitmask ) noexcept: //{{{
                        m_data ( bitmask ) // private internal constructor
                     {} //}}}
                     U8 m_data;
                  };
                  // Static Modifier definitions {{{
                     Mod const Mod::none       {   0 };
                     Mod const Mod::bold       {   1 };
                     Mod const Mod::faint      {   2 };
                     Mod const Mod::italic     {   4 };
                     Mod const Mod::underline  {   8 };
                     Mod const Mod::fast_blink {  16 };
                     Mod const Mod::slow_blink {  32 };
                     Mod const Mod::clear      { 128 };
                  // Static Modifier definitions }}} 
               // Mod }}}
               // Style {{{
               inline Str ansi( Str const &text, FG fg, BG bg, Mod mod ) noexcept; // forward decl

               struct Style {
                  FG  fg  {};
                  BG  bg  {};
                  Mod mod {};

                  inline Str operator()( Str const &text ) const noexcept { //{{{
                     return ansi( text, fg, bg, mod );
                  } //}}}
               };
               // Style }}}
               // Theme {{{
               struct Theme {
                  Style critical,
                        error,
                        warning,
                        plain,
                        header1,
                        header2,
                        header3,
                        debug,
                        selected,
                        list_positive,
                        list_neutral,
                        list_negative;
               };     
               // Theme }}}
            // Structs }}}
            // Global Constants {{{
               // Color Codes {{{
               namespace CC {
                  U8 constexpr blue_light   =  81;
                  U8 constexpr blue         =  33;
                  U8 constexpr blue_dark    =  26;
                  U8 constexpr red_light    = 196;
                  U8 constexpr red          = 124;
                  U8 constexpr red_dark     =  88;
                  U8 constexpr green_light  =  83;
                  U8 constexpr green        =  35;
                  U8 constexpr green_dark   =  29;
                  U8 constexpr yellow_light = 229;
                  U8 constexpr yellow       = 227;
                  U8 constexpr yellow_dark  = 178;
                  U8 constexpr purple_light = 177;
                  U8 constexpr purple       = 127;
                  U8 constexpr purple_dark  =  53;
                  U8 constexpr orange_light = 222;
                  U8 constexpr orange       = 215;
                  U8 constexpr orange_dark  = 202;
                  U8 constexpr pink_light   = 224;
                  U8 constexpr pink         = 212;
                  U8 constexpr pink_dark    = 200;
                  U8 constexpr cyan_light   = 159;
                  U8 constexpr cyan         =  87;
                  U8 constexpr cyan_dark    =  44;
                  U8 constexpr grey_light   = 250;
                  U8 constexpr grey         = 245;
                  U8 constexpr grey_dark    = 240;
                  U8 constexpr brown_light  = 137;
                  U8 constexpr brown        = 130;
                  U8 constexpr brown_dark   =  94;
                  U8 constexpr white        = 255;
                  U8 constexpr black        =   0;
               }
               // Colors }}}
               // Default Theme {{{
               static Theme const theme {
                  /* .critical      */ { FG{ CC::white       },  BG{ CC::red   },  Mod::bold | Mod::underline },
                  /* .error         */ { FG{ CC::red_light   },  BG{           },  Mod::bold                  },
                  /* .warning       */ { FG{ CC::yellow      },  BG{           },  Mod::bold                  },
                  /* .plain         */ { FG{ CC::grey_dark   },  BG{           },  Mod::none                  },
                  /* .header1       */ { FG{ CC::white       },  BG{           },  Mod::bold | Mod::underline },
                  /* .header2       */ { FG{ CC::grey_light  },  BG{           },  Mod::bold                  },
                  /* .header3       */ { FG{ CC::grey        },  BG{           },  Mod::bold                  },
                  /* .debug         */ { FG{ CC::blue_light  },  BG{           },  Mod::none                  },
                  /* .selected      */ { FG{ CC::green_light },  BG{ CC::black },  Mod::bold                  },
                  /* .list_positive */ { FG{ CC::green_light },  BG{           },  Mod::bold                  },
                  /* .list_neutral  */ { FG{ CC::grey        },  BG{           },  Mod::bold                  },
                  /* .list_negative */ { FG{ CC::red_dark    },  BG{           },  Mod::bold                  },
               };
               // Default Theme }}}
            // Global Constants }}}
            // Functions {{{
               // Style Decorator Functions {{{
                  inline Str ansi( Str const &text, FG fg, BG bg, Mod mod ) noexcept { //{{{
                     return fmt::format("\033[{};{};{}m{}\033[0m", mod.to_string(),
                                                                    fg.to_string(),
                                                                    bg.to_string(),
                                                                    text );
                  } //}}}
                  inline Str ansi( Str const &text, FG fg ) noexcept { //{{{
                     return ansi( text, fg, BG{}, Mod{} ); 
                  } //}}}
                  inline Str ansi( Str const &text, FG fg, BG bg ) noexcept { //{{{
                     return ansi( text, fg, bg, Mod{} ); 
                  } //}}}
                  inline Str ansi( Str const &text, FG fg, Mod mod ) noexcept { //{{{
                     return ansi( text, fg, BG{}, mod ); 
                  } //}}}
                  inline Str ansi( Str const &text, BG bg ) noexcept { //{{{
                     return ansi( text, FG{}, bg, Mod{} ); 
                  } //}}}
                  inline Str ansi( Str const &text, BG bg, FG fg ) noexcept { //{{{
                     return ansi( text, fg, bg, Mod{} ); 
                  } //}}}
                  inline Str ansi( Str const &text, BG bg, Mod mod ) noexcept { //{{{
                     return ansi( text, FG{}, bg, mod ); 
                  } //}}}
                  inline Str ansi( Str const &text, Mod mod ) noexcept { //{{{
                     return ansi( text, FG{}, BG{}, mod ); 
                  } //}}}
                  inline Str ansi( Str const &text, Mod mod, FG fg ) noexcept { //{{{
                     return ansi( text, fg, BG{}, mod ); 
                  } //}}}
                  inline Str ansi( Str const &text, Mod mod, BG bg ) noexcept { //{{{
                     return ansi( text, FG{}, bg, mod ); 
                  } //}}}
               // ANSI Decorator Functions }}}
               template <typename ...Params>
               inline void fatal( Params&&... params ) {
                  fmt::print( std::forward<Params>(params)... );
               }
               template <typename ...Params>
               inline void print( Params&&... params ) {
                  fmt::print( std::forward<Params>(params)... );
               }
            // Functions }}}
            }
         // Log }}}
      }
   // Ur }}}
   // Vk {{{
      // WHAT: Namespace for Vulkan-specific bindings.
      namespace vk {
         bool init();
      }
   // Vk }}}
// Namespaces }}}
// Implementation {{{
void ur::init() {
   ur::log::print( "Initializing UR..." );

   //if ( not vk::init() )
   //   log::fatal( "Vulkan initialization failed!" );
   
   //sys::init_io();
   // init   instance
   // create surface
   // enum   phys devs
   // create logical dev
   // create queues
   // create semaphores
   // create command pool
   // create command buffer
   // create allocator
   // create staging manager
   // create swap chain
   // create render targets
   // create render pass
   // create pipeline cache
   // create frame bufs
   // init   render program manager
   // init   vertex cache
   
   // create scene
   // place geometry
   // bake or present real-time
   // TODO
}

I32 main( I32 argc, CStr argv[] ) {
   Vec<Str> words = { "This", "Is", "A", "Wee", "Test" };
   for ( auto [i,e]: IPair(words) ) 
      fmt::print( "The word at index {} is: '{}'\n", i, e );
   return EXIT_SUCCESS;
}

// Implementations }}}
// EOF
