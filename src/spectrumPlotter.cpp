#include <sstream>
#include "com.h"
#include "plot2d.h"

class cGUI
{
public:
    cGUI();
    void run();

private:
    wex::gui &myForm;
    wex::label &myPortlb;
    wex::editbox &myPorteb;
    wex::button &myConnectbn;
    wex::com &myTalker;
    wex::list &myRcvList;
    wex::plot::plot &thePlot;
    wex::plot::trace &t1;
    wex::timer *myPlotTimer;

    void ConstructPlot();
};

cGUI::cGUI()
    : myForm(wex::maker::make()), myPortlb(wex::maker::make<wex::label>(myForm)), myPorteb(wex::maker::make<wex::editbox>(myForm)), myConnectbn(wex::maker::make<wex::button>(myForm)), myTalker(wex::maker::make<wex::com>(myForm)), myRcvList(wex::maker::make<wex::list>(myForm)), thePlot(wex::maker::make<wex::plot::plot>(myForm)), t1(thePlot.AddRealTimeTrace(100))
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

    ConstructPlot();

    // myRcvList.move( 20,70, 400,300 );

    myConnectbn.events().click([&]
                               {
                                   // user wants to connect to talker

                                   myTalker.port(myPorteb.text());
                                   myTalker.open();
                                   if (!myTalker.isOpen())
                                   {
                                       wex::msgbox mb("Open failed");
                                       exit(1);
                                   }
                                   myConnectbn.text("Connected");
                                   myConnectbn.bgcolor(0x90FF90);
                                   myConnectbn.enable(false);
                                   myConnectbn.update();

                                   // ready to read the first packet
                                   myTalker.read_async(-1); });

    myForm.events().asyncReadComplete([&](int id)
                                      {
        // packet has been received

        auto r = myTalker.readData();
        double data_point;
        memcpy(&data_point,r.data(),8);

        t1.add( data_point );

        // wait for next

        myTalker.read_async( -1 ); });

    myForm.show();
}
void cGUI::run()
{
    myForm.run();
}

void cGUI::ConstructPlot()
{
    // construct plot to be drawn on form
    thePlot.bgcolor(0);
    // thePlot.Grid( true );
    //  resize plot when form resizes
    myForm.events().resize(
        [&](int w, int h)
        {
            thePlot.size(w - 100, h - 200);
            thePlot.move(30, 100);
            thePlot.update();
        });

    // plot in blue
    t1.color(0xFF0000);

    // Update the plot 10 times/second
    myPlotTimer = new wex::timer(myForm, 100);
    myForm.events().timer(
        [this](int id)
        {
            thePlot.update();
        });
}

int main()
{
    cGUI gui;
    gui.run();
}
