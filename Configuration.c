const int iSectorCount = 25;       //Navigation resolution. MUST BE uneven 9,11,13,15,17,19,21 ect. biger count = more accuracy, but also more suceptible to noise

const short syncEdgePositive = 1; //new scan triggered by falling or rising edge
const int iScanDisplayThickness = 10; //how thick the scanlines should be
int DrawView = 0; //change with buttons, shows different graphs
float iDistanceDispScale = 200 / 64;
