// Main.cpp
//============================

//===========================
// Include Dependencies
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <memory>
#include "phys.h"
#include "coord.h"
#include "node.h"
#include "cell.h"
#include "tissue.h"
#include "rand.h"
//==============================

using namespace std;

//============================

int main(int argc, char* argv[]) {

	if (argc != 3) {

	}
	// reads in name of folder that 
	// stores vtk files, given in run.sh
	//usually called Animate1
	string anim_folder = argv[1];
	//Reads in the name of folder that stores VTK files for
	//visualization without cytoplasm nodes.
	string no_cyt_folder = argv[4];
	//reads in name of folder that 
	//stores data output, given in run.sh
	string locations_cyt_folder = argv[3];
	//this is a folder that holds node
	//locations in the fashion that weitao 
	//asked for to couple the models
	string locations_no_cyt_folder = argv[2];
	//keep track of time
	int start = clock();	

	//.txt file that tells initial
	//cell configuration 
	//cout << "before cell file is read in" << endl;
	string init_tissue = "one_cell.txt";
	//cout << "Read in cell starter" << endl;	

	//instantiate tissue
	//new cell and node objects
	//are made in this call
	Tissue growing_Tissue(init_tissue);
	cout << "Finished creating Cells" << endl;
	growing_Tissue.update_Signal();
	growing_Tissue.update_growth_direction();
	//parameters for time step
	double numSteps = 500;

	// Variable for dataoutput
	int digits;
	string format = ".vtk";
	string Number;
	string initial = "/Plant_Cell_";
	string Filename;
	ofstream ofs_anim;
	int out = 0;

	string noCyt_Filename;
	ofstream ofs_noCyt;
	string no_cyt_initial = "/Plant_Cell_NC_";

	int digits2;
	string Number2;
	string Locations_no_cyt;
	ofstream ofs_loc_no_cyt;
	int out2 = 0;

	int digits3;
	string Number3;
	string Locations_cyt;
	ofstream ofs_loc_cyt;
	string locations_initial = "/Locations_";
	int out3 = 0;

	//loop for time steps
	//which matlab file tells you how many
	//seconds each time step reprsents (2.5?)
	for(int Ti = 0; Ti*dt < numSteps; Ti++) {


		//keep track of simulation runs
		if (Ti %1000 == 0) {
			//	cout << "Simulation still running. Ti: " << Ti << endl;
		}

		// Tissue Growth

		//fills vector of neighbor cells for each cell
		//in tissue class this goes through each cell and calls
		//updated neighbor on each cell
		if(Ti%5000==0) {
			//cout << "Find Neighbors" << endl;
			growing_Tissue.update_Neighbor_Cells();
		}	
		if(Ti == 10000){
			growing_Tissue.update_Signal();
			growing_Tissue.update_growth_direction();
		}
		//adds one new cell wall node per cell everytime it is called
		//dont call it right away to give cell time to find initial configuration
		if(Ti>= 10000){
			if(Ti%1000 == 0){	
				//cout << "add new cell wall nodes if needed" << endl;
				growing_Tissue.add_Wall(Ti);
			}
		}
		//currently not in use
		//was used previously to help stability of cells
		//deletes a cell wall node if too close together
		//if(Ti%1000 == 0){	
		//cout << "delete wall" << endl;
		//growing_Tissue.delete_Wall(Ti);
		//}

		//make adhesion pairs for each cell
		if(Ti < 10000){
			if(Ti%1000 == 0) {
				//cout << "adhesion early" << endl;
				growing_Tissue.update_Adhesion();
			}
		}
		else{	
			if(Ti%5000 == 0) {
				//cout << "adhesion"<< endl;
				growing_Tissue.update_Adhesion();
			}
		}

		//adds internal node according to 
		//individual cell growth rate
		if(Ti >= 10000){
			//cout << "cell cycle" << endl;
			growing_Tissue.update_Cell_Cycle(Ti);
		}

		//will divide cell if time
		//cout << "divide necessary cells" << endl;
		if(Ti >= 10000){
			growing_Tissue.division_check();
		}

		//Calculate new forces on cells and nodes
		//cout << "forces" << endl;
		growing_Tissue.calc_New_Forces(Ti);

		//Update node positions
		//cout << "locations" << endl;
		growing_Tissue.update_Cell_Locations(Ti);	

		//cout << "Finished" << endl;

		// print to dataOutput and vtk files
		if(Ti%1000==0) {
			digits = ceil(log10(out + 1));
			if (digits == 1 || digits == 0) {
				Number = "0000" + to_string(out);
			}	
			else if (digits == 2) {
				Number = "000" + to_string(out);
			}	
			else if (digits == 3) {
				Number = "00" + to_string(out);
			}
			else if (digits == 4) {
				Number = "0" + to_string(out);
			}

			Filename = anim_folder + initial + Number + format;

			ofs_anim.open(Filename.c_str());
			growing_Tissue.print_VTK_File(ofs_anim,true);
			ofs_anim.close();	

			noCyt_Filename = no_cyt_folder + no_cyt_initial + Number + format;

			ofs_noCyt.open(noCyt_Filename.c_str());
			growing_Tissue.print_VTK_File(ofs_noCyt,false);
			ofs_noCyt.close();	

			out++;
		}
		/*if(Ti%1000==0) {
			digits2 = ceil(log10(out2 + 1));
			if (digits2 == 1 || digits2 == 0) {
				Number2 = "0000" + to_string(out2);
			}	
			else if (digits2 == 2) {
				Number2 = "000" + to_string(out2);
			}	
			else if (digits2 == 3) {
				Number2 = "00" + to_string(out2);
			}
			else if (digits2 == 4) {
				Number2 = "0" + to_string(out2);
			}

			nem_Filename = nem_folder+ initial2 + Number2 + format;

			ofs_nem.open(nem_Filename.c_str());
			growing_Tissue.print_VTK_Direction_File(ofs_nem);
			ofs_nem.close();	
			out2++;
		}*/
		//data output from simulations
		//for cell center etc
		/*if(Ti%5000 == 1){
		  nem_Filename = nematic_folder + nem_initial + to_string(Number2) + ".txt";
		  ofs_nem.open(nem_Filename.c_str());
		  growing_Tissue.nematic_output(ofs_nem);
		  ofs_nem.close();
		  Number2++;
		  }
		  if(Ti%1000 == 1){
		  locations_Filename = locations_folder + locations_initial + to_string(Number3) + ".txt";
		  ofs_loc.open(locations_Filename.c_str());
		  growing_Tissue.locations_output(ofs_loc);
		  ofs_loc.close();
		  Number3++;
		  }

			nem_Filename = nematic_folder + nem_initial + to_string(Number2) + ".txt";
			ofs_nem.open(nem_Filename.c_str());
			growing_Tissue.nematic_output(ofs_nem);
			ofs_nem.close();
			Number2++;
		}*/
		//locations with cyt nodes
		if(Ti%1000 == 1){
			Locations_no_cyt = locations_no_cyt_folder + locations_initial + to_string(out2) + ".txt";
			ofs_loc_no_cyt.open(Locations_no_cyt.c_str());
			growing_Tissue.locations_output_no_cyt(ofs_loc_no_cyt);
			ofs_loc_no_cyt.close();
			out2++;
			Locations_cyt = locations_cyt_folder + locations_initial + to_string(out3) + ".txt";
			ofs_loc_cyt.open(Locations_cyt.c_str());
			growing_Tissue.locations_output_cyt(ofs_loc_cyt);
			ofs_loc_cyt.close();
			out3++;
		}

	}
	/*nem_Filename = nematic_folder + nem_initial + to_string(Number2) + ".txt";
	  ofs_nem.open(nem_Filename.c_str());
	  growing_Tissue.nematic_output(ofs_nem);
	  ofs_nem.close();*/

	int stop = clock();

	cout << "Time: " << (stop - start) / double(CLOCKS_PER_SEC) * 1000 << endl;

	return 0;
}











