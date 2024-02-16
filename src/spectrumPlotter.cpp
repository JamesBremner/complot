#include <sstream>
#include <numeric>
#include "com.h"
#include "plot2d.h"

class cGUI
{
public:
    cGUI();
    void run();

private:
    // widgets
    wex::gui &myForm;
    wex::label &myPortlb;
    wex::editbox &myPorteb;
    wex::label &myFreqlb;
    wex::editbox &myFreqeb;
    wex::button &myConnectbn;
    wex::label &myDatalb;
    wex::choice &myDatach;

    // plot
    wex::plot::plot &thePlot;
    wex::plot::trace &t1;
    wex::timer *myPlotTimer;

    // COM interface
    wex::com &myTalker;

    // data
    int myDataCount;                   // number of frequencies in spectra
    int myDataLength;                  // number of bytes in each spectrum data point
    std::vector<double> myRecentDbl;   // the most recently arrived data
    std::vector<uint16_t> myRecentU16; // the most recently arrived data

    // event handlers
    void ConstructPlot();
    void COMReadHandler();
    void COMConnectHandler();
    void PlotTimerHandler();

    void debugText();
};

cGUI::cGUI()
    : myForm(wex::maker::make()),
      myPortlb(wex::maker::make<wex::label>(myForm)), myPorteb(wex::maker::make<wex::editbox>(myForm)),
      myFreqlb(wex::maker::make<wex::label>(myForm)), myFreqeb(wex::maker::make<wex::editbox>(myForm)),
      myDatalb(wex::maker::make<wex::label>(myForm)), myDatach(wex::maker::make<wex::choice>(myForm)),
      myConnectbn(wex::maker::make<wex::button>(myForm)), myTalker(wex::maker::make<wex::com>(myForm)),
      thePlot(wex::maker::make<wex::plot::plot>(myForm)),
      t1(thePlot.AddStaticTrace()),
      myDataLength(8)

{
    myForm.move(50, 50, 850, 500);
    myForm.text("Spectrum Plotter");

    myPortlb.move(20, 20, 100, 30);
    myPortlb.text("COM Port");
    myPorteb.move(140, 20, 50, 30);
    myPorteb.text("9");
    myConnectbn.move(210, 20, 100, 30);
    myConnectbn.bgcolor(0x9090FF);
    myConnectbn.text("Connect");

    myFreqlb.move(350, 20, 120, 30);
    myFreqlb.text("Freq Count");
    myFreqeb.move(450, 20, 50, 30);
    myFreqeb.text("100");

    myDatalb.move(530, 20, 90, 30);
    myDatalb.text("Data");
    myDatach.move(600, 20, 200, 30);
    myDatach.add("Double");
    myDatach.select(0);
    myDatach.add("unsigned 16 bit integer");
    myDatach.tooltip("Data points");

    ConstructPlot();

    myConnectbn.events().click(
        [&]
        {
            COMConnectHandler();
        });

    myForm.events().asyncReadComplete(
        [this](int id)
        {
            COMReadHandler();
        });
    myForm.events().timer(
        [this](int id)
        {
            PlotTimerHandler();
        });

    myForm.events().resize(
        [this](int w, int h)
        {
            thePlot.size(w - 100, h - 200);
            thePlot.move(30, 100);
            thePlot.update();
        });

    myForm.show();
}
void cGUI::run()
{
    myForm.run();
}

void cGUI::ConstructPlot()
{
    // black background
    thePlot.bgcolor(0);

    // plot in blue
    t1.color(0xFF0000);

    // set up y-axis
    thePlot.axisYminmax(0, 30);

    // Update the plot 2 times/second
    myPlotTimer = new wex::timer(myForm, 500);
}

void cGUI::COMReadHandler()
{
    // store received data
    switch (myDataLength)
    {
    case 8:
        memcpy(
            myRecentDbl.data(),
            myTalker.readData().data(),
            myDataCount * myDataLength);
        break;
    case 2:
        memcpy(
            myRecentU16.data(),
            myTalker.readData().data(),
            myDataCount * myDataLength);
        break;
    }

    // wait for next
    myTalker.read_async(myDataCount * myDataLength);
}

void cGUI::COMConnectHandler()
{
    // open the port
    myTalker.port(myPorteb.text());
    myTalker.open();
    if (!myTalker.isOpen())
    {
        wex::msgbox mb("Open failed");
        exit(1);
    }

    // display successful open
    myConnectbn.text("Connected");
    myConnectbn.bgcolor(0x90FF90);
    myConnectbn.enable(false);
    myConnectbn.update();

    // data storage
    myDataCount = atoi(myFreqeb.text().c_str());
    myRecentDbl.resize(myDataCount, 0);
    myRecentU16.resize(myDataCount, 0);
    switch (myDatach.selectedIndex())
    {
    case 0:
        myDataLength = 8;
        break;
    case 1:
        myDataLength = 2;
        break;
    }

    // ready to read the first packet
    myTalker.read_async(myDataCount * myDataLength);
}

void cGUI::PlotTimerHandler()
{
    if (!myTalker.isOpen())
        return;

    switch (myDataLength)
    {
    case 8:
        t1.set(myRecentDbl);
        break;
    case 2:
        myRecentDbl.clear();
        for( auto v : myRecentU16)
            myRecentDbl.push_back( v );
        t1.set(myRecentDbl);
        break;
    }

    thePlot.update();
}

void cGUI::debugText()
{
    for (int i = 0; i < 10; i++)
        std::cout << myRecentDbl[i] << " ";
    std::cout << "... ";
    for (int i = myRecentDbl.size() - 10; i < myRecentDbl.size(); i++)
        std::cout << myRecentDbl[i] << " ";
    std::cout << "\n\n";
}

int main()
{
    cGUI gui;
    gui.run();
}
