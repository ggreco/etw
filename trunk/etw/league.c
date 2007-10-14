#include "etw_locale.h"
#include "menu.h"

struct Match camp4[3][2]={
    {
        {1,2},
        {3,4},
    },
    {
        {1,3},
        {2,4},
    },
    {
        {2,3},
        {1,4},
    },
};
struct Match camp6[5][3]={
    {
        {1,2},
        {3,4},
        {5,6},
    },
    {
        {1,3},
        {2,5},
        {4,6},
    },
    {
        {2,3},
        {4,5},
        {1,6},
    },
    {
        {1,4},
        {3,5},
        {2,6},
    },
    {
        {2,4},
        {1,5},
        {3,6},
    },
};
struct Match camp8[7][4]={
    {
        {1,2},
        {3,4},
        {5,6},
        {7,8},
    },
    {
        {1,3},
        {2,4},
        {5,7},
        {6,8},
    },
    {
        {2,3},
        {1,4},
        {6,7},
        {5,8},
    },
    {
        {1,5},
        {2,6},
        {3,7},
        {4,8},
    },
    {
        {2,5},
        {1,6},
        {4,7},
        {3,8},
    },
    {
        {3,5},
        {4,6},
        {1,7},
        {2,8},
    },
    {
        {4,5},
        {3,6},
        {2,7},
        {1,8},
    },
};
struct Match camp10[9][5]={
    {
        {1,2},
        {3,4},
        {5,6},
        {7,8},
        {9,10},
    },
    {
        {1,3},
        {2,4},
        {5,7},
        {6,9},
        {8,10},
    },
    {
        {2,3},
        {1,4},
        {6,7},
        {8,9},
        {5,10},
    },
    {
        {1,5},
        {2,6},
        {3,8},
        {4,9},
        {7,10},
    },
    {
        {2,5},
        {1,6},
        {4,8},
        {7,9},
        {3,10},
    },
    {
        {3,5},
        {1,7},
        {6,8},
        {2,9},
        {4,10},
    },
    {
        {4,5},
        {2,7},
        {1,8},
        {3,9},
        {6,10},
    },
    {
        {3,6},
        {4,7},
        {2,8},
        {5,9},
        {1,10},
    },
    {
        {4,6},
        {3,7},
        {5,8},
        {1,9},
        {2,10},
    },
};
struct Match camp12[11][6]={
    {
        {1,2},
        {3,4},
        {5,6},
        {7,8},
        {9,10},
        {11,12},
    },
    {
        {1,3},
        {2,4},
        {5,7},
        {6,8},
        {9,11},
        {10,12},
    },
    {
        {2,3},
        {1,4},
        {6,7},
        {5,8},
        {10,11},
        {9,12},
    },
    {
        {1,5},
        {2,6},
        {3,9},
        {4,10},
        {7,11},
        {8,12},
    },
    {
        {2,5},
        {1,6},
        {4,9},
        {3,10},
        {8,11},
        {7,12},
    },
    {
        {3,5},
        {4,6},
        {7,9},
        {8,10},
        {1,11},
        {2,12},
    },
    {
        {4,5},
        {3,6},
        {8,9},
        {7,10},
        {2,11},
        {1,12},
    },
    {
        {1,7},
        {2,8},
        {5,9},
        {6,10},
        {3,11},
        {4,12},
    },
    {
        {2,7},
        {1,8},
        {6,9},
        {5,10},
        {4,11},
        {3,12},
    },
    {
        {3,7},
        {4,8},
        {1,9},
        {2,10},
        {5,11},
        {6,12},
    },
    {
        {4,7},
        {3,8},
        {2,9},
        {1,10},
        {6,11},
        {5,12},
    },
};
struct Match camp14[13][7]={
    {
        {1,2},
        {3,4},
        {5,6},
        {7,8},
        {9,10},
        {11,12},
        {13,14},
    },
    {
        {1,3},
        {2,4},
        {5,7},
        {6,8},
        {9,11},
        {10,13},
        {12,14},
    },
    {
        {2,3},
        {1,4},
        {6,7},
        {5,8},
        {10,11},
        {12,13},
        {9,14},
    },
    {
        {1,5},
        {2,6},
        {3,7},
        {4,8},
        {9,12},
        {11,13},
        {10,14},
    },
    {
        {2,5},
        {1,6},
        {4,7},
        {3,8},
        {10,12},
        {9,13},
        {11,14},
    },
    {
        {3,5},
        {1,9},
        {2,10},
        {4,11},
        {6,12},
        {7,13},
        {8,14},
    },
    {
        {4,5},
        {2,9},
        {1,10},
        {3,11},
        {7,12},
        {8,13},
        {6,14},
    },
    {
        {3,6},
        {4,9},
        {5,10},
        {1,11},
        {8,12},
        {2,13},
        {7,14},
    },
    {
        {4,6},
        {3,9},
        {7,10},
        {8,11},
        {1,12},
        {5,13},
        {2,14},
    },
    {
        {1,7},
        {5,9},
        {8,10},
        {2,11},
        {3,12},
        {6,13},
        {4,14},
    },
    {
        {2,7},
        {8,9},
        {3,10},
        {6,11},
        {4,12},
        {1,13},
        {5,14},
    },
    {
        {1,8},
        {7,9},
        {6,10},
        {5,11},
        {2,12},
        {4,13},
        {3,14},
    },
    {
        {2,8},
        {6,9},
        {4,10},
        {7,11},
        {5,12},
        {3,13},
        {1,14},
    },
};
struct Match camp16[15][8]={
    {
        {1,2},
        {3,4},
        {5,6},
        {7,8},
        {9,10},
        {11,12},
        {13,14},
        {15,16},
    },
    {
        {1,3},
        {2,4},
        {5,7},
        {6,8},
        {9,11},
        {10,12},
        {13,15},
        {14,16},
    },
    {
        {2,3},
        {1,4},
        {6,7},
        {5,8},
        {10,11},
        {9,12},
        {14,15},
        {13,16},
    },
    {
        {1,5},
        {2,6},
        {3,7},
        {4,8},
        {9,13},
        {10,14},
        {11,15},
        {12,16},
    },
    {
        {2,5},
        {1,6},
        {4,7},
        {3,8},
        {10,13},
        {9,14},
        {12,15},
        {11,16},
    },
    {
        {3,5},
        {4,6},
        {1,7},
        {2,8},
        {11,13},
        {12,14},
        {9,15},
        {10,16},
    },
    {
        {4,5},
        {3,6},
        {2,7},
        {1,8},
        {12,13},
        {11,14},
        {10,15},
        {9,16},
    },
    {
        {1,9},
        {2,10},
        {3,11},
        {4,12},
        {5,13},
        {6,14},
        {7,15},
        {8,16},
    },
    {
        {2,9},
        {1,10},
        {4,11},
        {3,12},
        {6,13},
        {5,14},
        {8,15},
        {7,16},
    },
    {
        {3,9},
        {4,10},
        {1,11},
        {2,12},
        {7,13},
        {8,14},
        {5,15},
        {6,16},
    },
    {
        {4,9},
        {3,10},
        {2,11},
        {1,12},
        {8,13},
        {7,14},
        {6,15},
        {5,16},
    },
    {
        {5,9},
        {6,10},
        {7,11},
        {8,12},
        {1,13},
        {2,14},
        {3,15},
        {4,16},
    },
    {
        {6,9},
        {5,10},
        {8,11},
        {7,12},
        {2,13},
        {1,14},
        {4,15},
        {3,16},
    },
    {
        {7,9},
        {8,10},
        {5,11},
        {6,12},
        {3,13},
        {4,14},
        {1,15},
        {2,16},
    },
    {
        {8,9},
        {7,10},
        {6,11},
        {5,12},
        {4,13},
        {3,14},
        {2,15},
        {1,16},
    },
};
struct Match camp18[17][9]={
    {
        {1,2},
        {3,4},
        {5,6},
        {7,8},
        {9,10},
        {11,12},
        {13,14},
        {15,16},
        {17,18},
    },
    {
        {1,3},
        {2,4},
        {5,7},
        {6,8},
        {9,11},
        {10,12},
        {13,15},
        {14,17},
        {16,18},
    },
    {
        {2,3},
        {1,4},
        {6,7},
        {5,8},
        {10,11},
        {9,12},
        {14,15},
        {16,17},
        {13,18},
    },
    {
        {1,5},
        {2,6},
        {3,7},
        {4,8},
        {9,13},
        {10,14},
        {11,16},
        {12,17},
        {15,18},
    },
    {
        {2,5},
        {1,6},
        {4,7},
        {3,8},
        {10,13},
        {9,14},
        {12,16},
        {15,17},
        {11,18},
    },
    {
        {3,5},
        {4,6},
        {1,7},
        {2,8},
        {11,13},
        {9,15},
        {14,16},
        {10,17},
        {12,18},
    },
    {
        {4,5},
        {3,6},
        {2,7},
        {1,8},
        {12,13},
        {10,15},
        {9,16},
        {11,17},
        {14,18},
    },
    {
        {1,9},
        {2,10},
        {3,11},
        {4,12},
        {5,14},
        {6,15},
        {7,16},
        {13,17},
        {8,18},
    },
    {
        {2,9},
        {1,10},
        {4,11},
        {3,12},
        {6,14},
        {5,15},
        {13,16},
        {8,17},
        {7,18},
    },
    {
        {3,9},
        {4,10},
        {1,11},
        {2,13},
        {7,14},
        {12,15},
        {8,16},
        {5,17},
        {6,18},
    },
    {
        {4,9},
        {3,10},
        {2,11},
        {1,13},
        {12,14},
        {8,15},
        {6,16},
        {7,17},
        {5,18},
    },
    {
        {5,9},
        {6,10},
        {1,12},
        {7,13},
        {8,14},
        {11,15},
        {2,16},
        {3,17},
        {4,18},
    },
    {
        {6,9},
        {5,10},
        {2,12},
        {8,13},
        {11,14},
        {7,15},
        {1,16},
        {4,17},
        {3,18},
    },
    {
        {7,9},
        {5,11},
        {8,12},
        {3,13},
        {1,14},
        {2,15},
        {4,16},
        {6,17},
        {10,18},
    },
    {
        {8,9},
        {6,11},
        {7,12},
        {5,13},
        {3,14},
        {4,15},
        {10,16},
        {1,17},
        {2,18},
    },
    {
        {7,10},
        {8,11},
        {5,12},
        {6,13},
        {4,14},
        {1,15},
        {3,16},
        {2,17},
        {9,18},
    },
    {
        {8,10},
        {7,11},
        {6,12},
        {4,13},
        {2,14},
        {3,15},
        {5,16},
        {9,17},
        {1,18},
    },
};
struct Match camp20[19][10]={
    {
        {1,2},
        {3,4},
        {5,6},
        {7,8},
        {9,10},
        {11,12},
        {13,14},
        {15,16},
        {17,18},
        {19,20},
    },
    {
        {1,3},
        {2,4},
        {5,7},
        {6,8},
        {9,11},
        {10,12},
        {13,15},
        {14,16},
        {17,19},
        {18,20},
    },
    {
        {2,3},
        {1,4},
        {6,7},
        {5,8},
        {10,11},
        {9,12},
        {14,15},
        {13,16},
        {18,19},
        {17,20},
    },
    {
        {1,5},
        {2,6},
        {3,7},
        {4,8},
        {9,13},
        {10,14},
        {11,17},
        {12,18},
        {15,19},
        {16,20},
    },
    {
        {2,5},
        {1,6},
        {4,7},
        {3,8},
        {10,13},
        {9,14},
        {12,17},
        {11,18},
        {16,19},
        {15,20},
    },
    {
        {3,5},
        {4,6},
        {1,7},
        {2,8},
        {11,13},
        {12,14},
        {15,17},
        {16,18},
        {9,19},
        {10,20},
    },
    {
        {4,5},
        {3,6},
        {2,7},
        {1,8},
        {12,13},
        {11,14},
        {16,17},
        {15,18},
        {10,19},
        {9,20},
    },
    {
        {1,9},
        {2,10},
        {3,11},
        {4,12},
        {5,15},
        {6,16},
        {7,17},
        {8,18},
        {13,19},
        {14,20},
    },
    {
        {2,9},
        {1,10},
        {4,11},
        {3,12},
        {6,15},
        {5,16},
        {8,17},
        {7,18},
        {14,19},
        {13,20},
    },
    {
        {3,9},
        {4,10},
        {1,11},
        {2,12},
        {7,15},
        {8,16},
        {13,17},
        {14,18},
        {5,19},
        {6,20},
    },
    {
        {4,9},
        {3,10},
        {2,11},
        {1,12},
        {8,15},
        {7,16},
        {14,17},
        {13,18},
        {6,19},
        {5,20},
    },
    {
        {5,9},
        {6,10},
        {1,13},
        {2,14},
        {11,15},
        {12,16},
        {3,17},
        {4,18},
        {7,19},
        {8,20},
    },
    {
        {6,9},
        {5,10},
        {2,13},
        {1,14},
        {12,15},
        {11,16},
        {4,17},
        {3,18},
        {8,19},
        {7,20},
    },
    {
        {7,9},
        {8,10},
        {3,13},
        {4,14},
        {1,15},
        {2,16},
        {5,17},
        {6,18},
        {11,19},
        {12,20},
    },
    {
        {8,9},
        {7,10},
        {4,13},
        {3,14},
        {2,15},
        {1,16},
        {6,17},
        {5,18},
        {12,19},
        {11,20},
    },
    {
        {5,11},
        {6,12},
        {7,13},
        {8,14},
        {3,15},
        {4,16},
        {9,17},
        {10,18},
        {1,19},
        {2,20},
    },
    {
        {6,11},
        {5,12},
        {8,13},
        {7,14},
        {4,15},
        {3,16},
        {10,17},
        {9,18},
        {2,19},
        {1,20},
    },
    {
        {7,11},
        {8,12},
        {5,13},
        {6,14},
        {9,15},
        {10,16},
        {1,17},
        {2,18},
        {3,19},
        {4,20},
    },
    {
        {8,11},
        {7,12},
        {6,13},
        {5,14},
        {10,15},
        {9,16},
        {2,17},
        {1,18},
        {4,19},
        {3,20},
    },
};

/*
struct Match **camps[]={
    &camp4,
    &camp6,
    &camp8,
    &camp10,
    &camp12,
    &camp14,
    &camp16,
    &camp18,
    &camp20,
};
*/
