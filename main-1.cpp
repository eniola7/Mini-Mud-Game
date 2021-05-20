
//----------------------------------------------------------------------------
//                  CS 215 - Project 2 - MiniMud
//----------------------------------------------------------------------------
// Author:  Mubarak EniolatokunboWale Adebayo
// Date:  3/13/19
// Description: This is Project 2 and it involves the use of functions and partial arrays.
// Assistance: I received help from the following: I used these websites to help http://www.cplusplus.com/doc/tutorial/functions/, https://www.tutorialspoint.com/cplusplus/cpp_functions.htm,
// https://stackoverflow.com/questions/8363325/using-only-part-of-an-array, http://flavor.sourceforge.net/spec/arrays.htm.
//-----------------------------------------------------------

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "gameutils.h"
using namespace std;

//----------------------------------------------------------------------------
//							printLogo
//----------------------------------------------------------------------------
void printLogo()
{
	cout << "-----------------------------------------" << endl;
	cout << "|       LOLO GAMES' MINIMUD             |" << endl;
	cout << "|       By: Mubarak Adebayo.            |" << endl;
	cout << "-----------------------------------------" << endl;
}// printLogo()

 //----------------------------------------------------------------------------
 //							getInputFileNameName
 //----------------------------------------------------------------------------
string getInputFileName()
{
	string nameGame;
	cout << "Enter name of game to play: ";
	cin >> nameGame;
	return nameGame;
}// getInputFileNameName()

 //----------------------------------------------------------------------------
 //							initItems
 //----------------------------------------------------------------------------
void initItems(int & numItems, item items[])
{
	for (int i = 0; i < MAX_ITEMS; i++)
	{
		items[i].shortDesc = "";
		items[i].longDesc = "";
	}
	numItems = 0;
}// initItems()

 //----------------------------------------------------------------------------
 //							intitRooms
 //----------------------------------------------------------------------------
void initRooms(room rooms[])
{
	for (int i = 0; i < MAX_ROOMS; i++)
	{
		rooms[i].shortDesc = "";
		rooms[i].longDesc = "";
		rooms[i].north = NO_EXIT;
		rooms[i].south = NO_EXIT;
		rooms[i].east = NO_EXIT;
		rooms[i].west = NO_EXIT;
		initItems(rooms[i].numItems, rooms[i].items);
	}

}// initRooms()

 //----------------------------------------------------------------------------
 //							initWinData
 //----------------------------------------------------------------------------
void initWinData(winDataStruct & winData)
{
	winData.winMessage = "";
	winData.winRoom = NO_EXIT;
	initItems(winData.numWinItems, winData.winItem);
}// initWinData()

 //----------------------------------------------------------------------------
 //							removeItem
 //----------------------------------------------------------------------------
item removeItem(int index, item items[], int & numItems)
{
	item rem;
	if (index < 0 || index > MAX_ITEMS)
	{
		string errorMessage = "removeItem: invalid index = " + index;
		gameAbort(errorMessage);

	}
	rem = items[index];
	for (int i = index; i < numItems; i++)
	{
		items[i] = items[i + 1];
	}

	numItems--;
	return rem;
}// removeItem()

 //----------------------------------------------------------------------------
 //							addItem
 //----------------------------------------------------------------------------
void addItem(item newItem, item items[], int & numItems)
{
	if (numItems > MAX_ITEMS)
	{
		string errorMessage = "addItem: maximum number of items exceeded!";
		gameAbort(errorMessage);
	}
	numItems++;

	items[numItems - 1] = newItem;
}// addItem()

 //----------------------------------------------------------------------------
 //							addItem(overloaded)
 //----------------------------------------------------------------------------
void addItem(string shortDesc, string longDesc, item items[], int & numItems)
{
	item newItem;
	newItem.shortDesc = shortDesc;
	newItem.longDesc = longDesc;
	addItem(newItem, items, numItems);
}// addItem(overloaded)()

 //----------------------------------------------------------------------------
 //							loadData
 //----------------------------------------------------------------------------
void loadData(room rooms[], int & roomNum, winDataStruct & winData) {
	// variable declarations
	ifstream f;               // input file handle
	string tag;               // first word extracted from each line
	string filename;       // name of input file
	string data;           // data read from file using cleanGetline()
	int roomNdx = -1;       // index of the "current room" being read/populated
							// this should always be roomNum-1

							// initialize the rooms and winData
	initRooms(rooms);
	initWinData(winData);

	// get input file name and open the input file; abort on failure
	filename = getInputFileName() + ".txt";
	f.open(filename);
	if (f.fail())
	{
		string message = "Unable to open: " + filename;
		gameAbort(message);
	}

	// read line-by-line and store data in the game data structures
	f >> tag;
	while (tag != "END:") {
		if (tag == "ROOM:") {
			roomNum++;       // starting a new room
			roomNdx++;       // increment every time roomNum is incremented
			cleanGetline(f, rooms[roomNdx].shortDesc); // short desc on remainder of ROOM: line
			cleanGetline(f, rooms[roomNdx].longDesc); // long desc on next line by itself (no tag)
		}
		else if (tag == "ITEM:") {
			string shortDesc, longDesc;
			cleanGetline(f, shortDesc);
			cleanGetline(f, longDesc);
			addItem(shortDesc, longDesc, rooms[roomNdx].items, rooms[roomNdx].numItems);
		}
		else if (tag == "WIN_ITEM:") {

			cleanGetline(f, data);
			addItem(data, "", winData.winItem, winData.numWinItems);
		}
		else if (tag == "NORTH:")
			f >> rooms[roomNdx].north;
		else if (tag == "SOUTH:")
			f >> rooms[roomNdx].south;
		else if (tag == "EAST:")
			f >> rooms[roomNdx].east;
		else if (tag == "WEST:")
			f >> rooms[roomNdx].west;
		else if (tag == "WIN_ROOM:")
			f >> winData.winRoom;
		else if (tag == "REMARK:")
			cleanGetline(f, data);       // data is junk, throw it away
		else if (tag == "WIN_TEXT:")
			cleanGetline(f, winData.winMessage);
		else if (tag == "END:")
			f.close();
		else
			gameAbort("Unknown tag in " + filename + ": " + tag);

		// check for read failure and read the next tag
		if (f.fail())
			gameAbort("Failure while reading input file " + filename);
		f >> tag;
	}
} // loadData()

  //----------------------------------------------------------------------------
  //							makeExitList
  //----------------------------------------------------------------------------
string makeExitList(room thisRoom) {
	string exitList = "You can go:";
	if (thisRoom.north != NO_EXIT) exitList += " north,";
	if (thisRoom.south != NO_EXIT) exitList += " south,";
	if (thisRoom.east != NO_EXIT) exitList += " east,";
	if (thisRoom.west != NO_EXIT) exitList += " west.";
	exitList[exitList.length() - 1] = '.'; // change last comma to a period
	return exitList;
} // makeExitList()

  //----------------------------------------------------------------------------
  //							getCmd
  //----------------------------------------------------------------------------
string getCmd(string & remainder)
{
	string input;
	cout << "====> ";
	if (cin.peek() == '\n') cin.ignore();
	getline(cin, input);
	input = split(input, remainder);

	return input;
}// getCmd()

 //----------------------------------------------------------------------------
 //							doLook
 //----------------------------------------------------------------------------
void doLook(room room, bool showShort)
{
	if (showShort)
	{
		cout << room.shortDesc << endl;
	}
	else
	{
		cout << room.longDesc << endl;
	}

	if (room.numItems == 1)
	{
		cout << "You notice the following: " << room.items[0].shortDesc << "." << endl;
	}
	else if (room.numItems > 1)
	{
		cout << "You notice the following: ";
		for (int i = 0; i < room.numItems - 1; i++)
		{
			cout << room.items[i].shortDesc << ", ";
		}
		cout << room.items[room.numItems - 1].shortDesc << "." << endl;
	}

	cout << makeExitList(room) << endl;
}// doLook()

 //----------------------------------------------------------------------------
 //							doLook(overloaded)
 //----------------------------------------------------------------------------
void doLook(room room)
{
	doLook(room, false);
}// doLook(overloaded)()

 //----------------------------------------------------------------------------
 //							sartUp
 //----------------------------------------------------------------------------
void sartUp(room rooms[], item inventory[], winDataStruct & winData, int & roomNum, int & numItems)
{
	printLogo();
	cout << endl;
	printHelp();
	loadData(rooms, roomNum, winData);
	initItems(numItems, inventory);
	cout << "You fall asleep and dream of a place far away..." << endl;
	doLook(rooms[START_ROOM], false);
}// sartUp()

 //----------------------------------------------------------------------------
 //							goSouth
 //----------------------------------------------------------------------------
void goSouth(int & currentRoom, room rooms[]) {
	if (rooms[currentRoom].south == NO_EXIT)
		cout << "You can't go south.\n";
	else {
		currentRoom = rooms[currentRoom].south;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goSouth()

  //----------------------------------------------------------------------------
  //							goEast
  //----------------------------------------------------------------------------
void goEast(int & currentRoom, room rooms[]) {
	if (rooms[currentRoom].east == NO_EXIT)
		cout << "You can't go east.\n";
	else {
		currentRoom = rooms[currentRoom].east;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goEast()

  //----------------------------------------------------------------------------
  //							goWest
  //----------------------------------------------------------------------------
void goWest(int & currentRoom, room rooms[]) {
	if (rooms[currentRoom].west == NO_EXIT)
		cout << "You can't go west.\n";
	else {
		currentRoom = rooms[currentRoom].west;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goWest()

  //----------------------------------------------------------------------------
  //							findItem
  //----------------------------------------------------------------------------
int findItem(string itemName, item items[], int numItems)
{
	for (int i = 0; i <= numItems; i++)//For every item
	{
		if (items[i].shortDesc == itemName)//the item is the item we are looking for
		{

			return i;
		}
	}
	return NOT_FOUND;
}// findItem()

 //----------------------------------------------------------------------------
 //							doTake
 //----------------------------------------------------------------------------
void doTake(string itemName, item roomInventory[], item playerInventory[], int & numItems, int & numRoomItems)
{
	int itemNdx = findItem(itemName, roomInventory, numRoomItems);
	item Item;

	if (itemNdx != -1) //Item is found
	{

		if (numItems < MAX_ITEMS) //This is if the invetory is not full
		{
			Item = removeItem(itemNdx, roomInventory, numRoomItems);
			addItem(Item, playerInventory, numItems);
			cout << "You take the " << itemName << endl;
		}
		else //if the player inventortu is full
		{
			cout << "Your hands are full, you can't take that." << endl;
		}
	}
	else //item is not seen
	{
		itemNdx = findItem(itemName, playerInventory, numItems);
		if (itemNdx != -1) //Item is in the player's inventory
		{
			cout << "You already have the " << itemName << endl;
		}
		else //Item is no in the player's inventory
		{
			cout << "You see no " << itemName << endl;
		}
	}
}// doTake()

 //----------------------------------------------------------------------------
 //							doExamine
 //----------------------------------------------------------------------------
void doExamine(string itemName, item roomInventory[], item playerInventory[], int numItems, int numRoomItems)
{
	int itemNdx = findItem(itemName, roomInventory, numRoomItems);
	if (itemNdx > -1)//When item is found in room
		cout << roomInventory[itemNdx].longDesc << endl;
	else//When item is not found in room
	{
		itemNdx = findItem(itemName, playerInventory, numItems);
		if (itemNdx != -1)//Item is found in playerInventory
			cout << playerInventory[itemNdx].longDesc << endl;
		else //Item is not found in playerInventory
			cout << "You see no " << itemName << endl;
	}

}//doExamine()

 //----------------------------------------------------------------------------
 //							doDrop
 //----------------------------------------------------------------------------
void doDrop(string itemName, item roomInventory[], item playerInventory[], int & numItems, int & numRoomItems)
{
	int itemNdx = findItem(itemName, playerInventory, numItems);
	item Item;
	if (itemNdx != -1) //Found
	{
		if (numRoomItems <= MAX_ITEMS) //Inventory is not full
		{
			Item = removeItem(itemNdx, playerInventory, numItems);
			addItem(Item.shortDesc, Item.longDesc, roomInventory, numRoomItems);
			cout << "You drop the " << itemName << endl;
		}
		else //full inventory in room
		{
			cout << "The room is full of junk; you can't drop that." << endl;
		}
	}
	else //Not found 
	{
		itemNdx = findItem(itemName, roomInventory, numRoomItems);
		if (itemNdx != -1) //Item found in the room's inventory
		{
			cout << "You don't have the " << itemName << endl;
		}
		else //Item not found
		{
			cout << "You have no " << itemName << endl;
		}
	}
}//doDrop()

 //----------------------------------------------------------------------------
 //							gameWon
 //----------------------------------------------------------------------------
bool gameWon(winDataStruct winData, room rooms[])
{
	int itemNdex = -1;
	int collected = 0;
	for (int i = 0; i < winData.numWinItems; i++)
	{
		itemNdex = findItem(winData.winItem[i].shortDesc, rooms[winData.winRoom].items, rooms[winData.winRoom].numItems);
		if (itemNdex != -1)
		{
			collected++;
		}

	}
	if (collected == winData.numWinItems)
		return true;
	else
		return false;
}//gameWon()

 //----------------------------------------------------------------------------
 //							doInventory
 //----------------------------------------------------------------------------
void doInventory(item item[], int numItems) {
	if (numItems == 0)
		cout << "You don't have anything.\n";
	else {
		cout << "You are holding: ";
		for (int i = 0; i < (numItems - 1); i++)       // for all but last item
			cout << item[i].shortDesc << ", ";           // print short desc with comma
		cout << item[numItems - 1].shortDesc << ".\n"; // for last item, use period and newline
	}
} // doInventory()


  //----------------------------------------------------------------------------
  //							goNorth
  //----------------------------------------------------------------------------
void goNorth(int & currentRoom, room rooms[]) {
	if (rooms[currentRoom].north == NO_EXIT)
		cout << "You can't go north.\n";
	else {
		currentRoom = rooms[currentRoom].north;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goNorth()

  //----------------------------------------------------------------------------
  //							main()
  //----------------------------------------------------------------------------
int main() {
	// declare data structures
	room rooms[MAX_ROOMS];
	item items[MAX_ITEMS];
	winDataStruct winData;
	// declare other variables
	string cmd;                   // command entered by user (first word of input)
	string remainder;           // remainder of command entered by user
	bool gameWasWon = false;   // check with gameWon() as needed
	int numItems, roomNum = 0, roomNdx = 0;

	// start up: load game data from file and initialize player inventory
	sartUp(rooms, items, winData, roomNum, numItems);

	// get first command input
	cmd = getCmd(remainder);
	// repeat until "exit" entered or the game is won
	while (cmd != "exit" && !gameWasWon) {
		if (cmd == "help")
			printHelp();
			// add cases for other commands and invocations
		else if (cmd == "look" || cmd == "l")
			doLook(rooms[roomNdx], false);
		else if (cmd == "inv" || cmd == "i")
			doInventory(items, numItems);
		else if (cmd == "exa" || cmd == "x")
			doExamine(remainder, rooms[roomNdx].items, items, numItems, rooms[roomNdx].numItems);
		else if (cmd == "take" || cmd == "t")
			doTake(remainder, rooms[roomNdx].items, items, numItems, rooms[roomNdx].numItems);
		else if (cmd == "drop" || cmd == "t")
		{
			doDrop(remainder, rooms[roomNdx].items, items, numItems, rooms[roomNdx].numItems);
		}
		else if (cmd == "north" || cmd == "n")
			goNorth(roomNdx, rooms);
		else if (cmd == "south" || cmd == "s")
			goSouth(roomNdx, rooms);
		else if (cmd == "east" || cmd == "e")
			goEast(roomNdx, rooms);
		else if (cmd == "west" || cmd == "w")
			goWest(roomNdx, rooms);
		else // the user entered an unrecognized command
			cout << "What??\n";

		// check if game was won and print win message or get next user input
		gameWasWon = gameWon(winData, rooms);

		if (gameWasWon)
			cout << winData.winMessage << endl;
		else
			cmd = getCmd(remainder);

	}//main()

	system("pause");
	return 0;
}






