#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <utility>
#include <sstream>
#include <ctime>
#include <chrono>
#include "lib.h"

using namespace std;
//I/O prototype
void parser(char *argv[], Placement& placement);
void write_file(char *argv[], Placement& placement);

int main(int argc, char *argv[]){
    time_t start;
    chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
    start = clock();
    Placement placement;
    parser(argv, placement);
    placement.createCLB();
    placement.initialPlace();
    cout << "initial HPWL: " << placement.getHPWL() << "\n";

    //greedy-random place
    srand(7122);
    bool peturb_type = 0;
    //1680 can be replaced by any time slice for greedy-random swap/move
    while (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - t1).count() < 1680){
        for(int i = 0; i < 1000; ++i){
            peturb_type = rand() % 2;
            if(peturb_type)
                placement.moveModule();
            else
                placement.swapModule();
        }
    }
    cout << "final HPWL: " << placement.getHPWL() << "\n";
    write_file(argv, placement);
    cout << "time(chrono): " << chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - t1).count() << "\n";

} 

void parser(char *argv[], Placement& placement){
    // read *.info file
    fstream info_file(argv[1], ifstream::in);
    string skip_string;
    char skip_char;
    int C, R, Q, P, PI, PO, num_LUT, num_FF;
    info_file >> skip_string >> C >> R;
    placement.setCol(C);
    placement.setRow(R);

    info_file >> skip_string >> Q >> P;
    placement.setHPin(Q);
    placement.setVPin(P);

    info_file >> skip_string >> PI;
    double x, y;
    for(int i = 0; i < PI; ++i){
        info_file >> skip_string >> x >> y;
        placement.addPI(Pin(make_pair(x, y)));
    }

    info_file >> skip_string >> PO;
    for(int i = 0; i < PO; ++i){
        info_file >> skip_string >> x >> y;
        placement.addPO(Pin(make_pair(x, y)));
    }
    info_file >> skip_string >> num_LUT >> num_FF;
    placement.setLUT(num_LUT);
    placement.setFF(num_FF);
    for(int i = 1; i <= placement.getnumLUT(); ++i){
        Module *tmp_mod = new Module(i, 0);
        placement.addmodule(tmp_mod);
    }
    for(int i = 1; i <= placement.getnumFF(); ++i){
        Module *tmp_mod = new Module(i, 1);
        placement.addmodule(tmp_mod);
    }
    info_file.close();

    //read *.net file
    fstream net_file(argv[2], ifstream::in);
    stringstream ss;
    string line;
    int pinID, moduleID;
    while (getline(net_file, line)){
        ss << line;
        ss >> placement.numnet;
        ss.str("");
        ss.clear();
        placement._netVec.resize(placement.numnet + 1);
        for(int i = 1; i <= placement.numnet; ++i){
            string tmp_module;
            getline(net_file, line);
            ss << line;
            ss >> tmp_module;
            while (ss >> tmp_module){
                if(tmp_module[0] == 'I'){
                    pinID = stoi(tmp_module.substr(tmp_module.find_first_of("I", 0) + 1, tmp_module.size()));
                    placement._netVec[i].addPin(placement._piVec[pinID]);
                }
                else if(tmp_module[0] == 'L'){
                    moduleID = stoi(tmp_module.substr(tmp_module.find_first_of("L", 0) + 1, tmp_module.size()));
                    placement._netVec[i].addModule(placement.getmodule(moduleID));
                }
                else if(tmp_module[0] == 'O'){
                    pinID = stoi(tmp_module.substr(tmp_module.find_first_of("O", 0) + 1, tmp_module.size()));
                    placement._netVec[i].addPin(placement._poVec[pinID]);
                }
                else if(tmp_module[0] == 'F'){
                    moduleID = stoi(tmp_module.substr(tmp_module.find_first_of("F", 0) + 1, tmp_module.size()));
                    placement._netVec[i].addModule(placement.getmodule(moduleID + placement.getnumLUT()));
                }
            }
            ss.str("");
            ss.clear();
        }
    }
    net_file.close();
}
void write_file(char *argv[], Placement& placement){
    ofstream out_file(argv[3]);
    for(int i = 1; i <= placement.getnumLUT(); ++i){
        out_file << "L" << i << " " << static_cast<int>(placement.getmodule(i)->getCoordinate().first) << " " << static_cast<int>(placement.getmodule(i)->getCoordinate().second) << "\n";
    }
    for(int i = 1; i <= placement.getnumFF(); ++i){
        out_file << "F" << i << " " << static_cast<int>(placement.getmodule(placement.getnumLUT() + i)->getCoordinate().first) << " " << static_cast<int>(placement.getmodule(placement.getnumLUT() + i)->getCoordinate().second) << "\n";
    }
    out_file.close();
}