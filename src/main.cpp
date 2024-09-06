/* 
 * The implementation of the BIP algorithm.
 * Developed by Hamid Fathi sfathi4@uwo.ca
 * Based on paper: J. E. Wieselthier, G. D. Nguyen and A. Ephremides, 
 * "On the construction of energy-efficient broadcast and multicast trees in wireless networks," 
 * Proceedings IEEE INFOCOM 2000. Conference on Computer Communications.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <cmath>

// The transmitting power is equal to a*r^b in which 
// r is the communication range of the transmitting node,
// a is a constant and b is the attenuation rate.
// For simplicity we assume a = 1 and b = 2.

bool doubleEquals(double x1, double x2, double epsilon = 0.0001)
{
    return std::abs(x1 - x2) < epsilon;
}

int main(int argc, char *argv[]) {
	
	if(argc != 2) {
		std::cout <<"Run the command: ./a.out locations.txt" << std::endl;
		return -1;
	}
	
	std::string line;
	double i, j;
	std::ifstream file(argv[1]);
	std::vector<std::vector<double>> locations;
	std::vector<std::vector<double>> uncovered;	
	// each row of transmitting: x, y, power
	std::vector<std::vector<double>> transmitting;
	// covered nodes that do not transmit:	
	std::vector<std::vector<double>> nontransmitting;
	// each row of transmissionPath: x, y, power
	std::vector<std::vector<double>> transmissionPath;

	// Reading locations of nodes from file and storing them into 
	// locations vector.
	if (file.is_open()) {
		while (std::getline(file, line)) {
			if (!line.empty()){
				
				line = line.substr(1, line.size() - 2);
				
				i = std::stod(line.substr(0, line.find(",")));
				j = std::stod(line.substr(line.find(",") + 1));

				locations.push_back({i,j});	
			}	
		}
		file.close();
	
	}

	// At the beginning, only source is covered, other nodes are uncovered.
	// Covered nodes include transmitting and non-transmitting nodes.
	// At the beginning, only source is transmitting.	
	// the last element of transmitting vector is transmitting power
	// which is basically d^2.
	transmitting.push_back({locations[0][0], locations[0][1], 0});

	for (int i = 1; i < locations.size(); ++i) {
		uncovered.push_back({locations[i][0], locations[i][1]});
	}

	int round = 0;

	// loop until all nodes are covered
	while (uncovered.size() > 0) {
	round++;

	// The BIP algorithms iterates over covered nodes (initially only the source
	// which is transmitting) and checks whether increasing power of a transmitting 
	// node or transmit via a covered non-transmiting node would consume less energy
	// to cover one additional node.

	// vector of min powers: each row: minPower, xSource, ySource, xUncovered, yUncovered
	std::vector<std::vector<double>> minPowersForAllNodes;
	
	double currentPower;	
	double powerToReachUncoveredNode;
	double additionalPower; // if a node is already transmitting, we consider
				// the additional power required to reach an uncovered node.
	double x1, x2, y1, y2;

	int indexMin;
	double minValue;

	// check all transmitting nodes
	// any transmitting node against any uncovered node
	for (int i = 0; i < transmitting.size(); ++i) {	
		x1 = transmitting[i][0];	
		y1 = transmitting[i][1];
		currentPower = transmitting[i][2];
		
		// iterate over all uncovered nodes and find the nearest one.
		for (int i = 0; i < uncovered.size(); ++i) {
			x2 = uncovered[i][0];	
			y2 = uncovered[i][1];	
			
			powerToReachUncoveredNode = pow(x1 - x2, 2) + pow(y1 - y2, 2);
			additionalPower = powerToReachUncoveredNode - currentPower;
			minPowersForAllNodes.push_back({additionalPower, x1, y1, x2, y2});		
		}	

	}
	
	// check all covered non-transmitting nodes
	// any nontransmitting node against any uncovered node
	for (int i = 0; i < nontransmitting.size(); ++i) {
		x1 = nontransmitting[i][0];	
		y1 = nontransmitting[i][1];	
		for (int i = 0; i < uncovered.size(); ++i) {
			x2 = uncovered[i][0];	
			y2 = uncovered[i][1];	
			powerToReachUncoveredNode = pow(x1 - x2, 2) + pow(y1 - y2, 2);
			minPowersForAllNodes.push_back({powerToReachUncoveredNode, x1, y1, x2, y2});	
		}	
	}
	
	// select the way which expends min power (either increasing power of a transmitting node
	// or using a relay node (nontransmitting node)) to cover one additional node.
	std::vector <double> temp;
	for (int i = 0; i < minPowersForAllNodes.size(); ++i) {
		temp.push_back(minPowersForAllNodes[i][0]);	
	}
	indexMin = std::distance(std::begin(temp), 
			std::min_element(std::begin(temp), std::end(temp)));
	temp.clear();
	
	transmissionPath.push_back({minPowersForAllNodes[indexMin][1], 
		minPowersForAllNodes[indexMin][2], minPowersForAllNodes[indexMin][0]});
	
	// select the next new transmitting node from nontransmitting nodes with its power
	// or an already transmitting node with additional amount of increased power

	// if it is already transmitting, update its power
	for (int i = 0; i < transmitting.size(); ++i) {
		if (doubleEquals(transmitting[i][0], minPowersForAllNodes[indexMin][1]) 
				&& doubleEquals(transmitting[i][1], minPowersForAllNodes[indexMin][2]))	{
			transmitting[i][2] += minPowersForAllNodes[indexMin][0];
		}
	}
	
	// if the selected transmitting node is new, remove the selected  
	// node from nontransmitting covered nodes and add it to transmitting
	for (int i = 0; i < nontransmitting.size(); ++i) {
		if (doubleEquals(nontransmitting[i][0], minPowersForAllNodes[indexMin][1]) 
				&& doubleEquals(nontransmitting[i][1], minPowersForAllNodes[indexMin][2]))	{
			nontransmitting.erase(nontransmitting.begin() + i);
			transmitting.push_back({minPowersForAllNodes[indexMin][1],
				 minPowersForAllNodes[indexMin][2], minPowersForAllNodes[indexMin][0]});
		}
	}
	
	// add the new covered node to nontransmitting covered nodes
	nontransmitting.push_back({minPowersForAllNodes[indexMin][3], 
								minPowersForAllNodes[indexMin][4]});	

	// remove the new covered node from uncovered nodes
	for (int i = 0; i < uncovered.size(); ++i) {
		if (doubleEquals(uncovered[i][0], minPowersForAllNodes[indexMin][3]) 
				&& doubleEquals(uncovered[i][1], minPowersForAllNodes[indexMin][4]))	
			uncovered.erase(uncovered.begin() + i);
	}
	
	minPowersForAllNodes.clear();
	
	
	std::cout <<"At the end of round " << round << ":"<< std::endl;
	for (int i = 0; i < transmitting.size(); ++i) {

		std::cout << "Transmitting node: " << "Node ("<< transmitting[i][0] << "," <<
		 transmitting[i][1] << ")" << ", with a power of: " <<  transmitting[i][2] << std::endl;
	}
	for (int i= 0; i< uncovered.size(); ++i){

		std::cout << "Uncovered node: "<< uncovered[i][0] <<", " << uncovered[i][1] << std::endl;
	}
	
	
	} // end of while loop

	std::cout << "======================== Final Results ========================" << std::endl;
	std::cout << "Transmitting nodes (at the end): " << std::endl;

	double totalBroadcastCost = 0;
	
	for (int i = 0; i < transmitting.size(); ++i) {
		std::cout << "Node ("<< transmitting[i][0] << ", " << transmitting[i][1] << ")" 
		<< ", transmitting with a power of: " << transmitting[i][2] << std::endl;
		totalBroadcastCost += transmitting[i][2];
	}
	
	std::cout << "Transmission path: " << std::endl;

	for (int i = 0; i < transmissionPath.size(); ++i) {
		std::cout << "Stage" << i +1 << ": Node " << "(" << transmissionPath[i][0] << ", " 
		<< transmissionPath[i][1] << ")" << ", increases its power by: " << 
		transmissionPath[i][2] << std::endl;
	}
	
	std::cout << "Total transmission cost is: " << totalBroadcastCost << std::endl;

	return 0;
}
