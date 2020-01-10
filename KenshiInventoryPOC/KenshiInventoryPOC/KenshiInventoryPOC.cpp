// KenshiInventoryPOC.cpp
#include "pch.h"
#include <iostream>
#include<algorithm>
#include <string> 
#include <vector> 
#include <list>
#include <tuple>
using namespace std;

class Item {
private:
	string name;
	int width;
	int height;
	bool isPlaced;
	int posX;
	int posY;

public:
	string GetName() const { return name; }
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	bool IsPlaced() const { return isPlaced; }
	int GetPosX() const { return posX; };
	int GetPosY() const { return posY; };
	
	void SetPosition(int newX, int newY) {
		isPlaced = true;
		posX = newX;
		posY = newY;
	}

	Item(string inputName, int inputWidth, int inputHeight) {
		name = inputName;
		width = inputWidth;
		height = inputHeight;
		isPlaced = false;
		posX = -1;
		posY = -1;
	}
};

class Rect {
	//rect paritions should never have values changed. Only making class for organizational purposes, could just be a tuple.
private:
	int originX;
	int originY;
	int width;
	int height;
public:
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	int GetOriginX() const { return originX; };
	int GetOriginY() const { return originY; };

	Rect(int inputWidth = -1, int inputHeight = -1, int inputPosX=-1, int inputPosY=-1) {
		originX = inputPosX;
		originY = inputPosY;
		width = inputWidth;
		height = inputHeight;
	}
};

class Backpack {
private:
	int packHeight;
	int packWidth;
	int size;
	string* displayArray;
	vector<Item> items;
	vector<Rect> remainingRects;
	tuple<list<int>, int, Rect, Rect>* decisionChart;
	/*
	Each index represents a location in the 3d chart where the axis are width/height/itemNumber
	at each index will be a list of the item indexes this area can contain which will be in ascending order, 
	and the two rectangles that should be created as a result of placing the current item. 

	The int shows how the remaining space will approached
		0 indicates that there is no remaining space => both following Rects will be NULL
		1 indicates that there is only one remaining rectangle => second following Rect will be NULL
		2 indicates that the remaining space was bisected vertically 
		3 indicates that the remaining space was bisected horizontally
		4 indicates that no items fit
		5 indicates that this is the smallest item and there are none remaining

	The Rects will hold their dimensions, and will also hold their offset from the previous rectangle
	*/

	//OLD IMPLEMENTATION
	string GetDisplayAtCoord(int x, int y) {
		return displayArray[(y*packWidth) + x];
	}

	void SetDisplayCoord(int x, int y, Item item) {
		displayArray[(y*packWidth) + x] = item.GetName().substr(0, 2);
	}

	void ClearBoolArray() {
		for (int i = 0; i < size; i++) {
			displayArray[i] = "";
		}
	}

	void PlaceItemInBoolArray(int xCoord, int yCoord, Item item) {
		int itemWidth = item.GetWidth();
		int itemHeight = item.GetHeight();
		//need to check item index if item is true or else not place it
		if (item.IsPlaced()) {
			for (int i = 0; i < itemHeight; i++) {
				for (int j = 0; j < itemWidth; j++) {
					SetDisplayCoord((xCoord + j), (yCoord + i), item);
				}
			}
		}
	}

	void UpdateBoolArray() {
		ClearBoolArray();
		for (Item item : items) {
			if (item.IsPlaced()) {
				PlaceItemInBoolArray(item.GetPosX(), item.GetPosY(), item);
			}
		}
	}

	void FitItemsInPack() {
		remainingRects.clear();
		remainingRects.push_back(Rect(0, 0, packWidth, packHeight));
		// initializing remaining rects to backpack size
		sort(items.begin(), items.end(), &Backpack::itemSortMaxDimDescending);
		// sorting items by largest dimension
		for (size_t i = 0; i < items.size(); i++) {
			for (size_t j = 0; j < remainingRects.size(); j++) {
				if (CheckItemFits(items[i], remainingRects[j].GetWidth(), remainingRects[j].GetHeight())) {
					//check to see if item fits in rectangle, if not move on to next largest rectangle
					PlaceItem(i, j);
					break;
				}
			}
		}
	}

	bool CheckItemFits(Item item, int rectWidth, int rectHeight) {
		return ((item.GetWidth() <= rectWidth) && (item.GetHeight() <= rectHeight));
	}

	void PlaceItem(int itemIndex, int rectIndex) {
		//Im gonna put the new rectangles in incorrectly for now and let the sorting function just redo itself every iteration because I have enough to deal with already
		Item item = items[itemIndex];
		Rect rect = remainingRects[rectIndex];
		int rectWidth = rect.GetWidth();
		int rectHeight = rect.GetHeight();
		int rectOriginX = rect.GetOriginX();
		int rectOriginY = rect.GetOriginY();
		int itemWidth = item.GetWidth();
		int itemHeight = item.GetHeight();
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
			newRect1Height += itemHeight; //Im almost certain these should all be -= not +=
			newRect1OriginX += itemHeight;
			remainingRects.erase(remainingRects.begin() + rectIndex);
			remainingRects.push_back(Rect(newRect1OriginX, newRect1OriginY, newRect1Width, newRect1Height));
		}
		else if (sameHeight) {
			newRect1Height += itemHeight;
			newRect1OriginX += itemHeight;
			remainingRects.erase(remainingRects.begin() + rectIndex);
			remainingRects.push_back(Rect(newRect1OriginX, newRect1OriginY, newRect1Width, newRect1Height));
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
			remainingRects.push_back(Rect(newRect2OriginX, newRect2OriginY, newRect2Width, newRect2Height));
			remainingRects.push_back(Rect(newRect1OriginX, newRect1OriginY, newRect1Width, newRect1Height));
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
			remainingRects.push_back(Rect(newRect2OriginX, newRect2OriginY, newRect2Width, newRect2Height));
			remainingRects.push_back(Rect(newRect1OriginX, newRect1OriginY, newRect1Width, newRect1Height));
		}
		items[itemIndex].SetPosition(rectOriginX, rectOriginY);
	}

	static bool itemSortMaxDimDescending(Item &item1, Item &item2) {
		return (max(item1.GetWidth(), item1.GetHeight()) > max(item2.GetWidth(), item2.GetHeight()));
	}

	static bool rectSortMinDim(Rect &rect1, Rect &rect2) {
		return (min(rect1.GetWidth(), rect1.GetHeight()) < min(rect2.GetWidth(), rect2.GetHeight()));
	}

	//NEW IMPLEMENTATION
	static bool itemSortMaxDimAscending(Item &item1, Item &item2) {
		//will return true if the first input item has a smaller maximum dimension than the second input item
		return (max(item1.GetWidth(), item1.GetHeight()) < max(item2.GetWidth(), item2.GetHeight()));
	}

	tuple<list<int>, int, Rect, Rect> GetDecisionAtIndex(int widthI, int heightI, int itemI) {
		widthI -= 1;
		heightI -= 1;
		return decisionChart[(widthI + (heightI * packWidth) + (itemI * packWidth*packHeight))];
	}

	void SetDecisionAtIndex(int widthI, int heightI, int itemI, tuple<list<int>, int, Rect, Rect> decision) {
		widthI -= 1;
		heightI -= 1;
		decisionChart[widthI + heightI * packWidth + itemI * packWidth * packHeight] = decision;
	}

	void CreateDecisionChart() {
		decisionChart = new tuple<list<int>, int, Rect, Rect>[items.size()*size];
	}

	list<int> mergeSortedSets(list<int> list1, list<int> list2) {
		//Merges list2 into list1 without affecting either input list, and returns a sorted merged set
		list<int> list1Copy(list1);
		list<int> list2Copy(list2);
		list1Copy.merge(list2Copy);
		list1Copy.unique();
		return list1Copy;
	}

	void CheckIfItemsFit() {
		CreateDecisionChart();
		//sorting items, smallest max dimension to largest max dimension
		sort(items.begin(), items.end(), &Backpack::itemSortMaxDimAscending);
		for (int rectWidth = 1; rectWidth <= packWidth; rectWidth++) {
			for (int rectHeight = 1; rectHeight <= packHeight; rectHeight++) {
				for (int itemNum = 0; itemNum < items.size(); itemNum++) {
					if (CheckItemFits(items[itemNum], rectWidth, rectHeight)) { //first check if the item fits within the specified size
						//check find the one or two possible ways to seperate the remaining space into rectangles
						if ((items[itemNum].GetWidth() == rectWidth) && (items[itemNum].GetHeight() == rectHeight)) { //case if item fits exactly
							list<int> singleItemList;
							singleItemList.push_front(itemNum);
							SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(singleItemList, 0, NULL, NULL));
						}
						else if (items[itemNum].GetWidth() == rectWidth) {
							Rect newRect = Rect(rectWidth, (rectHeight - items[itemNum].GetHeight()), 0, items[itemNum].GetHeight());
							list<int> itemRefsList;
							itemRefsList.push_front(itemNum);
							list<int> prevItemRefList;
							if (itemNum > 0) {
								prevItemRefList = get<0>(GetDecisionAtIndex(rectWidth, rectHeight, itemNum - 1));
							}
							else {
								list<int> emptyList;
								prevItemRefList = emptyList;
							}
							//this is completely unnnecssary, I can just put my new one on the back
							list<int> mergedList = mergeSortedSets(itemRefsList, prevItemRefList);
							SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(mergedList, 1, newRect, NULL));
						}
						else if (items[itemNum].GetHeight() == rectHeight) {
							Rect newRect = Rect((rectWidth - items[itemNum].GetWidth()), rectHeight, items[itemNum].GetWidth(), 0);
							list<int> itemRefsList;
							itemRefsList.push_front(itemNum);
							list<int> prevItemRefList;
							if (itemNum > 0) {
								prevItemRefList = get<0>(GetDecisionAtIndex(rectWidth, rectHeight, itemNum - 1));
							}
							else {
								list<int> emptyList;
								prevItemRefList = emptyList;
							}
							//this is completely unnnecssary, I can just put my new one on the back
							list<int> mergedList = mergeSortedSets(itemRefsList, prevItemRefList);
							SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(mergedList, 1, newRect, NULL));
						}
						else {//Create both possible partitions
							 //vertical bisection
							//int newRect1Width = items[itemNum].GetWidth();
							//int newRect1Height = rectHeight - items[itemNum].GetHeight();
							//int rect1OffsetX = 0;
							//int rect1OffsetY = items[itemNum].GetHeight();
							Rect newRect1 = Rect((items[itemNum].GetWidth()), (rectHeight - items[itemNum].GetHeight()), 0, items[itemNum].GetHeight());
							//int newRect2width = rectWidth - items[itemNum].GetWidth;
							//int newRect2Height = rectHeight;
							//int rect2OffsetX = items[itemNum].GetWidth();
							//int rect2OffsetY = 0;
							Rect newRect2 = Rect((rectWidth - items[itemNum].GetWidth()), (rectHeight), items[itemNum].GetWidth(), 0);
							//horizontal bisection
							//int newRect3Width = rectWidth - items[itemNum].GetWidth();
							//int newRect3Height = items[itemNum].GetHeight();
							//int rect3OffsetX = items[itemNum].GetWidth();
							//int rect3OffsetY = 0;
							Rect newRect3 = Rect((rectWidth - items[itemNum].GetWidth()), (items[itemNum].GetHeight()), items[itemNum].GetWidth(), 0);
							//int newRect4Width = rectWidth;
							//int newRect4Height = rectHeight - items[itemNum].GetHeight();
							//int rect4OffsetX = 0;
							//int rect4OffsetY = items[itemNum].GetHeight();
							Rect newRect4 = Rect((rectWidth), (rectHeight - items[itemNum].GetHeight()), 0, items[itemNum].GetHeight());
							//for both seperations, look at those rectangle sizes as indexs in this chart, at row itemNum-1, to find what set of items can fit in that amount of space	
							if (itemNum > 0) { //Don't check for smaller items if this is the first item
								list<int> mergedList1 = mergeSortedSets(get<0>(GetDecisionAtIndex(newRect1.GetWidth(), newRect1.GetHeight(), itemNum - 1)), get<0>(GetDecisionAtIndex(newRect2.GetWidth(), newRect2.GetHeight(), itemNum - 1)));
								list<int> mergedList2 = mergeSortedSets(get<0>(GetDecisionAtIndex(newRect3.GetWidth(), newRect3.GetHeight(), itemNum - 1)), get<0>(GetDecisionAtIndex(newRect4.GetWidth(), newRect4.GetHeight(), itemNum - 1)));
								mergedList1.push_back(itemNum);
								mergedList2.push_back(itemNum);
								//whichever seperation(pair of rectangles) sums to the greater subset of items, save that pair to this index, along with the subset of items
								if (mergedList1.size() > mergedList2.size()) {

									SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(mergedList1, 2, newRect1, newRect2));
								}
								else {
									SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(mergedList2, 3, newRect3, newRect4));
								}
							}
							else {
								list<int> singleItemList;
								singleItemList.push_back(itemNum);
								SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(singleItemList, 2, newRect1, newRect2));
							}
						}
					}
					else { //if the item doesn't fit, save the decision from the same rect size and item subset
						if (itemNum > 0) {
							SetDecisionAtIndex(rectWidth, rectHeight, itemNum, GetDecisionAtIndex(rectWidth, rectHeight, itemNum - 1));
						}
						else {
							list<int> emptyList;
							SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(emptyList, 5, NULL, NULL));
						}
					}
				}
			}
		}
	}

	void PlaceItems() {
		vector<Rect> remainingRects;
		remainingRects.push_back(Rect(packWidth, packHeight, 0, 0));
		for (int currItem = items.size() - 1; currItem >= 0; currItem--) {
			for (int rectIndex = 0; rectIndex < remainingRects.size(); rectIndex++) {
				bool breakOut = false;
				Rect currRect = remainingRects[rectIndex];
				tuple<list<int>, int, Rect, Rect> currTuple = GetDecisionAtIndex(currRect.GetWidth(), currRect.GetHeight(), currItem);
				list<int> itemsContained = get<0>(currTuple);
				//Need to change lists into sets so I can use the built in find method, they are already sets but I used the list data type mistakenly
				//this should always be the last item actually, IM gonna leave it how it is for now because I just wanna see if it works
				for (int itemNum : itemsContained) {
					if (itemNum == currItem) {//current (largest remaining) item fits within the current rect
						//PlaceItemInBoolArray(currRect.GetOriginX(), currRect.GetOriginY(), items[currItem]);
						int currRectOriginX = currRect.GetOriginX();
						int currRectOriginY = currRect.GetOriginY();
						items[currItem].SetPosition(currRectOriginX, currRectOriginY);
						switch (get<1>(currTuple)) {
						//A bunch of these cases have the same result, I wasn't sure if they would at the time of writing, maybe fix if I ensure I dont need to add stuff

						case 0: {  //0 indicates that there is no remaining space = > both following Rects will be NULL
							//shouldn't do anything here, no new rectangles to add
							break; 
						}
						case 1: {//1 indicates that there is only one remaining rectangle = > second following Rect will be NULL
							Rect rect1 = get<2>(currTuple); //need to adjust the origin and add 
							remainingRects.erase(remainingRects.begin() + rectIndex);
							remainingRects.push_back(Rect(rect1.GetWidth(), rect1.GetHeight(), rect1.GetOriginX() + currRectOriginX, rect1.GetOriginY() + currRectOriginY));
							break;
						}
						case 2: {//	2 indicates that the remaining space was bisected vertically
							Rect vertRect1 = get<2>(currTuple); //need to adjust the origin and add 
							Rect vertRect2 = get<3>(currTuple);
							remainingRects.erase(remainingRects.begin()+rectIndex);
							remainingRects.push_back(Rect(vertRect1.GetWidth(), vertRect1.GetHeight(), vertRect1.GetOriginX() + currRectOriginX, vertRect1.GetOriginY() + currRectOriginY));
							remainingRects.push_back(Rect(vertRect2.GetWidth(), vertRect2.GetHeight(), vertRect2.GetOriginX() + currRectOriginX, vertRect2.GetOriginY() + currRectOriginY));
							break;
						}
						case 3: {//3 indicates that the remaining space was bisected horizontally
							Rect horRect1 = get<2>(currTuple); //need to adjust the origin and add 
							Rect horRect2 = get<3>(currTuple);
							remainingRects.erase(remainingRects.begin() + rectIndex);
							remainingRects.push_back(Rect(horRect1.GetWidth(), horRect1.GetHeight(), horRect1.GetOriginX() + currRectOriginX, horRect1.GetOriginY() + currRectOriginY));
							remainingRects.push_back(Rect(horRect2.GetWidth(), horRect2.GetHeight(), horRect2.GetOriginX() + currRectOriginX, horRect2.GetOriginY() + currRectOriginY));
							break;
						}
						case 4: {//4 indicates that no items fit
							break;
						}
						case 5: {//5 indicates that this is the smallest item and there are none remaining
							break;
						}
						default: {
							break;
						}
						}
						breakOut = true;
						break;
					}
				}
				if (breakOut) {
					break;
				}
			}
		}
	}

public:
	Backpack(int inputWidth = 8, int inputHeight = 10)
	{
		if (inputHeight <= 0 || inputWidth <= 0) { throw std::invalid_argument("Error: May not initialize a Backpack with non-positive dimensions\n"); }
		else {
			packHeight = inputHeight;
			packWidth = inputWidth;
			size = packHeight * packWidth;
			displayArray = new string[size];
		}
	}

	void DisplayInv() {
		//UpdateDisplayArray();
		UpdateBoolArray();
		string label = "\nBackpack\n";
		string topMargin = "";
		string leftMargin = "|";
		string rightMargin = "||\n";
		string emptyCell = "|____";
		string fullCell = "|_O_";
		string borderCell = "|_B_";
		for (int i = 0; i < packWidth; i++) { topMargin += "_____"; }
		topMargin += "___\n";
		std::cout << label + topMargin;
		std::cout << topMargin;
		for (int i = 0; i < packHeight; i++) {
			string rowString = leftMargin;
			for (int j = 0; j < packWidth; j++) {
				if (displayArray[(i*packWidth) + j] == "") {
					rowString += emptyCell;
				}
				else {
					rowString += "|_";
					rowString += displayArray[(i*packWidth) + j];
					rowString += "_";
				}
			}
			rowString += rightMargin;
			std::cout << rowString;
		}
		std::cout << topMargin;
	}

	void AddItem(Item item) {
		items.push_back(item);
		//FitItemsInPack();
		CheckIfItemsFit();
		PlaceItems();
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
	kenshiBackpack.AddItem(itemC1);
	kenshiBackpack.AddItem(itemD1);
	kenshiBackpack.AddItem(itemD2);
	kenshiBackpack.AddItem(itemE1);
	kenshiBackpack.AddItem(itemF1);
	kenshiBackpack.AddItem(itemF2);
	kenshiBackpack.AddItem(itemG1);
	kenshiBackpack.AddItem(itemG2);
	kenshiBackpack.AddItem(itemG3);
	kenshiBackpack.AddItem(itemG4);
	kenshiBackpack.DisplayInv();
}
