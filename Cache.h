// File: Cache.h
// Author: Dominic Rivas
// Date: 4/30/2020
// Section: 508
// E-mail: dominicr1001@tamu.edu 
// Description: Chaied Hash table implementation of a cache.
#ifndef CACHE_H
#define CACHE_H
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

class Cache {
	public:
	Cache(int csize, int b , int assc, string rep , string hit , string miss )
	: cache_sz(csize) , root(new Set(-1 , nullptr , nullptr)) , block_sz(b) , assoc(assc)
	, replacement(rep) , wrt_hit(hit) , wrt_miss(miss) , misses(0) , hits(0) {}
	
	Cache & operator=( const Cache & rhs )
		{
        Cache copy = rhs;
        swap( *this, copy );
        return *this;
		}
	/*	
	~Cache(){
	  makeEmpty();
	}
	*/
	int getCSize(){return cache_sz;}
	int getBSize(){return block_sz;}
	int getAssoc(){return assoc;}

	void insert(int index , const std::string tag, const std::vector<std::string> & value )
    {
        return insert( index, tag, value, root);
    }
	
	void makeEmpty( )
    { makeEmpty(root->down); }

	void printMap( ostream & out = cout ) const
    {
        printMap( root, out );
    }
	
	void cache_read(string address, map<string,string> RAM){
		string hextag = address.substr(2, address.back());
		
		int deci = hexToDeci(hextag);
		
		string binary = hexToBin(hextag);
		
		int offset_bit = ceil(log2(block_sz));
		int sets = ceil(log2(cache_sz/(block_sz*assoc)));
		string offset_y = binary.substr(binary.size()-offset_bit, binary.size());
		int offset = binToDeci(offset_y);
		
		string indexboi= binary.substr(binary.size()-(offset_bit+sets), sets);
		int indexset = binToDeci(indexboi);
		
		string taggy = binary.substr(0, binary.size()-(offset_bit+sets));
		string tagg = deciToHex(binToDeci(taggy)).substr(2,taggy.size());
		string datamaker = taggy+indexboi;
		for(int i = 0 ; i < offset_bit ; i++){
			datamaker+='0';
		}
		string info;
		
		Set* currSet = root->down;
		bool hit = false;
		int eviction_line = -1;
		string RAM_Adr = "-1";
		
		while(currSet != nullptr){
			Block* currBlock = currSet->right;
			if(replacement == "least_recently_used"){
			while(currBlock!= nullptr){
					currBlock-> used++;
					currBlock = currBlock->right;
				}
			}
			if(currSet->index == indexset){
				currBlock = currSet->right;
				while(currBlock!=nullptr){
					currBlock->used++;
					if(currBlock->valid == 1 && currBlock->tag == tagg){
						hits++;
						hit = true;
						currBlock->frequncy++;
						currBlock->used = 0;
						info = currBlock->data[offset];
					}
					currBlock = currBlock->right;
				}
			}
			currSet = currSet->down;
		}
		
		if(hit == false){
			misses++;
			RAM_Adr = address;
			
			eviction_line=indexset*assoc;
			currSet = root->down;
			while(currSet!=nullptr){
				
				if(currSet->index == indexset){
					Block* currBlock = currSet->right;
					if(currSet->validator(currBlock) == false){
						while(currBlock->valid != 0){
							eviction_line++;
							currBlock=currBlock->right;
						}
					}	
					else if(replacement == "random_replacement"){
						int counter = 0;
						counter = rand() % assoc;
						for(int i = 0 ; i < counter ; i++){
							currBlock = currBlock->right;
							eviction_line++;
						}
					}
					else if(replacement == "least_recently_used"){
						Block* mover = currBlock;
						int index_last = 0;
						int i=0;
						while(mover != nullptr){
							if(mover->used > currBlock->used){
								currBlock = mover;
								index_last =i;
							}
							i++;
							mover = mover->right;
						}
						eviction_line+=index_last;
					}
					
					else if(replacement == "least_frequently_used"){
						int index_last = 0;
						int i=0;
						Block* moves = currBlock;
						while(moves != nullptr){
							if(moves->frequncy < currBlock->frequncy){
								currBlock = moves;
								index_last =i;
							}
							i++;
							moves = moves->right;
						}
						eviction_line+=index_last;
					}
					
					currBlock->valid = 1;
					currBlock->tag = tagg;
					currBlock->frequncy++;
					currBlock->used = 0;
					for(int i = 0 ; i < block_sz ; i++){
						currBlock->data[i] = RAM[deciToHex(binToDeci(datamaker)+i)];
					}
					info = currBlock->data[offset];
					taggy = deciToHex(binToDeci(datamaker));
					currBlock->addr = taggy.substr(2, taggy.size());
				}
				currSet = currSet->down;
			}
		}
		
		cout << "set:" << indexset << endl;
		cout << "tag:" << tagg << endl;
		if(hit==true){
			cout << "hit:yes" << endl;}
		else{
			cout<<"hit:no"<<endl;}
		cout << "eviction_line:" << eviction_line << endl;
		cout << "ram_address:"<<RAM_Adr<<endl;
		cout <<"data:0x"<< info << endl;
			
		
	}
	
	void cache_write(string address, string intel, map<string,string> &RAM){
		string hextag = address.substr(2, address.back());
		
		int deci = hexToDeci(hextag);
		
		string binary = hexToBin(hextag);
		
		int offset_bit = ceil(log2(block_sz));
		int sets = ceil(log2(cache_sz/(block_sz*assoc)));
		string offset_y = binary.substr(binary.size()-offset_bit, binary.size());
		int offset = binToDeci(offset_y);
		
		string indexboi= binary.substr(binary.size()-(offset_bit+sets), sets);
		int indexset = binToDeci(indexboi);
		
		string taggy = binary.substr(0, binary.size()-(offset_bit+sets));
		string tagg = deciToHex(binToDeci(taggy)).substr(2,taggy.size());
		cout << "HERE IS TAG: " << taggy << endl;
		string datamaker = taggy+indexboi;
		for(int i = 0 ; i < offset_bit ; i++){
			datamaker+='0';
		}
		
		int dirtybit=0;
		Set* currSet = root->down;
		bool hit = false;
		int eviction_line = -1;
		string RAM_Adr = "-1";
		
		while(currSet != nullptr){
			Block* currBlock = currSet->right;
			if(replacement == "least_recently_used"){
			while(currBlock!= nullptr){
					currBlock-> used++;
					currBlock = currBlock->right;
				}
			}
			if(currSet->index == indexset){
				currBlock = currSet->right;
				while(currBlock!=nullptr){
					currBlock->used++;
					if(currBlock->valid == 1 && currBlock->tag == tagg){
						hits++;
						hit = true;
						currBlock->frequncy++;
						currBlock->used = 0;
						currBlock->data[offset] = intel.substr(2,intel.size());
						currBlock->dirty = 1;
						dirtybit = 1;
						if(wrt_hit == "write_through"){
							RAM[address] = currBlock->data[offset];
							currBlock->dirty = 0;
							dirtybit = 0;
						}
					}
					currBlock = currBlock->right;
				}
			}
			currSet = currSet->down;
		}
		if(hit == false){
			misses++;
			RAM_Adr = address;
			eviction_line=indexset*assoc;
			currSet = root->down;
			while(currSet!=nullptr){
			
				if(currSet->index == indexset){
					Block* currBlock = currSet->right;
					if(currSet->validator(currBlock) == false){
						while(currBlock->valid != 0){
							eviction_line++;
							currBlock=currBlock->right;
						}
					}	
					else if(replacement == "random_replacement"){
						int counter = 0;
						counter = rand() % assoc;
						for(int i = 0 ; i < counter ; i++){
							currBlock = currBlock->right;
							eviction_line++;
						}
					}
					else if(replacement == "least_recently_used"){
						Block* mover = currBlock;
						int index_last = 0;
						int i=0;
						while(mover != nullptr){
							if(mover->used > currBlock->used){
								currBlock = mover;
								index_last =i;
							}
							i++;
							mover = mover->right;
						}
						eviction_line+=index_last;
					}
					
					else if(replacement == "least_frequently_used"){
						int index_last = 0;
						int i=0;
						Block* moves = currBlock;
						while(moves != nullptr){
							if(moves->frequncy < currBlock->frequncy){
								currBlock = moves;
								index_last =i;
							}
							i++;
							moves = moves->right;
						}
						eviction_line+=index_last;
					}
					if(wrt_miss == "write_allocate"){
						currBlock->valid = 1;
						currBlock->tag = tagg;
						currBlock->frequncy++;
						currBlock->used = 0;
						for(int i = 0 ; i < block_sz ; i++){
							currBlock->data[i] = RAM[deciToHex(binToDeci(datamaker)+i)];
						}
						currBlock->data[offset] = intel.substr(2,intel.size());
						taggy = deciToHex(binToDeci(datamaker));
						currBlock->addr = taggy.substr(2, taggy.size());
						currBlock->dirty = 1;
						dirtybit = 1;
						break;
					}
					else{
						RAM[address] = intel.substr(2,intel.size());
					}
				}
				currSet = currSet->down;
			}
		}
		
		cout << "set:" << indexset << endl;
		cout << "tag:" << tagg << endl;
		if(hit==true){
			cout << "write_hit:yes" << endl;}
		else{
		cout<<"write_hit:no"<<endl;}
		cout << "eviction_line:" << eviction_line << endl;
		cout << "ram_address:"<<RAM_Adr<<endl;
		cout <<"data:"<< intel << endl;
		cout <<"dirty_bit:" << dirtybit;
		
	}
	
	void check_dirty(map<string,string> &RAM){
		Set* currSet = root->down;
		while(currSet!=nullptr){
			Block* currBlock = currSet->right;
			while(currBlock != nullptr){
				if(currBlock->dirty == 1 && currBlock -> valid == 1){
					for(int i = 0; i < currBlock->data.size();i++){
						RAM[deciToHex(hexToDeci(currBlock->addr)+i)] = currBlock->data[i];
					}
					currBlock->dirty = 0;
				}
				currBlock = currBlock->right;
			}
			currSet = currSet->down;
		}
	}
	
	void cache_dump(){
		ofstream fout("cache.txt");
		
		Set*currSet = root->down;
		while(currSet!=nullptr){
			Block* currBlock = currSet->right;
			while(currBlock!=nullptr){
				for(int i=0 ; i < currBlock->data.size();i++){
					fout << currBlock->data[i] << " ";
				}
				fout << endl;
				currBlock = currBlock->right;
			}
			currSet = currSet->down;
		}
	}
	
	
	private:

	struct Block {
		int valid;
		int dirty;
		string tag;
		int frequncy;
		int used;
		string addr;
		std::vector<string> data;
		Block* right;

		Block(vector<string> data, string tag, Block* right) : valid(0) , dirty(0) ,frequncy(0) , used(0) , addr("00") , tag(tag) , data(data) , right(right) {}
		
		Block & operator=( const Block & rhs )
		{
        Block copy = rhs;
        swap( *this, copy );
        return *this;
		}
	};
	struct Set {
		int index;
		Set* down;
		Block* right;
		Set(int index , Block *rt , Set* dn) : index(index) , right(rt) , down(dn){}
		
		bool validator(Block* x){
			while(x!= nullptr){
				if(x->valid == 0){
					return false;
				}
				x=x->right;
			}
			return true;
		}
	};

	int cache_sz;
	int block_sz;
	int assoc;
	string replacement;
	string wrt_hit;
	string wrt_miss;
	int misses;
	int hits;
	Set *root;

	void insert( int index, const string tag, const vector<string> & value, Set* t )
    {
		Block* newVal = new Block(value, tag, nullptr);
		Set* prev = t;
		Set* curr = t->down;

		while(curr != nullptr){
			if (index == curr->index){
				Block* valist = curr->right;
				while(valist->right!=nullptr){
					valist = valist->right;
				}
				valist->right = newVal;
				return;
			}
			prev = prev->down;
			curr = curr->down;
		}
		Set* newbotKey = new Set(index,newVal,nullptr);
		prev->down = newbotKey;
    }

	void printMap( Set *t, ostream & out ) const
    {
        if( t != nullptr )
        {
            if(t->index == -1) {
				out << "cache_size:" << cache_sz << endl;
				out << "data_block_size:"<<block_sz<<endl;
				out << "associativity:"<<assoc<<endl;
				out << "replacment_policy:"<<replacement<<endl;
				out << "write_hit_policy:"<<wrt_hit<<endl;
				out << "write_miss_policy:"<<wrt_miss<<endl;
				out <<"number_of_cache_hits:"<<hits<<endl;
				out <<"number_of_cache_misses:"<<misses<<endl;
				out <<"cache_content:"<<endl;
            }
			else {
                printMap_row( t->right, out );
            }
            printMap( t->down, out );
        }
    }
	void printMap_row( Block *v, ostream & out ) const
    {
        if( v != nullptr ){
            out << v->valid << " " << v->dirty << " " << v->tag << " ";
			for(int i = 0 ; i < v->data.size(); i++){
				out << v->data[i] << " ";
			}
			out << endl;
            printMap_row( v->right, out );
        }
    }
	
	int hexToDeci(string hex){
		int deci=0,r=0;
		int len = hex.size();
		for (int i = 0; i < hex.size(); i++)
		{
			len--;
			if(hex[i] >= '0' && hex[i] <= '9'){
				r = hex[i] - 48;
			}
			else if(hex[i] >= 'A' && hex[i] <= 'F'){
				r = hex[i] - 55;
			}
			deci += r * pow(16,len);
		}
		return deci;
	}
	
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
	
	string hexToBin(string hex){
		int num = hexToDeci(hex);
		int rem;
		string bin;
		while(num!=0){
			rem = num%2;
			bin.insert(0,to_string(rem));
			num/=2;
		}
		while(bin.size()!= hex.size()*4){
			bin.insert(bin.begin(),'0');
		}
		return bin;	
	}
	
	int binToDeci(string n) 
	{ 
    string num = n; 
    int dec = 0; 
    int base = 1; 
  
    int len = num.length(); 
    for (int i = len - 1; i >= 0; i--) { 
        if (num[i] == '1'){ 
            dec+= base; 
		}
        base = base * 2; 
    } 
  
    return dec;
	}	
	
	void makeEmpty( Set * & t )
    {
        if( t != nullptr )
        {
            makeEmpty_row( t->right );
            makeEmpty( t->down );
            delete t;
        }
        t = nullptr;
    }

    void makeEmpty_row( Block * & v )
    {
        if( v != nullptr )
        {
            makeEmpty_row( v->right );
            delete v;
        }
        v = nullptr;
    }
	

	
};
#endif
