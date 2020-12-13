#include <verilated.h> // Defines common routines
#include <iostream>    // Need std::cout
#include "VCore.h"     // From Verilating "top.v"
#include <fstream>
#include <sstream>     // For the reading of instructions from ismp
#include <string>      // Ibid. 
VCore *top; // Instantiation of module
vluint64_t main_time = 0; // Current simulation time
// This is a 64-bit integer to reduce wrap over issues and
// allow modulus. This is in units of the timeprecision
// used in Verilog (or from --timescale-override)
double sc_time_stamp () { // Called by $time in Verilog
    return main_time; // converts to double, to match
    // what SystemC does
}


/*

TODO: Figure out the start and the end address for flexpret
Trouble if the data and the instruction memory starts at zero

Do we need to translate data and instruction mem addr?

Try writing own instructions directly 

*/
    
unsigned int * instr(const char* input, unsigned int* ispm) { // Reading the file directly
        ifstream file(input);
        string str;
        unsigned int instr[4096];
        unsigned int lines = 0;
        while (getline(file, str)) {
                instr[lines] = stoul(str, NULL, 16);
                lines++;
        }
	ispm = instr;
        return ispm; 
}
/*
io_imem_data_out,31,0);
  58     VL_IN(io_imem_data_in,31,0);
  59     VL_OUT(io_dmem_data_out,31,0);
  60     VL_IN(io_dmem_data_in,31,0);
  61     VL_IN(io_bus_data_out,31,0);
  62     VL_OUT(io_bus_data_in,31,0);
  63     VL_OUT(io_host_to_host,31,0);
  io_imem_enable,0,0
  io_imem_write
*/

int main(int argc, char** argv) {
    char* F = argv[1];
    cout << "File is: " << F << endl;
    unsigned int* ispm = new unsigned int;
    ispm = instr(F, ispm);
    unsigned int line = 0;
    Verilated::commandArgs(argc, argv); // Remember args
    top = new VCore; // Create instance
    top->reset = 0; // Set some inputs
    top->clock = 0;
    top->io_imem_enable = 1;
    top->io_dmem_enable = 1; 
    while (!Verilated::gotFinish()) {
	top->reset = (main_time > 50);
	top->clock = (main_time % 10 > 5);
	if (top->reset && main_time % 10 == 5) { // pos edge clock
		top->io_imem_data_in = ispm[line];
		top->io_bus_data_in = ispm[line];
		line++;
		cout << "Adding an Instruction" << endl;
	}	 
	if (main_time > 1000) {break;}
        top->eval(); // Evaluate model
	cout << " dmem in "   <<  top->io_dmem_data_in << "\t|"; // Read a output
        cout << " dmem out " << top->io_dmem_data_out << "\t|";
	cout << " dmem addr " << top->io_dmem_addr << "\t|";
	cout << " imem in " << top->io_imem_data_in << "\t|";
	cout << " imem out " << top->io_imem_data_out << "\t|";
	cout << " imem addr " << top->io_imem_addr << "\t|";	
	cout << " host to host " << top->io_host_to_host << "\t|";
	cout << " bus data in " << top->io_bus_data_in << "\t|";
	cout << " bus data out " << top->io_bus_data_out << "\t|";
	cout << " bus data addr " << top->io_bus_addr << "\t|";
	// cout << " GPIO in " << top->io_gpio_in_1 << "\t|";
	// cout << " GPIO out " << top->io_gpio_out_1 << "\t|";
	// cout << " clock " << top->clock << "\t|";
	// cout << " reset " << top->reset << endl;
	main_time++; // Time passes...
    }
    top->final(); // Done simulating
    cout << main_time << endl;
    delete top;
}
