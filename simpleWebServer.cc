#include <memory>
#include <thread>
#include <cstdlib>
#include <restbed>
#include <sstream>

using namespace std;
using namespace restbed;

void post_method_handler( const shared_ptr< Session > session )
{
    const auto request = session->get_request( );

    size_t content_length = request->get_header( "Content-Length", 0 );

    session->fetch( content_length, [ request ]( const shared_ptr< Session > session, const Bytes & body )
    {
        fprintf( stdout, "%.*s\n", ( int ) body.size( ), body.data( ) );
        session->close( OK, "Hello, World!", { { "Content-Length", "13" }, { "Connection", "close" } } );
    } );
}

void get_method_handler( const shared_ptr< Session > session )
{
    stringstream id;
    id << ::this_thread::get_id( );
    auto body = String::format( "Hello From Thread %s\n", id.str( ).data( ) );
    
    session->close( OK, body, { { "Content-Length", ::to_string( body.length( ) ) } } );
}

int main( const int, const char** )
{
    auto resource = make_shared< Resource >( );
    resource->set_path( "/resource" );
    resource->set_method_handler( "GET", get_method_handler );
    auto resource1 = make_shared< Resource >( );
    resource1->set_path( "/command" );
    resource1->set_method_handler( "POST", post_method_handler );

    
    auto settings = make_shared< Settings >( );
    settings->set_port( 1984 );
    settings->set_worker_limit( 4 );
    settings->set_default_header( "Connection", "close" );
    
    Service service;
    service.publish( resource );
    service.publish( resource1 );
    service.start( settings );
    
    return EXIT_SUCCESS;
}
