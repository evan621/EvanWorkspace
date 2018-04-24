#include <fstream>  
#include <string>  
#include <boost/program_options/variables_map.hpp>  
#include <boost/program_options.hpp>  
#include <boost/program_options/parsers.hpp>  
#include <boost/program_options/options_description.hpp>  
#include <iostream>
  
namespace po = boost::program_options;  
  
using std::string;  
using std::cout;  
using std::endl;  
using std::ifstream;  
  
int main(int argc, char** argv) {  
    po::options_description desc("Allowed options");  
    string clientType;  
    desc.add_options()  
        ("serverRealIP", po::value<string>(), "real ip of the server")  
        ("clientType", po::value<string>(&clientType), "the type of client");  
      
    po::variables_map vm;  
    ifstream confFile("client.conf");  
    po::store(po::parse_command_line(argc, argv, desc), vm);  
    po::store(po::parse_config_file(confFile, desc, true), vm);  
    po::notify(vm);  
      
    if(vm.count("serverRealIP"))  
        cout<<"serverRealIP: "<<vm["serverRealIP"].as<string>()<<endl;  
    cout<<"clientType: "<<clientType<<endl;  
    return 0;  
}  