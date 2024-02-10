#include <sstream>
#include "com.h"

class cGUI
{
public:
    cGUI();
    void run();

private:
    wex::gui &myForm;
    wex::label &myPortlb;
    wex::label &mySpeedlb;
    wex::editbox &myPorteb;
    wex::editbox &mySpeedeb;
    wex::button &myConnectbn;
    wex::button &myTxbn;
    wex::com &myTalker;
    wex::list &myRcvList;
    wex::timer *myTxTimer;
};

cGUI::cGUI()
    : myForm(wex::maker::make()),
     myPortlb(wex::maker::make<wex::label>(myForm)),
     mySpeedlb(wex::maker::make<wex::label>(myForm)),
      myPorteb(wex::maker::make<wex::editbox>(myForm)),
      mySpeedeb(wex::maker::make<wex::editbox>(myForm)),
      myConnectbn(wex::maker::make<wex::button>(myForm)), myTxbn(wex::maker::make<wex::button>(myForm)), myTalker(wex::maker::make<wex::com>(myForm)), myRcvList(wex::maker::make<wex::list>(myForm))
{
    myForm.move(50, 50, 450, 500);
    myForm.text("COMDeviceSim");

    myPortlb.move(20, 20, 100, 30);
    myPortlb.text("COM Port");
    myPorteb.move(140, 20, 50, 30);
    myPorteb.text("8");
    mySpeedlb.move(5,100,80,30);
    mySpeedlb.text("Tx Speed");
    mySpeedeb.move(100, 100, 50, 30);
    mySpeedeb.text("10");
    mySpeedeb.tooltip("frames/sec");
    myConnectbn.move(210, 20, 100, 30);
    myConnectbn.bgcolor(0x9090FF);
    myConnectbn.text("Connect");

    myTxbn.move(330, 20, 50, 30);
    myTxbn.text("TX");
    myTxbn.tooltip("Start sending data");
    myTxbn.events().click(
        [&]
        {
            myTxTimer = new wex::timer(myForm,
                                       1000 / atoi(mySpeedeb.text().c_str()));
        });

    myForm.events().timer(
        [this](int id)
        {
            static int p = 0;
            double data_point;

            data_point = 10 * sin(p++ / 10.0);

            std::vector<unsigned char> msgbuf(8);
            memcpy(msgbuf.data(), &data_point, 8);

            myTalker.write(msgbuf);
        });

    // myRcvList.move(20, 70, 400, 300);

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
        myRcvList.add( "read " + std::to_string( r.size() ) + " bytes");
        std::stringstream ss;
        ss << std::hex;
        int k = 0;
        for( auto c : r )
        {
            ss << "0x" << (int)c << " ";
            k++;
            if( k > 9 )
            {
                myRcvList.add( ss.str() );
                ss.str("");
                k = 0;
            }
        }
        myRcvList.add( ss.str() );
        myRcvList.update();

        // wait for next

        myTalker.read_async( -1 ); });

    myForm.show();
}
void cGUI::run()
{
    myForm.run();
}

int main()
{
    cGUI gui;
    gui.run();
}
