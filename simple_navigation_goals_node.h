#include <stdio.h>
#include <vector>
#include <fstream>
#include <iomanip>
#include <sstream>


using namespace std;


class readData{
public:
    readData(string filename);
    vector<vector<double> > allDataPointsVec;
    vector<double> time;
    vector<double> fb_joy;
    vector<double> lr_joy;
    vector<double> linear_vel;
    vector<double> angular_vel;

};
