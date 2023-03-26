#include <boost/test/unit_test.hpp>
#include <fc/safe.hpp>

using namespace fc;

BOOST_AUTO_TEST_SUITE(fc)

BOOST_AUTO_TEST_CASE(safe_test)
{

    BOOST_CHECK_EQUAL( safe<int8_t>( -128 ).value, -128 );
    BOOST_CHECK_EQUAL( safe<int8_t>( 0 ).value, 0 );
    BOOST_CHECK_EQUAL( safe<int8_t>( 127 ).value, 127 );

    BOOST_CHECK_EQUAL( safe<uint8_t>( 0 ).value, 0U );
    BOOST_CHECK_EQUAL( safe<uint8_t>( 255 ).value, 255U );

    BOOST_CHECK_THROW( safe<int8_t>( 128 ), fc::overflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( int16_t(128) ), fc::overflow_exception );

    BOOST_CHECK_THROW( safe<int8_t>( -129 ), fc::underflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( int16_t(-129) ), fc::underflow_exception );

    BOOST_CHECK_THROW( safe<int8_t>( uint16_t(128) ), fc::overflow_exception );

    BOOST_CHECK_THROW( safe<uint16_t>( -1 ), fc::underflow_exception );
    BOOST_CHECK_THROW( safe<uint16_t>( int16_t(-1) ), fc::underflow_exception );

    BOOST_CHECK_THROW( safe<uint8_t>( int16_t(256) ), fc::overflow_exception );
    BOOST_CHECK_THROW( safe<uint8_t>( uint16_t(256) ), fc::overflow_exception );

    BOOST_CHECK_THROW( safe<uint16_t>( uint32_t(-1) ), fc::overflow_exception );

    BOOST_CHECK_EQUAL( safe<int8_t>::min().value, -128 );
    BOOST_CHECK_EQUAL( safe<int8_t>::max().value, 127 );

    BOOST_CHECK_EQUAL( safe<uint8_t>::min().value, 0U );
    BOOST_CHECK_EQUAL( safe<uint8_t>::max().value, 255U );

    BOOST_CHECK( safe<int8_t>( 100 ) + safe<int8_t>( 27 ) == 127 );
    BOOST_CHECK( safe<int8_t>( -100 ) + safe<int8_t>( -28 ) == -128 );

    BOOST_CHECK_THROW( safe<int8_t>( 100 ) + safe<int8_t>( 28 ), fc::overflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( 100 ) + 28, fc::overflow_exception );
    BOOST_CHECK_THROW( 100 + safe<int8_t>( 28 ), fc::overflow_exception );

    BOOST_CHECK_THROW( safe<int8_t>( -100 ) + safe<int8_t>( -29 ), fc::underflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( -100 ) + (-29), fc::underflow_exception );
    BOOST_CHECK_THROW( (-100) + safe<int8_t>( -29 ), fc::underflow_exception );

    BOOST_CHECK( safe<int8_t>( 100 ) - safe<int8_t>( -27 ) == 127 );
    BOOST_CHECK( safe<int8_t>( -100 ) - safe<int8_t>( 28 ) == -128 );

    BOOST_CHECK_THROW( safe<int8_t>( 100 ) - safe<int8_t>( -28 ), fc::overflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( 100 ) - (-28), fc::overflow_exception );
    BOOST_CHECK_THROW( 100 - safe<int8_t>( -28 ), fc::overflow_exception );

    BOOST_CHECK_THROW( safe<int8_t>( -100 ) - safe<int8_t>( 29 ), fc::underflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( -100 ) - 29, fc::underflow_exception );
    BOOST_CHECK_THROW( (-100) - safe<int8_t>( 29 ), fc::underflow_exception );

    BOOST_CHECK( safe<int8_t>( 10 ) * safe<int8_t>( 11 ) == 110 );
    BOOST_CHECK( safe<int8_t>( -10 ) * safe<int8_t>( -11 ) == 110 );
    BOOST_CHECK( safe<int8_t>( -2 ) * safe<int8_t>( 64 ) == -128 );
    BOOST_CHECK( safe<int8_t>( 2 ) * safe<int8_t>( -64 ) == -128 );

    BOOST_CHECK_THROW( safe<int8_t>( 100 ) * 2, fc::overflow_exception );
    BOOST_CHECK_THROW( 2 * safe<int8_t>( 100 ), fc::overflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( 2 ) * safe<int8_t>( 100 ), fc::overflow_exception );

    BOOST_CHECK_THROW( safe<int8_t>( 100 ) * (-2), fc::underflow_exception );
    BOOST_CHECK_THROW( 2 * safe<int8_t>( -100 ), fc::underflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( 2 ) * safe<int8_t>( -100 ), fc::underflow_exception );

    BOOST_CHECK_THROW( safe<int8_t>( -100 ) * (-2), fc::overflow_exception );
    BOOST_CHECK_THROW( (-2) * safe<int8_t>( -100 ), fc::overflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( -2 ) * safe<int8_t>( -100 ), fc::overflow_exception );

    BOOST_CHECK_THROW( safe<int8_t>( -128 ) * (-1), fc::overflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( -1 ) * (-128), fc::overflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( -1 ) * safe<int8_t>( -128 ), fc::overflow_exception );

    BOOST_CHECK_THROW( safe<int8_t>( -100 ) * 2, fc::underflow_exception );
    BOOST_CHECK_THROW( (-2) * safe<int8_t>( 100 ), fc::underflow_exception );
    BOOST_CHECK_THROW( safe<int8_t>( -2 ) * safe<int8_t>( 100 ), fc::underflow_exception );

    BOOST_CHECK( safe<int8_t>( 100 ) / safe<int8_t>( 11 ) == 100 / 11 );
    BOOST_CHECK( safe<int8_t>( -100 ) / safe<int8_t>( -11 ) == (-100) / (-11) );
    BOOST_CHECK( safe<int8_t>( 100 ) / safe<int8_t>( -11 ) == 100 / (-11) );
    BOOST_CHECK( safe<int8_t>( -100 ) / safe<int8_t>( 11 ) == (-100) / 11 );

    BOOST_CHECK_THROW( safe<int8_t>( -128 ) / safe<int8_t>( 0 ), fc::divide_by_zero_exception );
    BOOST_CHECK_THROW( safe<int8_t>( -128 ) / safe<int8_t>( -1 ), fc::overflow_exception );

    BOOST_CHECK( safe<uint8_t>( 100 ) / safe<uint8_t>( 11 ) == 100U / 11U );

    BOOST_CHECK_THROW( safe<uint8_t>( 128 ) / safe<uint8_t>( 0 ), fc::divide_by_zero_exception );

    BOOST_CHECK( safe<int8_t>( 100 ) % safe<int8_t>( 11 ) == 100 % 11 );
    BOOST_CHECK( safe<int8_t>( -100 ) % safe<int8_t>( -11 ) == (-100) % (-11) );
    BOOST_CHECK( safe<int8_t>( 100 ) % safe<int8_t>( -11 ) == 100 % (-11) );
    BOOST_CHECK( safe<int8_t>( -100 ) % safe<int8_t>( 11 ) == (-100) % 11 );

    BOOST_CHECK_THROW( safe<int8_t>( -128 ) % safe<int8_t>( 0 ), fc::divide_by_zero_exception );
    BOOST_CHECK_THROW( safe<int8_t>( -128 ) % safe<int8_t>( -1 ), fc::overflow_exception );

    BOOST_CHECK( safe<uint8_t>( 100 ) % safe<uint8_t>( 11 ) == 100U % 11U );

    BOOST_CHECK_THROW( safe<uint8_t>( 128 ) % safe<uint8_t>( 0 ), fc::divide_by_zero_exception );

    BOOST_CHECK( -100 == -safe<int8_t>( 100 ) );
    BOOST_CHECK( safe<int8_t>( 0 ) == -safe<int8_t>( 0 ) );
    BOOST_CHECK( 100 == -safe<int8_t>( -100 ) );
    BOOST_CHECK_THROW( -safe<int8_t>( -128 ), fc::overflow_exception );

    BOOST_CHECK( safe<uint8_t>( 0 ) == -safe<uint8_t>( 0 ) );
    BOOST_CHECK_THROW( -safe<uint8_t>( 1 ), fc::underflow_exception );

    safe<int8_t> si = 0;

    BOOST_CHECK( (si+=1) == 1 );
    BOOST_CHECK_EQUAL( si.value, 1 );
    BOOST_CHECK( (++si) == 2 );
    BOOST_CHECK_EQUAL( si.value, 2 );
    BOOST_CHECK( (si++) == 2 );
    BOOST_CHECK_EQUAL( si.value, 3 );

    BOOST_CHECK( (si*=5) == 15 );
    BOOST_CHECK_EQUAL( si.value, 15 );

    BOOST_CHECK( (si/=2) == 7 );
    BOOST_CHECK_EQUAL( si.value, 7 );

    BOOST_CHECK( (si%=4) == 3 );
    BOOST_CHECK_EQUAL( si.value, 3 );

    BOOST_CHECK( (si-=1) == 2 );
    BOOST_CHECK_EQUAL( si.value, 2 );
    BOOST_CHECK( (--si) == 1 );
    BOOST_CHECK_EQUAL( si.value, 1 );
    BOOST_CHECK( (si--) == 1 );
    BOOST_CHECK_EQUAL( si.value, 0 );

    si = 127;

    BOOST_CHECK_THROW( si+=1, fc::overflow_exception );
    BOOST_CHECK_THROW( si++, fc::overflow_exception );
    BOOST_CHECK_THROW( ++si, fc::overflow_exception );
    BOOST_CHECK_THROW( si*=2, fc::overflow_exception );
    BOOST_CHECK_THROW( si*=(-2), fc::underflow_exception );

    si = -128;

    BOOST_CHECK_THROW( si-=1, fc::underflow_exception );
    BOOST_CHECK_THROW( si--, fc::underflow_exception );
    BOOST_CHECK_THROW( --si, fc::underflow_exception );
    BOOST_CHECK_THROW( si*=(-1), fc::overflow_exception );
    BOOST_CHECK_THROW( si/=(-1), fc::overflow_exception );
    BOOST_CHECK_THROW( si%=(-1), fc::overflow_exception );
    BOOST_CHECK_THROW( si/=(0), fc::divide_by_zero_exception );
    BOOST_CHECK_THROW( si%=(0), fc::divide_by_zero_exception );

    BOOST_CHECK( safe<int8_t>( 1 ) != safe<int8_t>( 2 ) );
    BOOST_CHECK( 1 != safe<int8_t>( 2 ) );
    BOOST_CHECK( safe<int8_t>( 1 ) != 2 );

    BOOST_CHECK( safe<int8_t>( 1 ) <= safe<int8_t>( 2 ) );
    BOOST_CHECK( 1 <= safe<int8_t>( 2 ) );
    BOOST_CHECK( safe<int8_t>( 1 ) <= 2 );

    BOOST_CHECK( safe<int8_t>( 1 ) < safe<int8_t>( 2 ) );
    BOOST_CHECK( 1 < safe<int8_t>( 2 ) );
    BOOST_CHECK( safe<int8_t>( 1 ) < 2 );

    BOOST_CHECK( safe<int8_t>( 3 ) >= safe<int8_t>( 2 ) );
    BOOST_CHECK( 3 >= safe<int8_t>( 2 ) );
    BOOST_CHECK( safe<int8_t>( 3 ) >= 2 );

    BOOST_CHECK( safe<int8_t>( 3 ) > safe<int8_t>( 2 ) );
    BOOST_CHECK( 3 > safe<int8_t>( 2 ) );
    BOOST_CHECK( safe<int8_t>( 3 ) > 2 );

}

BOOST_AUTO_TEST_SUITE_END()
