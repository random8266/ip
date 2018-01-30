#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <fstream>
#include <cstdlib>

#include "AddressTable.h"

std::string ip2string(unsigned int ip){
	return std::to_string((ip&0xFF000000)>>24)+"."+std::to_string((ip&0x00FF0000)>>16)+"."+
			std::to_string((ip&0x0000FF00)>>8)+"."+std::to_string((ip&0x000000FF));
}

int main(void) {


	unsigned int ip;
	char mask;

	AddressTable at;
	std::string s,se;

	std::cout<<"test for adding two prefixes with mask equal to 0"<<std::endl;
	s = "192.168.0.1/0";
	at.string2ip(s,&ip,&mask);	 std::cout<<"adding "<<s<<" "<<"result: "<<at.add(ip,mask)<<std::endl;
	s = "134.241.67.3/0";
	at.string2ip(s,&ip,&mask);	 std::cout<<"adding "<<s<<" "<<"result: "<<at.add(ip,mask)<<std::endl;
	s = "0.0.0.0/0";
	at.string2ip(s,&ip,&mask); std::cout<<"deleting "<<s<<" "<<"result: "<<at.del(ip,mask)<<std::endl;
	at.string2ip(s,&ip,&mask); std::cout<<"deleting "<<s<<" "<<"result: "<<at.del(ip,mask)<<std::endl;

	std::cout<<std::endl<<"test for adding same element twice"<<std::endl;
	s = "192.168.0.1/24";
	at.string2ip(s,&ip,&mask);	 std::cout<<"adding "<<s<<" "<<"result: "<<at.add(ip,mask)<<std::endl;
	at.string2ip(s,&ip,&mask);	 std::cout<<"adding "<<s<<" "<<"result: "<<at.add(ip,mask)<<std::endl;

	std::cout<<std::endl<<"test for adding same address with different mask"<<std::endl;
	s = "192.168.0.1/23";
	se = "192.168.0.5";
	std::cout<<"searching for mask for IP: "<<se<<" result:"<<(int)at.check(se)<<std::endl;
	at.string2ip(s,&ip,&mask);	 std::cout<<"adding "<<s<<" "<<"result: "<<at.add(ip,mask)<<std::endl;
	std::cout<<"searching for mask for IP: "<<se<<" result:"<<(int)at.check(se)<<std::endl;

	std::cout<<std::endl<<"test for deleting element with given mask"<<std::endl;
	s = "192.168.0.1/23";
	std::cout<<"searching for mask for IP: "<<se<<" result:"<<(int)at.check(se)<<std::endl;
	at.string2ip(s,&ip,&mask);   std::cout<<"deleting "<<s<<" "<<"result: "<<at.del(ip,mask)<<std::endl;
	std::cout<<"searching for mask for IP: "<<se<<" result:"<<(int)at.check(se)<<std::endl;
	at.string2ip(s,&ip,&mask);	  std::cout<<"deleting "<<s<<" "<<"result: "<<at.del(ip,mask)<<std::endl;
	s = "192.168.0.1/24";
	at.string2ip(s,&ip,&mask);
	std::cout<<"searching for mask for IP: "<<se<<" result:"<<(int)at.check(se)<<std::endl;
	std::cout<<"deleting "<<s<<" "<<"result: "<<at.del(ip,mask)<<std::endl;
	std::cout<<"searching for mask for IP: "<<se<<" result:"<<(int)at.check(se)<<std::endl;
	std::cout<<"deleting "<<s<<" "<<"result: "<<at.del(ip,mask)<<std::endl;
	std::cout<<"searching for mask for IP: "<<se<<" result:"<<(int)at.check(se)<<std::endl;

	int ips = 10000;
	std::cout<<std::endl<<"Generating "<<ips<<" prefixes."<<std::endl;

	//generate 1000 masks and ip addresses and insert them to tree and dump to file
	std::ofstream prefs;
	prefs.open ("prefs.txt");

	srand( time( nullptr ) );
	for(int i=0; i<100000; ++i){
		ip = (rand()%256)+((rand()%256)<<8)+((rand()%256)<<16)+((rand()%256)<<24);
		prefs<<ip2string(ip);
		prefs<<"/";
		mask = static_cast<char>( rand()%33);
		prefs<<std::to_string(mask)<<"\n";
		at.add(ip,mask);
	}
	prefs.close();

	//generate IP to search for
	ip = (rand()%256)+((rand()%256)<<8)+((rand()%256)<<16)+((rand()%256)<<24);

	std::cout<<"searching for mask for IP: "<<ip2string(ip)<<" result:"<<(int)at.check(ip)<<std::endl;

	return 0;
}
