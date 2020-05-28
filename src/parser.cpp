#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include "circuits.hpp"

Circuit::Schematic Circuit::parse(){
	for(const auto& line: v) {
		for (const auto& char: line){
			if(char == "I" || char == "R" || char == "L" || char == "C"){
				Circuit::Component::Component
			}
		}
	}
}
//Parses each line into a vector
std::vector<std::string> getLines(std::string filename){
	ifstream netlist;
	std::string line;
	std::vector<std::string> v;
	netlist.open(filename);
	if (!netlist.is_open()){
		std::cerr << "No such file exists" << '\n';
	}
	else {
		while(std::getline(cin, line)){
			v.push_back(line);
		}
	}
	return v;
}
//Takes each line and processes it into substrings
std::vector<std::string> generateComponent(std::string line) {
	auto iss = std::istringstream{line};
	auto str = std::string{};
	std::vector<std::string> v;
	while (iss >> str) {
		v.push_back(str);
	}
	return v;
}