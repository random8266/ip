#include <iostream>
#include <bitset>
#include <string>
#include <sstream>
#include "AddressTable.h"

//inner class to handle nodes in the interval tree
AddressTable::Node::Node(unsigned int b, char m):base(b), h(1), left(nullptr), right(nullptr), mask(m){

	if( m > 0 && m < 32){
		mask = (unsigned int)1 << (m-1);
		//we only take as important bits of address those bits that are covered by mask;
		base = b & (((unsigned int)(~0)) << (32-m));

		//calculate maximum address generated by the mask
		top = (((unsigned int)(~0)) >> m) | this->base;

		//set initial value for max parameter used in interval tree
		max = top;
	}
	else{ //m==32
		max = base = top = b;
		mask = 0x80000000;
	}
}

AddressTable::Node::~Node(){
	if( nullptr != left )
		delete left;
	if( nullptr != right )
		delete right;
}

//returns smallest mask in node
char AddressTable::Node::getMask(){

	char c=32;
	unsigned int m = mask;
	while( m != 0){
		c--; m<<=1;
	}
	return c+1;
}

//calculates top value using lowest bit in the mask
unsigned int AddressTable::Node::updateTop(){

	unsigned int m = getMask();
	if( 32 == m )
		return top=base;
	return top = (((unsigned int)(~0)) >> m) | this->base;

}

unsigned int AddressTable::Node::getHeight(){
	return h;
}

int AddressTable::Node::getBalance(){
	int l = (left!=nullptr)?left->getHeight():0;
	int r = (right!=nullptr)?right->getHeight():0;
	return l-r;
}

//update root's max
void AddressTable::Node::updateMax(){
	unsigned int l = (nullptr!=left)?left->getMax():0;
	unsigned int r = (nullptr!=right)?right->getMax():0;
	unsigned int m = l>r ? l : r;

	max = top;
	max = ((m>max)?m:max);
}

unsigned int AddressTable::Node::getMax(){
	return max;
}

AddressTable::Node* AddressTable::Node::minValueNode(){
	Node* min = this;
	while( nullptr != min->left ){
		min = min->left;
	}
	return min;
}

//function for rotating interval tree around root node to right
AddressTable::Node* AddressTable::Node::rotateRight(AddressTable::Node* y){
	AddressTable::Node* x = y->left;
	AddressTable::Node* T3 = x->right;

	// perform rotation
	x->right = y;
	y->left = T3;

	// update max values
	y->updateMax();

	// update heights
	int lh = (nullptr!=y->left) ? y->left->getHeight() : 0;
	int rh = (nullptr!=y->right)? y->right->getHeight(): 0;
	y->h = (lh>rh?lh:rh)+1;
	lh = (nullptr!=x->left) ? x->left->getHeight() : 0;
	rh = (nullptr!=x->right)? x->right->getHeight(): 0;
	x->h = (lh>rh?lh:rh)+1;

	return x;
}

//function for rotating interval tree around root node to left
AddressTable::Node* AddressTable::Node::rotateLeft(AddressTable::Node* x){
	AddressTable::Node* y = x->right;
	AddressTable::Node* T2 = y->left;

	// perform rotation
	y->left = x;
	x->right = T2;

	// update max values
	y->updateMax();

	// update heights
	int lh = (nullptr!=x->left) ? x->left->getHeight() : 0;
	int rh = (nullptr!=x->right)? x->right->getHeight(): 0;
	x->h = (lh>rh?lh:rh)+1;
	lh = (nullptr!=y->left) ? y->left->getHeight() : 0;
	rh = (nullptr!=y->right)? y->right->getHeight(): 0;
	y->h = (lh>rh?lh:rh)+1;


	return y;
}

AddressTable::AddressTable():root(nullptr), zero(false), res(false){
}

AddressTable::~AddressTable(){
	if( nullptr != root )
		delete root;
}


AddressTable::Node* AddressTable::insertNode( AddressTable::Node* root, AddressTable::Node* nnode ){

	if (nullptr == root)
		return nnode;
	//left, right or equal base
	if (nnode->base < root->base )
		root->left = insertNode(root->left, nnode);
	else if (nnode->base > root->base )
		root->right = insertNode(root->right, nnode);
	else{ // Equal base
		//base and mask is already added
		if( root->mask & nnode->mask){
			res = false;
		}// add mask to the set for given base and update top
		else{
			root->mask |= nnode->mask;
			root->updateTop();
			//update max value of root
			root->updateMax();
		}

		return root;
	}

	//update height of this ancestor node
	unsigned int l = (nullptr!=root->left)?root->left->getHeight():0;
	unsigned int r = (nullptr!=root->right)?root->right->getHeight():0;
	root->h = 1 + ((l>r)?l:r);

	//get the balance factor of this ancestor node to check whether this node became unbalanced
	int balance = root->getBalance();

	//left left case
	if (balance > 1 && nnode->base < root->left->base)
		return root->rotateRight(root);

	//right right case
	if (balance < -1 && nnode->base > root->right->base)
		return root->rotateLeft(root);

	//left right case
	if (balance > 1 && nnode->base > root->left->base)
	{
		root->left = root->rotateLeft(root->left);
		return root->rotateRight(root);
	}

	//right left case
	if (balance < -1 && nnode->base < root->right->base)
	{
		root->right = root->rotateRight(root->right);
		return root->rotateLeft(root);
	}

	//update max value of root
	root->updateMax();

	//return unchanged node pointer
	return root;
}

AddressTable::Node* AddressTable::deleteNode(AddressTable::Node* root, unsigned int base, char mask)
{
	if (nullptr==root)
		return root;

	//if the base to be deleted is smaller than the root's base, then it lies in left subtree
	if ( base < root->base)
		root->left = deleteNode(root->left, base, mask);

	//if the base to be deleted is greater than the root's key, then it lies in right subtree
	else if( base > root->base )
		root->right = deleteNode(root->right, base, mask );

	//if base is same as root's base, then this is the node to be deleted or where mask is going to be modified
	else
	{
		//check if mask has given bit
		if(!( root->mask & (((unsigned int)(1)) << (mask-1))) ){
			return root;
		}
		res = true;
		//remove mask bit for current node
		root->mask ^= (((unsigned int)(1)) << (mask-1));
		if( root->mask ){
			root->updateTop();
			 //update max
			root->updateMax();

			return root;
		}
		else{
			// node with only one child or no child
			if( (root->left == NULL) || (root->right == NULL) )
			{
				Node *temp = root->left ? root->left : root->right;

				// No child case
				if (temp == NULL)
				{
					temp = root;
					root = NULL;
				}
				else //one child case
					*root = *temp; //copy the contents of the non-empty child
				delete temp;
			}
			else
			{
				//node with two children. Get the successor (smallest in the right subtree)
				Node* temp = root->right->minValueNode();

				//copy the successor's data to this node
				root->base = temp->base;
				root->mask = temp->mask;

				//set mask to have only one bit set
				temp->mask = 1;
				//delete temp node
				root->right = deleteNode(root->right, temp->base, temp->mask);

				root->updateTop();
				root->updateMax();
			}
		}
	}

	// If the tree had only one node then return
	if (root == NULL)
	  return root;

	//update height of current node
	int lh = (nullptr!=root->left)?root->left->getHeight():0;
	int rh = (nullptr!=root->right)?root->right->getHeight():0;
	root->h = 1 + ((lh>rh)?lh:rh);

	// check whether this node became unbalanced
	int balance = root->getBalance();

	//left left case
	if (balance > 1 && root->left->getBalance() >= 0)
		return root->rotateRight(root);

	//left right case
	if (balance > 1 && root->left->getBalance() < 0)
	{
		root->left = root->rotateLeft(root->left);
		return root->rotateRight(root);
	}

	//right right case
	if (balance < -1 && root->right->getBalance() <= 0)
		return root->rotateLeft(root);

	//right left case
	if (balance < -1 && root->right->getBalance() > 0)
	{
		root->right = root->rotateRight(root->right);
		return root->rotateLeft(root);
	}

	//update max
	root->updateMax();

	return root;
}

int AddressTable::add( std::string s ){
	unsigned int ip;
	char mask;
	if( string2ip(s, &ip, &mask))
		return add(ip,mask);
	return false;
}

int AddressTable::add(unsigned int base, char mask){
	res = true;
	//0 mask case
	if( mask == 0 ){
		if( zero )
			return -1;
		zero = true;
		return 0;
	}
	//check if mask is equal or lower than 32 and greater than 0
	if(mask<0 || mask>32 )
		return -1;
	//create new node object
	AddressTable::Node* an = new AddressTable::Node(base, mask);
	//insert new
	root = insertNode( root, an);

	if( !res ){
		delete an;
		return -1;
	}
	return 0;
}

int AddressTable::del( std::string s ){
	unsigned int ip;
	char mask;
	if( string2ip(s, &ip, &mask))
		return del(ip,mask);
	return -1;
}

int AddressTable::del(unsigned int base, char mask){

	if( mask == 0 ){
		if( zero ){
			zero = false;
			return 0;
		}
		return -1;
	}

	res = false;

	unsigned int nbase = base & (~(unsigned int)0<<(32-mask));
	root = deleteNode( root, nbase, mask );

	if( !res )
		return -1;

	return 0;
}

void AddressTable::search(AddressTable::Node* root, unsigned int value, AddressTable::Node** maxa){

	//check ip fits in the interval
	if( root->base <= value && value <= root->top ){
		//no appropriate interval found previously
		if(nullptr == *maxa )
			*maxa = root;
		//compare new solution with latest and greatest interval
		else{
			if( root->getMask() > (*maxa)->getMask())
				*maxa = root;

		}
	}
	//search left and right leafs if they exist
	if( root->left && root->left->max >= value )
		search( root->left, value, maxa );
	if( root->right && root->right->max >= value )
		search( root->right, value, maxa );
}

char AddressTable::check(std::string s){
	unsigned int ip;
	if( string2ip(s, &ip, nullptr))
		return check(ip);
	return -1;
}

char AddressTable::check( unsigned int ip){

	//return when there is no interval in the tree
	if( nullptr == root )
		return -1;

	//search the tree
	AddressTable::Node* s = nullptr;
	search( root, ip, &s);

	//no appropriate interval was found
	if( nullptr == s ){
		if( zero )
			return 0;
		return -1;
	}

	return s->getMask();
}

//simple function to convert read string with ip and prefix
bool AddressTable::string2ip(std::string s, unsigned int* ip, char* mask){
	std::istringstream iss (s);
	std::string token;
	*ip=0;
	try{
		for(int i=0; i<3; i++){
			std::getline(iss, token, '.');
			*ip |= (std::stoi( token ));
			*ip <<= 8;
		}
		if( nullptr != mask ){
			std::getline(iss, token, '/');
			*ip |= (std::stoi( token ));
			//mask
			std::getline(iss,token);
			*mask = (char)std::stoi( token );
		}
		else{//only ip without /mask
			std::getline(iss, token);
			*ip |= (std::stoi( token ));
		}
	}
	catch(...){
		return false;
	}

	return true;
}


