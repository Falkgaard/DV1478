#pragma once

#include <cstdio>
#include <cstdint>
#include <cassert>
#include <ctime>
#include <array>
#include <limits>
#include <type_traits>

namespace RNG {

static bool constexpr isDebugging { false };

// Based off of the xoroshiro128+ v1.0 algorithm by David Blackman and Sebastiano Vigna (vigna@acm.org)
class Engine {
public:
	inline /*HOL*/ Engine( uint64_t seed = static_cast<uint64_t>(std::time(nullptr)) * 104729 * 104723 * 104717 ) noexcept:
		state ({ splitmix64(seed), splitmix64(seed+1) })
	{
		if constexpr ( RNG::isDebugging ) {
			auto i = (uint32_t*)(&seed);
			printf( "RNG::Engine::Engine( seed: 0x%08X'%08X )\n", i[0], i[1] );
		}
	}
	
	inline /*HOL*/ uint64_t operator()() noexcept {
		if constexpr ( RNG::isDebugging ) {
			auto *i = (uint32_t*)state.data();
			printf( "RNG::Engine::():  0x%08X'%08X'%08X'%08X ", i[0], i[1], i[2], i[3] );
		}
		uint64_t const s0  { state[0] };
		uint64_t       s1  { state[1] };
		uint64_t const res { s0 + s1  };
		s1 ^= s0;
		state[0] = mutate(s0, 24) ^ s1 ^ (s1 << 16); // a, b
		if constexpr ( RNG::isDebugging ) {
			auto i = (uint32_t*)state.data();
			printf( "... 0x%08X'%08X'%08X'%08X ", i[0], i[1], i[2], i[3] );
		}
		state[1] = mutate(s1, 37); // c
		if constexpr ( RNG::isDebugging ) {
			auto i = (uint32_t*)state.data();
			printf( "... 0x%08X'%08X'%08X'%08X ", i[0], i[1], i[2], i[3] );
		}
		return res;
	}

/* Equivalent to 2^64 next() calls. Can produce 2^64 non-overlapping subsequences for parallel computations. */
	inline /*HOL*/ void jump() noexcept {
		if constexpr ( RNG::isDebugging )
			puts( "RNG::Engine::jump()\n" );

		static uint64_t constexpr jump[] { 0xDF90'0294'D8F5'54A5, 0x1708'65DF'4B32'01FC };
	
		uint64_t s0 { 0 },
		         s1 { 0 };
		for ( int i = 0;  i < sizeof jump / sizeof *jump;  ++i )
			for ( int j = 0;  j < 64;  ++j ) {
				if ( jump[i] & UINT64_C(1) << j ) {
					s0 ^= state[0];
					s1 ^= state[1];
				}
				(*this)();
			}
		state[0] = s0;
		state[1] = s1;
	}

/* Equivalent to 2^96 next() calls. Can produce 2^32 starting points from each of which jump()
	will generate 2^32 non-overlapping subsequences for parallel distributed computations. */
	inline /*HOL*/ void longJump() noexcept {
		if constexpr ( RNG::isDebugging )
			puts( "RNG::Engine::longJump()\n" );
		static uint64_t constexpr long_jump[] { 0xD2A9'8B26'625E'EE7B, 0xDDDF'9B10'90AA'7AC1 };
		uint64_t s0 { 0 },
		         s1 { 0 };
		for ( int i = 0; i < sizeof long_jump / sizeof *long_jump; ++i ) {
			for ( int j = 0;  j < 64; ++j ) {
				if ( long_jump[i] & UINT64_C(1) << j ) {
					s0 ^= state[0];
					s1 ^= state[1];
				}
				(*this)();
			}
		}
		state[0] = s0;
		state[1] = s1;
	}

private:

	inline uint64_t mutate( uint64_t const x, int const k ) noexcept {
		return (x << k) | (x >> (64 - k));
	}

	// Many thanks to the algorithhm originator, D. Lemire!
	static inline uint64_t splitmix64( uint64_t idx ) {
		idx +=                       UINT64_C(0x9E37'79B9'7F4A'7C15);
		idx  = (idx ^ (idx >> 30)) * UINT64_C(0xBF58'476D'1CE4'E5B9);
		idx  = (idx ^ (idx >> 27)) * UINT64_C(0x94D0'49BB'1331'11EB);
		return  idx ^ (idx >> 31);
	}

	std::array<uint64_t,2> state; // 128-bit buffer
};

template <typename Domain = int>
class Int {
static_assert( std::is_integral_v<Domain>, "Unsupported domain!" );
public:
	inline /*HOL*/ Int( Engine &engine,
	         Domain  min = std::numeric_limits<Domain>::min(),
	         Domain  max = std::numeric_limits<Domain>::max() ):
		engine ( &engine ),
		min    (  min    ),
		max    (  max    )
	{
		assert( min <= max and "Range min can't be greater than max!" );
		updateSpan();
	}

	inline /*HOL*/ Domain operator()() noexcept {
		return min + ( (*engine)() % span );
	}

	inline /*HOL*/ void setEngine( Engine &new_engine ) noexcept {
		engine = &new_engine;
	}

	inline /*HOL*/ void setMin( Domain new_min ) noexcept {
		min = new_min;
		updateSpan();
	}

	inline /*HOL*/ void setMax( Domain new_max ) noexcept {
		max = new_max;
		updateSpan();
	}

	inline /*HOL*/ Domain getMin() const noexcept {
		return min;
	}

	inline /*HOL*/ Domain getMix() const noexcept {
		return max;
	}

private:
	inline /*HOL*/ void updateSpan() noexcept {
		span = max - min;
	}

	Engine *engine;
	Domain  min, max, span;
};

template <typename Domain = float>
class Real {
static_assert( std::is_floating_point_v<Domain>, "Unsupported domain!" );
public:
	inline /*HOL*/ Real( Engine &engine,
	      Domain  min = 0.0,
	      Domain  max = 1.0 ):
		engine ( &engine ),
		min    (  min    ),
		max    (  max    )
	{
		assert( min <= max and "Range min can't be greater than max!" );
		updateSpan();
	}

	inline /*HOL*/ Domain operator()() noexcept {
//      return ((Domain)(*engine)() / UINT64_MAX);
		return ((Domain)(*engine)() / UINT64_MAX) * span + min;
	}

	inline /*HOL*/ void setEngine( Engine &new_engine ) noexcept {
		engine = &new_engine;
	}

	inline /*HOL*/ void setMin( Domain new_min ) noexcept {
		min = new_min;
		updateSpan();
	}

	inline /*HOL*/ void setMax( Domain new_max ) noexcept {
		max = new_max;
		updateSpan();
	}

	inline /*HOL*/ Domain getMin() const noexcept {
		return min;
	}

	inline /*HOL*/ Domain getMix() const noexcept {
		return max;
	}

private:
	inline /*HOL*/ void updateSpan() noexcept {
		span = max - min;
	}

	Engine *engine;
	Domain  min, max, span;
};

class Bool {
public:
	inline /*HOL*/ Bool( Engine &engine ):
		engine ( &engine )
	{}

	inline bool operator()() noexcept {
		return (*engine)() & (1ull << 63); // sign bit
	}

	inline /*HOL*/ void setEngine( Engine &new_engine ) noexcept {
		engine = &new_engine;
	}

private:
	Engine *engine;
};

} // namespace RNG end

/*
int main() {
	RNG::Engine engine; //( 0xDEFEC8 );

	puts( "\n" "// bool:" );
	{
		RNG::Bool  rng( engine );
		for ( int i = 0;  i < 10;  ++i )
			printf( "//    %s\n", rng()? "true":"false" );
	}

	puts( "\n" "// implicit float:" );	
	{
		RNG::Real<>  rng( engine );
		for ( int i = 0;  i < 10;  ++i )
			printf( "//    %2.2ff\n", rng() );
	}

	puts( "\n" "// explicit double:" );
	{
		RNG::Real<double>  rng( engine, .6, .7 );
		for ( int i = 0;  i < 10;  ++i )
			printf( "//    %2.2f\n", rng() );
	}

	puts( "\n" "// implicit int32_t:" );
	{
	  	RNG::Int<>  rng( engine, -6, +9 );
		for ( int i = 0;  i < 10;  ++i ) {
         auto n = rng();
			printf( "//    %s%i\n", n<0?"":"+", n );
      }
	}

	puts( "\n" "// explicit uint64_t:" );
	{
		RNG::Int<uint64_t>  rng( engine, 0, 9000 );
		for ( int i = 0;  i < 10;  ++i )
			printf( "//    %lluu\n", rng() );
	}

	puts( "\n" "// statistical sampling:" );
	{
		auto constexpr sample_size = 10'000'000u;
		auto constexpr bucket_size =  1'000'000u;
		std::array<size_t,100>  histogram { 0 };
		RNG::Int<>  rng( engine, 0, histogram.size() * bucket_size );
		for ( size_t i = 0;  i < sample_size;  ++i )
			++histogram[ rng() / bucket_size ];
		auto min = histogram[0], max = min;
		for ( auto const &e : histogram )
			if ( e < min )
				min = e;
		for ( auto const &e : histogram )
			if ( e > max )
				max = e;
		auto avg = .0;
		for ( auto const &e : histogram )
			avg += e;
		avg /= histogram.size();
		auto span = max - min;
		auto span_percentile = .01 * span;
		for ( size_t i = 0;  i < histogram.size();  ++i ) {
			auto bars = (histogram[i] - min) / span_percentile;
			printf( "%8u: #", i );
			for ( size_t bar = 0;  bar < 100;  ++bar )
				printf( "%s", bar < bars? "#":"." );
			printf( ". %d\n", histogram[i] );
		}
		int lower_deviation = min - avg,
		    upper_deviation = max - avg;
		float low_dev_per = float(-lower_deviation) * 100.0f * (1.0f / avg),
		      upp_dev_per = float( upper_deviation) * 100.0f * (1.0f / avg);
		auto ideal_avg = float(sample_size) / histogram.size();
		printf( "\n Analysis:"                                         "\n"
		        "     Avg:  %.2f (ideal: %.2f)"                        "\n"
		        "     Min:  %9u (standard deviation: %i;"  "%6.3f%%)"  "\n"
		        "     Max:  %9u (standard deviation: +%i;" "%6.3f%%)"  "\n",
		        avg, ideal_avg, min, lower_deviation, low_dev_per, max, upper_deviation, upp_dev_per );
	}
}*/
