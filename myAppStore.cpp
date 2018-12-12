#include <iostream>
#include <iomanip>
#include <string.h>
#include <string>
#include "defn.h"

using namespace std;


int indexCategory(char* category, categories* apps, int size); // sets up the category index
int hashFunction(const char* name, int butts);
tree* newNode(app_info info);
tree* insertBinaryTree(tree* root, tree* key);
tree* deleteBinaryTree(tree* root, tree* key);
tree* minNode(tree* node);
//void insertHash(tree*& node, hash_table_entry*& hashList, int count);
hash_table_entry* insertHash(tree*& node, hash_table_entry* head, int count);
hash_table_entry* findNode(hash_table_entry* hashList, const char* key, int count);
void printApp(app_info* app);
void printOrder(tree* root);
void printName(app_info app);
void printPriceRange(tree* root, float low, float high);
void printAppRange(tree* root, char* low, char* high);
bool isInPriceRange(tree* root, float low, float high);
bool isInAppRange(tree* root, char*low, float high);
void deleteLst(hash_table_entry** head_lst, hash_table_entry** del_lst);

categories* categoryList;
hash_table_entry* hashList;
float size;
float price;
float fHigh;
float fLow;
int numApps;
int numCategories;
int numForCommands;
char  app_name[APP_NAME_LEN];

char c_low[CAT_NAME_LEN];
char c_high[CAT_NAME_LEN];


bool TestForPrime(int val)
{
	int limit, factor = 2;

	limit = (long)(sqrtf((float)val) + 0.5f);
	while ((factor <= limit) && (val % factor))
		factor++;

	return(factor > limit);
}


int main()
{
	cin >> numCategories;
	categoryList = new categories[numCategories];
	
	
	
	for (int i = 0; i < numCategories; i++)
	{
		cin.getline(categoryList[i].category, CAT_NAME_LEN);
		categoryList[i].root = NULL;
	}

	cin >> numApps;
	int hashTableSize = 2 * numApps + 1;
	while(TestForPrime(hashTableSize))
	{
		hashTableSize = hashTableSize + 1;
	}

	hashList = new hash_table_entry[hashTableSize];

	for (int i = 0; i < numApps; i++)
	{
		app_info temp;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.getline(temp.category, CAT_NAME_LEN);
		cin.getline(temp.app_name, APP_NAME_LEN);
		cin.getline(temp.version, VERSION_LEN);
		cin >> temp.size;
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cin.getline(temp.units, UNIT_SIZE);
		cin >> temp.price;
		tree* tempN = newNode(temp);
		int t = indexCategory(temp.category, categoryList, CAT_NAME_LEN);
		categoryList[t].root = insertBinaryTree(categoryList[t].root, tempN);
		int h = hashFunction(tempN->info.app_name, hashTableSize);
		hashList[h] = *(insertHash(tempN, &hashList[h], hashTableSize));
	}

	cin >> numForCommands;
	for (int i = 0; i < numForCommands; i++)
	{
		string commandN;
		cin >> commandN;
		string subCommand;
		if (strcmp(commandN.data(), "find") == 0) {
			cin >> subCommand;
			if (strcmp(subCommand.data(), "app") == 0) {
				(cin >> ws).getline(app_name, APP_NAME_LEN);
				hash_table_entry* app = findNode(hashList, app_name, hashTableSize);
				if (app != NULL)
					printApp(&app -> app_node->info);
				else
					cout << "App is not real." << endl;
	}
			else if (strcmp(subCommand.data(), "category") == 0) {
				char category[CAT_NAME_LEN];
				cin.getline(category, CAT_NAME_LEN);
				int c = indexCategory(category, categoryList, CAT_NAME_LEN);
				if (c != -1)
					printOrder(categoryList[c].root);
				else
					cout << "Category not found" << endl;
			}
			else if (strcmp(subCommand.data(), "price") == 0) {
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				bool foundFree = false;
				for (int p = 0; p < numCategories; p++) {
					foundFree = (isInPriceRange(categoryList[p].root, 0, 0) || foundFree);
					if (isInPriceRange(categoryList[p].root, 0, 0)) {
						cout << categoryList[p].category << endl;
						printPriceRange(categoryList[p].root, 0, 0);
						cout << endl;
					}
				}
				if (!foundFree)
					cout << "Free apps not found." << endl;
			}
		}
		else if (strcmp(commandN.data(), "range") == 0) {
			char category[CAT_NAME_LEN];
			cin >> category;
			cin >> subCommand;
			int c = indexCategory(category, categoryList, CAT_NAME_LEN);
			if (strcmp(subCommand.data(), "price") == 0) {
				cin >> fLow >> fHigh;
				if (c != -1) {
					if (isInPriceRange(categoryList[c].root, fLow, fHigh))
						printPriceRange(categoryList[c].root, fLow, fHigh);
					else
						cout << "No applications found for given range" << endl;
				}
				else
					cout << "Category not found" << endl;
			}
			else if (strcmp(subCommand.data(), "app") == 0) {
				cin >> c_low >> c_high;
				if (c != -1) {
					if (isInAppRange(categoryList[c].root, c_low, c_high))
						printAppRange(categoryList[c].root, c_low, c_high);
					else
						cout << "Category not found" << endl;
				}
				else
					cout << "Category not found" << endl;
			}

			else if (strcmp(commandN.data(), "delete") == 0) 
			{
				cin >> category;
				(cin >> ws).getline(app_name, APP_NAME_LEN);
				hash_table_entry* tableNode = findNode(hashList, app_name, hashTableSize); //points to node with app_name in hashList
				hash_table_entry* headNode = &hashList[hashFunction(app_name, hashTableSize)];  //points to head of hashlist at it's index
				if (tableNode != NULL) 
				{ //the node actually exists
					
					deleteLst(&headNode, &tableNode); //delete entry from hashList
					int c = indexCategory(category, categoryList, CAT_NAME_LEN);
					categoryList[c].root = deleteBinaryTree(categoryList[c].root, tableNode->app_node); //delete entry from binary search tree
				}
				else
				{
					cout << "Application not found; unable to delete" << endl;
			}
		}
		delete[] hashList;
		delete[] categoryList;

		}
		cout << endl;
	}
	return 0;
}


int indexCategory(char* category, categories* cag, int size)
{
	for (int j = 0; j < size; j++)
	{
		if (strcmp(cag[j].category, category) == 0)
			return j;


	}
	return 0;

}

tree* insertBinaryTree(tree* root, tree* key)
{
	if ( root == NULL)
	{
		return key;
	}
	if (strcmp(key->info.app_name, root->info.app_name) < 0)
	{
		root->left = insertBinaryTree(root->left, key);
	}
	else if (strcmp(key->info.app_name, root->info.app_name) > 0)
	{
		root->right = insertBinaryTree(root->right, key);
	}
	return root;
}
tree* deleteBinaryTree(tree* root, tree* target)
{ //returns pointer to the position of the root after deleting it target node
	if (root == NULL)
	{
		return root;
	}
	if (strcmp(target->info.app_name, root->info.app_name) < 0) //target is in left 
	{
		root->left = deleteBinaryTree(root->left, target);
	}
	else if (strcmp(target->info.app_name, root->info.app_name) > 0) //target is in right 
	{
		root->right = deleteBinaryTree(root->right, target);
	}

    else { 
		if (root->left == NULL) { 
			tree* temp = root;
			root = root->right; //can simply delete target and reach the node around
			delete temp;
		}
		else if (root->right == NULL) { //one left child
			tree* temp = root;
			root = root->left; //can simply delete target and reach the node around
			delete temp;
		}
		else { //two children
			tree* temp = minNode(root->right); //get pointer to minimum element in right subtree
			root->info = temp->info; //replace target with it
			root->right = deleteBinaryTree(root->right, target); //recurse on right sub tree
		}
	}
	return root;
}
tree* minNode(tree* node) { //returns smallest node in subtree
	tree* current = node;
	while (current->left != NULL) //go left till we can't no more
		current = current->left;
	return current;
}



int hashFunction(const char* name, int butts)
{
	int done = 0;
	for (int p = 0; name[p] != '\0'; p++)
	{
		done = done + name[p];
		return done % butts;
	}
}
hash_table_entry* insertHash(tree*& node, hash_table_entry*head, int butts) {
	int i = hashFunction(node->info.app_name, butts);
	hash_table_entry* current = &hashList[i];
	while (current->app_node != NULL) {
		i++;
		current = &hashList[i];
	}
	strcpy(current->app_name, node->info.app_name);
	current->app_node = node;
	
}

hash_table_entry* findNode(hash_table_entry* hashList, const char* key, int butts) {
	int i = hashFunction(key, butts);
	hash_table_entry* current = &hashList[i];
	while (i < butts && current->app_node != NULL) {
		if (strcmp(current->app_node->info.app_name, key) == 0)
			return current;
		current = &hashList[i++];
	}
	return NULL;
}

void printApp(app_info* app) {
	cout <<  "category:" << app->category << endl;
	cout <<  "app name:" << app->app_name << endl;
	cout <<  "version:" << app->version << endl;
	cout << "size:" << app->size << endl;
	cout << "units:" << app->units << endl;
	cout <<  "price:" << app->price << endl;
}


bool isInPriceRange(tree* root, float low, float high) {
	bool found = false;
	if (root == NULL)
		return false;
	if (root->info.price >= low && root->info.price <= high)
		return true;
	found = isInPriceRange(root->left, low, high) || isInPriceRange(root->right, low, high);
	return found;
}
void printPriceRange(tree* root, float low, float high) {
		if (root == NULL)
			return;
		printPriceRange(root->left, low, high);
		if (root->info.price >= low && root->info.price <= high)
			cout << root->info.app_name << endl;
		printPriceRange(root->right, low, high);
	}

void printInOrder(tree* root) {
	if (root == NULL)
		return;
	printInOrder(root->left);
	cout << root->info.app_name << endl;
	printInOrder(root->right);
}

bool isInAppRange(tree* root, char* low, char* high) {
	bool found = false;
	if (root == NULL)
		return false;
	if (strcmp(root->info.app_name, low) > -1 && strcmp(root->info.app_name, high) < 1)
		return true;
	found = isInAppRange(root->left, low, high) || isInAppRange(root->right, low, high);
	return found;
}

void printAppRange(tree* root, char* low, char* high) {
	if (root == NULL)
		return;
	printAppRange(root->left, low, high);
	if (strcmp(root->info.app_name, low) > -1 && strcmp(root->info.app_name, high) < 1)
		cout << root->info.app_name << endl;
	printAppRange(root->right, low, high);
}
