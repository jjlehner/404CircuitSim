#include <iostream>
#include <fstream>
#include <circuit.hpp>

int main(){

	std::ifstream netlist;
	netlist.open("../test/SpiceNetlists/example.cir");
	if(netlist.fail()){
		std::cout<<"File Not Found!"<<std::endl;
		return 1;
	}
	Circuit::Schematic* schem = Circuit::Parser::parse( netlist );
	Circuit::ParamTable *param = nullptr;
	schem->out(param);
	netlist.close();
	delete schem;
}
