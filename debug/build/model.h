#include <PhaseI/Link.h>
#include <PhaseI/NetworkQueue.h>
#include <PhaseI/PacketPrioritySelect.h>
#include <vector/Vector2Scalar.h>
#include <PhaseI/Router.h>
#include <engine.h>
#include <sinks/RunScilabJob.h>
#include <sinks/multipleSimulationCommands.h>
#include <PhaseI/FelixServer.h>
#include <sinks/SimulationExperimentTracker.h>
#include <PhaseI/Vector_NetworkQueueWithSizeOutput.h>
#include <PhaseI/Packetsink.h>
#include <PhaseI/WRRScheduler.h>
class Model: public RootSimulator {
  public:
    Model (Time ti): RootSimulator((char*)"MyTopology") {
      mainCoupling = new RootCoupling((char*)"MyTopology");
      Simulator **D0 = new Simulator* [13];
      Connection **IC0 = new Connection* [16];
      //D0[0] is  LoadScilabParams
      D0[0] = new RunScilabJob("LoadScilabParams");
      ((Simulator*)D0[0])->father = mainCoupling;
      D0[0]->init(0.0,"exec('/home/alaurito/.haikunet/debug/host_to_host.hk/Scilab/model.scilabParams', 0)","","");
      //D0[1] is  ExperimenetTracker
      D0[1] = new SimulationExperimentTracker("ExperimenetTracker");
      ((Simulator*)D0[1])->father = mainCoupling;
      D0[1]->init(0.0,"SimulationName","ExperimentNumber","ParameterValues","ParameterNames");
      //D0[2] is  UpdateScilabParams
      D0[2] = new RunScilabJob("UpdateScilabParams");
      ((Simulator*)D0[2])->father = mainCoupling;
      D0[2]->init(0.0,"","","");
      //D0[3] is 16:C9:43:0A:5C:81/None
      D0[3] = new Coupling("16:C9:43:0A:5C:81/None");
      ((Simulator*)D0[3])->father = mainCoupling;
       Simulator **D1 = new Simulator* [4];
       Connection **EIC1 = new Connection* [0];
       Connection **EOC1 = new Connection* [1];
       Connection **IC1 = new Connection* [2];
        //D1[0] is  GeneratorApplication
        D1[0] = new FelixServer("GeneratorApplication");
        ((Simulator*)D1[0])->father = (Coupling*)D0[3];
        D1[0]->init(0.0);
        //D1[1] is  Routing
        D1[1] = new Router("Routing");
        ((Simulator*)D1[1])->father = (Coupling*)D0[3];
        D1[1]->init(0.0);
        //D1[2] is egressPort0
        D1[2] = new Coupling("egressPort0");
        ((Simulator*)D1[2])->father = (Coupling*)D0[3];
         Simulator **D2 = new Simulator* [2];
         Connection **EIC2 = new Connection* [1];
         Connection **EOC2 = new Connection* [1];
         Connection **IC2 = new Connection* [2];
          //D2[0] is  portQueue
          D2[0] = new NetworkQueue("portQueue");
          ((Simulator*)D2[0])->father = (Coupling*)D1[2];
          D2[0]->init(0.0,"16:C9:43:0A:5C:81/None.egressPort.queueCapacity","-1");
          //D2[1] is  Link
          D2[1] = new Link("Link");
          ((Simulator*)D2[1])->father = (Coupling*)D1[2];
          D2[1]->init(0.0,"16:C9:43:0A:5C:81/None.egressPort0.link.capacity","16:C9:43:0A:5C:81/None.egressPort0.link.delay");
         EIC2[0] = new Connection();
         EIC2[0]->setup(0,0,0,0);
         EOC2[0] = new Connection();
         EOC2[0]->setup(1,0,0,0);
         IC2[0] = new Connection();
         IC2[0]->setup(0,0,1,0);
         IC2[1] = new Connection();
         IC2[1]->setup(1,0,0,1);
        ((Coupling*)D1[2])->setup(D2,2,IC2,2,EIC2,1,EOC2,1);
        ((Coupling*)D1[2])->init(ti);
        //D1[3] is  ReceivingApplication
        D1[3] = new Packetsink("ReceivingApplication");
        ((Simulator*)D1[3])->father = (Coupling*)D0[3];
        D1[3]->init(0.0);
       EOC1[0] = new Connection();
       EOC1[0]->setup(2,0,0,0);
       IC1[0] = new Connection();
       IC1[0]->setup(0,0,1,0);
       IC1[1] = new Connection();
       IC1[1]->setup(1,0,2,0);
      ((Coupling*)D0[3])->setup(D1,4,IC1,2,EIC1,0,EOC1,1);
      ((Coupling*)D0[3])->init(ti);
      //D0[4] is 2A:DC:FD:E5:EF:97/None
      D0[4] = new Coupling("2A:DC:FD:E5:EF:97/None");
      ((Simulator*)D0[4])->father = mainCoupling;
       Simulator **D3 = new Simulator* [4];
       Connection **EIC3 = new Connection* [0];
       Connection **EOC3 = new Connection* [1];
       Connection **IC3 = new Connection* [2];
        //D3[0] is  GeneratorApplication
        D3[0] = new FelixServer("GeneratorApplication");
        ((Simulator*)D3[0])->father = (Coupling*)D0[4];
        D3[0]->init(0.0);
        //D3[1] is  Routing
        D3[1] = new Router("Routing");
        ((Simulator*)D3[1])->father = (Coupling*)D0[4];
        D3[1]->init(0.0);
        //D3[2] is egressPort0
        D3[2] = new Coupling("egressPort0");
        ((Simulator*)D3[2])->father = (Coupling*)D0[4];
         Simulator **D4 = new Simulator* [2];
         Connection **EIC4 = new Connection* [1];
         Connection **EOC4 = new Connection* [1];
         Connection **IC4 = new Connection* [2];
          //D4[0] is  portQueue
          D4[0] = new NetworkQueue("portQueue");
          ((Simulator*)D4[0])->father = (Coupling*)D3[2];
          D4[0]->init(0.0,"2A:DC:FD:E5:EF:97/None.egressPort.queueCapacity","-1");
          //D4[1] is  Link
          D4[1] = new Link("Link");
          ((Simulator*)D4[1])->father = (Coupling*)D3[2];
          D4[1]->init(0.0,"2A:DC:FD:E5:EF:97/None.egressPort0.link.capacity","2A:DC:FD:E5:EF:97/None.egressPort0.link.delay");
         EIC4[0] = new Connection();
         EIC4[0]->setup(0,0,0,0);
         EOC4[0] = new Connection();
         EOC4[0]->setup(1,0,0,0);
         IC4[0] = new Connection();
         IC4[0]->setup(0,0,1,0);
         IC4[1] = new Connection();
         IC4[1]->setup(1,0,0,1);
        ((Coupling*)D3[2])->setup(D4,2,IC4,2,EIC4,1,EOC4,1);
        ((Coupling*)D3[2])->init(ti);
        //D3[3] is  ReceivingApplication
        D3[3] = new Packetsink("ReceivingApplication");
        ((Simulator*)D3[3])->father = (Coupling*)D0[4];
        D3[3]->init(0.0);
       EOC3[0] = new Connection();
       EOC3[0]->setup(2,0,0,0);
       IC3[0] = new Connection();
       IC3[0]->setup(0,0,1,0);
       IC3[1] = new Connection();
       IC3[1]->setup(1,0,2,0);
      ((Coupling*)D0[4])->setup(D3,4,IC3,2,EIC3,0,EOC3,1);
      ((Coupling*)D0[4])->init(ti);
      //D0[5] is B6:97:69:55:A6:0E/None
      D0[5] = new Coupling("B6:97:69:55:A6:0E/None");
      ((Simulator*)D0[5])->father = mainCoupling;
       Simulator **D5 = new Simulator* [4];
       Connection **EIC5 = new Connection* [0];
       Connection **EOC5 = new Connection* [1];
       Connection **IC5 = new Connection* [2];
        //D5[0] is  GeneratorApplication
        D5[0] = new FelixServer("GeneratorApplication");
        ((Simulator*)D5[0])->father = (Coupling*)D0[5];
        D5[0]->init(0.0);
        //D5[1] is  Routing
        D5[1] = new Router("Routing");
        ((Simulator*)D5[1])->father = (Coupling*)D0[5];
        D5[1]->init(0.0);
        //D5[2] is egressPort0
        D5[2] = new Coupling("egressPort0");
        ((Simulator*)D5[2])->father = (Coupling*)D0[5];
         Simulator **D6 = new Simulator* [2];
         Connection **EIC6 = new Connection* [1];
         Connection **EOC6 = new Connection* [1];
         Connection **IC6 = new Connection* [2];
          //D6[0] is  portQueue
          D6[0] = new NetworkQueue("portQueue");
          ((Simulator*)D6[0])->father = (Coupling*)D5[2];
          D6[0]->init(0.0,"B6:97:69:55:A6:0E/None.egressPort.queueCapacity","-1");
          //D6[1] is  Link
          D6[1] = new Link("Link");
          ((Simulator*)D6[1])->father = (Coupling*)D5[2];
          D6[1]->init(0.0,"B6:97:69:55:A6:0E/None.egressPort0.link.capacity","B6:97:69:55:A6:0E/None.egressPort0.link.delay");
         EIC6[0] = new Connection();
         EIC6[0]->setup(0,0,0,0);
         EOC6[0] = new Connection();
         EOC6[0]->setup(1,0,0,0);
         IC6[0] = new Connection();
         IC6[0]->setup(0,0,1,0);
         IC6[1] = new Connection();
         IC6[1]->setup(1,0,0,1);
        ((Coupling*)D5[2])->setup(D6,2,IC6,2,EIC6,1,EOC6,1);
        ((Coupling*)D5[2])->init(ti);
        //D5[3] is  ReceivingApplication
        D5[3] = new Packetsink("ReceivingApplication");
        ((Simulator*)D5[3])->father = (Coupling*)D0[5];
        D5[3]->init(0.0);
       EOC5[0] = new Connection();
       EOC5[0]->setup(2,0,0,0);
       IC5[0] = new Connection();
       IC5[0]->setup(0,0,1,0);
       IC5[1] = new Connection();
       IC5[1]->setup(1,0,2,0);
      ((Coupling*)D0[5])->setup(D5,4,IC5,2,EIC5,0,EOC5,1);
      ((Coupling*)D0[5])->init(ti);
      //D0[6] is A2:87:7B:98:CA:DC/None
      D0[6] = new Coupling("A2:87:7B:98:CA:DC/None");
      ((Simulator*)D0[6])->father = mainCoupling;
       Simulator **D7 = new Simulator* [4];
       Connection **EIC7 = new Connection* [0];
       Connection **EOC7 = new Connection* [1];
       Connection **IC7 = new Connection* [2];
        //D7[0] is  GeneratorApplication
        D7[0] = new FelixServer("GeneratorApplication");
        ((Simulator*)D7[0])->father = (Coupling*)D0[6];
        D7[0]->init(0.0);
        //D7[1] is  Routing
        D7[1] = new Router("Routing");
        ((Simulator*)D7[1])->father = (Coupling*)D0[6];
        D7[1]->init(0.0);
        //D7[2] is egressPort0
        D7[2] = new Coupling("egressPort0");
        ((Simulator*)D7[2])->father = (Coupling*)D0[6];
         Simulator **D8 = new Simulator* [2];
         Connection **EIC8 = new Connection* [1];
         Connection **EOC8 = new Connection* [1];
         Connection **IC8 = new Connection* [2];
          //D8[0] is  portQueue
          D8[0] = new NetworkQueue("portQueue");
          ((Simulator*)D8[0])->father = (Coupling*)D7[2];
          D8[0]->init(0.0,"A2:87:7B:98:CA:DC/None.egressPort.queueCapacity","-1");
          //D8[1] is  Link
          D8[1] = new Link("Link");
          ((Simulator*)D8[1])->father = (Coupling*)D7[2];
          D8[1]->init(0.0,"A2:87:7B:98:CA:DC/None.egressPort0.link.capacity","A2:87:7B:98:CA:DC/None.egressPort0.link.delay");
         EIC8[0] = new Connection();
         EIC8[0]->setup(0,0,0,0);
         EOC8[0] = new Connection();
         EOC8[0]->setup(1,0,0,0);
         IC8[0] = new Connection();
         IC8[0]->setup(0,0,1,0);
         IC8[1] = new Connection();
         IC8[1]->setup(1,0,0,1);
        ((Coupling*)D7[2])->setup(D8,2,IC8,2,EIC8,1,EOC8,1);
        ((Coupling*)D7[2])->init(ti);
        //D7[3] is  ReceivingApplication
        D7[3] = new Packetsink("ReceivingApplication");
        ((Simulator*)D7[3])->father = (Coupling*)D0[6];
        D7[3]->init(0.0);
       EOC7[0] = new Connection();
       EOC7[0]->setup(2,0,0,0);
       IC7[0] = new Connection();
       IC7[0]->setup(0,0,1,0);
       IC7[1] = new Connection();
       IC7[1]->setup(1,0,2,0);
      ((Coupling*)D0[6])->setup(D7,4,IC7,2,EIC7,0,EOC7,1);
      ((Coupling*)D0[6])->init(ti);
      //D0[7] is of:0000000000000003
      D0[7] = new Coupling("of:0000000000000003");
      ((Simulator*)D0[7])->father = mainCoupling;
       Simulator **D9 = new Simulator* [6];
       Connection **EIC9 = new Connection* [5];
       Connection **EOC9 = new Connection* [5];
       Connection **IC9 = new Connection* [5];
        //D9[0] is  Routing
        D9[0] = new Router("Routing");
        ((Simulator*)D9[0])->father = (Coupling*)D0[7];
        D9[0]->init(0.0);
        //D9[1] is egressPort0
        D9[1] = new Coupling("egressPort0");
        ((Simulator*)D9[1])->father = (Coupling*)D0[7];
         Simulator **D10 = new Simulator* [2];
         Connection **EIC10 = new Connection* [1];
         Connection **EOC10 = new Connection* [1];
         Connection **IC10 = new Connection* [2];
          //D10[0] is  Link
          D10[0] = new Link("Link");
          ((Simulator*)D10[0])->father = (Coupling*)D9[1];
          D10[0]->init(0.0,"of:0000000000000003.egressPort0.link.capacity","of:0000000000000003.egressPort0.link.delay");
          //D10[1] is PriorityQueue
          D10[1] = new Coupling("PriorityQueue");
          ((Simulator*)D10[1])->father = (Coupling*)D9[1];
           Simulator **D11 = new Simulator* [4];
           Connection **EIC11 = new Connection* [2];
           Connection **EOC11 = new Connection* [1];
           Connection **IC11 = new Connection* [4];
            //D11[0] is  prioritySelect
            D11[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D11[0])->father = (Coupling*)D10[1];
            D11[0]->init(0.0);
            //D11[1] is  VQueues
            D11[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D11[1])->father = (Coupling*)D10[1];
            D11[1]->init(0.0,"of:0000000000000003.queueCapacity","-1","of:0000000000000003.numberOfQueues");
            //D11[2] is  WRRScheduler
            D11[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D11[2])->father = (Coupling*)D10[1];
            D11[2]->init(0.0,"of:0000000000000003.numberOfQueues","of:0000000000000003.egressPort0.PriorityQueue.WRRScheduler.weights");
            //D11[3] is  Vect2Scal
            D11[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D11[3])->father = (Coupling*)D10[1];
            D11[3]->init(0.0,"-1");
           EIC11[0] = new Connection();
           EIC11[0]->setup(0,0,0,0);
           EIC11[1] = new Connection();
           EIC11[1]->setup(0,1,2,1);
           EOC11[0] = new Connection();
           EOC11[0]->setup(3,0,0,0);
           IC11[0] = new Connection();
           IC11[0]->setup(0,0,1,0);
           IC11[1] = new Connection();
           IC11[1]->setup(1,1,2,0);
           IC11[2] = new Connection();
           IC11[2]->setup(2,0,1,1);
           IC11[3] = new Connection();
           IC11[3]->setup(1,0,3,0);
          ((Coupling*)D10[1])->setup(D11,4,IC11,4,EIC11,2,EOC11,1);
          ((Coupling*)D10[1])->init(ti);
         EIC10[0] = new Connection();
         EIC10[0]->setup(0,0,1,0);
         EOC10[0] = new Connection();
         EOC10[0]->setup(0,0,0,0);
         IC10[0] = new Connection();
         IC10[0]->setup(1,0,0,0);
         IC10[1] = new Connection();
         IC10[1]->setup(0,0,1,1);
        ((Coupling*)D9[1])->setup(D10,2,IC10,2,EIC10,1,EOC10,1);
        ((Coupling*)D9[1])->init(ti);
        //D9[2] is egressPort1
        D9[2] = new Coupling("egressPort1");
        ((Simulator*)D9[2])->father = (Coupling*)D0[7];
         Simulator **D12 = new Simulator* [2];
         Connection **EIC12 = new Connection* [1];
         Connection **EOC12 = new Connection* [1];
         Connection **IC12 = new Connection* [2];
          //D12[0] is  Link
          D12[0] = new Link("Link");
          ((Simulator*)D12[0])->father = (Coupling*)D9[2];
          D12[0]->init(0.0,"of:0000000000000003.egressPort1.link.capacity","of:0000000000000003.egressPort1.link.delay");
          //D12[1] is PriorityQueue
          D12[1] = new Coupling("PriorityQueue");
          ((Simulator*)D12[1])->father = (Coupling*)D9[2];
           Simulator **D13 = new Simulator* [4];
           Connection **EIC13 = new Connection* [2];
           Connection **EOC13 = new Connection* [1];
           Connection **IC13 = new Connection* [4];
            //D13[0] is  prioritySelect
            D13[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D13[0])->father = (Coupling*)D12[1];
            D13[0]->init(0.0);
            //D13[1] is  VQueues
            D13[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D13[1])->father = (Coupling*)D12[1];
            D13[1]->init(0.0,"of:0000000000000003.queueCapacity","-1","of:0000000000000003.numberOfQueues");
            //D13[2] is  WRRScheduler
            D13[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D13[2])->father = (Coupling*)D12[1];
            D13[2]->init(0.0,"of:0000000000000003.numberOfQueues","of:0000000000000003.egressPort1.PriorityQueue.WRRScheduler.weights");
            //D13[3] is  Vect2Scal
            D13[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D13[3])->father = (Coupling*)D12[1];
            D13[3]->init(0.0,"-1");
           EIC13[0] = new Connection();
           EIC13[0]->setup(0,0,0,0);
           EIC13[1] = new Connection();
           EIC13[1]->setup(0,1,2,1);
           EOC13[0] = new Connection();
           EOC13[0]->setup(3,0,0,0);
           IC13[0] = new Connection();
           IC13[0]->setup(0,0,1,0);
           IC13[1] = new Connection();
           IC13[1]->setup(1,1,2,0);
           IC13[2] = new Connection();
           IC13[2]->setup(2,0,1,1);
           IC13[3] = new Connection();
           IC13[3]->setup(1,0,3,0);
          ((Coupling*)D12[1])->setup(D13,4,IC13,4,EIC13,2,EOC13,1);
          ((Coupling*)D12[1])->init(ti);
         EIC12[0] = new Connection();
         EIC12[0]->setup(0,0,1,0);
         EOC12[0] = new Connection();
         EOC12[0]->setup(0,0,0,0);
         IC12[0] = new Connection();
         IC12[0]->setup(1,0,0,0);
         IC12[1] = new Connection();
         IC12[1]->setup(0,0,1,1);
        ((Coupling*)D9[2])->setup(D12,2,IC12,2,EIC12,1,EOC12,1);
        ((Coupling*)D9[2])->init(ti);
        //D9[3] is egressPort2
        D9[3] = new Coupling("egressPort2");
        ((Simulator*)D9[3])->father = (Coupling*)D0[7];
         Simulator **D14 = new Simulator* [2];
         Connection **EIC14 = new Connection* [1];
         Connection **EOC14 = new Connection* [1];
         Connection **IC14 = new Connection* [2];
          //D14[0] is  Link
          D14[0] = new Link("Link");
          ((Simulator*)D14[0])->father = (Coupling*)D9[3];
          D14[0]->init(0.0,"of:0000000000000003.egressPort2.link.capacity","of:0000000000000003.egressPort2.link.delay");
          //D14[1] is PriorityQueue
          D14[1] = new Coupling("PriorityQueue");
          ((Simulator*)D14[1])->father = (Coupling*)D9[3];
           Simulator **D15 = new Simulator* [4];
           Connection **EIC15 = new Connection* [2];
           Connection **EOC15 = new Connection* [1];
           Connection **IC15 = new Connection* [4];
            //D15[0] is  prioritySelect
            D15[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D15[0])->father = (Coupling*)D14[1];
            D15[0]->init(0.0);
            //D15[1] is  VQueues
            D15[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D15[1])->father = (Coupling*)D14[1];
            D15[1]->init(0.0,"of:0000000000000003.queueCapacity","-1","of:0000000000000003.numberOfQueues");
            //D15[2] is  WRRScheduler
            D15[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D15[2])->father = (Coupling*)D14[1];
            D15[2]->init(0.0,"of:0000000000000003.numberOfQueues","of:0000000000000003.egressPort2.PriorityQueue.WRRScheduler.weights");
            //D15[3] is  Vect2Scal
            D15[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D15[3])->father = (Coupling*)D14[1];
            D15[3]->init(0.0,"-1");
           EIC15[0] = new Connection();
           EIC15[0]->setup(0,0,0,0);
           EIC15[1] = new Connection();
           EIC15[1]->setup(0,1,2,1);
           EOC15[0] = new Connection();
           EOC15[0]->setup(3,0,0,0);
           IC15[0] = new Connection();
           IC15[0]->setup(0,0,1,0);
           IC15[1] = new Connection();
           IC15[1]->setup(1,1,2,0);
           IC15[2] = new Connection();
           IC15[2]->setup(2,0,1,1);
           IC15[3] = new Connection();
           IC15[3]->setup(1,0,3,0);
          ((Coupling*)D14[1])->setup(D15,4,IC15,4,EIC15,2,EOC15,1);
          ((Coupling*)D14[1])->init(ti);
         EIC14[0] = new Connection();
         EIC14[0]->setup(0,0,1,0);
         EOC14[0] = new Connection();
         EOC14[0]->setup(0,0,0,0);
         IC14[0] = new Connection();
         IC14[0]->setup(1,0,0,0);
         IC14[1] = new Connection();
         IC14[1]->setup(0,0,1,1);
        ((Coupling*)D9[3])->setup(D14,2,IC14,2,EIC14,1,EOC14,1);
        ((Coupling*)D9[3])->init(ti);
        //D9[4] is egressPort3
        D9[4] = new Coupling("egressPort3");
        ((Simulator*)D9[4])->father = (Coupling*)D0[7];
         Simulator **D16 = new Simulator* [2];
         Connection **EIC16 = new Connection* [1];
         Connection **EOC16 = new Connection* [1];
         Connection **IC16 = new Connection* [2];
          //D16[0] is  Link
          D16[0] = new Link("Link");
          ((Simulator*)D16[0])->father = (Coupling*)D9[4];
          D16[0]->init(0.0,"of:0000000000000003.egressPort3.link.capacity","of:0000000000000003.egressPort3.link.delay");
          //D16[1] is PriorityQueue
          D16[1] = new Coupling("PriorityQueue");
          ((Simulator*)D16[1])->father = (Coupling*)D9[4];
           Simulator **D17 = new Simulator* [4];
           Connection **EIC17 = new Connection* [2];
           Connection **EOC17 = new Connection* [1];
           Connection **IC17 = new Connection* [4];
            //D17[0] is  prioritySelect
            D17[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D17[0])->father = (Coupling*)D16[1];
            D17[0]->init(0.0);
            //D17[1] is  VQueues
            D17[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D17[1])->father = (Coupling*)D16[1];
            D17[1]->init(0.0,"of:0000000000000003.queueCapacity","-1","of:0000000000000003.numberOfQueues");
            //D17[2] is  WRRScheduler
            D17[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D17[2])->father = (Coupling*)D16[1];
            D17[2]->init(0.0,"of:0000000000000003.numberOfQueues","of:0000000000000003.egressPort3.PriorityQueue.WRRScheduler.weights");
            //D17[3] is  Vect2Scal
            D17[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D17[3])->father = (Coupling*)D16[1];
            D17[3]->init(0.0,"-1");
           EIC17[0] = new Connection();
           EIC17[0]->setup(0,0,0,0);
           EIC17[1] = new Connection();
           EIC17[1]->setup(0,1,2,1);
           EOC17[0] = new Connection();
           EOC17[0]->setup(3,0,0,0);
           IC17[0] = new Connection();
           IC17[0]->setup(0,0,1,0);
           IC17[1] = new Connection();
           IC17[1]->setup(1,1,2,0);
           IC17[2] = new Connection();
           IC17[2]->setup(2,0,1,1);
           IC17[3] = new Connection();
           IC17[3]->setup(1,0,3,0);
          ((Coupling*)D16[1])->setup(D17,4,IC17,4,EIC17,2,EOC17,1);
          ((Coupling*)D16[1])->init(ti);
         EIC16[0] = new Connection();
         EIC16[0]->setup(0,0,1,0);
         EOC16[0] = new Connection();
         EOC16[0]->setup(0,0,0,0);
         IC16[0] = new Connection();
         IC16[0]->setup(1,0,0,0);
         IC16[1] = new Connection();
         IC16[1]->setup(0,0,1,1);
        ((Coupling*)D9[4])->setup(D16,2,IC16,2,EIC16,1,EOC16,1);
        ((Coupling*)D9[4])->init(ti);
        //D9[5] is egressPort4
        D9[5] = new Coupling("egressPort4");
        ((Simulator*)D9[5])->father = (Coupling*)D0[7];
         Simulator **D18 = new Simulator* [2];
         Connection **EIC18 = new Connection* [1];
         Connection **EOC18 = new Connection* [1];
         Connection **IC18 = new Connection* [2];
          //D18[0] is  Link
          D18[0] = new Link("Link");
          ((Simulator*)D18[0])->father = (Coupling*)D9[5];
          D18[0]->init(0.0,"of:0000000000000003.egressPort4.link.capacity","of:0000000000000003.egressPort4.link.delay");
          //D18[1] is PriorityQueue
          D18[1] = new Coupling("PriorityQueue");
          ((Simulator*)D18[1])->father = (Coupling*)D9[5];
           Simulator **D19 = new Simulator* [4];
           Connection **EIC19 = new Connection* [2];
           Connection **EOC19 = new Connection* [1];
           Connection **IC19 = new Connection* [4];
            //D19[0] is  prioritySelect
            D19[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D19[0])->father = (Coupling*)D18[1];
            D19[0]->init(0.0);
            //D19[1] is  VQueues
            D19[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D19[1])->father = (Coupling*)D18[1];
            D19[1]->init(0.0,"of:0000000000000003.queueCapacity","-1","of:0000000000000003.numberOfQueues");
            //D19[2] is  WRRScheduler
            D19[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D19[2])->father = (Coupling*)D18[1];
            D19[2]->init(0.0,"of:0000000000000003.numberOfQueues","of:0000000000000003.egressPort4.PriorityQueue.WRRScheduler.weights");
            //D19[3] is  Vect2Scal
            D19[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D19[3])->father = (Coupling*)D18[1];
            D19[3]->init(0.0,"-1");
           EIC19[0] = new Connection();
           EIC19[0]->setup(0,0,0,0);
           EIC19[1] = new Connection();
           EIC19[1]->setup(0,1,2,1);
           EOC19[0] = new Connection();
           EOC19[0]->setup(3,0,0,0);
           IC19[0] = new Connection();
           IC19[0]->setup(0,0,1,0);
           IC19[1] = new Connection();
           IC19[1]->setup(1,1,2,0);
           IC19[2] = new Connection();
           IC19[2]->setup(2,0,1,1);
           IC19[3] = new Connection();
           IC19[3]->setup(1,0,3,0);
          ((Coupling*)D18[1])->setup(D19,4,IC19,4,EIC19,2,EOC19,1);
          ((Coupling*)D18[1])->init(ti);
         EIC18[0] = new Connection();
         EIC18[0]->setup(0,0,1,0);
         EOC18[0] = new Connection();
         EOC18[0]->setup(0,0,0,0);
         IC18[0] = new Connection();
         IC18[0]->setup(1,0,0,0);
         IC18[1] = new Connection();
         IC18[1]->setup(0,0,1,1);
        ((Coupling*)D9[5])->setup(D18,2,IC18,2,EIC18,1,EOC18,1);
        ((Coupling*)D9[5])->init(ti);
       EIC9[0] = new Connection();
       EIC9[0]->setup(0,0,0,0);
       EIC9[1] = new Connection();
       EIC9[1]->setup(0,1,0,1);
       EIC9[2] = new Connection();
       EIC9[2]->setup(0,2,0,2);
       EIC9[3] = new Connection();
       EIC9[3]->setup(0,3,0,3);
       EIC9[4] = new Connection();
       EIC9[4]->setup(0,4,0,4);
       EOC9[0] = new Connection();
       EOC9[0]->setup(1,0,0,0);
       EOC9[1] = new Connection();
       EOC9[1]->setup(2,0,0,1);
       EOC9[2] = new Connection();
       EOC9[2]->setup(3,0,0,2);
       EOC9[3] = new Connection();
       EOC9[3]->setup(4,0,0,3);
       EOC9[4] = new Connection();
       EOC9[4]->setup(5,0,0,4);
       IC9[0] = new Connection();
       IC9[0]->setup(0,0,1,0);
       IC9[1] = new Connection();
       IC9[1]->setup(0,1,2,0);
       IC9[2] = new Connection();
       IC9[2]->setup(0,2,3,0);
       IC9[3] = new Connection();
       IC9[3]->setup(0,3,4,0);
       IC9[4] = new Connection();
       IC9[4]->setup(0,4,5,0);
      ((Coupling*)D0[7])->setup(D9,6,IC9,5,EIC9,5,EOC9,5);
      ((Coupling*)D0[7])->init(ti);
      //D0[8] is of:0000000000000001
      D0[8] = new Coupling("of:0000000000000001");
      ((Simulator*)D0[8])->father = mainCoupling;
       Simulator **D20 = new Simulator* [4];
       Connection **EIC20 = new Connection* [3];
       Connection **EOC20 = new Connection* [3];
       Connection **IC20 = new Connection* [3];
        //D20[0] is  Routing
        D20[0] = new Router("Routing");
        ((Simulator*)D20[0])->father = (Coupling*)D0[8];
        D20[0]->init(0.0);
        //D20[1] is egressPort0
        D20[1] = new Coupling("egressPort0");
        ((Simulator*)D20[1])->father = (Coupling*)D0[8];
         Simulator **D21 = new Simulator* [2];
         Connection **EIC21 = new Connection* [1];
         Connection **EOC21 = new Connection* [1];
         Connection **IC21 = new Connection* [2];
          //D21[0] is  Link
          D21[0] = new Link("Link");
          ((Simulator*)D21[0])->father = (Coupling*)D20[1];
          D21[0]->init(0.0,"of:0000000000000001.egressPort0.link.capacity","of:0000000000000001.egressPort0.link.delay");
          //D21[1] is PriorityQueue
          D21[1] = new Coupling("PriorityQueue");
          ((Simulator*)D21[1])->father = (Coupling*)D20[1];
           Simulator **D22 = new Simulator* [4];
           Connection **EIC22 = new Connection* [2];
           Connection **EOC22 = new Connection* [1];
           Connection **IC22 = new Connection* [4];
            //D22[0] is  prioritySelect
            D22[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D22[0])->father = (Coupling*)D21[1];
            D22[0]->init(0.0);
            //D22[1] is  VQueues
            D22[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D22[1])->father = (Coupling*)D21[1];
            D22[1]->init(0.0,"of:0000000000000001.queueCapacity","-1","of:0000000000000001.numberOfQueues");
            //D22[2] is  WRRScheduler
            D22[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D22[2])->father = (Coupling*)D21[1];
            D22[2]->init(0.0,"of:0000000000000001.numberOfQueues","of:0000000000000001.egressPort0.PriorityQueue.WRRScheduler.weights");
            //D22[3] is  Vect2Scal
            D22[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D22[3])->father = (Coupling*)D21[1];
            D22[3]->init(0.0,"-1");
           EIC22[0] = new Connection();
           EIC22[0]->setup(0,0,0,0);
           EIC22[1] = new Connection();
           EIC22[1]->setup(0,1,2,1);
           EOC22[0] = new Connection();
           EOC22[0]->setup(3,0,0,0);
           IC22[0] = new Connection();
           IC22[0]->setup(0,0,1,0);
           IC22[1] = new Connection();
           IC22[1]->setup(1,1,2,0);
           IC22[2] = new Connection();
           IC22[2]->setup(2,0,1,1);
           IC22[3] = new Connection();
           IC22[3]->setup(1,0,3,0);
          ((Coupling*)D21[1])->setup(D22,4,IC22,4,EIC22,2,EOC22,1);
          ((Coupling*)D21[1])->init(ti);
         EIC21[0] = new Connection();
         EIC21[0]->setup(0,0,1,0);
         EOC21[0] = new Connection();
         EOC21[0]->setup(0,0,0,0);
         IC21[0] = new Connection();
         IC21[0]->setup(1,0,0,0);
         IC21[1] = new Connection();
         IC21[1]->setup(0,0,1,1);
        ((Coupling*)D20[1])->setup(D21,2,IC21,2,EIC21,1,EOC21,1);
        ((Coupling*)D20[1])->init(ti);
        //D20[2] is egressPort1
        D20[2] = new Coupling("egressPort1");
        ((Simulator*)D20[2])->father = (Coupling*)D0[8];
         Simulator **D23 = new Simulator* [2];
         Connection **EIC23 = new Connection* [1];
         Connection **EOC23 = new Connection* [1];
         Connection **IC23 = new Connection* [2];
          //D23[0] is  Link
          D23[0] = new Link("Link");
          ((Simulator*)D23[0])->father = (Coupling*)D20[2];
          D23[0]->init(0.0,"of:0000000000000001.egressPort1.link.capacity","of:0000000000000001.egressPort1.link.delay");
          //D23[1] is PriorityQueue
          D23[1] = new Coupling("PriorityQueue");
          ((Simulator*)D23[1])->father = (Coupling*)D20[2];
           Simulator **D24 = new Simulator* [4];
           Connection **EIC24 = new Connection* [2];
           Connection **EOC24 = new Connection* [1];
           Connection **IC24 = new Connection* [4];
            //D24[0] is  prioritySelect
            D24[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D24[0])->father = (Coupling*)D23[1];
            D24[0]->init(0.0);
            //D24[1] is  VQueues
            D24[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D24[1])->father = (Coupling*)D23[1];
            D24[1]->init(0.0,"of:0000000000000001.queueCapacity","-1","of:0000000000000001.numberOfQueues");
            //D24[2] is  WRRScheduler
            D24[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D24[2])->father = (Coupling*)D23[1];
            D24[2]->init(0.0,"of:0000000000000001.numberOfQueues","of:0000000000000001.egressPort1.PriorityQueue.WRRScheduler.weights");
            //D24[3] is  Vect2Scal
            D24[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D24[3])->father = (Coupling*)D23[1];
            D24[3]->init(0.0,"-1");
           EIC24[0] = new Connection();
           EIC24[0]->setup(0,0,0,0);
           EIC24[1] = new Connection();
           EIC24[1]->setup(0,1,2,1);
           EOC24[0] = new Connection();
           EOC24[0]->setup(3,0,0,0);
           IC24[0] = new Connection();
           IC24[0]->setup(0,0,1,0);
           IC24[1] = new Connection();
           IC24[1]->setup(1,1,2,0);
           IC24[2] = new Connection();
           IC24[2]->setup(2,0,1,1);
           IC24[3] = new Connection();
           IC24[3]->setup(1,0,3,0);
          ((Coupling*)D23[1])->setup(D24,4,IC24,4,EIC24,2,EOC24,1);
          ((Coupling*)D23[1])->init(ti);
         EIC23[0] = new Connection();
         EIC23[0]->setup(0,0,1,0);
         EOC23[0] = new Connection();
         EOC23[0]->setup(0,0,0,0);
         IC23[0] = new Connection();
         IC23[0]->setup(1,0,0,0);
         IC23[1] = new Connection();
         IC23[1]->setup(0,0,1,1);
        ((Coupling*)D20[2])->setup(D23,2,IC23,2,EIC23,1,EOC23,1);
        ((Coupling*)D20[2])->init(ti);
        //D20[3] is egressPort2
        D20[3] = new Coupling("egressPort2");
        ((Simulator*)D20[3])->father = (Coupling*)D0[8];
         Simulator **D25 = new Simulator* [2];
         Connection **EIC25 = new Connection* [1];
         Connection **EOC25 = new Connection* [1];
         Connection **IC25 = new Connection* [2];
          //D25[0] is  Link
          D25[0] = new Link("Link");
          ((Simulator*)D25[0])->father = (Coupling*)D20[3];
          D25[0]->init(0.0,"of:0000000000000001.egressPort2.link.capacity","of:0000000000000001.egressPort2.link.delay");
          //D25[1] is PriorityQueue
          D25[1] = new Coupling("PriorityQueue");
          ((Simulator*)D25[1])->father = (Coupling*)D20[3];
           Simulator **D26 = new Simulator* [4];
           Connection **EIC26 = new Connection* [2];
           Connection **EOC26 = new Connection* [1];
           Connection **IC26 = new Connection* [4];
            //D26[0] is  prioritySelect
            D26[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D26[0])->father = (Coupling*)D25[1];
            D26[0]->init(0.0);
            //D26[1] is  VQueues
            D26[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D26[1])->father = (Coupling*)D25[1];
            D26[1]->init(0.0,"of:0000000000000001.queueCapacity","-1","of:0000000000000001.numberOfQueues");
            //D26[2] is  WRRScheduler
            D26[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D26[2])->father = (Coupling*)D25[1];
            D26[2]->init(0.0,"of:0000000000000001.numberOfQueues","of:0000000000000001.egressPort2.PriorityQueue.WRRScheduler.weights");
            //D26[3] is  Vect2Scal
            D26[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D26[3])->father = (Coupling*)D25[1];
            D26[3]->init(0.0,"-1");
           EIC26[0] = new Connection();
           EIC26[0]->setup(0,0,0,0);
           EIC26[1] = new Connection();
           EIC26[1]->setup(0,1,2,1);
           EOC26[0] = new Connection();
           EOC26[0]->setup(3,0,0,0);
           IC26[0] = new Connection();
           IC26[0]->setup(0,0,1,0);
           IC26[1] = new Connection();
           IC26[1]->setup(1,1,2,0);
           IC26[2] = new Connection();
           IC26[2]->setup(2,0,1,1);
           IC26[3] = new Connection();
           IC26[3]->setup(1,0,3,0);
          ((Coupling*)D25[1])->setup(D26,4,IC26,4,EIC26,2,EOC26,1);
          ((Coupling*)D25[1])->init(ti);
         EIC25[0] = new Connection();
         EIC25[0]->setup(0,0,1,0);
         EOC25[0] = new Connection();
         EOC25[0]->setup(0,0,0,0);
         IC25[0] = new Connection();
         IC25[0]->setup(1,0,0,0);
         IC25[1] = new Connection();
         IC25[1]->setup(0,0,1,1);
        ((Coupling*)D20[3])->setup(D25,2,IC25,2,EIC25,1,EOC25,1);
        ((Coupling*)D20[3])->init(ti);
       EIC20[0] = new Connection();
       EIC20[0]->setup(0,0,0,0);
       EIC20[1] = new Connection();
       EIC20[1]->setup(0,1,0,1);
       EIC20[2] = new Connection();
       EIC20[2]->setup(0,2,0,2);
       EOC20[0] = new Connection();
       EOC20[0]->setup(1,0,0,0);
       EOC20[1] = new Connection();
       EOC20[1]->setup(2,0,0,1);
       EOC20[2] = new Connection();
       EOC20[2]->setup(3,0,0,2);
       IC20[0] = new Connection();
       IC20[0]->setup(0,0,1,0);
       IC20[1] = new Connection();
       IC20[1]->setup(0,1,2,0);
       IC20[2] = new Connection();
       IC20[2]->setup(0,2,3,0);
      ((Coupling*)D0[8])->setup(D20,4,IC20,3,EIC20,3,EOC20,3);
      ((Coupling*)D0[8])->init(ti);
      //D0[9] is of:0000000000000002
      D0[9] = new Coupling("of:0000000000000002");
      ((Simulator*)D0[9])->father = mainCoupling;
       Simulator **D27 = new Simulator* [6];
       Connection **EIC27 = new Connection* [5];
       Connection **EOC27 = new Connection* [5];
       Connection **IC27 = new Connection* [5];
        //D27[0] is  Routing
        D27[0] = new Router("Routing");
        ((Simulator*)D27[0])->father = (Coupling*)D0[9];
        D27[0]->init(0.0);
        //D27[1] is egressPort0
        D27[1] = new Coupling("egressPort0");
        ((Simulator*)D27[1])->father = (Coupling*)D0[9];
         Simulator **D28 = new Simulator* [2];
         Connection **EIC28 = new Connection* [1];
         Connection **EOC28 = new Connection* [1];
         Connection **IC28 = new Connection* [2];
          //D28[0] is  Link
          D28[0] = new Link("Link");
          ((Simulator*)D28[0])->father = (Coupling*)D27[1];
          D28[0]->init(0.0,"of:0000000000000002.egressPort0.link.capacity","of:0000000000000002.egressPort0.link.delay");
          //D28[1] is PriorityQueue
          D28[1] = new Coupling("PriorityQueue");
          ((Simulator*)D28[1])->father = (Coupling*)D27[1];
           Simulator **D29 = new Simulator* [4];
           Connection **EIC29 = new Connection* [2];
           Connection **EOC29 = new Connection* [1];
           Connection **IC29 = new Connection* [4];
            //D29[0] is  prioritySelect
            D29[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D29[0])->father = (Coupling*)D28[1];
            D29[0]->init(0.0);
            //D29[1] is  VQueues
            D29[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D29[1])->father = (Coupling*)D28[1];
            D29[1]->init(0.0,"of:0000000000000002.queueCapacity","-1","of:0000000000000002.numberOfQueues");
            //D29[2] is  WRRScheduler
            D29[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D29[2])->father = (Coupling*)D28[1];
            D29[2]->init(0.0,"of:0000000000000002.numberOfQueues","of:0000000000000002.egressPort0.PriorityQueue.WRRScheduler.weights");
            //D29[3] is  Vect2Scal
            D29[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D29[3])->father = (Coupling*)D28[1];
            D29[3]->init(0.0,"-1");
           EIC29[0] = new Connection();
           EIC29[0]->setup(0,0,0,0);
           EIC29[1] = new Connection();
           EIC29[1]->setup(0,1,2,1);
           EOC29[0] = new Connection();
           EOC29[0]->setup(3,0,0,0);
           IC29[0] = new Connection();
           IC29[0]->setup(0,0,1,0);
           IC29[1] = new Connection();
           IC29[1]->setup(1,1,2,0);
           IC29[2] = new Connection();
           IC29[2]->setup(2,0,1,1);
           IC29[3] = new Connection();
           IC29[3]->setup(1,0,3,0);
          ((Coupling*)D28[1])->setup(D29,4,IC29,4,EIC29,2,EOC29,1);
          ((Coupling*)D28[1])->init(ti);
         EIC28[0] = new Connection();
         EIC28[0]->setup(0,0,1,0);
         EOC28[0] = new Connection();
         EOC28[0]->setup(0,0,0,0);
         IC28[0] = new Connection();
         IC28[0]->setup(1,0,0,0);
         IC28[1] = new Connection();
         IC28[1]->setup(0,0,1,1);
        ((Coupling*)D27[1])->setup(D28,2,IC28,2,EIC28,1,EOC28,1);
        ((Coupling*)D27[1])->init(ti);
        //D27[2] is egressPort1
        D27[2] = new Coupling("egressPort1");
        ((Simulator*)D27[2])->father = (Coupling*)D0[9];
         Simulator **D30 = new Simulator* [2];
         Connection **EIC30 = new Connection* [1];
         Connection **EOC30 = new Connection* [1];
         Connection **IC30 = new Connection* [2];
          //D30[0] is  Link
          D30[0] = new Link("Link");
          ((Simulator*)D30[0])->father = (Coupling*)D27[2];
          D30[0]->init(0.0,"of:0000000000000002.egressPort1.link.capacity","of:0000000000000002.egressPort1.link.delay");
          //D30[1] is PriorityQueue
          D30[1] = new Coupling("PriorityQueue");
          ((Simulator*)D30[1])->father = (Coupling*)D27[2];
           Simulator **D31 = new Simulator* [4];
           Connection **EIC31 = new Connection* [2];
           Connection **EOC31 = new Connection* [1];
           Connection **IC31 = new Connection* [4];
            //D31[0] is  prioritySelect
            D31[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D31[0])->father = (Coupling*)D30[1];
            D31[0]->init(0.0);
            //D31[1] is  VQueues
            D31[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D31[1])->father = (Coupling*)D30[1];
            D31[1]->init(0.0,"of:0000000000000002.queueCapacity","-1","of:0000000000000002.numberOfQueues");
            //D31[2] is  WRRScheduler
            D31[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D31[2])->father = (Coupling*)D30[1];
            D31[2]->init(0.0,"of:0000000000000002.numberOfQueues","of:0000000000000002.egressPort1.PriorityQueue.WRRScheduler.weights");
            //D31[3] is  Vect2Scal
            D31[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D31[3])->father = (Coupling*)D30[1];
            D31[3]->init(0.0,"-1");
           EIC31[0] = new Connection();
           EIC31[0]->setup(0,0,0,0);
           EIC31[1] = new Connection();
           EIC31[1]->setup(0,1,2,1);
           EOC31[0] = new Connection();
           EOC31[0]->setup(3,0,0,0);
           IC31[0] = new Connection();
           IC31[0]->setup(0,0,1,0);
           IC31[1] = new Connection();
           IC31[1]->setup(1,1,2,0);
           IC31[2] = new Connection();
           IC31[2]->setup(2,0,1,1);
           IC31[3] = new Connection();
           IC31[3]->setup(1,0,3,0);
          ((Coupling*)D30[1])->setup(D31,4,IC31,4,EIC31,2,EOC31,1);
          ((Coupling*)D30[1])->init(ti);
         EIC30[0] = new Connection();
         EIC30[0]->setup(0,0,1,0);
         EOC30[0] = new Connection();
         EOC30[0]->setup(0,0,0,0);
         IC30[0] = new Connection();
         IC30[0]->setup(1,0,0,0);
         IC30[1] = new Connection();
         IC30[1]->setup(0,0,1,1);
        ((Coupling*)D27[2])->setup(D30,2,IC30,2,EIC30,1,EOC30,1);
        ((Coupling*)D27[2])->init(ti);
        //D27[3] is egressPort2
        D27[3] = new Coupling("egressPort2");
        ((Simulator*)D27[3])->father = (Coupling*)D0[9];
         Simulator **D32 = new Simulator* [2];
         Connection **EIC32 = new Connection* [1];
         Connection **EOC32 = new Connection* [1];
         Connection **IC32 = new Connection* [2];
          //D32[0] is  Link
          D32[0] = new Link("Link");
          ((Simulator*)D32[0])->father = (Coupling*)D27[3];
          D32[0]->init(0.0,"of:0000000000000002.egressPort2.link.capacity","of:0000000000000002.egressPort2.link.delay");
          //D32[1] is PriorityQueue
          D32[1] = new Coupling("PriorityQueue");
          ((Simulator*)D32[1])->father = (Coupling*)D27[3];
           Simulator **D33 = new Simulator* [4];
           Connection **EIC33 = new Connection* [2];
           Connection **EOC33 = new Connection* [1];
           Connection **IC33 = new Connection* [4];
            //D33[0] is  prioritySelect
            D33[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D33[0])->father = (Coupling*)D32[1];
            D33[0]->init(0.0);
            //D33[1] is  VQueues
            D33[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D33[1])->father = (Coupling*)D32[1];
            D33[1]->init(0.0,"of:0000000000000002.queueCapacity","-1","of:0000000000000002.numberOfQueues");
            //D33[2] is  WRRScheduler
            D33[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D33[2])->father = (Coupling*)D32[1];
            D33[2]->init(0.0,"of:0000000000000002.numberOfQueues","of:0000000000000002.egressPort2.PriorityQueue.WRRScheduler.weights");
            //D33[3] is  Vect2Scal
            D33[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D33[3])->father = (Coupling*)D32[1];
            D33[3]->init(0.0,"-1");
           EIC33[0] = new Connection();
           EIC33[0]->setup(0,0,0,0);
           EIC33[1] = new Connection();
           EIC33[1]->setup(0,1,2,1);
           EOC33[0] = new Connection();
           EOC33[0]->setup(3,0,0,0);
           IC33[0] = new Connection();
           IC33[0]->setup(0,0,1,0);
           IC33[1] = new Connection();
           IC33[1]->setup(1,1,2,0);
           IC33[2] = new Connection();
           IC33[2]->setup(2,0,1,1);
           IC33[3] = new Connection();
           IC33[3]->setup(1,0,3,0);
          ((Coupling*)D32[1])->setup(D33,4,IC33,4,EIC33,2,EOC33,1);
          ((Coupling*)D32[1])->init(ti);
         EIC32[0] = new Connection();
         EIC32[0]->setup(0,0,1,0);
         EOC32[0] = new Connection();
         EOC32[0]->setup(0,0,0,0);
         IC32[0] = new Connection();
         IC32[0]->setup(1,0,0,0);
         IC32[1] = new Connection();
         IC32[1]->setup(0,0,1,1);
        ((Coupling*)D27[3])->setup(D32,2,IC32,2,EIC32,1,EOC32,1);
        ((Coupling*)D27[3])->init(ti);
        //D27[4] is egressPort3
        D27[4] = new Coupling("egressPort3");
        ((Simulator*)D27[4])->father = (Coupling*)D0[9];
         Simulator **D34 = new Simulator* [2];
         Connection **EIC34 = new Connection* [1];
         Connection **EOC34 = new Connection* [1];
         Connection **IC34 = new Connection* [2];
          //D34[0] is  Link
          D34[0] = new Link("Link");
          ((Simulator*)D34[0])->father = (Coupling*)D27[4];
          D34[0]->init(0.0,"of:0000000000000002.egressPort3.link.capacity","of:0000000000000002.egressPort3.link.delay");
          //D34[1] is PriorityQueue
          D34[1] = new Coupling("PriorityQueue");
          ((Simulator*)D34[1])->father = (Coupling*)D27[4];
           Simulator **D35 = new Simulator* [4];
           Connection **EIC35 = new Connection* [2];
           Connection **EOC35 = new Connection* [1];
           Connection **IC35 = new Connection* [4];
            //D35[0] is  prioritySelect
            D35[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D35[0])->father = (Coupling*)D34[1];
            D35[0]->init(0.0);
            //D35[1] is  VQueues
            D35[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D35[1])->father = (Coupling*)D34[1];
            D35[1]->init(0.0,"of:0000000000000002.queueCapacity","-1","of:0000000000000002.numberOfQueues");
            //D35[2] is  WRRScheduler
            D35[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D35[2])->father = (Coupling*)D34[1];
            D35[2]->init(0.0,"of:0000000000000002.numberOfQueues","of:0000000000000002.egressPort3.PriorityQueue.WRRScheduler.weights");
            //D35[3] is  Vect2Scal
            D35[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D35[3])->father = (Coupling*)D34[1];
            D35[3]->init(0.0,"-1");
           EIC35[0] = new Connection();
           EIC35[0]->setup(0,0,0,0);
           EIC35[1] = new Connection();
           EIC35[1]->setup(0,1,2,1);
           EOC35[0] = new Connection();
           EOC35[0]->setup(3,0,0,0);
           IC35[0] = new Connection();
           IC35[0]->setup(0,0,1,0);
           IC35[1] = new Connection();
           IC35[1]->setup(1,1,2,0);
           IC35[2] = new Connection();
           IC35[2]->setup(2,0,1,1);
           IC35[3] = new Connection();
           IC35[3]->setup(1,0,3,0);
          ((Coupling*)D34[1])->setup(D35,4,IC35,4,EIC35,2,EOC35,1);
          ((Coupling*)D34[1])->init(ti);
         EIC34[0] = new Connection();
         EIC34[0]->setup(0,0,1,0);
         EOC34[0] = new Connection();
         EOC34[0]->setup(0,0,0,0);
         IC34[0] = new Connection();
         IC34[0]->setup(1,0,0,0);
         IC34[1] = new Connection();
         IC34[1]->setup(0,0,1,1);
        ((Coupling*)D27[4])->setup(D34,2,IC34,2,EIC34,1,EOC34,1);
        ((Coupling*)D27[4])->init(ti);
        //D27[5] is egressPort4
        D27[5] = new Coupling("egressPort4");
        ((Simulator*)D27[5])->father = (Coupling*)D0[9];
         Simulator **D36 = new Simulator* [2];
         Connection **EIC36 = new Connection* [1];
         Connection **EOC36 = new Connection* [1];
         Connection **IC36 = new Connection* [2];
          //D36[0] is  Link
          D36[0] = new Link("Link");
          ((Simulator*)D36[0])->father = (Coupling*)D27[5];
          D36[0]->init(0.0,"of:0000000000000002.egressPort4.link.capacity","of:0000000000000002.egressPort4.link.delay");
          //D36[1] is PriorityQueue
          D36[1] = new Coupling("PriorityQueue");
          ((Simulator*)D36[1])->father = (Coupling*)D27[5];
           Simulator **D37 = new Simulator* [4];
           Connection **EIC37 = new Connection* [2];
           Connection **EOC37 = new Connection* [1];
           Connection **IC37 = new Connection* [4];
            //D37[0] is  prioritySelect
            D37[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D37[0])->father = (Coupling*)D36[1];
            D37[0]->init(0.0);
            //D37[1] is  VQueues
            D37[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D37[1])->father = (Coupling*)D36[1];
            D37[1]->init(0.0,"of:0000000000000002.queueCapacity","-1","of:0000000000000002.numberOfQueues");
            //D37[2] is  WRRScheduler
            D37[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D37[2])->father = (Coupling*)D36[1];
            D37[2]->init(0.0,"of:0000000000000002.numberOfQueues","of:0000000000000002.egressPort4.PriorityQueue.WRRScheduler.weights");
            //D37[3] is  Vect2Scal
            D37[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D37[3])->father = (Coupling*)D36[1];
            D37[3]->init(0.0,"-1");
           EIC37[0] = new Connection();
           EIC37[0]->setup(0,0,0,0);
           EIC37[1] = new Connection();
           EIC37[1]->setup(0,1,2,1);
           EOC37[0] = new Connection();
           EOC37[0]->setup(3,0,0,0);
           IC37[0] = new Connection();
           IC37[0]->setup(0,0,1,0);
           IC37[1] = new Connection();
           IC37[1]->setup(1,1,2,0);
           IC37[2] = new Connection();
           IC37[2]->setup(2,0,1,1);
           IC37[3] = new Connection();
           IC37[3]->setup(1,0,3,0);
          ((Coupling*)D36[1])->setup(D37,4,IC37,4,EIC37,2,EOC37,1);
          ((Coupling*)D36[1])->init(ti);
         EIC36[0] = new Connection();
         EIC36[0]->setup(0,0,1,0);
         EOC36[0] = new Connection();
         EOC36[0]->setup(0,0,0,0);
         IC36[0] = new Connection();
         IC36[0]->setup(1,0,0,0);
         IC36[1] = new Connection();
         IC36[1]->setup(0,0,1,1);
        ((Coupling*)D27[5])->setup(D36,2,IC36,2,EIC36,1,EOC36,1);
        ((Coupling*)D27[5])->init(ti);
       EIC27[0] = new Connection();
       EIC27[0]->setup(0,0,0,0);
       EIC27[1] = new Connection();
       EIC27[1]->setup(0,1,0,1);
       EIC27[2] = new Connection();
       EIC27[2]->setup(0,2,0,2);
       EIC27[3] = new Connection();
       EIC27[3]->setup(0,3,0,3);
       EIC27[4] = new Connection();
       EIC27[4]->setup(0,4,0,4);
       EOC27[0] = new Connection();
       EOC27[0]->setup(1,0,0,0);
       EOC27[1] = new Connection();
       EOC27[1]->setup(2,0,0,1);
       EOC27[2] = new Connection();
       EOC27[2]->setup(3,0,0,2);
       EOC27[3] = new Connection();
       EOC27[3]->setup(4,0,0,3);
       EOC27[4] = new Connection();
       EOC27[4]->setup(5,0,0,4);
       IC27[0] = new Connection();
       IC27[0]->setup(0,0,1,0);
       IC27[1] = new Connection();
       IC27[1]->setup(0,1,2,0);
       IC27[2] = new Connection();
       IC27[2]->setup(0,2,3,0);
       IC27[3] = new Connection();
       IC27[3]->setup(0,3,4,0);
       IC27[4] = new Connection();
       IC27[4]->setup(0,4,5,0);
      ((Coupling*)D0[9])->setup(D27,6,IC27,5,EIC27,5,EOC27,5);
      ((Coupling*)D0[9])->init(ti);
      //D0[10] is 9A:4A:43:D4:36:45/-1
      D0[10] = new Coupling("9A:4A:43:D4:36:45/-1");
      ((Simulator*)D0[10])->father = mainCoupling;
       Simulator **D38 = new Simulator* [4];
       Connection **EIC38 = new Connection* [0];
       Connection **EOC38 = new Connection* [1];
       Connection **IC38 = new Connection* [2];
        //D38[0] is  GeneratorApplication
        D38[0] = new FelixServer("GeneratorApplication");
        ((Simulator*)D38[0])->father = (Coupling*)D0[10];
        D38[0]->init(0.0);
        //D38[1] is  Routing
        D38[1] = new Router("Routing");
        ((Simulator*)D38[1])->father = (Coupling*)D0[10];
        D38[1]->init(0.0);
        //D38[2] is egressPort0
        D38[2] = new Coupling("egressPort0");
        ((Simulator*)D38[2])->father = (Coupling*)D0[10];
         Simulator **D39 = new Simulator* [2];
         Connection **EIC39 = new Connection* [1];
         Connection **EOC39 = new Connection* [1];
         Connection **IC39 = new Connection* [2];
          //D39[0] is  portQueue
          D39[0] = new NetworkQueue("portQueue");
          ((Simulator*)D39[0])->father = (Coupling*)D38[2];
          D39[0]->init(0.0,"9A:4A:43:D4:36:45/-1.egressPort.queueCapacity","-1");
          //D39[1] is  Link
          D39[1] = new Link("Link");
          ((Simulator*)D39[1])->father = (Coupling*)D38[2];
          D39[1]->init(0.0,"9A:4A:43:D4:36:45/-1.egressPort0.link.capacity","9A:4A:43:D4:36:45/-1.egressPort0.link.delay");
         EIC39[0] = new Connection();
         EIC39[0]->setup(0,0,0,0);
         EOC39[0] = new Connection();
         EOC39[0]->setup(1,0,0,0);
         IC39[0] = new Connection();
         IC39[0]->setup(0,0,1,0);
         IC39[1] = new Connection();
         IC39[1]->setup(1,0,0,1);
        ((Coupling*)D38[2])->setup(D39,2,IC39,2,EIC39,1,EOC39,1);
        ((Coupling*)D38[2])->init(ti);
        //D38[3] is  ReceivingApplication
        D38[3] = new Packetsink("ReceivingApplication");
        ((Simulator*)D38[3])->father = (Coupling*)D0[10];
        D38[3]->init(0.0);
       EOC38[0] = new Connection();
       EOC38[0]->setup(2,0,0,0);
       IC38[0] = new Connection();
       IC38[0]->setup(0,0,1,0);
       IC38[1] = new Connection();
       IC38[1]->setup(1,0,2,0);
      ((Coupling*)D0[10])->setup(D38,4,IC38,2,EIC38,0,EOC38,1);
      ((Coupling*)D0[10])->init(ti);
      //D0[11] is 72:D2:0D:24:C5:36/-1
      D0[11] = new Coupling("72:D2:0D:24:C5:36/-1");
      ((Simulator*)D0[11])->father = mainCoupling;
       Simulator **D40 = new Simulator* [4];
       Connection **EIC40 = new Connection* [0];
       Connection **EOC40 = new Connection* [1];
       Connection **IC40 = new Connection* [2];
        //D40[0] is  GeneratorApplication
        D40[0] = new FelixServer("GeneratorApplication");
        ((Simulator*)D40[0])->father = (Coupling*)D0[11];
        D40[0]->init(0.0);
        //D40[1] is  Routing
        D40[1] = new Router("Routing");
        ((Simulator*)D40[1])->father = (Coupling*)D0[11];
        D40[1]->init(0.0);
        //D40[2] is egressPort0
        D40[2] = new Coupling("egressPort0");
        ((Simulator*)D40[2])->father = (Coupling*)D0[11];
         Simulator **D41 = new Simulator* [2];
         Connection **EIC41 = new Connection* [1];
         Connection **EOC41 = new Connection* [1];
         Connection **IC41 = new Connection* [2];
          //D41[0] is  portQueue
          D41[0] = new NetworkQueue("portQueue");
          ((Simulator*)D41[0])->father = (Coupling*)D40[2];
          D41[0]->init(0.0,"72:D2:0D:24:C5:36/-1.egressPort.queueCapacity","-1");
          //D41[1] is  Link
          D41[1] = new Link("Link");
          ((Simulator*)D41[1])->father = (Coupling*)D40[2];
          D41[1]->init(0.0,"72:D2:0D:24:C5:36/-1.egressPort0.link.capacity","72:D2:0D:24:C5:36/-1.egressPort0.link.delay");
         EIC41[0] = new Connection();
         EIC41[0]->setup(0,0,0,0);
         EOC41[0] = new Connection();
         EOC41[0]->setup(1,0,0,0);
         IC41[0] = new Connection();
         IC41[0]->setup(0,0,1,0);
         IC41[1] = new Connection();
         IC41[1]->setup(1,0,0,1);
        ((Coupling*)D40[2])->setup(D41,2,IC41,2,EIC41,1,EOC41,1);
        ((Coupling*)D40[2])->init(ti);
        //D40[3] is  ReceivingApplication
        D40[3] = new Packetsink("ReceivingApplication");
        ((Simulator*)D40[3])->father = (Coupling*)D0[11];
        D40[3]->init(0.0);
       EOC40[0] = new Connection();
       EOC40[0]->setup(2,0,0,0);
       IC40[0] = new Connection();
       IC40[0]->setup(0,0,1,0);
       IC40[1] = new Connection();
       IC40[1]->setup(1,0,2,0);
      ((Coupling*)D0[11])->setup(D40,4,IC40,2,EIC40,0,EOC40,1);
      ((Coupling*)D0[11])->init(ti);
      //D0[12] is  FinalizationCommands
      D0[12] = new multipleSimulationCommands("FinalizationCommands");
      ((Simulator*)D0[12])->father = mainCoupling;
      D0[12]->init(0.0,"/home/alaurito/.haikunet/debug/host_to_host.hk/Scilab/firstSimulation.sce","/home/alaurito/.haikunet/debug/host_to_host.hk/Scilab/eachSimulation.sce","/home/alaurito/.haikunet/debug/host_to_host.hk/Scilab/lastSimulation.sce");
      IC0[0] = new Connection();
      IC0[0]->setup(3,0,9,1);
      IC0[1] = new Connection();
      IC0[1]->setup(9,1,3,0);
      IC0[2] = new Connection();
      IC0[2]->setup(4,0,7,1);
      IC0[3] = new Connection();
      IC0[3]->setup(7,1,4,0);
      IC0[4] = new Connection();
      IC0[4]->setup(5,0,7,2);
      IC0[5] = new Connection();
      IC0[5]->setup(7,2,5,0);
      IC0[6] = new Connection();
      IC0[6]->setup(6,0,9,2);
      IC0[7] = new Connection();
      IC0[7]->setup(9,2,6,0);
      IC0[8] = new Connection();
      IC0[8]->setup(7,3,8,2);
      IC0[9] = new Connection();
      IC0[9]->setup(8,1,9,3);
      IC0[10] = new Connection();
      IC0[10]->setup(9,3,8,1);
      IC0[11] = new Connection();
      IC0[11]->setup(8,2,7,3);
      IC0[12] = new Connection();
      IC0[12]->setup(10,0,9,4);
      IC0[13] = new Connection();
      IC0[13]->setup(9,4,10,0);
      IC0[14] = new Connection();
      IC0[14]->setup(11,0,7,4);
      IC0[15] = new Connection();
      IC0[15]->setup(7,4,11,0);
      mainCoupling->setup(D0,13,IC0,16);
      ((RootCoupling*)mainCoupling)->rootSim = this;
    }
  };
