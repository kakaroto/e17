#include <iostream>
using namespace std;

#include <sigc++/sigc++.h>

class Test : public sigc::trackable
{
    public:
        void print0() {
            cout << "print0" << endl;
        }
        void print1(int a) {
            cout << "print1 " << a << endl;
        }
        void print2(int a, int b) {
            cout << "print2 " << a << ", " << b << endl;
        }

};

typedef sigc::signal<void> VoidSignal;

void connectSomething( VoidSignal* signal, const sigc::slot0<void>& slot )
{
    signal->connect( slot );
}

int main(int argc, char **argv, char **envv)
{
    Test test;
    VoidSignal* sig = new VoidSignal();
    connectSomething( sig, sigc::mem_fun(&test, &Test::print0) );

    sig->emit();

    return 0;
}
