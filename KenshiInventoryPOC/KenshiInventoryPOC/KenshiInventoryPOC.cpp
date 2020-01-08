// KenshiInventoryPOC.cpp
#include "pch.h"
#include <iostream>
#include<algorithm>
#include <string> 
#include <vector> 
#include <tuple>
using namespace std;

class Item {
private:
	string name;
	int height;
	int width;


public:
	//void SetHeight(int h) { height = h; }
	int GetHeight() const { return height; }
	int GetWidth() const { return width; }
	string GetName() const { return name; }

	Item(string inputName = "null", int inputWidth = 0, int inputHeight = 0) {
		name = inputName;
		width = inputWidth;
		height = inputHeight;
	}
};


class Backpack {
private:
	int packHeight;
	int packWidth;
	int size;
	char* displayArray;
	vector<tuple<bool, int, int, Item>> items;
	//items[i] = (itemHasBeenPlaced, originX, originY, Item)
	vector<tuple<int, int, int, int>> remainingRects;
	//remainingRects[i] = (rectangleOriginX, rectangleOriginY, rectangleWidth, rectangleHeight)

	//void UpdateDisplayArray() {
	//	ClearBoolArray();
	//	int currIndex = 0;
	//	int currX = 0;
	//	int currY = 0;
	//	for (size_t i = 0; i < items.size(); i++) {
	//		bool placeFound = false;
	//		while (!placeFound)
	//		{
	//			Item currItem = get<3>(items[i]);
	//			int currItemWidth = get<3>(items[i]).GetWidth();
	//			int currItemHeight = get<3>(items[i]).GetHeight();
	//			bool fitsWidth = currItemWidth <= packWidth - currX;  //check if the item can fit in the remaining width
	//			bool fitsHeight = currItemHeight <= packHeight - currY;  //check if the item can fit in the remaining height, put it in if it does
	//			if (fitsWidth && fitsHeight)
	//			{
	//				int blockedRight = DetectObstructionRight(currX, currY, currItem);
	//				if (!blockedRight)
	//				{// if it fits, place the object and move on to the next
	//					PlaceItemInBoolArray(currX, currY, currItem);
	//					currX += currItemWidth;
	//					placeFound = true;
	//				}
	//				else
	//				{ //if it fits within the backpack, but there is an item blocking it to it's right, find the next empty row and try again
	//					currY += FindNextEmptyRow(currX + blockedRight, currY);
	//					currX = 0;
	//				}
	//			}
	//			else if (!fitsWidth)
	//			{ // if it does not fit between the current index and the boundary of the backpack, find the next empty row and try again
	//				currY += FindNextEmptyRow(0, currY);
	//				currX = 0;
	//			}
	//			else
	//			{ // if the width or an obstructing item to the right are not an issue, the item will not fit in the backpack per the current algorithm, quit
	//				return;
	//			}
	//		}
	//	}
	//}

	//int DetectObstructionRight(int startX, int startY, Item item) {
	//	for (int i = 0; i < item.GetWidth(); i++) {
	//		if (GetDisplayAtCoord(startX + i, startY)) {
	//			return i;
	//		}
	//	}
	//	return 0;
	//}

	//int FindNextEmptyRow(int startX, int startY) {//returns the difference between provided yCoord and first empty yCoord at xCoord provided
	//	for (int i = 0; i < packHeight; i++) {
	//		if (!GetDisplayAtCoord(startX, (startY + i))) {
	//			return i;
	//		}
	//	}
	//	return -1;
	//}

	char GetDisplayAtCoord(int x, int y) {
		return displayArray[(y*packWidth) + x];
	}

	void SetDisplayCoord(int x, int y, Item item) {
		displayArray[(y*packWidth) + x] = item.GetName().at(0);
	}

	void ClearBoolArray() {
		for (int i = 0; i < size; i++) {
			displayArray[i] = NULL;
		}
	}

	void PlaceItemInBoolArray(int xCoord, int yCoord, Item item) {
		int itemWidth = item.GetWidth();
		int itemHeight = item.GetHeight();
		//need to check item index if item is true or else not place it
		for (int i = 0; i < item.GetHeight(); i++) {
			for (int j = 0; j < item.GetWidth(); j++) {
				if (i == 0 || j == 0 || i == itemHeight - 1 || j == itemWidth - 1)
				{ //Set the displayArray coordinate to 2 if you are placing an item's border
					SetDisplayCoord((xCoord + j), (yCoord + i), item);
				}
				else { SetDisplayCoord((xCoord + j), (yCoord + i), item); }
			}
		}
	}

	void UpdateBoolArray() {
		ClearBoolArray();
		for (tuple<bool, int, int, Item> item : items) {
			PlaceItemInBoolArray(get<1>(item), get<2>(item), get<3>(item));
		}
	}

	void FitItemsInPack() {
		remainingRects.clear();
		remainingRects.push_back(make_tuple(0, 0, packWidth, packHeight));
		// initializing remaining rects to backpack size
		sort(items.begin(), items.end(), &Backpack::itemSortMaxDim);
		// sorting items by largest dimension
		for (size_t i = 0; i < items.size(); i++) {
			for (size_t j = 0; j < remainingRects.size(); j++) {
				if (CheckItemFits(items[i], remainingRects[j])) {
					//check to see if item fits in rectangle, if not move on to next largest rectangle
					PlaceItem(i, j);
					break;
				}
			}
		}
	}

	bool CheckItemFits(tuple<bool, int, int, Item> item, tuple<int, int, int, int> rect) {
		return ((get<3>(item).GetWidth() <= get<2>(rect)) && (get<3>(item).GetHeight() <= get<3>(rect)));
	}

	void PlaceItem(int itemIndex, int rectIndex) {
		//Im gonna put the new rectangles in incorrectly for now and let the sorting function just redo itself every iteration because I have enough to deal with already
		tuple<bool, int, int, Item> item = items[itemIndex];
		tuple<int, int, int, int> rect = remainingRects[rectIndex];
		int rectWidth = get<2>(rect);
		int rectHeight = get<3>(rect);
		int rectOriginX = get<0>(rect);
		int rectOriginY = get<1>(rect);
		int itemWidth = get<3>(item).GetWidth();
		int itemHeight = get<3>(item).GetHeight();
		bool sameWidth = rectWidth == itemWidth;
		bool sameHeight = rectHeight == itemHeight;

		int newRect1Width = rectWidth;
		int newRect1Height = rectHeight;
		int newRect1OriginX = rectOriginX;
		int newRect1OriginY = rectOriginY;

		int newRect2Width = rectWidth;
		int newRect2Height = rectHeight;
		int newRect2OriginX = rectOriginX;
		int newRect2OriginY = rectOriginY;

		if (sameWidth && sameHeight) {
			remainingRects.erase(remainingRects.begin() + rectIndex);
		}
		else if (sameWidth) {
			newRect1Height += itemHeight;
			newRect1OriginX += itemHeight;
			remainingRects.erase(remainingRects.begin() + rectIndex);
			remainingRects.push_back(make_tuple(newRect1OriginX, newRect1OriginY, newRect1Width, newRect1Height));
		}
		else if (sameHeight) {
			newRect1Height += itemHeight;
			newRect1OriginX += itemHeight;
			remainingRects.erase(remainingRects.begin() + rectIndex);
			remainingRects.push_back(make_tuple(newRect1OriginX, newRect1OriginY, newRect1Width, newRect1Height));
		}
		else if (itemWidth > itemHeight) {
			//first split rectangle into two, where first rect equals the height exactly
			// First new rect will only adjust height, origin and width stay the same
			newRect1Height = itemHeight;
			// Second new rect will adjust height and shift origin down by height of the first, width will remain the same
			newRect2OriginY += itemHeight;
			newRect2Height = rectHeight - itemHeight;
			//first rectangle will then have the item's width subtracted
			newRect1Width -= itemWidth;
			newRect1OriginX += itemWidth;
			remainingRects.erase(remainingRects.begin() + rectIndex);
			remainingRects.push_back(make_tuple(newRect2OriginX, newRect2OriginY, newRect2Width, newRect2Height));
			remainingRects.push_back(make_tuple(newRect1OriginX, newRect1OriginY, newRect1Width, newRect1Height));
		}
		else {
			//first split rectangle into two, where first rect equals the width exactly
			// First new rect will only adjust width, origin and width stay the same
			newRect1Width = itemWidth;
			// Second new rect will adjust width and shift origin right by width of the first, height will remain the same
			newRect2OriginX += itemWidth;
			newRect2Width = rectWidth - itemWidth;
			//first rectangle will then have the item's height subtracted
			newRect1Height -= itemHeight;
			newRect1OriginY += itemHeight;
			remainingRects.erase(remainingRects.begin() + rectIndex);
			remainingRects.push_back(make_tuple(newRect2OriginX, newRect2OriginY, newRect2Width, newRect2Height));
			remainingRects.push_back(make_tuple(newRect1OriginX, newRect1OriginY, newRect1Width, newRect1Height));
		}
		get<0>(items[itemIndex]) = true;
		get<1>(items[itemIndex]) = rectOriginX;
		get<2>(items[itemIndex]) = rectOriginY;
	}

	static bool itemSortMaxDim(tuple<bool, int, int, Item> &item1, tuple<bool, int, int, Item> &item2) {
		return (max(get<3>(item1).GetWidth(), get<3>(item1).GetHeight()) > max(get<3>(item2).GetWidth(), get<3>(item2).GetHeight()));
	}

	static bool rectSortMinDim(tuple<int, int, int, int> &rect1, tuple<int, int, int, int> &rect2) {
		return (min(get<2>(rect1), get<3>(rect1)) < min(get<2>(rect2), get<3>(rect2)));
	}

public:
	Backpack(int inputWidth = 8, int inputHeight = 10)
	{
		if (inputHeight <= 0 || inputWidth <= 0) { throw std::invalid_argument("Error: May not initialize a Backpack with non=positive dimensions\n"); }
		else {
			packHeight = inputHeight;
			packWidth = inputWidth;
			size = packHeight * packWidth;
			displayArray = new char[size];
		}
	}

	void DisplayInv() {
		//UpdateDisplayArray();
		UpdateBoolArray();
		string label = "\nBackpack\n";
		string topMargin = "";
		string leftMargin = "|";
		string rightMargin = "||\n";
		string emptyCell = "|___";
		string fullCell = "|_O_";
		string borderCell = "|_B_";
		for (int i = 0; i < packWidth; i++) { topMargin += "____"; }
		topMargin += "___\n";
		std::cout << label + topMargin;
		std::cout << topMargin;
		for (int i = 0; i < packHeight; i++) {
			string rowString = leftMargin;
			for (int j = 0; j < packWidth; j++) {
				if (displayArray[(i*packWidth) + j] == NULL) {
					rowString += emptyCell;
				}
				else {
					rowString += "|_";
					rowString += displayArray[(i*packWidth) + j];
					rowString += "_";
				}
				//else if (displayArray[(i*packWidth) + j] == 2) {
				//	rowString += borderCell;
				//}
			}
			rowString += rightMargin;
			std::cout << rowString;
		}
		std::cout << topMargin;
	}

	void AddItem(Item item) {
		items.push_back(make_tuple(false, 0, 0, item));
		FitItemsInPack();
	}
};

int main()
{
	/*
	Item testItemA = Item("A", 1, 1);
	Item testItemB = Item("B", 2, 1);
	Item testItemC = Item("C", 3, 3);
	Item testItemD = Item("D", 7, 3);
	Item testItemE = Item("E", 4, 4);
	Item testItemF = Item("F", 5, 5);
	Item testItemG = Item("G", 12, 1);
	Item testItemH = Item("H", 4, 8);
	Item testItemI = Item("I", 8, 4);

	//Inventory display test 1
	std::cout << "\n Inventory display test 1 \n";
	Backpack testBackpack1;
	testBackpack1.DisplayInv();


	//Inventory display test 2
	std::cout << "\n Inventory display test 2 \n";
	Backpack testBackpack2 = Backpack(20, 20);
	testBackpack2.DisplayInv();


	//Item add and display test
	std::cout << "\n Item add and display test \n";
	Backpack testBackpack3 = Backpack(20, 20);
	testBackpack3.AddItem(testItemA);
	testBackpack3.DisplayInv();

	//Adding two items display test
	std::cout << "\n Adding two items display test \n";
	Backpack testBackpack4 = Backpack(20, 20);
	testBackpack4.AddItem(testItemC);
	testBackpack4.AddItem(testItemD);
	testBackpack4.DisplayInv();

	//adding items that need to go under firsts ones
	std::cout << "\n adding items that need to go under firsts ones \n";
	Backpack testBackpack5 = Backpack(20, 20);
	testBackpack5.AddItem(testItemE);
	testBackpack5.AddItem(testItemF);
	testBackpack5.AddItem(testItemG);
	testBackpack5.AddItem(testItemA);
	testBackpack5.AddItem(testItemB);
	testBackpack5.AddItem(testItemC);
	testBackpack5.DisplayInv();

	//Checking to ensure new border printing is setting width and height correctly
	std::cout << "\n Checking to ensure new border printing is setting width and height correctly \n";
	Backpack testBackpack6 = Backpack(20, 20);
	testBackpack6.AddItem(testItemH);
	testBackpack6.AddItem(testItemI);
	testBackpack6.DisplayInv();

	//Testing the sort function
	std::cout << "\n Testing the sort function \n";
	Backpack testBackpack7 = Backpack(20, 20);
	testBackpack7.AddItem(testItemA);
	testBackpack7.AddItem(testItemB);

	testBackpack7.AddItem(testItemC);
	testBackpack7.AddItem(testItemD);
	testBackpack7.AddItem(testItemE);
	testBackpack7.AddItem(testItemF);
	testBackpack7.AddItem(testItemG);
	testBackpack7.AddItem(testItemH);
	testBackpack7.AddItem(testItemI);
	testBackpack7.DisplayInv();
	//testBackpack7.FitItemsInPack();
	testBackpack7.DisplayInv();
	*/

	//Testing Kenshi Item and backpack sizes
	std::cout << "\n Testing Kenshi Item and backpack sizes \n";
	Item itemA1 = Item("A1", 4, 6);
	Item itemA2 = Item("A2", 4, 6);
	Item itemB1 = Item("B1", 5, 4);
	Item itemB2 = Item("B2", 5, 4);
	Item itemC1 = Item("C1", 2, 6);
	Item itemD1 = Item("D1", 4, 2);
	Item itemD2 = Item("D2", 4, 2);
	Item itemE1 = Item("E1", 2, 4);
	Item itemF1 = Item("F1", 1, 2);
	Item itemF2 = Item("F2", 1, 2);
	Item itemG1 = Item("G1", 3, 1);
	Item itemG2 = Item("G2", 3, 1);
	Item itemG3 = Item("G3", 3, 1);
	Item itemG4 = Item("G4", 3, 1);

	Backpack kenshiBackpack = Backpack(10, 14);
	kenshiBackpack.AddItem(itemA1);
	kenshiBackpack.AddItem(itemA2);
	kenshiBackpack.AddItem(itemB1);
	kenshiBackpack.AddItem(itemB2);
	//kenshiBackpack.AddItem(itemC1);
	//kenshiBackpack.AddItem(itemD1);
	//kenshiBackpack.AddItem(itemD2);
	//kenshiBackpack.AddItem(itemE1);
	//kenshiBackpack.AddItem(itemF1);
	//kenshiBackpack.AddItem(itemF2);
	//kenshiBackpack.AddItem(itemG1);
	//kenshiBackpack.AddItem(itemG2);
	//kenshiBackpack.AddItem(itemG3);
	//kenshiBackpack.AddItem(itemG4);
	kenshiBackpack.DisplayInv();
}
