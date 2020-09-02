// C++ File for main

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstring>


typedef struct complex_s {
	float real;
	float imag;
   complex_s(): real( 0.f ), imag( 0.f ) {}
   complex_s( float new_real, float new_imag ):
      real( new_real ), imag( new_imag ) {}
	void operator=(const complex_s& other) {
		real = other.real;
		imag = other.imag;
	}
} complex_t;


inline void print_complexes( const std::string& prefix, const std::vector<complex_t>& vals ) {
	std::cout << prefix;

	for ( int index = 0; index < vals.size(); ++index ) {
		std::cout << "{" << vals.at(index).real << ", " << vals.at(index).imag << "} ";
	}
	
	std::cout << "\n";
}

inline void gen_complexes( std::vector<complex_t>& vals, int num_vals ) {
	
	for ( int index =0; index < num_vals; ++index ) {
		vals.at(index) = {(float)index, (float)index };
	}
}

inline void delay_vals( std::vector<complex_t>& dvals, const std::vector<complex_t>& vals, const int delta, const bool debug=false ) {
	
	auto skip_it = dvals.begin();
	std:advance( skip_it, delta );
	
   std::fill( dvals.begin(), skip_it, complex_t{0.f,0.f} );
	std::copy( vals.begin(), vals.end(), skip_it );
	
}


inline void delay_vals_insert( std::vector<complex_t>& dvals, const std::vector<complex_t>& vals, const int delta, const bool debug=false ) {
	
	dvals.insert( dvals.begin(), delta, complex_t{0.f,0.f} );
   std::copy( vals.begin(), vals.end(), std::back_inserter(dvals) );
}

typedef unsigned char uchar;


inline void delay_vals_array( uchar* __restrict__ dvals, const uchar* __restrict__ vals, const int delta, const int num_vals, const bool debug=false) {

   std::memset( dvals, 0, delta * sizeof(complex_t) );
   std::memcpy( dvals + (delta * sizeof(complex_t)), vals, num_vals * sizeof(complex_t) );   
}


// Based on stackoverflow: https://stackoverflow.com/questions/3991478/building-a-32-bit-float-out-of-its-4-composite-bytes, 
// users phuclv and Madgeek
inline complex_t complex_bytes_to_complex_t( const uchar* complex_bytes ) {
   complex_t output;

   *((uchar*)(&output) + 7) = complex_bytes[0];
   *((uchar*)(&output) + 6) = complex_bytes[1];
   *((uchar*)(&output) + 5) = complex_bytes[2];
   *((uchar*)(&output) + 4) = complex_bytes[3];
   *((uchar*)(&output) + 3) = complex_bytes[4];
   *((uchar*)(&output) + 2) = complex_bytes[5];
   *((uchar*)(&output) + 1) = complex_bytes[6];
   *((uchar*)(&output) + 0) = complex_bytes[7];

   //std::memcpy( &output, complex_bytes, sizeof(complex_t) );
   return output;
}

inline void complex_t_to_complex_bytes( uchar* complex_bytes, const complex_t val ) {

   complex_bytes[7] = *((uchar*)(&val) + 0);
   complex_bytes[6] = *((uchar*)(&val) + 1);
   complex_bytes[5] = *((uchar*)(&val) + 2);
   complex_bytes[4] = *((uchar*)(&val) + 3);
   complex_bytes[3] = *((uchar*)(&val) + 4);
   complex_bytes[2] = *((uchar*)(&val) + 5);
   complex_bytes[1] = *((uchar*)(&val) + 6);
   complex_bytes[0] = *((uchar*)(&val) + 7);
      

}

inline void complexes_to_complex_bytes( uchar* complex_bytes, const complex_t* vals, const int num_vals, const int chars_per_val ) {
   for( int index = 0; index < num_vals; ++index ) {
      complex_t_to_complex_bytes( &(complex_bytes[index*chars_per_val]), vals[index] );
   } 
}

inline void print_char_array( const std::string& prefix, const uchar* vals, const int num_vals, const int chars_per_val ) {
   int num_chars = num_vals * chars_per_val;

   std::cout << prefix; 
   for( int index = 0; index < num_chars; index+= chars_per_val ) {
      complex_t temp_comp = complex_bytes_to_complex_t( &(vals[index]) );
      std::cout << "{" << temp_comp.real << ", " << temp_comp.imag << "} "; 
   } 
   std::cout << "\n"; 
}

int main(){

	bool debug =true;
	int delta = 4;
	int num_vals = 8;
	int num_dvals = num_vals + delta;
	
	std::vector<complex_t> vals{(unsigned long)num_vals};
   std::vector<complex_t> dvals{(unsigned long)num_dvals};
	std::vector<complex_t> dvals2;
	
	gen_complexes( vals, num_vals );
	
	delay_vals( dvals, vals, delta, debug );
	print_complexes( "Vals = ", vals );
	print_complexes( "Delayed Vals (set zero then skip to copy) = ", dvals );

	delay_vals_insert( dvals2, vals, delta, debug );
	print_complexes( "Vals = ", vals );
	print_complexes( "Delayed Vals (insert zeros) = ", dvals2 );
    
   uchar avals[num_vals * sizeof(complex_t)];
   uchar advals[num_dvals * sizeof(complex_t)];
   std::memcpy( avals, vals.data(), num_vals * sizeof(complex_t) );
   complexes_to_complex_bytes( avals, vals.data(), num_vals, sizeof(complex_t) );

   delay_vals_array( advals, avals, delta, num_vals, debug );
	print_char_array( "Vals = ", avals, num_vals, sizeof(complex_t) );
	print_char_array( "Delayed Vals (array) = ", advals, num_dvals, sizeof(complex_t) );
	return 0;
}

