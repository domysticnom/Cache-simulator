// File: cachesimulator.cpp
// Author: Dominic Rivas
// Date: 4/30/2020
// Section: 508
// E-mail: dominicr1001@tamu.edu 
// Description: Simulates the behavior of a cache
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <fstream>
#include "Cache.h"
using namespace std;

string deciToHex(int n){
	string hex="00";
	int x = 0;
    while(n!=0){
		char y = 0;
        int temp  = 0;
        temp = n % 16;
        if(temp < 10) {
            y = temp + 48;
			hex[x]=y;
			x++;
        }
        else {
            y = temp + 55;
            hex[x]=y;
			x++;
        }

        n = n/16;
    }
		string truehex="0x";
		for(int i = hex.size()-1 ; i >=0 ; i--){
			truehex+=hex[i];
		}
    return truehex;
}

void initializer(Cache cache){
		vector<string> x;
		for(int i = 0 ; i < cache.getBSize() ; i++){
			x.push_back("00");
		}
		int num_sets = (cache.getCSize() / cache.getBSize()) / cache.getAssoc();

		for(int i = 0 ; i < num_sets ; i++){
			for(int j = 0 ; j < cache.getAssoc() ; j++){
				vector<string> vec2(x);
				cache.insert(i,"00",vec2);
			}
		}
	}

void menu(Cache cache , map<string,string> RAM){
	string menu = "";
	string address;
	string data;
	
	while(menu != "quit"){
		cout <<"*** Cache simulator menu ***"<<endl;
		cout <<"type one command:"<<endl;
		cout <<"1. cache-read"<<endl;
		cout <<"2. cache-write"<<endl;
		cout <<"3. cache-flush"<<endl;
	    cout <<"4. cache-view"<<endl;
		cout <<"5. memory-view"<<endl;
		cout <<"6. cache-dump"<<endl;
		cout <<"7. memory-dump"<<endl;
		cout <<"8. quit"<<endl;
		cout <<"****************************"<< endl;
		cin >> menu;
		if(menu == "cache-view"){
			cache.printMap();
		}
		if(menu =="memory-view"){
			cout << "memory_size: " << RAM.size() << endl;
			cout << "memory_content" << endl << "Address:Data" << endl;
			for(int i = 0 ; i < RAM.size();i++ ){
				if(i == 0){
					cout <<"Ox00:" << RAM[deciToHex(0)] << " ";
				}
				else if(i%8 == 0){
					cout << endl << deciToHex(i) << ":" << RAM[deciToHex(i)]<< " ";
				}
				else {
					cout << RAM[deciToHex(i)] << " ";
				}
			}
			cout << endl;
		}
		if(menu == "cache-dump"){
			cache.cache_dump();
		}
		if(menu == "memory-dump"){
			ofstream fout("ram.txt");
			
			for(int i = 0 ; i < RAM.size();i++ ){
				fout << RAM[deciToHex(i)] << endl;
			}
		}
		if(menu == "cache-write"){
			cin>>address;
			if(cin.fail() || address.substr(0,2) != "0x" || address.size() > 4 || address.find_first_of("GHIJKLMNOPQRSTUVWXYZ") != string::npos){
				cout << "Please enter a valid address" << endl;
				continue;
			}
			cin>>data;
			if(cin.fail() || address.substr(0,2) != "0x" || address.size() > 4 || address.find_first_of("GHIJKLMNOPQRSTUVWXYZ") != string::npos){
				cout << "Please enter valid data" << endl;
				continue;
			}
			cache.cache_write(address,data,RAM);
		}
		if(menu == "cache-read"){
			cin>>address;
			if(cin.fail() || address.substr(0,2) != "0x" || address.size() > 4 || address.find_first_of("GHIJKLMNOPQRSTUVWXYZ") != string::npos){
				cout << "Please enter a valid address" << endl;
				continue;
			}
			cache.cache_read(address , RAM);
			
		}
		if(menu == "cache-flush"){
			cache.check_dirty(RAM);
			cache.makeEmpty();
			initializer(cache);
			cout << "cache_cleared" << endl;
		}
	}
}


int main(int argc , char *file[]){
	map<string , string> RAM;
	string line;
	int c , b, a ,rep,hit,miss;
	string r, h, m;
	string filename;
	for(int i = 0 ; i < strlen(file[1]) ; i++){
		filename+=file[1][i];
	}
	cout << "*** Welcome to the cache simulator ***\n" << "initialize the RAM:" << endl;

	fstream fin (filename);
	for(int i = 0 ; i < 256 ; i++){
		fin >> line;
		//cout <<"hex: " << deciToHex(i) << endl;
		RAM.insert({deciToHex(i) , line});
	}
	cout << "init-ram 0x00 0xFF" << endl << "ram successfully initialized!" << endl;
	cout << "configure the cache:" << endl;

//******************************CACHE CONFIG************************************
	cout << "cache size: ";
	cin >> c;
	while((c < 8 || c > 256) || cin.fail()){
		cin.clear();
		cin.ignore();
		cout <<"Please enter an integer inclusively between 8 and 256: ";
		cin >> c;
	}

	cout << "data block size: ";
	cin >> b;

	cout << "associativity: ";
	cin >> a;
	while((a != 1 && a!=2&&a!=4) || cin.fail()){
		cin.clear();
		cin.ignore();
		cout <<"Please enter 1, 2, or 4 for associativity";
		cin >> a;
	}

	cout << "replacement policy: ";
	cin >> rep;
	while((rep!= 1 && rep!=2 && rep!=3)|| cin.fail()){
		cin.clear();
		cin.ignore();
		cout << "Please enter 1 or 2: ";
		cin >> rep;
	}
	if (rep == 1){
		r = "random_replacement";
	}
	
	else if(rep == 3){
		r = "least_frequently_used";
	}
	
	else{
		r = "least_recently_used";
	}

	cout<<"write hit policy: ";
	cin >> hit;
	while ((hit !=1 && hit != 2)|| cin.fail()){
		cin.clear();
		cin.ignore();
		cout <<"Please enter 1 or 2: ";
		cin >> hit;
	}
	if (hit ==1){
		h = "write_through";
	}
	else{
		h= "write_back";
	}

	cout << "write miss policy: ";
	cin >> miss;
	while((miss != 1 && miss!=2)|| cin.fail()){
		cin.clear();
		cin.ignore();
		cout << "Please enter a 1 or a 2: ";
		cin >> miss;
	}
	if (miss ==1){
		m = "write_allocate";
	}
	else{
		m = "no_write_allocate";
	}
	cout << "cache successfully configured!" << endl;
//*****************************************************************************

	//int c=32,b=8,a=2;
	//string r="least_recently_used",h="fdsfsd",m="fdshkfj";
	Cache cache(c,b,a,r,h,m);
	initializer(cache);

	menu(cache,RAM);
	return 0;
}
