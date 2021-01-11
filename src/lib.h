#include <string>
#include <vector>
#include <utility>

class Pin{
    public:
        Pin(std::pair<double, double> coordinate);
        std::pair<double, double> getCoordinate();
    private:
        std::pair<double, double> _cooridinate;
};

class Module{
    public:
        Module(int name, bool type);
        void setType(bool type);
        std::pair<double, double> getCoordinate();
        void setCoordinate(std::pair<double, double> coordinate);
        bool getType();
        int getName();

    private:
        int _name;
        bool _type;
        std::pair<int, int> _cooridinate;
};

class Net{
    public:
        Net();
        void addModule(Module*);
        void addPin(Pin);
        std::vector<Module*> getModule();
        std::vector<Pin> getPin();
    private:
        std::vector<Module*> _ModuleVec;
        std::vector<Pin> _pinVec;
};

class Placement{
    public:
        Placement();
        void setCol(int C);
        void setRow(int R);
        void setHPin(int h);
        void setVPin(int v);
        void setLUT(int L);
        void setFF(int F);
        void addPI(Pin p);
        void addPO(Pin p);
        int getnumLUT();
        int getnumFF();
        int getclbRow();
        int getclbCol();
        std::vector<Pin> _piVec;
        std::vector<Pin> _poVec;
        std::vector<Net> _netVec;
        int numnet;
        void addmodule(Module *m);
        Module* getmodule(int id);
        double getHPWL();
        void createCLB();
        int getclbInfo(int x, int y, bool type);
        void setclb(int x, int y, bool type);
        void subclb(int x, int y, bool type);

        void initialPlace();
        void swapModule();
        void moveModule();

    private:
        int _clbCol;
        int _clbRow;
        int _horizontalPin;
        int _verticalPin;
        int _numPI;
        int _numPO;
        int _numLUT;
        int _numFF;
        std::vector<Module*> _moduleVec;
        std::vector<std::vector<std::pair<int, int> > > _clbInfo;
        //std::vector<Pin> _piVec;
        //std::vector<Pin> _poVec;
};
