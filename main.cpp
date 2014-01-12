/* Dummy main() function for running test cases
To build, run the Makefile
To build documentation, run: doxygen doxygen.conf
*/
#include <iostream>
#include "lemon.hpp"

int main(){
	std::cout <<"Congartz! All automatic test cases were completed successfully. :D" <<std::endl;
	
	std::cout <<"Do you want to test NoCheat manually? [y/N]" <<std::endl;
	std::cin.sync();
	if(std::cin.peek()=='y'||std::cin.peek()=='Y'){
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		std::cout <<"Type any non-numeric data to leave." <<std::endl;
		NoCheat<int> score = 0;
		while(std::cin.good()){
			std::cout<<"[Score: "<<score<<" ] Enter the delta: ";
			int delta;
			std::cin>>delta;
			score += delta;
		}
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	std::cout <<"Done." <<std::endl;
	return 0;
}
