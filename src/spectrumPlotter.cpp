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

    // plot
    wex::plot::plot &thePlot;
    wex::plot::trace &t1;
    wex::timer *myPlotTimer;

    // COM interface
    wex::com &myTalker;

    // data
    int myDataLengthDoubles;
    std::vector<double> myRecent; // the most recently arrived data

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
      myConnectbn(wex::maker::make<wex::button>(myForm)), myTalker(wex::maker::make<wex::com>(myForm)),
      thePlot(wex::maker::make<wex::plot::plot>(myForm)),
      t1(thePlot.AddScatterTrace())
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
    memcpy(
        myRecent.data(),
        myTalker.readData().data(),
        myDataLengthDoubles * 8);

    // wait for next
    myTalker.read_async(myDataLengthDoubles * 8);
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
    myDataLengthDoubles = atoi(myFreqeb.text().c_str());
    myRecent.resize(myDataLengthDoubles);

    // plot x values
    std::vector<double> vx(myDataLengthDoubles);
    int v = 0;
    for (auto &x : vx)
    {
        x = v;
        v += 10;
    }
    t1.setScatterX(vx);

    // ready to read the first packet
    myTalker.read_async(myDataLengthDoubles * 8);
}

void cGUI::PlotTimerHandler()
{
    if (!myTalker.isOpen())
        return;

    t1.set(myRecent);

    thePlot.update();
}

void cGUI::debugText()
{
    for (int i = 0; i < 10; i++)
        std::cout << myRecent[i] << " ";
    std::cout << "... ";
    for (int i = myRecent.size() - 10; i < myRecent.size(); i++)
        std::cout << myRecent[i] << " ";
    std::cout << "\n\n";
}

int main()
{
    cGUI gui;
    gui.run();
}
