// KenshiInventoryPOC.cpp
#include "pch.h"
#include <iostream>
#include<algorithm>
#include <iterator>
#include <string> 
#include <vector> 
#include <list>
#include <tuple>
#include <set>
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
	tuple<set<int>, int, Rect, Rect>* decisionChart;
	/*
	Each index represents a location in the 3D chart where the axis are width/height/itemNumber
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

	bool CheckItemFits(Item item, int rectWidth, int rectHeight) {
		return ((item.GetWidth() <= rectWidth) && (item.GetHeight() <= rectHeight));
	}

	static bool itemSortMaxDimDescending(Item &item1, Item &item2) {
		return (max(item1.GetWidth(), item1.GetHeight()) > max(item2.GetWidth(), item2.GetHeight()));
	}

	static bool itemSortMaxDimAscending(Item &item1, Item &item2) {
		//will return true if the first input item has a smaller maximum dimension than the second input item,
		//if the maximum dimensions are equal it will decide based on minimum dimension
		bool returnBool;
		if (max(item1.GetWidth(), item1.GetHeight()) == max(item2.GetWidth(), item2.GetHeight())) {
			return (min(item1.GetWidth(), item1.GetHeight()) < min(item2.GetWidth(), item2.GetHeight()));
		}
		else {
			return (max(item1.GetWidth(), item1.GetHeight()) < max(item2.GetWidth(), item2.GetHeight()));
		}
		return (max(item1.GetWidth(), item1.GetHeight()) < max(item2.GetWidth(), item2.GetHeight()));
	}

	static bool rectSortMinDim(Rect &rect1, Rect &rect2) {
		return (min(rect1.GetWidth(), rect1.GetHeight()) < min(rect2.GetWidth(), rect2.GetHeight()));
	}

	bool CompareItemSetArea(set<int> itemSet1, set<int> itemSet2) {
		//returns true if set1 is bigger than or equal to set2
		int set1TotalArea = 0;
		int set2TotalArea = 0;
		for (int itemListIndex : itemSet1) {
			set1TotalArea += (items[itemListIndex].GetWidth() * items[itemListIndex].GetHeight());
		}
		for (int itemListIndex : itemSet2) {
			set2TotalArea += (items[itemListIndex].GetWidth() * items[itemListIndex].GetHeight());
		}
		return set1TotalArea >= set2TotalArea;
	}

	tuple<set<int>, int, Rect, Rect> GetDecisionAtIndex(int widthI, int heightI, int itemI) {
		widthI -= 1;
		heightI -= 1;
		return decisionChart[(widthI + (heightI * packWidth) + (itemI * packWidth*packHeight))];
	}

	void SetDecisionAtIndex(int widthI, int heightI, int itemI, tuple<set<int>, int, Rect, Rect> decision) {
		widthI -= 1;
		heightI -= 1;
		decisionChart[widthI + heightI * packWidth + itemI * packWidth * packHeight] = decision;
	}

	void InitializeDecisionChart() {
		decisionChart = new tuple<set<int>, int, Rect, Rect>[items.size()*size];
	}

	set<int> mergeSortedSets(set<int> set1, set<int> set2) {
		//Merges set1 and set2, and returns a sorted merged set, only real purpose of this function is to make merge calls less messy
		set<int> resultSet;
		merge(set1.begin(), set1.end(), set2.begin(), set2.end(), inserter(resultSet, resultSet.begin()));
		return resultSet;
	}

	/*
	void CheckIfItemsFit() {
		InitializeDecisionChart();
		//sorting items, smallest max dimension to largest max dimension
		sort(items.begin(), items.end(), &Backpack::itemSortMaxDimAscending);
		//for (int rectWidth = 1; rectWidth <= packWidth; rectWidth++) {
		//	for (int rectHeight = 1; rectHeight <= packHeight; rectHeight++) {
		//		for (int itemNum = 0; itemNum < items.size(); itemNum++) {
		for (int itemNum = 0; itemNum < items.size(); itemNum++) {
			for (int rectHeight = 1; rectHeight <= packHeight; rectHeight++) {
				for (int rectWidth = 1; rectWidth <= packWidth; rectWidth++) {
					if (CheckItemFits(items[itemNum], rectWidth, rectHeight)) { //first check if the item fits within the specified size
						int currIWidth = items[itemNum].GetWidth();
						int currIHeight = items[itemNum].GetHeight();
						//determine the one or two possible ways to seperate the remaining space into rectangles
						if ((currIWidth == rectWidth) && (currIHeight == rectHeight)) { //case if item fits exactly
							set<int> singleItemSet;
							singleItemSet.insert(itemNum);
							SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(singleItemSet, 0, NULL, NULL));
						}
						else if ((currIWidth == rectWidth) || (currIHeight == rectHeight)) {
							Rect newRect;
							if (currIWidth == rectWidth) { newRect = Rect(rectWidth, (rectHeight - currIHeight), 0, currIHeight); }
							else { newRect = Rect((rectWidth - currIWidth), rectHeight, currIWidth, 0); }
							set<int> itemRefSet;
							//This line just uses the current space without the current item's space subtracted
							//if (itemNum > 0) { itemRefSet = get<0>(GetDecisionAtIndex(rectWidth, rectHeight, itemNum - 1)); }
							if (itemNum > 0) { itemRefSet = get<0>(GetDecisionAtIndex(newRect.GetWidth(), newRect.GetHeight(), itemNum - 1)); }
							itemRefSet.insert(itemNum);
							SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(itemRefSet, 1, newRect, NULL));
						}
						else {
							//Create both possible partitions
							//vertical bisection
							Rect newRect1 = Rect((currIWidth), (rectHeight - currIHeight), 0, currIHeight);
							Rect newRect2 = Rect((rectWidth - currIWidth), (rectHeight), currIWidth, 0);
							//horizontal bisection
							Rect newRect3 = Rect((rectWidth - currIWidth), (currIHeight), currIWidth, 0);
							Rect newRect4 = Rect((rectWidth), (rectHeight - currIHeight), 0, currIHeight);
							//for both seperations, look at those rectangle sizes as indexs in this chart, at row itemNum-1, to find what set of items can fit in that amount of space	
							if (itemNum > 0) { //Don't check for smaller items if this is the first item
								set<int> mergedList1 = mergeSortedSets(get<0>(GetDecisionAtIndex(newRect1.GetWidth(), newRect1.GetHeight(), itemNum - 1)),
									get<0>(GetDecisionAtIndex(newRect2.GetWidth(), newRect2.GetHeight(), itemNum - 1)));
								set<int> mergedList2 = mergeSortedSets(get<0>(GetDecisionAtIndex(newRect3.GetWidth(), newRect3.GetHeight(), itemNum - 1)),
									get<0>(GetDecisionAtIndex(newRect4.GetWidth(), newRect4.GetHeight(), itemNum - 1)));
								mergedList1.insert(itemNum);
								mergedList2.insert(itemNum);
								//whichever seperation(pair of rectangles) sums to the greater subset of items, save that pair to this index, along with the subset of items
								if (CompareItemSetArea(mergedList1, mergedList2)) {

									SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(mergedList1, 2, newRect1, newRect2));
								}
								else {
									SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(mergedList2, 3, newRect3, newRect4));
								}
							}
							else {
								set<int> singleItemList;
								singleItemList.insert(itemNum);
								SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(singleItemList, 2, newRect1, newRect2));
							}
						}
					}
					else { //if the item doesn't fit, save the decision from the same rect size and item subset
						if (itemNum > 0) { SetDecisionAtIndex(rectWidth, rectHeight, itemNum, GetDecisionAtIndex(rectWidth, rectHeight, itemNum - 1)); }
						else {
							set<int> emptyList;
							SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(emptyList, 5, NULL, NULL));
						}
					}
				}
			}
		}
	}
	*/

	void CheckIfItemsFit() {
		InitializeDecisionChart();
		//sorting items, smallest max dimension to largest max dimension
		sort(items.begin(), items.end(), &Backpack::itemSortMaxDimAscending);
		//for (int rectWidth = 1; rectWidth <= packWidth; rectWidth++) {
		//	for (int rectHeight = 1; rectHeight <= packHeight; rectHeight++) {
		//		for (int itemNum = 0; itemNum < items.size(); itemNum++) {
		for (int itemNum = 0; itemNum < items.size(); itemNum++) {
			for (int rectHeight = 1; rectHeight <= packHeight; rectHeight++) {
				for (int rectWidth = 1; rectWidth <= packWidth; rectWidth++) {
					if (CheckItemFits(items[itemNum], rectWidth, rectHeight)) { //first check if the item fits within the specified size
						int currIWidth = items[itemNum].GetWidth();
						int currIHeight = items[itemNum].GetHeight();
						tuple<set<int>, int, Rect, Rect> decisionWithoutCurrItem;
						if (itemNum > 0) { 
							decisionWithoutCurrItem = GetDecisionAtIndex(rectWidth, rectHeight, itemNum - 1); 
						}
						else {
							set<int> emptyList;
							decisionWithoutCurrItem = make_tuple(emptyList, 5, NULL, NULL);
						}
						//determine the one or two possible ways to seperate the remaining space into rectangles
						if ((currIWidth == rectWidth) && (currIHeight == rectHeight)) { //case if item fits exactly
							set<int> singleItemSet;
							singleItemSet.insert(itemNum);
							bool currenItemSetBigger = CompareItemSetArea(singleItemSet, get<0>(decisionWithoutCurrItem));
							if (currenItemSetBigger) {
								SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(singleItemSet, 0, NULL, NULL));
							}
							else {
								SetDecisionAtIndex(rectWidth, rectHeight, itemNum, decisionWithoutCurrItem);
							}
						}
						else if ((currIWidth == rectWidth) || (currIHeight == rectHeight)) {
							Rect newRect;
							if (currIWidth == rectWidth) {newRect = Rect(rectWidth, (rectHeight - currIHeight), 0, currIHeight);}
							else {newRect = Rect((rectWidth - currIWidth), rectHeight, currIWidth, 0);}
							set<int> itemRefSet;
							//This line just uses the current space without the current item's space subtracted
							//if (itemNum > 0) { itemRefSet = get<0>(GetDecisionAtIndex(rectWidth, rectHeight, itemNum - 1)); }
							if (itemNum > 0) { itemRefSet = get<0>(GetDecisionAtIndex(newRect.GetWidth(), newRect.GetHeight(), itemNum - 1)); }
							itemRefSet.insert(itemNum);
							bool currenItemSetBigger = CompareItemSetArea(itemRefSet, get<0>(decisionWithoutCurrItem));
							if (currenItemSetBigger) {
								SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(itemRefSet, 1, newRect, NULL));
							}
							else {
								SetDecisionAtIndex(rectWidth, rectHeight, itemNum, decisionWithoutCurrItem);
							}
						}
						else {
							//Create both possible partitions
							//vertical bisection
							Rect newRect1 = Rect((currIWidth), (rectHeight - currIHeight), 0, currIHeight);
							Rect newRect2 = Rect((rectWidth - currIWidth), (rectHeight), currIWidth, 0);
							//horizontal bisection
							Rect newRect3 = Rect((rectWidth - currIWidth), (currIHeight), currIWidth, 0);
							Rect newRect4 = Rect((rectWidth), (rectHeight - currIHeight), 0, currIHeight);
							//for both seperations, look at those rectangle sizes as indexs in this chart, at row itemNum-1, to find what set of items can fit in that amount of space	
							if (itemNum > 0) { //Don't check for smaller items if this is the first item
								set<int> mergedList1 = mergeSortedSets(get<0>(GetDecisionAtIndex(newRect1.GetWidth(), newRect1.GetHeight(), itemNum - 1)),
																	   get<0>(GetDecisionAtIndex(newRect2.GetWidth(), newRect2.GetHeight(), itemNum - 1)));
								set<int> mergedList2 = mergeSortedSets(get<0>(GetDecisionAtIndex(newRect3.GetWidth(), newRect3.GetHeight(), itemNum - 1)),
									                                   get<0>(GetDecisionAtIndex(newRect4.GetWidth(), newRect4.GetHeight(), itemNum - 1)));
								mergedList1.insert(itemNum);
								mergedList2.insert(itemNum);
								//whichever seperation(pair of rectangles) sums to the greater subset of items, save that pair to this index, along with the subset of items
								if (CompareItemSetArea(mergedList1, mergedList2)) {
									bool currenItemSetBigger = CompareItemSetArea(mergedList1, get<0>(decisionWithoutCurrItem));
									if (currenItemSetBigger) {
										SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(mergedList1, 2, newRect1, newRect2));
									}
									else {
										SetDecisionAtIndex(rectWidth, rectHeight, itemNum, decisionWithoutCurrItem);
									}
								}
								else {
									bool currenItemSetBigger = CompareItemSetArea(mergedList2, get<0>(decisionWithoutCurrItem));
									if (currenItemSetBigger) {
										SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(mergedList2, 3, newRect3, newRect4));
									}
									else {
										SetDecisionAtIndex(rectWidth, rectHeight, itemNum, decisionWithoutCurrItem);
									}
								}
							}
							else {
								set<int> singleItemList;
								singleItemList.insert(itemNum);
								SetDecisionAtIndex(rectWidth, rectHeight, itemNum, make_tuple(singleItemList, 2, newRect1, newRect2));
							}
						}
					}
					else { //if the item doesn't fit, save the decision from the same rect size and item subset
						if (itemNum > 0) { SetDecisionAtIndex(rectWidth, rectHeight, itemNum, GetDecisionAtIndex(rectWidth, rectHeight, itemNum - 1)); }
						else {
							set<int> emptyList;
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
				tuple<set<int>, int, Rect, Rect> currTuple = GetDecisionAtIndex(currRect.GetWidth(), currRect.GetHeight(), currItem);
				set<int> itemsContained = get<0>(currTuple);
 				if (!itemsContained.empty() && (*itemsContained.rbegin() == currItem)) {//confirm that the last item in the list is the currItem
					int currRectOriginX = currRect.GetOriginX();
					int currRectOriginY = currRect.GetOriginY();
					items[currItem].SetPosition(currRectOriginX, currRectOriginY);
					switch (get<1>(currTuple)) {
					case 0: {//0 indicates that there is no remaining space = > both following Rects will be NULL
						remainingRects.erase(remainingRects.begin() + rectIndex);
						break;
					}
					case 1: {//1 indicates that there is only one remaining rectangle = > second following Rect will be NULL
						Rect rect1 = get<2>(currTuple); //need to adjust the origin and add 
						remainingRects.erase(remainingRects.begin() + rectIndex);
						remainingRects.insert(remainingRects.begin(), Rect(rect1.GetWidth(),
							rect1.GetHeight(),
							rect1.GetOriginX() + currRectOriginX,
							rect1.GetOriginY() + currRectOriginY));
						break;
					}
					case 2: {//	2 indicates that the remaining space was bisected vertically
						Rect vRect1 = get<2>(currTuple); //need to adjust the origin before adding
						Rect vRect2 = get<3>(currTuple);
						remainingRects.erase(remainingRects.begin() + rectIndex);
						remainingRects.insert(remainingRects.begin(), Rect(vRect1.GetWidth(),
							vRect1.GetHeight(),
							vRect1.GetOriginX() + currRectOriginX,
							vRect1.GetOriginY() + currRectOriginY));
						remainingRects.insert(remainingRects.begin(), Rect(vRect2.GetWidth(),
							vRect2.GetHeight(),
							vRect2.GetOriginX() + currRectOriginX,
							vRect2.GetOriginY() + currRectOriginY));
						break;
					}
					case 3: {//3 indicates that the remaining space was bisected horizontally
						Rect hRect1 = get<2>(currTuple); //need to adjust the origin before adding
						Rect hRect2 = get<3>(currTuple);
						remainingRects.erase(remainingRects.begin() + rectIndex);
						remainingRects.insert(remainingRects.begin(), Rect(hRect1.GetWidth(),
							hRect1.GetHeight(),
							hRect1.GetOriginX() + currRectOriginX,
							hRect1.GetOriginY() + currRectOriginY));
						remainingRects.insert(remainingRects.begin(), Rect(hRect2.GetWidth(),
							hRect2.GetHeight(),
							hRect2.GetOriginX() + currRectOriginX,
							hRect2.GetOriginY() + currRectOriginY));
						break;
					}
					//4 indicates that no items fit 
					case 4: {break;}
				    //5 indicates that this is the smallest item and there are none remaining 
					case 5: {break;}
					default: {break;}
					}
					breakOut = true;
					break;
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

	void DisplayDecisionChart() {
		//for (int width = 1; width <= packWidth; width++) {
		//	cout << "WIDTH = " + to_string(width);
		//	for (int height = 1; height <= packWidth; height++) {
		//		cout << "WIDTH = " + to_string(height);
		//		for (int item = 0; item < items.size(); item++) {
		for (int item = 0; item < items.size(); item++) {
			cout << "***ITEM*** = " + to_string(item) + "\n";
			for (int height = 1; height <= packHeight; height++) {
				cout << "\nHEIGHT = " + to_string(height) + "\n";
				for (int width = 1; width <= packWidth; width++) {
					cout << "WIDTH = " + to_string(width) + "    ";
					tuple<set<int>, int, Rect, Rect> currTuple = GetDecisionAtIndex(width, height, item);
					string setString = "| ";
					for (int setItem : get<0>(currTuple)) {
						setString += to_string(setItem) + ", ";
					}
					setString += " |\n";
					cout << setString;
				}
			}
		}
	}

	void AddItem(Item item) {
		items.push_back(item);
		//SortInventory();
	}

	void SortInventory() {
		CheckIfItemsFit();
		PlaceItems();
	}
};

int main()
{
	//Backpack sorting Alg testing
	std::cout << "\n Testing 1x1 examples\n";
	Item oneByOne1 = Item("H1", 1, 1);
	Item oneByOne2 = Item("I1", 1, 1);
	Item oneByOne3 = Item("J1", 1, 1);
	Item oneByOne4 = Item("K1", 1, 1);
	Item oneByOne5 = Item("L1", 1, 1);
	Item oneByOne6 = Item("M1", 1, 1);
	Item oneByOne7 = Item("N1", 1, 1);
	Item oneByOne8 = Item("O1", 1, 1);
	Item oneByOne9 = Item("P1", 1, 1);
	Item oneByOne0 = Item("Q1", 1, 1);
	Item oneByOne01 = Item("R1", 1, 1);
	Item oneByOne02 = Item("S1", 1, 1);

	std::cout << "\n 2X5\n";
	Backpack oneByOnePack1 = Backpack(2, 5);
	oneByOnePack1.AddItem(oneByOne1);
	oneByOnePack1.AddItem(oneByOne2);
	oneByOnePack1.AddItem(oneByOne3);
	oneByOnePack1.AddItem(oneByOne4);
	oneByOnePack1.AddItem(oneByOne5);
	oneByOnePack1.AddItem(oneByOne6);
	oneByOnePack1.AddItem(oneByOne7);
	oneByOnePack1.AddItem(oneByOne8);
	oneByOnePack1.AddItem(oneByOne9);
	oneByOnePack1.AddItem(oneByOne0);
	oneByOnePack1.SortInventory();
	oneByOnePack1.DisplayInv();

	std::cout << "\n 5X2\n";
	Backpack oneByOnePack2 = Backpack(5, 2);
	oneByOnePack2.AddItem(oneByOne1);
	oneByOnePack2.AddItem(oneByOne2);
	oneByOnePack2.AddItem(oneByOne3);
	oneByOnePack2.AddItem(oneByOne4);
	oneByOnePack2.AddItem(oneByOne5);
	oneByOnePack2.AddItem(oneByOne6);
	oneByOnePack2.AddItem(oneByOne7);
	oneByOnePack2.AddItem(oneByOne8);
	oneByOnePack2.AddItem(oneByOne9);
	oneByOnePack2.AddItem(oneByOne0);
	oneByOnePack2.SortInventory();
	oneByOnePack2.DisplayInv();

	std::cout << "\n 3X4\n";
	Backpack oneByOnePack3 = Backpack(3, 4);
	oneByOnePack3.AddItem(oneByOne1);
	oneByOnePack3.AddItem(oneByOne2);
	oneByOnePack3.AddItem(oneByOne3);
	oneByOnePack3.AddItem(oneByOne4);
	oneByOnePack3.AddItem(oneByOne5);
	oneByOnePack3.AddItem(oneByOne6);
	oneByOnePack3.AddItem(oneByOne7);
	oneByOnePack3.AddItem(oneByOne8);
	oneByOnePack3.AddItem(oneByOne9);
	oneByOnePack3.AddItem(oneByOne0);
	oneByOnePack3.AddItem(oneByOne01);
	oneByOnePack3.AddItem(oneByOne02);
	oneByOnePack3.SortInventory();
	oneByOnePack3.DisplayInv();

	std::cout << "\n 4X3\n";
	Backpack oneByOnePack4 = Backpack(4, 3);
	oneByOnePack4.AddItem(oneByOne1);
	oneByOnePack4.AddItem(oneByOne2);
	oneByOnePack4.AddItem(oneByOne3);
	oneByOnePack4.AddItem(oneByOne4);
	oneByOnePack4.AddItem(oneByOne5);
	oneByOnePack4.AddItem(oneByOne6);
	oneByOnePack4.AddItem(oneByOne7);
	oneByOnePack4.AddItem(oneByOne8);
	oneByOnePack4.AddItem(oneByOne9);
	oneByOnePack4.AddItem(oneByOne0);
	oneByOnePack4.AddItem(oneByOne01);
	oneByOnePack4.AddItem(oneByOne02);
	oneByOnePack4.SortInventory();
	oneByOnePack4.DisplayInv();

	std::cout << "\n Testing with large items: \n";
	Item largeItem1 = Item("A1", 1, 4);
	Item largeItem2 = Item("A2", 1, 4);
	Item largeItem3 = Item("A3", 1, 4);
	Item largeItem4 = Item("A4", 1, 4);
	Item largeItem5 = Item("A5", 4, 1);
	Item largeItem6 = Item("A6", 4, 1);
	Item largeItem7 = Item("A7", 4, 1);
	Item largeItem8 = Item("A8", 4, 1);
	Item largeItem9 = Item("B1", 2, 2);
	Item largeItem10 = Item("B2", 2, 2);
	Item largeItem11 = Item("B3", 2, 2);
	Item largeItem12 = Item("B4", 2, 2);
	Item largeItem13 = Item("C1", 4, 4);

	std::cout << "\n Large Items: 1X4\n";
	Backpack largeItemPack1 = Backpack(4, 4);
	largeItemPack1.AddItem(largeItem1);
	largeItemPack1.AddItem(largeItem2);
	largeItemPack1.AddItem(largeItem3);
	largeItemPack1.AddItem(largeItem4);
	largeItemPack1.SortInventory();
	largeItemPack1.DisplayInv();

	std::cout << "\n Large Items: 4X1\n";
	Backpack largeItemPack2 = Backpack(4, 4);
	largeItemPack2.AddItem(largeItem5);
	largeItemPack2.AddItem(largeItem6);
	largeItemPack2.AddItem(largeItem7);
	largeItemPack2.AddItem(largeItem8);
	largeItemPack2.SortInventory();
	largeItemPack2.DisplayInv();

	std::cout << "\n Large Items: square\n";
	Backpack largeItemPack3 = Backpack(4, 4);
	largeItemPack3.AddItem(largeItem9);
	largeItemPack3.AddItem(largeItem10);
	largeItemPack3.AddItem(largeItem11);
	largeItemPack3.AddItem(largeItem12);
	largeItemPack3.SortInventory();
	largeItemPack3.DisplayInv();

	std::cout << "\n Large Items: full Inv sized item\n";
	Backpack largeItemPack4 = Backpack(4, 4);
	largeItemPack4.AddItem(largeItem13);
	largeItemPack4.SortInventory();
	largeItemPack4.DisplayInv();

	std::cout << "\n Complex example #1\n";
	Item complexTestItem1 = Item("H1", 1, 1);
	Item complexTestItem2 = Item("I1", 2, 1);
	Item complexTestItem3 = Item("J1", 1, 2);
	Item complexTestItem4 = Item("K1", 3, 1);
	Item complexTestItem5 = Item("L1", 1, 3);
	Item complexTestItem6 = Item("M1", 4, 1);
	Item complexTestItem7 = Item("N1", 2, 1);
	Item complexTestItem8 = Item("O1", 2, 1);
	Item complexTestItem9 = Item("P1", 2, 1);
	Item complexTestItem0 = Item("Q1", 2, 1);

	Backpack simpleTestPack = Backpack(5, 3);
	simpleTestPack.AddItem(complexTestItem1);
	simpleTestPack.AddItem(complexTestItem2);
	simpleTestPack.AddItem(complexTestItem3);
	simpleTestPack.AddItem(complexTestItem4);
	simpleTestPack.AddItem(complexTestItem5);
	simpleTestPack.AddItem(complexTestItem6);
	simpleTestPack.SortInventory();
	simpleTestPack.DisplayInv();

	//Testing Kenshi Item and backpack sizes
	std::cout << "\n Kenshi full backpack example \n";
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
	kenshiBackpack.SortInventory();
	kenshiBackpack.DisplayDecisionChart();
	kenshiBackpack.DisplayInv();
}
