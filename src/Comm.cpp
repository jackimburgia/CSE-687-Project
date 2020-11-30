/////////////////////////////////////////////////////////////////////
// Comm.h - message-passing communication facility                 //
// ver 1.0                                                         //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017    //
/////////////////////////////////////////////////////////////////////

#include "Comm.h"
#include "Logger.h"
#include "Utilities.h"
#include "Cpp11-BlockingQueue.h"
#include <iostream>
#include <functional>
#include <conio.h>
#include <thread> 
#include <vector>
#include <string>

using std::thread;
using std::vector;
using std::cout;
using std::endl;
using std::string;

using namespace MsgPassingCommunication;
using namespace Sockets;
using SUtils = Utilities::StringHelper;

//----< constructor sets port >--------------------------------------

Receiver::Receiver(EndPoint ep, const std::string& name) : listener(ep.port), rcvrName(name)
{
  StaticLogger<1>::write("\n  -- starting Receiver");
}
//----< returns reference to receive queue >-------------------------

BlockingQueue<Message>* Receiver::queue()
{
  return &rcvQ;
}
//----< starts listener thread running callable object >-------------

template<typename CallableObject>
void Receiver::start(CallableObject& co)
{
  listener.start(co);
}
//----< stops listener thread >--------------------------------------

void Receiver::stop()
{
  listener.stop();
}
//----< retrieves received message >---------------------------------

Message Receiver::getMessage()
{
  StaticLogger<1>::write("\n  -- " + rcvrName + " deQing message");
  return rcvQ.deQ();
}
//----< constructor initializes endpoint object >--------------------

Sender::Sender(const std::string& name) : sndrName(name)
{
  lastEP = EndPoint();  // used to detect change in destination
}
//----< destructor waits for send thread to terminate >--------------

Sender::~Sender()
{
  if (sendThread.joinable())
    sendThread.join();
}
//----< starts send thread deQ, inspect, and send loop >-------------

void Sender::start()
{
  std::function <void()> threadProc = [&]() {
    while (true)
    {
      Message msg = sndQ.deQ();

      if (msg.command() == "quit")
      {
        StaticLogger<1>::write("\n  -- send thread shutting down");
        return;
      }
      StaticLogger<1>::write("\n  -- " + sndrName + " send thread sending " + msg.name());
      std::string msgStr = msg.toString();

      if (msg.to().address != lastEP.address || msg.to().port != lastEP.port)
      {
        connecter.shutDown();
        //connecter.close();
        StaticLogger<1>::write("\n  -- attempting to connect to new endpoint: " + msg.to().toString());
        if (!connect(msg.to()))
        {
          StaticLogger<1>::write("\n can't connect");
          continue;
        }
        else
        {
          StaticLogger<1>::write("\n  connected to " + msg.to().toString());
        }
      }
      bool sendRslt = connecter.send(msgStr.length(), (Socket::byte*)msgStr.c_str());
    }
  };
  std::thread t(threadProc);
  sendThread = std::move(t);
}
//----< stops send thread by posting quit message >------------------

void Sender::stop()
{
  Message msg;
  msg.name("quit");
  msg.command("quit");
  postMessage(msg);
  connecter.shutDown();
}
//----< attempts to connect to endpoint ep >-------------------------

bool Sender::connect(EndPoint ep)
{
  lastEP = ep;
  return connecter.connect(ep.address, ep.port);
}
//----< posts message to send queue >--------------------------------

void Sender::postMessage(Message msg)
{
  sndQ.enQ(msg);
}
//----< sends binary file >------------------------------------------
/*
*  - not implemented yet
*/
bool Sender::sendFile(const std::string& fileName)
{
  return false;
}
//----< callable object posts incoming message to rcvQ >-------------
/*
*  This is ClientHandler for receiving messages and posting
*  to the receive queue.
*/
class ClientHandler
{
public:
  //----< acquire reference to shared rcvQ >-------------------------

  ClientHandler(BlockingQueue<Message>* pQ, const std::string& name = "clientHandler") : pQ_(pQ), clientHandlerName(name)
  {
    StaticLogger<1>::write("\n  -- starting ClientHandler");
  }
  //----< shutdown message >-----------------------------------------

  ~ClientHandler() 
  { 
    StaticLogger<1>::write("\n  -- ClientHandler destroyed;"); 
  }
  //----< set BlockingQueue >----------------------------------------

  void setQueue(BlockingQueue<Message>* pQ)
  {
    pQ_ = pQ;
  }
  //----< frame message string by reading bytes from socket >--------

  std::string readMsg(Socket& socket)
  {
    std::string temp, msgString;
    while (socket.validState())
    {
      temp = socket.recvString('\n');  // read attribute
      msgString += temp;
      if (temp.length() < 2)           // if empty line we are done
        break;
    }
    return msgString;
  }
  //----< reads messages from socket and enQs in rcvQ >--------------

  void operator()(Socket socket)
  {
    while (socket.validState())
    {
      std::string msgString = readMsg(socket);
      if (msgString.length() == 0)
      {
        // invalid message
        break;
      }
      Message msg = Message::fromString(msgString);
      StaticLogger<1>::write("\n  -- " + clientHandlerName + " RecvThread read message: " + msg.name());
      //std::cout << "\n  -- " + clientHandlerName + " RecvThread read message: " + msg.name();
      pQ_->enQ(msg);
      //std::cout << "\n  -- message enqueued in rcvQ";
      if (msg.command() == "quit")
        break;
    }
    StaticLogger<1>::write("\n  -- terminating ClientHandler thread");
  }
private:
  BlockingQueue<Message>* pQ_;
  std::string clientHandlerName;
};

Comm::Comm(EndPoint ep, const std::string& name) : rcvr(ep, name), sndr(name), commName(name) {}

void Comm::start()
{
  BlockingQueue<Message>* pQ = rcvr.queue();
  ClientHandler* pCh = new ClientHandler(pQ, commName);
  /*
    There is a trivial memory leak here.  
    This ClientHandler is a prototype used to make ClientHandler copies for each connection.
    Those are not created on the heap, and are destroyed when the connection closes.
    Only one Client handler prototype is created for each Comm object and will live until
    the end of the program.

    I will clean this up in the next version.
  */
  rcvr.start(*pCh);
  sndr.start();
}

void Comm::stop()
{
  rcvr.stop();
  sndr.stop();
}

void Comm::postMessage(Message msg)
{
  sndr.postMessage(msg);
}

Message Comm::getMessage()
{
  return rcvr.getMessage();
}

std::string Comm::name()
{
  return commName;
}

//----< test stub >--------------------------------------------------

#ifdef TEST_COMM


void startClient(int port, EndPoint & serverEP) {
    string name = "client" + std::to_string(0);
    cout << name << endl;
    EndPoint clientEP("localhost", port);
    Comm client(clientEP, name);
    client.start();

    auto clientThread = thread([&clientEP, &serverEP, &client]() {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        Message msg;
        msg.name("It is ready");
        msg.from(clientEP);
        msg.to(serverEP);
        client.postMessage(msg);
    });
}

void Test2() {
    SocketSystem ss;

    const int cnt = 3;
    int port = 9191;
    //vector<Comm*> clients;
    Comm* clients[cnt];
    EndPoint* endpoints[cnt];
    vector<thread> threads(cnt);
    //thread threads[cnt];
    EndPoint serverEP("localhost", port++);
    Comm server(serverEP, "server");
    server.start();



    //for (int i = 0; i < cnt; i++)
    //{
    //    string name = "client" + std::to_string(i);
    //    cout << name << endl;
    //    //std::this_thread::sleep_for(std::chrono::seconds(2));
    //    EndPoint clientEP("localhost", port++);
    //    Comm client(clientEP, name);
    //    //client.start();
    //    clients[i] = &client;
    //    endpoints[i] = &clientEP;
    //}

    //for (auto c : clients) {
    //    c->start();
    //}

    auto mainThread = thread([&server,&serverEP]() {
        while (true)
        {
            auto msg = server.getMessage();
            cout << "Server receives message:" << endl;
            msg.show();

            //msg.to(msg.from());
            //msg.from(serverEP);
            //msg.name("I heard you - you said: " + msg.name());
            //server.postMessage(msg);
        }
    });

    std::vector<std::thread> threads2;
    for (int x = 0; x < cnt; x++) {
        thread t([](int num, int p, EndPoint to) {
            string name = "client" + std::to_string(num);
            //cout << name << endl;
            EndPoint clientEP("localhost", p);
            Comm client(clientEP, name);
            client.start();

            // send the ready message
            Message msg;
            msg.name("Ready:" + std::to_string(num));
            msg.from(clientEP);
            msg.to(to);
            
            client.postMessage(msg);

            // listen for tests to run
            while (true)
            {
                msg = client.getMessage();
                cout << "Client " << std::to_string(num) << " receives message" << endl;
                msg.show();

                // run the test
                int sleep = rand() % 5 + 1;     // in the range 1 to 5
                std::this_thread::sleep_for(std::chrono::seconds(sleep));
                cout << "Run the test" << endl;

                // notify the server that the thread is free again
                msg.to(msg.from());
                msg.from(clientEP);
                msg.name(std::to_string(num) + " is Ready");
            }



            }, x, port++, serverEP);
        //threads2.emplace_back(t);
        t.detach();
    }

    for (std::thread& t : threads2) {
        t.join();
    }

    //std::vector<std::thread*> ThreadVector;


    //for (int x = 0 ; x < cnt; x++)
    //{
    //    //ThreadVector.emplace_back([&]() {function(a, b, Obj, Obj2)}); // Pass by reference here, make sure the object lifetime is correct
    //    string name = "client" + std::to_string(x);
    //    cout << name << endl;
    //    EndPoint clientEP("localhost", port++);
    //    Comm client(clientEP, name);
    //    //client.start();
    //    clients[x] = &client;
    //}

    //for (int x = 0; x < cnt; x++) {
    //    cout << "start" << endl;
    //    clients[x]->start();
    //}

    //for (auto& t : ThreadVector)
    //{
    //    t->join();
    //}

    //startClient(port++, serverEP);

    //for (int x = 0; x < 1; x++) {
        //string name = "client" + std::to_string(0);
        //cout << name << endl;
        //EndPoint clientEP("localhost", port++);
        //Comm client(clientEP, name);
        //client.start();

        //auto clientThread = thread([&clientEP, &serverEP, &client]() {
        //        std::this_thread::sleep_for(std::chrono::seconds(2));
        //        Message msg;
        //        msg.name("msg #2");
        //        msg.from(clientEP);
        //        msg.to(serverEP);
        //        client.postMessage(msg);

        //        msg = client.getMessage();
        //        msg.show();
        //    });
    //}





    //Message msg;
    //msg.name("msg #2");
    //msg.from(clientEP);
    //msg.to(serverEP);
    //client.postMessage(msg);

    //for (int i = 0; i < cnt; i++) {
    //    auto c = clients[i];
    //    auto ep = endpoints[i];
    //    auto t = [&c, &ep, &serverEP]() {
    //        Message msg;
    //        msg.name("msg #2");
    //        msg.from(*ep);
    //        msg.to(serverEP);
    //        c->postMessage(msg);
    //        //while (true) {
    //        //    auto msg = c->getMessage();
    //        //    msg.show();
    //        //}
    //        };
    //    auto tt = thread(t);
    //    threads.push_back(std::move(tt));
    //}

    //for (std::thread& th : threads)
    //{
    //    // If thread Object is Joinable then Join that thread.
    //    if (th.joinable())
    //        th.join();
    //}

    mainThread.join();
    //clientThread.join();




    //for (auto c : clients) {
    //    c->start();

    //    auto t = thread([&c]() {
    //        while (true) {
    //            auto msg = c->getMessage();
    //            msg.show();
    //        }
    //        });
    //    //threads.push_back(&t);
    //    //t.join();
    //}


    //for (auto t : threads) {
    //    t->join();
    //}



    //for (auto c : clients) {
    //    c->stop();
    //}

    //server.stop();

    std::cout << "Done!" << std::endl;
}

void Test1() {
    SocketSystem ss;

    EndPoint serverEP("localhost", 9191);
    Comm server(serverEP, "server");
    server.start();

    EndPoint client1EP("localhost", 9192);
    Comm client1(client1EP, "client1");
    client1.start();

    // send msg from comm1 to comm2
    Message msg;
    msg.name("msg #2");
    msg.from(serverEP);
    msg.to(client1EP);
    //StaticLogger<1>::flush();
    //std::cout << "\n  comm1 in main posting message:  " << msg.name();
    server.postMessage(msg);
    server.postMessage(msg);

    auto l1 = [&client1]() {
        while (true)
        {
            //std::this_thread::sleep_for(std::chrono::seconds(2));
            Message m = client1.getMessage();
            //m.from().port
            //m.from().address
            m.show();
        }
        //std::this_thread::sleep_for(std::chrono::seconds(2000));
        //cout << "Lambda 1 = " << word << endl;
    };

    thread t1(l1);

    //thread threads[3];



    t1.join();

    //while (true)
    //{
    //    Message m = client1.getMessage();
    //    m.show();
    //}
    //msg = client1.getMessage();
    ////StaticLogger<1>::flush();
    ////std::cout << "\n  comm2 in main received message: " << msg.name();
    //msg.show();


    server.stop();
    client1.stop();
    //clinet2.stop();
}

/////////////////////////////////////////////////////////////////////
// Test #1 - Demonstrates Sender and Receiver operations

void DemoSndrRcvr(const std::string& machineName)
{
  SUtils::title("Demonstrating Sender and Receiver classes");

  SocketSystem ss;
  EndPoint ep1;
  ep1.port = 9091;
  ep1.address = "localhost";
  Receiver rcvr1(ep1);
  BlockingQueue<Message>* pQ1 = rcvr1.queue();

  ClientHandler ch1(pQ1);
  rcvr1.start(ch1);

  EndPoint ep2;
  ep2.port = 9092;
  ep2.address = "localhost";
  Receiver rcvr2(ep2);
  BlockingQueue<Message>* pQ2 = rcvr2.queue();

  ClientHandler ch2(pQ2);
  rcvr2.start(ch2);

  Sender sndr;
  sndr.start();
  bool connected = sndr.connect(ep1);
  Message msg;
  msg.name("msg #1");
  msg.to(ep1);
  msg.from(msg.to());
  msg.command("do it");
  msg.attribute("bodyAttrib", "zzz");
  StaticLogger<1>::flush();
  std::cout << "\n  sndr in main posting message:  " << msg.name();
  sndr.postMessage(msg);

  msg.name("msg #2");
  msg.to(EndPoint(machineName, 9092));
  StaticLogger<1>::flush();
  std::cout << "\n  sndr in main posting message:  " << msg.name();
  sndr.postMessage(msg);

  Message rcvdMsg = rcvr1.getMessage();  // blocks until message arrives
  StaticLogger<1>::flush();
  std::cout << "\n  rcvr1 in main received message: " << rcvdMsg.name();
  rcvdMsg.show();

  rcvdMsg = rcvr2.getMessage();  // blocks until message arrives
  StaticLogger<1>::flush();
  std::cout << "\n  rcvr2 in main received message: " << rcvdMsg.name();
  rcvdMsg.show();

  SUtils::title("Sending message to EndPoint that doesn't exist");

  msg.name("msg #3");
  msg.to(EndPoint("DoesNotExist", 1111));  // Unknown endpoint - should fail
  StaticLogger<1>::flush();
  std::cout << "\n  sndr in main posting message:  " << msg.name();
  msg.show();
  sndr.postMessage(msg);                   // will never reach rcvr

  msg.name("msg #4");
  msg.to(EndPoint("localhost", 9091));
  StaticLogger<1>::flush();
  std::cout << "\n  sndr in main posting message:  " << msg.name();
  sndr.postMessage(msg);                  // this should succeed
  StaticLogger<1>::flush();
  rcvdMsg = rcvr1.getMessage();
  std::cout << "\n  rcvr1 in main received message: " << rcvdMsg.name();
  rcvdMsg.show();

  rcvr1.stop();
  rcvr2.stop();
  sndr.stop();
  StaticLogger<1>::flush();

  std::cout << "\n  press enter to quit DemoSndrRcvr";
  _getche();
  std::cout << "\n";
}

/////////////////////////////////////////////////////////////////////
// Test #2 - Demonstrates Comm class using a single thread
//           sending and receiving messages from two Comm
//           instances.

void DemoCommClass(const std::string& machineName)
{
  SUtils::title("Demonstrating Comm class");

  SocketSystem ss;

  EndPoint ep1("localhost", 9191);
  Comm comm1(ep1, "comm1");
  comm1.start();

  EndPoint ep2("localhost", 9192);
  Comm comm2(ep2, "comm2");
  comm2.start();

  // send msg from comm1 to comm1
  Message msg;
  msg.name("msg #1");
  msg.to(ep1);
  msg.from(ep1);
  StaticLogger<1>::flush();
  std::cout << "\n  comm1 in main posting message:   " << msg.name();
  comm1.postMessage(msg);
  msg = comm1.getMessage();
  StaticLogger<1>::flush();
  std::cout << "\n  comm1 in main received message:  " << msg.name();
  msg.show();

  // send msg from comm1 to comm2
  msg.name("msg #2");
  msg.from(ep1);
  msg.to(ep2);
  StaticLogger<1>::flush();
  std::cout << "\n  comm1 in main posting message:  " << msg.name();
  comm1.postMessage(msg);
  msg = comm2.getMessage();
  StaticLogger<1>::flush();
  std::cout << "\n  comm2 in main received message: " << msg.name();
  msg.show();

  // send msg from comm2 to comm1
  msg.name("msg #3");
  msg.to(ep1);
  msg.from(ep2);
  StaticLogger<1>::flush();
  std::cout << "\n  comm2 in main posting message:  " << msg.name();
  comm2.postMessage(msg);
  msg = comm1.getMessage();
  StaticLogger<1>::flush();
  std::cout << "\n  comm1 in main received message: " << msg.name();
  msg.show();

  // send msg from comm2 to comm2
  msg.name("msg #4");
  msg.from(ep2);
  msg.to(ep2);
  StaticLogger<1>::flush();
  std::cout << "\n  comm2 in main posting message:  " << msg.name();
  comm2.postMessage(msg);
  msg = comm2.getMessage();
  StaticLogger<1>::flush();
  std::cout << "\n  comm2 in main received message: " << msg.name();
  msg.show();

  comm1.stop();
  comm2.stop();
  StaticLogger<1>::flush();
  std::cout << "\n  press enter to quit DemoComm";
  _getche();
}
/////////////////////////////////////////////////////////////////////
// Test #3 - Demonstrate server with two concurrent clients
//           sending and receiving messages

//----< handler for first concurrent client >------------------------

void ThreadProcClnt1()
{
  Comm comm(EndPoint("localhost", 9891), "client1Comm");
  comm.start();
  EndPoint serverEP("localhost", 9890);
  EndPoint clientEP("localhost", 9891);
  size_t IMax = 3;
  for (size_t i = 0; i < IMax; ++i)
  {
    Message msg(serverEP, clientEP);
    msg.name("client #1 : msg #" + Utilities::Converter<size_t>::toString(i));
    std::cout << "\n  " + comm.name() + " posting:  " << msg.name();
    comm.postMessage(msg);
    Message rply = comm.getMessage();
    std::cout << "\n  " + comm.name() + " received: " << rply.name();
    ::Sleep(100);
  }
  ::Sleep(200);
  Message stop;
  stop.name("stop");
  stop.to(serverEP);
  stop.command("stop");
  comm.postMessage(stop);
}
//----< handler for 2nd concurrent client >--------------------------

void ThreadProcClnt2()
{
  Comm comm(EndPoint("localhost", 9892), "client2Comm");
  comm.start();
  EndPoint serverEP("localhost", 9890);
  EndPoint clientEP("localhost", 9892);
  size_t IMax = 3;
  for (size_t i = 0; i < IMax; ++i)
  {
    Message msg(serverEP, clientEP);
    msg.name("client #2 : msg #" + Utilities::Converter<size_t>::toString(i));
    std::cout << "\n  " + comm.name() + " posting:  " << msg.name();
    comm.postMessage(msg);
    Message rply = comm.getMessage();
    std::cout << "\n  " + comm.name() + " received: " << rply.name();
  }
}

void Demo2() {
    SocketSystem ss;

    EndPoint serverEP("localhost", 9890);
    Comm comm(serverEP, "serverComm");
    comm.start();
}

//----< server demonstrates two-way asynchronous communication >-----
/*
*  - One server receiving messages and sending replies to
*    two concurrent clients.
*/
void DemoClientServer()
{
  SUtils::title("Demonstrating Client-Server - one server with two concurrent clients");

  SocketSystem ss;

  EndPoint serverEP("localhost", 9890);
  //EndPoint clientEP("localhost", 9891);
  Comm comm(serverEP, "serverComm");
  comm.start();
  std::thread t1(ThreadProcClnt1);
  t1.detach();
  std::thread t2(ThreadProcClnt2);
  t2.detach();

  Message msg, rply;
  rply.name("reply");
  size_t count = 0;
  while (true)
  {
    msg = comm.getMessage();
    std::cout << "\n  " + comm.name() + " received message: " << msg.name();
    //msg.show();
    rply.to(msg.from());
    rply.from(serverEP);
    rply.name("server reply #" + Utilities::Converter<size_t>::toString(++count) + " to " + msg.from().toString());
    //rply.show();
    comm.postMessage(rply);
    if (msg.command() == "stop")
    {
      break;
    }
  }
  comm.stop();
  StaticLogger<1>::flush();
  std::cout << "\n  press enter to quit DemoClientServer";
  _getche();
}

Cosmetic cosmetic;

int main_Comm()
//int main()
{
  //SUtils::Title("Demo of Message-Passing Communication");
  //Utilities::putline();

  //StaticLogger<1>::attach(&std::cout);

    DemoClientServer();

  ///////////////////////////////////////////////////////////////////
  // remove comment on line below to show many of the gory details
  //
  //StaticLogger<1>::start();

  ///////////////////////////////////////////////////////////////////
  // if you uncomment the lines below, you will run all demos

  //DemoSndrRcvr("LAPTOP-KF78KIA6");  // replace "Odin" with your machine name
  //DemoCommClass("Odin");
  //DemoClientServer();

  //Test1();
  //Test2();
  //DemoCommClass("LAPTOP-KF78KIA6");

  return 0;
}
#endif
