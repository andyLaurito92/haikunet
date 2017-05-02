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
      Simulator **D0 = new Simulator* [8];
      Connection **IC0 = new Connection* [6];
      //D0[0] is  LoadScilabParams
      D0[0] = new RunScilabJob("LoadScilabParams");
      ((Simulator*)D0[0])->father = mainCoupling;
      D0[0]->init(0.0,"exec('/home/andylaurito/.haikunet/debug/simulation_debug_2017-05-02_17:10:44_UTC/Scilab/model.scilabParams', 0)","","");
      //D0[1] is  ExperimenetTracker
      D0[1] = new SimulationExperimentTracker("ExperimenetTracker");
      ((Simulator*)D0[1])->father = mainCoupling;
      D0[1]->init(0.0,"SimulationName","ExperimentNumber","ParameterValues","ParameterNames");
      //D0[2] is  UpdateScilabParams
      D0[2] = new RunScilabJob("UpdateScilabParams");
      ((Simulator*)D0[2])->father = mainCoupling;
      D0[2]->init(0.0,"","","");
      //D0[3] is Host1
      D0[3] = new Coupling("Host1");
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
          D2[0]->init(0.0,"Host1.egressPort.queueCapacity","-1");
          //D2[1] is  Link
          D2[1] = new Link("Link");
          ((Simulator*)D2[1])->father = (Coupling*)D1[2];
          D2[1]->init(0.0,"Host1.egressPort0.link.capacity","Host1.egressPort0.link.delay");
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
      //D0[4] is Host2
      D0[4] = new Coupling("Host2");
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
          D4[0]->init(0.0,"Host2.egressPort.queueCapacity","-1");
          //D4[1] is  Link
          D4[1] = new Link("Link");
          ((Simulator*)D4[1])->father = (Coupling*)D3[2];
          D4[1]->init(0.0,"Host2.egressPort0.link.capacity","Host2.egressPort0.link.delay");
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
      //D0[5] is Host3
      D0[5] = new Coupling("Host3");
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
          D6[0]->init(0.0,"Host3.egressPort.queueCapacity","-1");
          //D6[1] is  Link
          D6[1] = new Link("Link");
          ((Simulator*)D6[1])->father = (Coupling*)D5[2];
          D6[1]->init(0.0,"Host3.egressPort0.link.capacity","Host3.egressPort0.link.delay");
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
      //D0[6] is Router1
      D0[6] = new Coupling("Router1");
      ((Simulator*)D0[6])->father = mainCoupling;
       Simulator **D7 = new Simulator* [4];
       Connection **EIC7 = new Connection* [3];
       Connection **EOC7 = new Connection* [3];
       Connection **IC7 = new Connection* [3];
        //D7[0] is  Routing
        D7[0] = new Router("Routing");
        ((Simulator*)D7[0])->father = (Coupling*)D0[6];
        D7[0]->init(0.0);
        //D7[1] is egressPort0
        D7[1] = new Coupling("egressPort0");
        ((Simulator*)D7[1])->father = (Coupling*)D0[6];
         Simulator **D8 = new Simulator* [2];
         Connection **EIC8 = new Connection* [1];
         Connection **EOC8 = new Connection* [1];
         Connection **IC8 = new Connection* [2];
          //D8[0] is  Link
          D8[0] = new Link("Link");
          ((Simulator*)D8[0])->father = (Coupling*)D7[1];
          D8[0]->init(0.0,"Router1.egressPort0.link.capacity","Router1.egressPort0.link.delay");
          //D8[1] is PriorityQueue
          D8[1] = new Coupling("PriorityQueue");
          ((Simulator*)D8[1])->father = (Coupling*)D7[1];
           Simulator **D9 = new Simulator* [4];
           Connection **EIC9 = new Connection* [2];
           Connection **EOC9 = new Connection* [1];
           Connection **IC9 = new Connection* [4];
            //D9[0] is  prioritySelect
            D9[0] = new PacketPrioritySelect("prioritySelect");
            ((Simulator*)D9[0])->father = (Coupling*)D8[1];
            D9[0]->init(0.0);
            //D9[1] is  VQueues
            D9[1] = new Vector_NetworkQueueWithSizeOutput("VQueues");
            ((Simulator*)D9[1])->father = (Coupling*)D8[1];
            D9[1]->init(0.0,"Router1.queueCapacity","-1","Router1.numberOfQueues");
            //D9[2] is  WRRScheduler
            D9[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D9[2])->father = (Coupling*)D8[1];
            D9[2]->init(0.0,"Router1.numberOfQueues","Router1.egressPort0.PriorityQueue.WRRScheduler.weights");
            //D9[3] is  Vect2Scal
            D9[3] = new Vector2Scalar("Vect2Scal");
            ((Simulator*)D9[3])->father = (Coupling*)D8[1];
            D9[3]->init(0.0,"-1");
           EIC9[0] = new Connection();
           EIC9[0]->setup(0,0,0,0);
           EIC9[1] = new Connection();
           EIC9[1]->setup(0,1,2,1);
           EOC9[0] = new Connection();
           EOC9[0]->setup(3,0,0,0);
           IC9[0] = new Connection();
           IC9[0]->setup(0,0,1,0);
           IC9[1] = new Connection();
           IC9[1]->setup(1,1,2,0);
           IC9[2] = new Connection();
           IC9[2]->setup(2,0,1,1);
           IC9[3] = new Connection();
           IC9[3]->setup(1,0,3,0);
          ((Coupling*)D8[1])->setup(D9,4,IC9,4,EIC9,2,EOC9,1);
          ((Coupling*)D8[1])->init(ti);
         EIC8[0] = new Connection();
         EIC8[0]->setup(0,0,1,0);
         EOC8[0] = new Connection();
         EOC8[0]->setup(0,0,0,0);
         IC8[0] = new Connection();
         IC8[0]->setup(1,0,0,0);
         IC8[1] = new Connection();
         IC8[1]->setup(0,0,1,1);
        ((Coupling*)D7[1])->setup(D8,2,IC8,2,EIC8,1,EOC8,1);
        ((Coupling*)D7[1])->init(ti);
        //D7[2] is egressPort1
        D7[2] = new Coupling("egressPort1");
        ((Simulator*)D7[2])->father = (Coupling*)D0[6];
         Simulator **D10 = new Simulator* [2];
         Connection **EIC10 = new Connection* [1];
         Connection **EOC10 = new Connection* [1];
         Connection **IC10 = new Connection* [2];
          //D10[0] is  Link
          D10[0] = new Link("Link");
          ((Simulator*)D10[0])->father = (Coupling*)D7[2];
          D10[0]->init(0.0,"Router1.egressPort1.link.capacity","Router1.egressPort1.link.delay");
          //D10[1] is PriorityQueue
          D10[1] = new Coupling("PriorityQueue");
          ((Simulator*)D10[1])->father = (Coupling*)D7[2];
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
            D11[1]->init(0.0,"Router1.queueCapacity","-1","Router1.numberOfQueues");
            //D11[2] is  WRRScheduler
            D11[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D11[2])->father = (Coupling*)D10[1];
            D11[2]->init(0.0,"Router1.numberOfQueues","Router1.egressPort1.PriorityQueue.WRRScheduler.weights");
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
        ((Coupling*)D7[2])->setup(D10,2,IC10,2,EIC10,1,EOC10,1);
        ((Coupling*)D7[2])->init(ti);
        //D7[3] is egressPort2
        D7[3] = new Coupling("egressPort2");
        ((Simulator*)D7[3])->father = (Coupling*)D0[6];
         Simulator **D12 = new Simulator* [2];
         Connection **EIC12 = new Connection* [1];
         Connection **EOC12 = new Connection* [1];
         Connection **IC12 = new Connection* [2];
          //D12[0] is  Link
          D12[0] = new Link("Link");
          ((Simulator*)D12[0])->father = (Coupling*)D7[3];
          D12[0]->init(0.0,"Router1.egressPort2.link.capacity","Router1.egressPort2.link.delay");
          //D12[1] is PriorityQueue
          D12[1] = new Coupling("PriorityQueue");
          ((Simulator*)D12[1])->father = (Coupling*)D7[3];
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
            D13[1]->init(0.0,"Router1.queueCapacity","-1","Router1.numberOfQueues");
            //D13[2] is  WRRScheduler
            D13[2] = new WRRScheduler("WRRScheduler");
            ((Simulator*)D13[2])->father = (Coupling*)D12[1];
            D13[2]->init(0.0,"Router1.numberOfQueues","Router1.egressPort2.PriorityQueue.WRRScheduler.weights");
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
        ((Coupling*)D7[3])->setup(D12,2,IC12,2,EIC12,1,EOC12,1);
        ((Coupling*)D7[3])->init(ti);
       EIC7[0] = new Connection();
       EIC7[0]->setup(0,0,0,0);
       EIC7[1] = new Connection();
       EIC7[1]->setup(0,1,0,1);
       EIC7[2] = new Connection();
       EIC7[2]->setup(0,2,0,2);
       EOC7[0] = new Connection();
       EOC7[0]->setup(1,0,0,0);
       EOC7[1] = new Connection();
       EOC7[1]->setup(2,0,0,1);
       EOC7[2] = new Connection();
       EOC7[2]->setup(3,0,0,2);
       IC7[0] = new Connection();
       IC7[0]->setup(0,0,1,0);
       IC7[1] = new Connection();
       IC7[1]->setup(0,1,2,0);
       IC7[2] = new Connection();
       IC7[2]->setup(0,2,3,0);
      ((Coupling*)D0[6])->setup(D7,4,IC7,3,EIC7,3,EOC7,3);
      ((Coupling*)D0[6])->init(ti);
      //D0[7] is  FinalizationCommands
      D0[7] = new multipleSimulationCommands("FinalizationCommands");
      ((Simulator*)D0[7])->father = mainCoupling;
      D0[7]->init(0.0,"/home/andylaurito/.haikunet/debug/simulation_debug_2017-05-02_17:10:44_UTC/Scilab/firstSimulation.sce","/home/andylaurito/.haikunet/debug/simulation_debug_2017-05-02_17:10:44_UTC/Scilab/eachSimulation.sce","/home/andylaurito/.haikunet/debug/simulation_debug_2017-05-02_17:10:44_UTC/Scilab/lastSimulation.sce");
      IC0[0] = new Connection();
      IC0[0]->setup(3,0,6,0);
      IC0[1] = new Connection();
      IC0[1]->setup(4,0,6,1);
      IC0[2] = new Connection();
      IC0[2]->setup(6,0,5,0);
      IC0[3] = new Connection();
      IC0[3]->setup(5,0,6,2);
      IC0[4] = new Connection();
      IC0[4]->setup(6,1,3,0);
      IC0[5] = new Connection();
      IC0[5]->setup(6,2,4,0);
      mainCoupling->setup(D0,8,IC0,6);
      ((RootCoupling*)mainCoupling)->rootSim = this;
    }
  };
