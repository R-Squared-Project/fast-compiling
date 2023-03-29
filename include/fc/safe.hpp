#pragma once
#include <fc/exception/exception.hpp>
#include <fc/reflect/reflect.hpp>

#include <limits>

namespace fc {

   template< typename T, std::enable_if_t< std::is_integral<T>::value, bool > = true >
   struct safe_base
   {
      // Nothing here
   };

   /**
    *  This type is designed to provide automatic checks for
    *  integer overflow and default initialization. It will
    *  throw an exception on overflow conditions.
    *
    *  It can only be used on built-in types.
    *
    *  Implemented using spec from:
    *  https://www.securecoding.cert.org/confluence/display/c/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow
    */
   template<typename T>
   struct safe : safe_base<T>
   {
      T value = 0;

      safe() = default;

      // Both T and V are signed
      template< typename V, typename... Dummy, typename X = T,
                std::enable_if_t< std::is_signed<X>::value, bool > = true,
                std::enable_if_t< std::is_integral<V>::value, bool > = true,
                std::enable_if_t< std::is_signed<V>::value, bool > = true >
      static T check( V v )
      {
         static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
         if( v > std::numeric_limits<T>::max() )
            FC_CAPTURE_AND_THROW( overflow_exception, (v) );
         if( v < std::numeric_limits<T>::min() )
            FC_CAPTURE_AND_THROW( underflow_exception, (v) );
         return static_cast<T>( v );
      }

      // Both T and V are unsigned
      template< typename V, typename... Dummy, typename X = T,
                std::enable_if_t< std::is_unsigned<X>::value, bool > = true,
                std::enable_if_t< std::is_integral<V>::value, bool > = true,
                std::enable_if_t< std::is_unsigned<V>::value, bool > = true >
      static T check( V v )
      {
         static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
         if( v > std::numeric_limits<T>::max() )
            FC_CAPTURE_AND_THROW( overflow_exception, (v) );
         return static_cast<T>( v );
      }

      // T is unsigned and V is signed
      template< typename V, typename... Dummy, typename X = T,
                std::enable_if_t< std::is_unsigned<X>::value, bool > = true,
                std::enable_if_t< std::is_integral<V>::value, bool > = true,
                std::enable_if_t< std::is_signed<V>::value, bool > = true >
      static T check( V v )
      {
         static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
         if( v < 0 )
            FC_CAPTURE_AND_THROW( underflow_exception, (v) );
         if( static_cast< typename std::make_unsigned_t<V> >(v) > std::numeric_limits<T>::max() )
            FC_CAPTURE_AND_THROW( overflow_exception, (v) );
         return static_cast<T>( v );
      }

      // T is signed and V is unsigned
      template< typename V, typename... Dummy, typename X = T,
                std::enable_if_t< std::is_signed<X>::value, bool > = true,
                std::enable_if_t< std::is_integral<V>::value, bool > = true,
                std::enable_if_t< std::is_unsigned<V>::value, bool > = true >
      static T check( V v )
      {
         static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
         if( v > static_cast< typename std::make_unsigned_t<T> >( std::numeric_limits<T>::max() ) )
            FC_CAPTURE_AND_THROW( overflow_exception, (v) );
         return static_cast<T>( v );
      }

      template< typename V >
      safe( V v ) : value( check(v) )
      {
         // Nothing else to do
      }

      template< typename V >
      safe& operator = ( V v )
      {
         value = check(v);
         return *this;
      }

      static constexpr safe min()
      {
          return std::numeric_limits<T>::min();
      }
      static constexpr safe max()
      {
          return std::numeric_limits<T>::max();
      }

      friend safe operator + ( const safe& a, const safe& b )
      {
          if( b.value > 0 && a.value > (std::numeric_limits<T>::max() - b.value) )
             FC_CAPTURE_AND_THROW( overflow_exception, (a)(b) );
          if( b.value < 0 && a.value < (std::numeric_limits<T>::min() - b.value) )
             FC_CAPTURE_AND_THROW( underflow_exception, (a)(b) );
          return safe( a.value + b.value );
      }
      friend safe operator - ( const safe& a, const safe& b )
      {
          if( b.value > 0 && a.value < (std::numeric_limits<T>::min() + b.value) )
             FC_CAPTURE_AND_THROW( underflow_exception, (a)(b) );
          if( b.value < 0 && a.value > (std::numeric_limits<T>::max() + b.value) )
             FC_CAPTURE_AND_THROW( overflow_exception, (a)(b) );
          return safe( a.value - b.value );
      }

      friend safe operator * ( const safe& a, const safe& b )
      {
          if( a.value > 0 )
          {
              if( b.value > 0 )
              {
                  if( a.value > (std::numeric_limits<T>::max() / b.value) )
                     FC_CAPTURE_AND_THROW( overflow_exception, (a)(b) );
              }
              else
              {
                  if( b.value < (std::numeric_limits<T>::min() / a.value) )
                     FC_CAPTURE_AND_THROW( underflow_exception, (a)(b) );
              }
          }
          else
          {
              if( b.value > 0 )
              {
                  if( a.value < (std::numeric_limits<T>::min() / b.value) )
                     FC_CAPTURE_AND_THROW( underflow_exception, (a)(b) );
              }
              else
              {
                  if( a.value != 0 && b.value < (std::numeric_limits<T>::max() / a.value) )
                     FC_CAPTURE_AND_THROW( overflow_exception, (a)(b) );
              }
          }

          return safe( a.value * b.value );
      }

      // T is signed
      template< typename... Dummy, typename X = T,
                std::enable_if_t< std::is_signed<X>::value, bool > = true >
      friend safe operator / ( const safe& a, const safe& b )
      {
          static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
          if( b.value == 0 ) FC_CAPTURE_AND_THROW( divide_by_zero_exception, (a)(b) );
          if( a.value == std::numeric_limits<T>::min() && b.value == -1 )
             FC_CAPTURE_AND_THROW( overflow_exception, (a)(b) );
          return safe( a.value / b.value );
      }
      // T is unsigned
      template< typename... Dummy, typename X = T,
                std::enable_if_t< std::is_unsigned<X>::value, bool > = true >
      friend safe operator / ( const safe& a, const safe& b )
      {
          static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
          if( b.value == 0 ) FC_CAPTURE_AND_THROW( divide_by_zero_exception, (a)(b) );
          return safe( a.value / b.value );
      }

      // T is signed
      template< typename... Dummy, typename X = T,
                std::enable_if_t< std::is_signed<X>::value, bool > = true >
      friend safe operator % ( const safe& a, const safe& b )
      {
          static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
          if( b.value == 0 ) FC_CAPTURE_AND_THROW( divide_by_zero_exception, (a)(b) );
          if( a.value == std::numeric_limits<T>::min() && b.value == -1 )
             FC_CAPTURE_AND_THROW( overflow_exception, (a)(b) );
          return safe( a.value % b.value );
      }
      // T is unsigned
      template< typename... Dummy, typename X = T,
                std::enable_if_t< std::is_unsigned<X>::value, bool > = true >
      friend safe operator % ( const safe& a, const safe& b )
      {
          static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
          if( b.value == 0 ) FC_CAPTURE_AND_THROW( divide_by_zero_exception, (a)(b) );
          return safe( a.value % b.value );
      }

      // T is signed
      template< typename... Dummy, typename X = T,
                std::enable_if_t< std::is_signed<X>::value, bool > = true >
      safe operator - ()const
      {
          static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
          if( value == min() ) FC_CAPTURE_AND_THROW( overflow_exception, (*this) );
          return safe( -value );
      }
      // T is unsigned
      template< typename... Dummy, typename X = T,
                std::enable_if_t< std::is_unsigned<X>::value, bool > = true >
      safe operator - ()const
      {
          static_assert( 0 == sizeof...(Dummy), "Unexpected template argument(s)" );
          if( value != 0 ) FC_CAPTURE_AND_THROW( underflow_exception, (*this) );
          return safe( 0 );
      }

      safe& operator += ( const safe& b )
      {
          value = (*this + b).value;
          return *this;
      }
      safe& operator -= ( const safe& b )
      {
          value = (*this - b).value;
          return *this;
      }
      safe& operator *= ( const safe& b )
      {
          value = (*this * b).value;
          return *this;
      }
      safe& operator /= ( const safe& b )
      {
          value = (*this / b).value;
          return *this;
      }
      safe& operator %= ( const safe& b )
      {
          value = (*this % b).value;
          return *this;
      }

      safe& operator++()
      {
          *this += 1;
          return *this;
      }
      safe operator++( int )
      {
          safe bak = *this;
          *this += 1;
          return bak;
      }

      safe& operator--()
      {
          *this -= 1;
          return *this;
      }
      safe operator--( int )
      {
          safe bak = *this;
          *this -= 1;
          return bak;
      }

      friend bool operator == ( const safe& a, const safe& b )
      {
          return a.value == b.value;
      }
      friend bool operator == ( const safe& a, T b )
      {
          return a.value == b;
      }
      friend bool operator == ( T a, const safe& b )
      {
          return a == b.value;
      }

      friend bool operator < ( const safe& a, const safe& b )
      {
          return a.value < b.value;
      }
      friend bool operator < ( const safe& a, T b )
      {
          return a.value < b;
      }
      friend bool operator < ( T a, const safe& b )
      {
          return a < b.value;
      }

      friend bool operator > ( const safe& a, const safe& b )
      {
          return a.value > b.value;
      }
      friend bool operator > ( const safe& a, T b )
      {
          return a.value > b;
      }
      friend bool operator > ( T a, const safe& b )
      {
          return a > b.value;
      }

      friend bool operator != ( const safe& a, const safe& b )
      {
          return !(a == b);
      }
      friend bool operator != ( const safe& a, T b )
      {
          return !(a == b);
      }
      friend bool operator != ( T a, const safe& b )
      {
          return !(a == b);
      }

      friend bool operator <= ( const safe& a, const safe& b )
      {
          return !(a > b);
      }
      friend bool operator <= ( const safe& a, T b )
      {
          return !(a > b);
      }
      friend bool operator <= ( T a, const safe& b )
      {
          return !(a > b);
      }

      friend bool operator >= ( const safe& a, const safe& b )
      {
          return !(a < b);
      }
      friend bool operator >= ( const safe& a, T b )
      {
          return !(a < b);
      }
      friend bool operator >= ( T a, const safe& b )
      {
          return !(a < b);
      }
   };

}

FC_REFLECT_TEMPLATE( (typename T), safe<T>, (value) )
