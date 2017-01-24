class Factory {
  using Directory = std::unordered_set< std::string >;
  CommandLine commandLine;
  std::unique_ptr< io::Manager > manager;
  Directory legacy;
  Directory modern;
  
  static io::Manager setupManager( CommandLine& commandLine ){
    io::Manager::Builder builder;
    if ( commandLine.inputPath ){
      builder.input( *(commandLine.inputPath) );
    }
    if ( commandLine.outputPath ){
      builder.output( *(commandLine.outputPath) );
    }
    return builder.construct();      
  }

  static Directory setupLegacyDirectory( CommandLine& commandLine ){
    return ( commandLine.legacySwitch ) ?
      Directory( { "MODER", "RECONR", "BROADR" } ) :
      Directory( { "MODER", "RECONR", "BROADR" } );
  }

  static Directory setupModernDirectory( CommandLine& commandLine ){
    return ( commandLine.legacySwitch ) ? Directory() : Directory();
  }

  template< typename First, typename Second, typename... Args >
  static void cycle( First&& first, Second&& second, Args&... args ){
    while ( first( args... ) and second( args... ) ){}    
  }
  
public:
  Factory( int argc, char* argv[] ) :
    commandLine( argc, argv ),
    manager( std::make_unique< io::Manager >
	     ( setupManager( this->commandLine ) ) ),
    legacy( setupLegacyDirectory( this->commandLine ) ),
    modern( setupModernDirectory( this->commandLine ) ){}

  Driver operator()(){
    legacy::Sequence::Factory legacyFactory( *(this->manager), this->legacy );
    legacy::Sequence::Factory modernFactory( *(this->manager), this->modern ); //placeholder    

    auto makeProcessor = []( auto& set, auto& factory ){
      return [&]( auto& label, auto& queue ){
        if ( set.count(label) ){
  	  queue.push( factory( label ) );
	  return true;
        }
        return false;
      };
    };

    auto legacyProcessor = makeProcessor( this->legacy, legacyFactory );
    auto modernProcessor = makeProcessor( this->modern, modernFactory );
    Queue queue;
    auto label = input::Label::extract( manager->input() );
    this->legacy.count( label ) ?
      cycle( legacyProcessor, modernProcessor, label, queue ) :
      cycle( modernProcessor, legacyProcessor, label, queue );
    if ( label != "STOP" ){
      Log::error( "Unrecognized routine label" );
      Log::info( "Requested routine: {}", label );
      throw std::exception();
    }
    if ( this->commandLine.verifyOnly ){ queue = Queue(); }
    return Driver( std::move(this->manager), std::move(queue) );
  }
};
    