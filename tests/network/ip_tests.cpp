#include <boost/test/unit_test.hpp>

#include <fc/network/ip.hpp>
#include <fc/variant.hpp>

BOOST_AUTO_TEST_SUITE(fc_network)

BOOST_AUTO_TEST_CASE(ip_address_test)
{
   std::vector<std::string> local_ips { "127.0.0.1", "127.20.30.40" };
   std::vector<std::string> multicast_ips { "224.88.77.66", "239.11.23.45" };
   std::vector<std::string> private_ips { "10.2.3.4", "169.254.7.9", "192.168.5.6", "172.30.8.10" };
   std::vector<std::string> public_ips { "2.3.4.55", "200.100.50.25" };
   std::vector<std::string> bad_ips { "some_string", "2.300.4.55", "200:100.50.25.1" };

   for( auto& ip_str : local_ips )
   {
      fc::ip::address ip(ip_str);
      BOOST_CHECK( ip.is_loopback_address() );
      BOOST_CHECK( !ip.is_multicast_address() );
      BOOST_CHECK( !ip.is_private_address() );
      BOOST_CHECK( !ip.is_public_address() );
   }
   for( auto& ip_str : multicast_ips )
   {
      fc::ip::address ip;
      ip = ip_str;
      BOOST_CHECK( !ip.is_loopback_address() );
      BOOST_CHECK( ip.is_multicast_address() );
      BOOST_CHECK( !ip.is_private_address() );
      BOOST_CHECK( !ip.is_public_address() );
   }
   for( auto& ip_str : private_ips )
   {
      fc::ip::address ip(ip_str);
      BOOST_CHECK( !ip.is_loopback_address() );
      BOOST_CHECK( !ip.is_multicast_address() );
      BOOST_CHECK( ip.is_private_address() );
      BOOST_CHECK( !ip.is_public_address() );
   }
   for( auto& ip_str : public_ips )
   {
      fc::ip::address ip;
      ip = ip_str;
      BOOST_CHECK( !ip.is_loopback_address() );
      BOOST_CHECK( !ip.is_multicast_address() );
      BOOST_CHECK( !ip.is_private_address() );
      BOOST_CHECK( ip.is_public_address() );
   }
   for( auto& ip_str : bad_ips )
   {
      BOOST_CHECK_THROW( fc::ip::address( ip_str ).is_public_address(), fc::exception );
      fc::ip::address ip;
      BOOST_CHECK_THROW( ip = ip_str, fc::exception );
   }

   fc::ip::address ip1(100U);
   fc::ip::address ip2;
   ip2 = 100U;
   fc::ip::address ip3("0.0.0.100");
   fc::ip::address ip4;
   ip4 = std::string("0.0.0.100");
   std::string ip5_str("100.0.0.0");
   fc::ip::address ip5("100.0.0.0");

   BOOST_CHECK( ip1 == ip2 );
   BOOST_CHECK( ip1 == ip3 );
   BOOST_CHECK( ip1 == ip4 );
   BOOST_CHECK( ip1 != ip5 );
   BOOST_CHECK( uint32_t(ip1) == 100U );
   BOOST_CHECK( std::string(ip1) == std::string("0.0.0.100") );
}

BOOST_AUTO_TEST_CASE(ip_endpoint_test)
{
   fc::ip::address ip100("0.0.0.100");
   fc::ip::endpoint ep1(ip100, 100);
   fc::ip::endpoint ep2(100, 100);
   fc::ip::endpoint ep3(100, 101);
   fc::ip::endpoint ep4(101, 100);
   BOOST_CHECK( ep1 == ep2 );
   BOOST_CHECK( ep1 != ep3 );
   BOOST_CHECK( ep1 < ep3 );
   BOOST_CHECK( ep1 < ep4 );
   BOOST_CHECK( ep3 < ep4 );

   std::string ep5_str("1.2.3.4:567");
   fc::ip::endpoint ep5 = fc::ip::endpoint::from_string( ep5_str );
   BOOST_CHECK( std::string(ep5) == ep5_str );

   fc::variant v;
   fc::to_variant( ep5, v, 2 );

   fc::ip::endpoint ep6;
   fc::from_variant( v, ep6, 2 );

   BOOST_CHECK( ep5 == ep6 );
}

BOOST_AUTO_TEST_SUITE_END()
