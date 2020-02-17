# Kenshi-Inventory-POC
This project is an attempt to independently adapt the knapsack algorithm to dynamically sort non-rotatable rectangles such that they would fit within another rectangular container. I was inspired to create this by the pc game "Kenshi". The goal was to create an algorithm whereby an inventory would autosort itself to create space for any new item a user attempted to place with that inventory. After attempting to deduce a method independently, I checked the available pre-existing methods, and found that my solution resembles the “guillotine heuristic”.  It currently has a run time of O(⋅Height⋅Width⋅Items2).

In any example of simple or medium complexity, its will find the optimal solution. Using examples taken from the game "Kenshi" I created as complex examples as I possibly could, and was somewhat successful. My algorithm was able to find the correct solution in a few of these examples, but was unable to find the correct solution in others.

Since there is a finite number of items and backpack dimensions in "Kenshi", it would be possible to "hard code" a solution for this, where an optimal solution doesn't currently exist in real world applications (Not counting solutions that exhaustively check every possibility, do to unmanageable runtime). If I choose to create a game mod with this functionality, I may be able to take some short cuts due to known edge cases, but it would take quite a bit of time.

Here are the test results in its recent state:




 Testing 1x1 examples

 2X5
___________________________________________
total backpack area: 10
number of items: 10
total area of items: 10
total area of included items: 10
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_____________
_____________
||_Q1_|_P1_||
||_K1_|_O1_||
||_J1_|_N1_||
||_I1_|_M1_||
||_H1_|_L1_||
_____________

 5X2
___________________________________________
total backpack area: 10
number of items: 10
total area of items: 10
total area of included items: 10
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
____________________________
____________________________
||_Q1_|_P1_|_O1_|_N1_|_M1_||
||_H1_|_I1_|_J1_|_K1_|_L1_||
____________________________

 3X4
___________________________________________
total backpack area: 12
number of items: 12
total area of items: 12
total area of included items: 12
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
__________________
__________________
||_S1_|_R1_|_Q1_||
||_J1_|_M1_|_P1_||
||_I1_|_L1_|_O1_||
||_H1_|_K1_|_N1_||
__________________

 4X3
___________________________________________
total backpack area: 12
number of items: 12
total area of items: 12
total area of included items: 12
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_______________________
_______________________
||_S1_|_R1_|_Q1_|_P1_||
||_I1_|_K1_|_M1_|_O1_||
||_H1_|_J1_|_L1_|_N1_||
_______________________

 Testing with large items:

 Large Items: 1X4
___________________________________________
total backpack area: 16
number of items: 4
total area of items: 16
total area of included items: 16
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_______________________
_______________________
||_A4_|_A3_|_A2_|_A1_||
||_A4_|_A3_|_A2_|_A1_||
||_A4_|_A3_|_A2_|_A1_||
||_A4_|_A3_|_A2_|_A1_||
_______________________

 Large Items: 4X1
___________________________________________
total backpack area: 16
number of items: 4
total area of items: 16
total area of included items: 16
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_______________________
_______________________
||_A8_|_A8_|_A8_|_A8_||
||_A7_|_A7_|_A7_|_A7_||
||_A6_|_A6_|_A6_|_A6_||
||_A5_|_A5_|_A5_|_A5_||
_______________________

 Large Items: square
___________________________________________
total backpack area: 16
number of items: 4
total area of items: 16
total area of included items: 16
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_______________________
_______________________
||_B4_|_B4_|_B3_|_B3_||
||_B4_|_B4_|_B3_|_B3_||
||_B1_|_B1_|_B2_|_B2_||
||_B1_|_B1_|_B2_|_B2_||
_______________________

 Large Items: full Inv sized item
___________________________________________
total backpack area: 16
number of items: 1
total area of items: 16
total area of included items: 16
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_______________________
_______________________
||_C1_|_C1_|_C1_|_C1_||
||_C1_|_C1_|_C1_|_C1_||
||_C1_|_C1_|_C1_|_C1_||
||_C1_|_C1_|_C1_|_C1_||
_______________________

 Complex example #1
___________________________________________
total backpack area: 15
number of items: 6
total area of items: 15
total area of included items: 15
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
____________________________
____________________________
||_M1_|_M1_|_M1_|_M1_|_L1_||
||_K1_|_K1_|_K1_|_J1_|_L1_||
||_I1_|_I1_|_H1_|_J1_|_L1_||
____________________________
___________________________________________
total backpack area: 140
number of items: 14
total area of items: 140
total area of included items: 140
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_G2_|_G2_|_G2_|_G2_|_G2_|_G1_|_G1_|_G1_|_G1_|_G1_||
||_G2_|_G2_|_G2_|_G2_|_G2_|_G1_|_G1_|_G1_|_G1_|_G1_||
||_G2_|_G2_|_G2_|_G2_|_G2_|_G1_|_G1_|_G1_|_G1_|_G1_||
||_G2_|_G2_|_G2_|_G2_|_G2_|_G1_|_G1_|_G1_|_G1_|_G1_||
||_J2_|_J2_|_J2_|_J2_|_Q2_|_K1_|_K1_|_O4_|_O4_|_O4_||
||_J2_|_J2_|_J2_|_J2_|_Q2_|_K1_|_K1_|_O3_|_O3_|_O3_||
||_J1_|_J1_|_J1_|_J1_|_Q1_|_K1_|_K1_|_O2_|_O2_|_O2_||
||_J1_|_J1_|_J1_|_J1_|_Q1_|_K1_|_K1_|_O1_|_O1_|_O1_||
_____________________________________________________

Kenshi full backpack example 2

___________________________________________
total backpack area: 140
number of items: 18
total area of items: 140
total area of included items: 134
total area of excluded items: 6
List of excluded items: [ O1, O2,  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_D2_|_D2_|_D2_|_D2_|_D1_|_D1_|_D1_|_D1_|_E1_|_E1_||
||_J1_|_J1_|_J1_|_J1_|_G1_|_G1_|_G1_|_G1_|_G1_|____||
||_J1_|_J1_|_J1_|_J1_|_G1_|_G1_|_G1_|_G1_|_G1_|____||
||_L1_|_L1_|_L1_|_Q2_|_G1_|_G1_|_G1_|_G1_|_G1_|____||
||_L1_|_L1_|_L1_|_Q2_|_G1_|_G1_|_G1_|_G1_|_G1_|____||
||_L1_|_L1_|_L1_|_Q1_|_K2_|_K2_|_K1_|_K1_|_P1_|_P1_||
||_M1_|_M1_|_M1_|_Q1_|_K2_|_K2_|_K1_|_K1_|_P1_|_P1_||
||_M1_|_M1_|_M1_|_S2_|_K2_|_K2_|_K1_|_K1_|_R1_|_R1_||
||_O3_|_O3_|_O3_|_S1_|_K2_|_K2_|_K1_|_K1_|____|____||
_____________________________________________________
___________________________________________
total backpack area: 80
number of items: 15
total area of items: 80
total area of included items: 74
total area of excluded items: 6
List of excluded items: [ O1, O2,  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_G1_|_G1_|_G1_|_G1_|_G1_|_J1_|_J1_|_J1_|_J1_|____||
||_G1_|_G1_|_G1_|_G1_|_G1_|_J1_|_J1_|_J1_|_J1_|____||
||_G1_|_G1_|_G1_|_G1_|_G1_|_L1_|_L1_|_L1_|_P1_|_P1_||
||_G1_|_G1_|_G1_|_G1_|_G1_|_L1_|_L1_|_L1_|_P1_|_P1_||
||_K2_|_K2_|_K1_|_K1_|____|_L1_|_L1_|_L1_|_R1_|_R1_||
||_K2_|_K2_|_K1_|_K1_|____|_M1_|_M1_|_M1_|_Q2_|_Q1_||
||_K2_|_K2_|_K1_|_K1_|____|_M1_|_M1_|_M1_|_Q2_|_Q1_||
||_K2_|_K2_|_K1_|_K1_|____|_O3_|_O3_|_O3_|_S2_|_S1_||
_____________________________________________________

Kenshi full backpack example 2.6, attempting to isolate issue

___________________________________________
total backpack area: 40
number of items: 7
total area of items: 40
total area of included items: 40
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_G1_|_G1_|_G1_|_G1_|_G1_|_L1_|_L1_|_L1_|_P1_|_P1_||
||_G1_|_G1_|_G1_|_G1_|_G1_|_L1_|_L1_|_L1_|_P1_|_P1_||
||_G1_|_G1_|_G1_|_G1_|_G1_|_L1_|_L1_|_L1_|_R1_|_R1_||
||_G1_|_G1_|_G1_|_G1_|_G1_|_O1_|_O1_|_O1_|_S2_|_S1_||
_____________________________________________________

Kenshi full backpack example 2.7, attempting to isolate issue

___________________________________________
total backpack area: 40
number of items: 8
total area of items: 40
total area of included items: 40
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_K2_|_K2_|_K1_|_K1_|_J1_|_J1_|_J1_|_J1_|_Q2_|_Q1_||
||_K2_|_K2_|_K1_|_K1_|_J1_|_J1_|_J1_|_J1_|_Q2_|_Q1_||
||_K2_|_K2_|_K1_|_K1_|_M1_|_M1_|_M1_|_O2_|_O2_|_O2_||
||_K2_|_K2_|_K1_|_K1_|_M1_|_M1_|_M1_|_O1_|_O1_|_O1_||
_____________________________________________________

Kenshi full backpack example 3

___________________________________________
total backpack area: 140
number of items: 22
total area of items: 140
total area of included items: 120
total area of excluded items: 20
List of excluded items: [ J1, I2,  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_E1_|_E1_|_F2_|_F2_|_F2_|_F2_|_F2_|_F2_|____|____||
||_E1_|_E1_|_F1_|_F1_|_F1_|_F1_|_F1_|_F1_|____|____||
||_E1_|_E1_|_G1_|_G1_|_G1_|_G1_|_G1_|_K1_|_K1_|_Q1_||
||_E1_|_E1_|_G1_|_G1_|_G1_|_G1_|_G1_|_K1_|_K1_|_Q1_||
||_E1_|_E1_|_G1_|_G1_|_G1_|_G1_|_G1_|_K1_|_K1_|____||
||_E1_|_E1_|_G1_|_G1_|_G1_|_G1_|_G1_|_K1_|_K1_|____||
||_N1_|_N1_|_I1_|_I1_|_I1_|_I1_|____|____|____|____||
||_N1_|_N1_|_I1_|_I1_|_I1_|_I1_|____|____|____|____||
||_N1_|_N1_|_I1_|_I1_|_I1_|_I1_|____|____|____|____||
||_P1_|_P1_|_J3_|_J3_|_J3_|_J3_|_J2_|_J2_|_J2_|_J2_||
||_P1_|_P1_|_J3_|_J3_|_J3_|_J3_|_J2_|_J2_|_J2_|_J2_||
||_Q4_|_Q3_|_N2_|_N2_|_O4_|_O4_|_O4_|_O3_|_O3_|_O3_||
||_Q4_|_Q3_|_N2_|_N2_|_O2_|_O2_|_O2_|_P2_|_P2_|_Q2_||
||____|____|_N2_|_N2_|_O1_|_O1_|_O1_|_P2_|_P2_|_Q2_||
_____________________________________________________

Kenshi full backpack example 4

___________________________________________
total backpack area: 140
number of items: 19
total area of items: 140
total area of included items: 122
total area of excluded items: 18
List of excluded items: [ F1, E1,  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_F2_|_F2_|_F2_|_F2_|_F2_|_F2_|____|____|____|____||
||_G1_|_G1_|_G1_|_G1_|_G1_|_I2_|_I2_|_I2_|_I2_|____||
||_G1_|_G1_|_G1_|_G1_|_G1_|_I2_|_I2_|_I2_|_I2_|____||
||_G1_|_G1_|_G1_|_G1_|_G1_|_I2_|_I2_|_I2_|_I2_|____||
||_G1_|_G1_|_G1_|_G1_|_G1_|____|____|____|____|____||
||_I1_|_I1_|_I1_|_I1_|_J2_|_J2_|_J2_|_J2_|____|____||
||_I1_|_I1_|_I1_|_I1_|_J2_|_J2_|_J2_|_J2_|____|____||
||_I1_|_I1_|_I1_|_I1_|_O1_|_O1_|_O1_|_S1_|____|____||
||_J1_|_J1_|_J1_|_J1_|_K1_|_K1_|_N1_|_N1_|_P2_|_P2_||
||_J1_|_J1_|_J1_|_J1_|_K1_|_K1_|_N1_|_N1_|_P2_|_P2_||
||_O3_|_O3_|_O3_|_Q1_|_K1_|_K1_|_N1_|_N1_|_P1_|_P1_||
||_O2_|_O2_|_O2_|_Q1_|_K1_|_K1_|_R1_|_R1_|_P1_|_P1_||
_____________________________________________________

Kenshi full backpack example 5

___________________________________________
total backpack area: 140
number of items: 24
total area of items: 140
total area of included items: 126
total area of excluded items: 14
List of excluded items: [ J3, F1,  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_E1_|_E1_|_F2_|_F2_|_F2_|_F2_|_F2_|_F2_|____|____||
||_E1_|_E1_|_I2_|_I2_|_I2_|_I2_|_I1_|_I1_|_I1_|_I1_||
||_E1_|_E1_|_I2_|_I2_|_I2_|_I2_|_I1_|_I1_|_I1_|_I1_||
||_E1_|_E1_|_I2_|_I2_|_I2_|_I2_|_I1_|_I1_|_I1_|_I1_||
||_E1_|_E1_|_K1_|_K1_|_J2_|_J2_|_J2_|_J2_|_Q2_|_Q1_||
||_E1_|_E1_|_K1_|_K1_|_J2_|_J2_|_J2_|_J2_|_Q2_|_Q1_||
||____|____|_K1_|_K1_|_J1_|_J1_|_J1_|_J1_|_P1_|_P1_||
||____|____|_K1_|_K1_|_J1_|_J1_|_J1_|_J1_|_P1_|_P1_||
||____|____|_N1_|_N1_|_O3_|_O3_|_O3_|_O2_|_O2_|_O2_||
||____|____|_N1_|_N1_|_O1_|_O1_|_O1_|_R1_|_R1_|_S3_||
||____|____|_N1_|_N1_|_P4_|_P4_|_P3_|_P3_|_P2_|_P2_||
||____|____|_S2_|_S1_|_P4_|_P4_|_P3_|_P3_|_P2_|_P2_||
_____________________________________________________

Kenshi full backpack example 6

___________________________________________
total backpack area: 140
number of items: 23
total area of items: 140
total area of included items: 140
total area of excluded items: 0
List of excluded items: [  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_E1_|_E1_|_F1_|_F1_|_F1_|_F1_|_F1_|_F1_|_S2_|_S1_||
||_E1_|_E1_|_H1_|_H1_|_H1_|_H1_|_H1_|_O3_|_O3_|_O3_||
||_E1_|_E1_|_I2_|_I2_|_I2_|_I2_|_I1_|_I1_|_I1_|_I1_||
||_E1_|_E1_|_I2_|_I2_|_I2_|_I2_|_I1_|_I1_|_I1_|_I1_||
||_E1_|_E1_|_I2_|_I2_|_I2_|_I2_|_I1_|_I1_|_I1_|_I1_||
||_E1_|_E1_|_J3_|_J3_|_J3_|_J3_|_J2_|_J2_|_J2_|_J2_||
||_K1_|_K1_|_J3_|_J3_|_J3_|_J3_|_J2_|_J2_|_J2_|_J2_||
||_K1_|_K1_|_J1_|_J1_|_J1_|_J1_|_M1_|_M1_|_M1_|_Q3_||
||_K1_|_K1_|_J1_|_J1_|_J1_|_J1_|_M1_|_M1_|_M1_|_Q3_||
||_K1_|_K1_|_N1_|_N1_|_O2_|_O2_|_O2_|_O1_|_O1_|_O1_||
||_Q2_|_Q1_|_N1_|_N1_|_P3_|_P3_|_P2_|_P2_|_P1_|_P1_||
||_Q2_|_Q1_|_N1_|_N1_|_P3_|_P3_|_P2_|_P2_|_P1_|_P1_||
_____________________________________________________

Kenshi full backpack example 7

___________________________________________
total backpack area: 140
number of items: 25
total area of items: 140
total area of included items: 135
total area of excluded items: 5
List of excluded items: [ H1,  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_C1_|_C1_|_C1_|_C1_|_C1_|_C1_|_P1_|_P1_|_Q3_|_Q2_||
||_C1_|_C1_|_C1_|_C1_|_C1_|_C1_|_P1_|_P1_|_Q3_|_Q2_||
||_E1_|_E1_|_F1_|_F1_|_F1_|_F1_|_F1_|_F1_|____|____||
||_E1_|_E1_|_I1_|_I1_|_I1_|_I1_|_J2_|_J2_|_J2_|_J2_||
||_E1_|_E1_|_I1_|_I1_|_I1_|_I1_|_J2_|_J2_|_J2_|_J2_||
||_E1_|_E1_|_I1_|_I1_|_I1_|_I1_|_O4_|_O4_|_O4_|____||
||_E1_|_E1_|_J1_|_J1_|_J1_|_J1_|_M1_|_M1_|_M1_|_Q1_||
||_E1_|_E1_|_J1_|_J1_|_J1_|_J1_|_M1_|_M1_|_M1_|_Q1_||
||_K2_|_K2_|_N1_|_N1_|_O3_|_O3_|_O3_|_O2_|_O2_|_O2_||
||_K2_|_K2_|_N1_|_N1_|_O1_|_O1_|_O1_|_S3_|_S2_|_S1_||
||_K2_|_K2_|_N1_|_N1_|_P4_|_P4_|_P3_|_P3_|_P2_|_P2_||
||_K2_|_K2_|____|____|_P4_|_P4_|_P3_|_P3_|_P2_|_P2_||
_____________________________________________________

Kenshi full backpack example 8

___________________________________________
total backpack area: 140
number of items: 24
total area of items: 140
total area of included items: 127
total area of excluded items: 13
List of excluded items: [ K1, H1,  ]
___________________________________________

Backpack View
_____________________________________________________
_____________________________________________________
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_|_A1_||
||_B1_|_B1_|_B1_|_B1_|_B1_|_B1_|_B1_|_B1_|____|____||
||_B1_|_B1_|_B1_|_B1_|_B1_|_B1_|_B1_|_B1_|____|____||
||_C1_|_C1_|_C1_|_C1_|_C1_|_C1_|_Q3_|_Q2_|_Q1_|_S2_||
||_C1_|_C1_|_C1_|_C1_|_C1_|_C1_|_Q3_|_Q2_|_Q1_|_S1_||
||_I1_|_I1_|_I1_|_I1_|_J2_|_J2_|_J2_|_J2_|_P1_|_P1_||
||_I1_|_I1_|_I1_|_I1_|_J2_|_J2_|_J2_|_J2_|_P1_|_P1_||
||_I1_|_I1_|_I1_|_I1_|____|____|____|____|_R1_|_R1_||
||_J1_|_J1_|_J1_|_J1_|_M1_|_M1_|_M1_|_O3_|_O3_|_O3_||
||_J1_|_J1_|_J1_|_J1_|_M1_|_M1_|_M1_|_O2_|_O2_|_O2_||
||_N1_|_N1_|_O1_|_O1_|_O1_|____|____|____|____|____||
||_N1_|_N1_|_P5_|_P5_|_P4_|_P4_|_P3_|_P3_|_P2_|_P2_||
||_N1_|_N1_|_P5_|_P5_|_P4_|_P4_|_P3_|_P3_|_P2_|_P2_||
_____________________________________________________
