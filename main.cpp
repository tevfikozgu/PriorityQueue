#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include <time.h>  
#include <math.h>       

using namespace std;

//COMPILATION CODE: g++ -Wall -Werror -std=c++11  main.cpp 

class Locations{ // Locations class 

    double latitude;
    double longitude;
    double distance;
    
	public:
    void set_latitude(double); 
    void set_longitude(double);
    void set_distance(double);
	double get_latitude();
	double get_longitude();
    double get_distance();

};

void Locations::set_latitude(double latitude){
	this->latitude = latitude;
}

void Locations::set_longitude(double longitude){
	this->longitude = longitude;
}

void Locations::set_distance(double distance){
	this->distance = distance;
}

double Locations::get_latitude(){
	return latitude;
}

double Locations::get_longitude(){
	return longitude;
}

double Locations::get_distance(){
	return distance;
}

void printArray(vector<Locations> locations_array){ //for printing array
    int array_size = locations_array.size();
	for(int i = 0; i<array_size; i++){
		cout << i+1 << ". Called Taxi Distance: " << locations_array[i].get_distance() << " meter"<< endl;
	}
}

void swap(Locations &first_location, Locations &second_location)  //swaps two element
{  
    Locations temp_location = first_location;  //temp sale
    first_location = second_location;  
    second_location = temp_location;  
}

void min_heapify(vector<Locations> &inc_vec, int index){
    int left, right,min_index=index;
    int arr_size = inc_vec.size();

    left = 2*index+1; //left child index
    right = 2*index+2; //right child index

    if (left <= arr_size && inc_vec[left].get_distance() < inc_vec[index].get_distance()){ //if left child is smaller
        if (inc_vec[right].get_distance() < inc_vec[left].get_distance()){ //if right child is smaller than left child
            min_index = right;
        } else {
            min_index = left;
        }
        
    }
    else if (right <= arr_size && inc_vec[right].get_distance() < inc_vec[index].get_distance()){ // if left child is bigger but right child is lower
        min_index = right;
    }

    if (min_index != index){
        swap(inc_vec[index],inc_vec[min_index]);
        min_heapify(inc_vec,min_index); //recursive vall
    }

}

Locations delete_min(vector<Locations> &delete_vec){
    if (delete_vec.size() < 1){ //if there is no element
        cerr << "Heap Underflow" << endl;
		exit(1);
    }

    Locations min_distance = delete_vec[0]; //first element which is nearest
    delete_vec[0] = delete_vec.back(); //last became first element
    delete_vec.pop_back();
    min_heapify(delete_vec,0);

    return min_distance;
}

void decrease_current_location(vector<Locations> &dec_vec, int index,int mode){
    if (mode == 0){ //if operation is update
        if (dec_vec[index].get_distance() - 0.01 <= 0){ //for being sure that distance is not lower than 0.01
            dec_vec[index].set_distance(0);
        } else{
            dec_vec[index].set_distance(dec_vec[index].get_distance() - 0.01);
        }
    }  
    int parent_index = 0;
    double parent_index_double = (index-1)/2; //parent index in double format
    if (parent_index_double < 0){ //convert to integer
        parent_index = 0; 
    } else {
        parent_index = floor(parent_index_double);
    }
    while((index > 0) && (parent_index >= 0) && (dec_vec[parent_index].get_distance() > dec_vec[index].get_distance())){ // loop while parent is higher
        swap(dec_vec[index],dec_vec[parent_index]);
        index = parent_index;
        parent_index = (index-1)/2;
    }
}

void insert_taxi(vector<Locations> &insert_vec, Locations new_location){
    insert_vec.push_back(new_location); //new location is put into the array
    decrease_current_location(insert_vec, insert_vec.size() - 1,1);
}

int main(int argc, char** argv){
	ifstream file;
	file.open("locations.txt");

	if (!file){
		cerr << "File cannot be opened!";
		exit(1);
	}
    double hotel_location[2] = {33.40819, 39.19001}; //longitude and latitude
	vector<Locations> locations_array; // locations will be collected at this array
    vector<Locations> called_taxis; // locations will be collected at this array
    srand(time(NULL));

    string line;
    int additions = 0; //total number of addition
    int updates = 0; //total number of updates
    getline(file, line); //this is the header line

	int m = stoi(argv[1]); //operation amount is read from command line
    double p = stof(argv[2]); //porbability value is read from command line
    double probability = p*100; // probability value
    clock_t t;
	t = clock();

    for(int i = 0; i<m; i++){
        if (i!=0 && (i%99 == 0)){ // DELETION Operation
            Locations min_dist_taxi = delete_min(locations_array);
            called_taxis.push_back(min_dist_taxi);
        } else{
            int random_prob_value = rand() % 101;
            if (random_prob_value < probability){ //UPDATE Operation
                if (locations_array.size() != 0){
                    int random_taxi = rand() % (locations_array.size()); //select random taxi
                    decrease_current_location(locations_array,random_taxi,0);
                }
                updates += 1;

            } else { // ADDITION Operation
            
                Locations new_location;
                ////// READ FROM FILE
                file >> line; //latitude (double)
                new_location.set_longitude(stod(line));
                file >> line; //longitude (double)
                new_location.set_latitude(stof(line));
                getline(file, line, '\n'); //this is for reading the \n character into dummy variable.
                
                double longitude_diff = pow((hotel_location[0] - new_location.get_longitude()),2);
                double latitude_diff = pow((hotel_location[1] - new_location.get_latitude()),2);
                double distance = sqrt(longitude_diff + latitude_diff);
                new_location.set_distance(distance);
                insert_taxi(locations_array,new_location);
                additions += 1;
            }
        }
    }

    t = clock() - t; // calculate running time

    cout  <<"----------------------------------------------" << endl;
    cout << "Called Taxi Distances:" << endl;
    printArray(called_taxis);
    cout  <<"----------------------------------------------" << endl;
    cout  <<  "Total Addition: " <<  additions << " Updates: " << updates <<  endl;
    cout << "----------------------------------------------" << endl;
  	printf ("It took %f milliseconds.\n",1000 * ((float)t)/CLOCKS_PER_SEC);
    cout << "----------------------------------------------" << endl;
    file.close();
	return 0;

}