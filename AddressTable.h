#ifndef ADDRESSTABLE_H_
#define ADDRESSTABLE_H_

#include <string>

/**
	 * Class that represents range of IP addresses
	 */

/**
 * Class that allows adding and removing ranges of IP addresses. It also allows searching if specific IP address fits into one of the added ranges.
 */
class AddressTable {

	/**
	 * @brief Inner class to handle nodes in the interval tree
	 */
	class Node{
	public:
		unsigned int base;	/** starting address of the addresses range.*/
		unsigned int h;		/** Height of the node in the tree. For leafs h==1. */
		Node  *left,*right; /** Pointers the left and right children. */
		unsigned int mask;	/** Holds information about masks with the same base. */
		unsigned int top; 	/** Holds value of the base with applied mask. */
		unsigned int max;	/** maximum value of the addresses range from left and right children and current node. */
		/**
		 * @brief Constructor that accepts base address and mask.
		 * @param [in] b Base value of the IP prefix.
		 * @param [in] m Mask value of the IP prefix.
		 */
		Node(unsigned int b, char m);
		/**
		 * Destructor responsible for removing children objects in the tree.
		 */
		~Node();
		/**
		 * @brief Method that returns smallest mask for prefixes with the same base.
		 * @return Returns smallest of the masks for given node.
		 */
		char getMask();
		/**
		 * @brief Method that checks and updates top parameter after tree operations.
		 * @return Returns new top value for given node.
		 */
		unsigned int updateTop();
		/**
		 * @brief Method that returns height of the node.
		 * @return Returns level of the node in the tree.
		 */
		unsigned int getHeight();
		/**
		 * @brief Method that calculates whether and how node is of the balance (difference between height of left and right children is greater than 1).
		 * @return Returns integer that informs about difference of number of tree levels between left and right children nodes. Positive if Left children has more levels, negative otherwise. Zero is returned when both children have same height in tree.
		 */
		int getBalance();
		/**
		 * @brief Method that checks and updates max parameter of current node.
		 */
		void updateMax();
		/**
		 * @brief Method that returns maximum value for given node.
		 * @return Returns unsigned integer with the maximum IP address from current node and its children.
		 */
		unsigned int getMax();
		/**
		 * @brief Returns the node with minimum base value found in that subtree.
		 * @return Returns pointer to the node in the bottom left-leaf.
		 */
		Node* minValueNode();
		/**
		 * Rotates unbalanced tree to the right.
		 * @param [in] y Pointer to the root of the subtree that needs to be rotated.
		 * @return new root node of the rotated subtree.
		 */
		AddressTable::Node* rotateRight(AddressTable::Node* y);
		/**
		 * @brief Rotates unbalanced tree to the left.
		 * @param [in] x Pointer to the root of the subtree that needs to be rotated.
		 * @return new root node of the rotated subtree.
		 */
		AddressTable::Node* rotateLeft(AddressTable::Node* x);
	};

	Node* root; /** top level node of the tree. */
	bool zero;  /** Variable for /0 prefix. true when this address and mask is added. */
	bool res;	/** Status of insert, delete and search operations. */
public:
	/**
	 * @brief Constructor that will initialize the object.
	 */
	AddressTable();
	/**
	 * @brief Destructor that will destroy all nodes in the tree that holds information about IP prefixes.
	 */
	virtual ~AddressTable();
	/**
	 * @brief Internal function for inserting new IP prefixes to the internal tree structure.
	 * @param [in] root Root node of the tree at the given branch and level.
	 * @param [in] nnode New node that is going to be inserted to the tree
	 * @return Returns pointer to the Node structure that is a new root at the given tree level. Can return null pointer.
	 */
	AddressTable::Node* insertNode( AddressTable::Node* root, AddressTable::Node* nnode );
	/**
	 * @brief Internal function for removing IP prefixes from the internal tree structure.
	 * @param [in] root Node from which searching for the node to delete should proceed.
	 * @param [in] base Base part of the prefix designated for removal.
	 * @param [in] mask mask part of the prefix designated for removal.
	 * @return Returns pointer to the Node structure that is a new root at the given tree level. Can return null pointer.
	 */
	AddressTable::Node* deleteNode(AddressTable::Node* root, unsigned int base, char mask);
	/**
	 * @brief Adds new prefix to table by providing string defined in IPv4 CIDR notation.
	 * @param [in] s String that defines IP address by 4 values separated with dots and mask value after the slash.
	 * @return Returns 0 for success, -1 for failure - identical prefix is already in the table or mask parameter was outside 0-32 range.
	 * @note Only part of the base parameter that lies within provided mask will be taken into consideration during adding new prefix.
	 */
	int add( std::string s );
	/**
	 * @brief Adds new prefix to table by providing IP and mask values.
	 * @param [in] base Unsigned integer that corresponds to the IP address.
	 * @param [in] mask A value between 0 and 32 that defines a range of fixed bits in the base parameter.
	 * @return Returns 0 for success, -1 for failure - identical prefix is already in the table or mask parameter was outside 0-32 range.
	 * @note Only part of the base parameter that lies within provided mask will be taken into consideration during adding new prefix.
	 */
	int add(unsigned int base, char mask);
	/**
	 * @brief Removes prefix provided with the string defined in IPv4 CIDR notation from the table.
	 * @param [in] s String that defines IP address by 4 values separated with dots and mask value after the slash.
	 * @return Returns 0 for success, -1 for failure - prefix is not in the table or mask parameter was outside 0-32 range.
	 * @note Only part of the base parameter that lies within provided mask will be taken into consideration during searching for a node to delete.
	 */
	int del( std::string s );
	/**
	 * @brief Removes prefix provided with the string defined in IPv4 CIDR notation from the table.
	 * @param [in] base
	 * @param [in] mask
	 * @return Returns 0 for success, -1 for failure - prefix is not in the table or mask parameter was outside 0-32 range.
	 * @note Only part of the base parameter that lies within provided mask will be taken into consideration during searching for a node to delete.
	 */
	int del(unsigned int base, char mask);
	/**
	 * @brief Searches the table for a prefix with a smallest mask that that holds given IP.
	 * @param [in] root Node's pointer that holds information about IP prefix.
	 * @param [in] ip IP that is used for searching.
	 * @param [out] Pointer to the node's pointer that holds search results.
	 * @returns Returns null pointer if the IP wasn't found. Pointer to smallest prefix node that holds searched IP.
	 */
	void search(Node* root, unsigned int ip, Node** maxa);
	/**
	 * @brief Searches the table for a prefix with a smallest mask that that holds given IP.
	 * @param [in] s IP address in a string format.
	 * @return Returns -1 if there was no prefix that holds provided IP. If there was a prefix that holds provided IP function returns mask parameter of that prefix.
	 */
	char check(std::string s);
	/**
	 * @brief Function that returns smallest prefix that holds provided IP.
	 * @param [in] ip IP address in a 32bit integer format
	 * @return -1 if there was no prefix that holds provided IP. If there was a prefix that holds provided IP function returns mask parameter of that prefix.
	 */
	char check( unsigned int ip);
	/**
	 * @brief Simple function to convert a string with ip and prefix to integer values that represent 32bit address nad mask.
	 * @param [in] s String that contains prefix or IP in a text format.
	 * @param [out] ip Pointer to integer where function will store address part of the provided string.
	 * @param [out] mask Pointer to the char where function will store mask part of the provided string. If this parameter is null then function will read only address.
	 * @warning only basic syntax checking is performed by this function.
	 * @return True if convertion was successful, false otherwise.
	 */
	bool string2ip(std::string s, unsigned int* ip, char* mask);
};

#endif /* ADDRESSTABLE_H_ */
