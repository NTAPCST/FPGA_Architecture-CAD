#include <iostream>
#include <string>
#include "lib.h"

Placement::Placement(){
    this->_piVec.push_back(Pin(std::make_pair(0, 0)));
    this->_poVec.push_back(Pin(std::make_pair(0, 0)));
    Module tmp(0, 1);
    this->_moduleVec.push_back(&tmp);
};

void Placement::createCLB(){
    this->_clbInfo.resize(this->_clbCol + 1);
    for(int i = 1; i < _clbInfo.size(); ++i){
        _clbInfo[i].resize(this->_clbRow + 1);
    }
}

void Placement::setCol(int c){
    this->_clbCol = c;
};


void Placement::setRow(int r){
    this->_clbRow = r;
};

void Placement::setHPin(int h){
    this->_horizontalPin = h;
};

void Placement::setVPin(int v){
    this->_verticalPin = v;
};

void Placement::addPI(Pin p){
    this->_piVec.push_back(p);
};

void Placement::addPO(Pin p){
    this->_poVec.push_back(p);
};

void Placement::setLUT(int L){
    this->_numLUT = L;
};

void Placement::setFF(int F){
    this->_numFF = F;
};

void Placement::initialPlace(){
    srand(426612);
    int rx, ry;
    for(int i = 1; i <= this->_numLUT; ++i){
        rx = rand() % this->_clbCol + 1;
        ry = rand() % this->_clbRow + 1;
        while (this->getclbInfo(rx, ry, 0) >= 2){
            rx = rand() % this->_clbCol + 1;
            ry = rand() % this->_clbRow + 1;
        }
        this->_moduleVec[i]->setCoordinate(std::make_pair(rx, ry));
        this->setclb(rx, ry, 0);
    }
    
    
    for(int i = 1; i <= this->_numFF; ++i){
        rx = rand() % this->_clbCol + 1;
        ry = rand() % this->_clbRow + 1;
        while (this->getclbInfo(rx, ry, 1) >= 2){
            rx = rand() % this->_clbCol + 1;
            ry = rand() % this->_clbRow + 1;
        }
        this->_moduleVec[this->_numLUT + i]->setCoordinate(std::make_pair(rx, ry));
        this->setclb(rx, ry, 1);
    }
};

void Placement::swapModule(){
    int idx1, idx2;
    int x1, y1;
    int HPWL = this->getHPWL();
    int newHPWL = 0;
    idx1 = rand() % (this->_numLUT + this->_numFF) + 1;
    idx2 = rand() % (this->_numLUT + this->_numFF) + 1;
    Module *m1 = this->_moduleVec[idx1], *m2 = this->_moduleVec[idx2];
    
    if(m1->getType() == m2->getType()){
        x1 = m1->getCoordinate().first;
        y1 = m1->getCoordinate().second;
        m1->setCoordinate(std::make_pair(m2->getCoordinate().first, m2->getCoordinate().second));
        m2->setCoordinate(std::make_pair(x1, y1));
        newHPWL = this->getHPWL();
    }

    if(newHPWL <= HPWL){
        HPWL = newHPWL;
        //std::cout << "Accept swap! HPWL: " << HPWL << "\n";
    }
    else{
        m2->setCoordinate(std::make_pair(m1->getCoordinate().first, m1->getCoordinate().second));
        m1->setCoordinate(std::make_pair(x1, y1));
    }
}

void Placement::moveModule(){
    int idx;
    int HPWL = this->getHPWL();
    int newHPWL = 0;
    idx = rand() % (this->_numLUT + this->_numFF) + 1;
    Module *m = this->_moduleVec[idx];
    double old_coord1 = m->getCoordinate().first;
    double old_coord2 = m->getCoordinate().second;

    double coord1 = rand() % this->_clbCol + 1;
    double coord2 = rand() % this->_clbRow + 1;

    while (this->getclbInfo(coord1, coord2, m->getType()) >= 2){
        coord1 = rand() % this->_clbCol + 1;
        coord2 = rand() % this->_clbRow + 1;
    }
    this->subclb(old_coord1, old_coord2, m->getType());
    this->_moduleVec[idx]->setCoordinate(std::make_pair(coord1, coord2));
    this->setclb(coord1, coord2, m->getType());
    newHPWL = this->getHPWL();

    //recovery
    if(newHPWL <= HPWL){
        HPWL = newHPWL;
        //std::cout << "Accept move, HPWL: " << HPWL << "\n";
    }
    else{
        this->setclb(old_coord1, old_coord2, m->getType());
        this->_moduleVec[idx]->setCoordinate(std::make_pair(old_coord1, old_coord2));
        this->subclb(coord1, coord2, m->getType());
    }
}

Module* Placement::getmodule(int id){
    return this->_moduleVec[id];
};

void Placement::addmodule(Module *m){
    this->_moduleVec.push_back(m);
};

int Placement::getnumLUT(){
    return this->_numLUT;
};

int Placement::getnumFF(){
    return this->_numFF;
};

double Placement::getHPWL(){
    double HPWL = 0;
    double x_min, x_MAX, y_min, y_MAX;
    for(int i = 1; i <= this->numnet; ++i){
        x_min = 1000000, x_MAX = 0, y_min = 1000000, y_MAX = 0;
        std::vector<Module*> modvec = this->_netVec[i].getModule();
        std::vector<Pin> pinvec = this->_netVec[i].getPin();
        for(auto p:pinvec){
            x_min = std::min(p.getCoordinate().first, x_min);
            x_MAX = std::max(p.getCoordinate().first, x_MAX);
            y_min = std::min(p.getCoordinate().second, y_min);
            y_MAX = std::max(p.getCoordinate().second, y_MAX);
        }
        for(auto m:modvec){
            x_min = std::min(m->getCoordinate().first, x_min);
            x_MAX = std::max(m->getCoordinate().first, x_MAX);
            y_min = std::min(m->getCoordinate().second, y_min);
            y_MAX = std::max(m->getCoordinate().second, y_MAX);
        }
        HPWL += (x_MAX - x_min) + (y_MAX - y_min);
    }
    return HPWL;
};

void Placement::setclb(int x, int y, bool type){
    if(!type)
        this->_clbInfo[x][y].first += 1;
    else{
        this->_clbInfo[x][y].second += 1;
    }
}

void Placement::subclb(int x, int y, bool type){
    if(!type)
        this->_clbInfo[x][y].first -= 1;
    else{
        this->_clbInfo[x][y].second -= 1;
    }
}

int Placement::getclbInfo(int x, int y, bool type){
    if(!type)
        return this->_clbInfo[x][y].first;
    else
        return this->_clbInfo[x][y].second;
}

int Placement::getclbRow(){
    return this->_clbRow;
}

int Placement::getclbCol(){
    return this->_clbCol;
}

Pin::Pin(std::pair<double, double> coordinate){
    this->_cooridinate = coordinate;
};

std::pair<double, double> Pin::getCoordinate(){
    return this->_cooridinate;
}

Module::Module(int name, bool type){
    this->_name = name;
    this->_type = type;
};

void Module::setType(bool type){
    this->_type = type;
};

void Module::setCoordinate(std::pair<double, double> coordinate){
    this->_cooridinate = coordinate;
};

std::pair<double, double> Module::getCoordinate(){
    return this->_cooridinate;
};

bool Module::getType(){
    return this->_type;
}

int Module::getName(){
    return this->_name;
}

Net::Net(){

};

void Net::addModule(Module *m){
    this->_ModuleVec.push_back(m);
};

void Net::addPin(Pin p){
    this->_pinVec.push_back(p);
};

std::vector<Module*> Net::getModule(){
    return this->_ModuleVec;
}

std::vector<Pin> Net::getPin(){
    return this->_pinVec;
}