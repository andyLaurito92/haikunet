module RouterConcreteBuilder
    def build_output_representation
        template = 
        "   Coupled
              {
              Type = Coordinator
              Name = #{id}
              Ports = #{@in_elements.size}; #{@out_elements.size}
              Description = Coupled DEVS model
              Graphic
                  {
                  Position = #{-13455 + 2000 * (my_number - 2)} ; #{-9510 + 1600 * (my_number - 2)}
                  Dimension = 660; 1470
                  Direction = Down
                  Color = 15
                  Icon = %datanetworks%switch.jpg
                  Window = 5000; 5000; 5000; 5000
                  }
              Parameters
                  {
                  }
              System
                  {
        "

        @in_elements.each_with_index do |in_elem, index|
          template += buildInPort index
        end
        
        @out_elements.each_with_index do |out_elem, index|
          template += buildOutPort index
        end 

        template += buildRouting @in_elements.size, @out_elements.size

        scilab_router_definition = ''
        scilab_router_definition += "\n"
        scilab_router_definition += "#{id}.numberOfQueues = #{priority_weights.size} ;\n"
        scilab_router_definition += "#{id}.queueCapacity = #{buffer}; \n"
        @out_elements.each_with_index do |out_elem, index|
            template += buildEgressPort index
            scilab_router_definition += "#{id}.egressPort#{index}.PriorityQueue.WRRScheduler.weights = #{priority_weights}; \n"
        end

        template += buildLines @in_elements.size, @out_elements.size
             
        template += "       }
              }
        "

        {'scilab'=>scilab_router_definition, 'pdm'=>template}
    end
    
    def buildInPort (portNumber)
"                    Inport
                      {
                      Name = Inport#{portNumber}
                      CoupledPort = #{portNumber+1}
                      Description = 
                      Graphic
                          {
                          Position = #{-5000 + 1000* portNumber+1}  ; -6525
                          Dimension = 600
                          Direction = Right
                          }
                      }
"      
    end
    
  def buildOutPort (portNumber)
"               Outport
                     {
                     Name = Outport#{portNumber+1}
                     CoupledPort = #{portNumber+1}
                     Description = 
                     Graphic
                        {
                        Position = #{-5000 + 1000* portNumber} ; 2175
                        Dimension = 600
                        Direction = Right
                        }
                     }
"
  end
  
  def buildRouting (nInports, nOutports)
"               Atomic
                      {
                      Name = Routing
                      Ports = #{nInports} ; #{nOutports}
                      Path = PhaseI/Router.h
                      Description = InN: Incomming packets\\nInN: Outgoing packets \\n This model routes incoming packets to outports. The decision is based on the global table GlobalFlowBasedRoutingTable(flowId, routerName). If there are more than one possible outport, the model uses a routingPolicy (currently hardcoded to Round-Robin) to decide.
                      Graphic
                          {
                          Position = -3600 ; -4155
                          Dimension = 615 ; 1785
                          Direction = Down
                          Color = 15
                          Icon = %datanetworks%router.jpg
                          }
                      Parameters
                          {
                          }
                      }
"
  end
    
  def buildEgressPort (portNumber)
"               Coupled
                     {
                     Type = Coordinator
                     Name = egressPort#{portNumber}
                     Ports = 1; 1
                     Description = Coupled DEVS model
                     Graphic
                         {
                         Position = #{-4710 + 1200 * portNumber}; -1860
                         Dimension = 1170; 870
                         Direction = Down
                         Color = 15
                         Icon = %datanetworks%priorityegressport.jpg
                         Window = 5000; 5000; 5000; 5000
                         }
                     Parameters
                         {
                         }
                     System
                         {
                         Inport
                             {
                             Name = Inport0
                             CoupledPort = 1
                             Description = 
                             Graphic
                                 {
                                 Position = -4695 ; -3525
                                 Dimension = 390
                                 Direction = Right
                                 }
                             }
                         Outport
                             {
                             Name = Outport0
                             CoupledPort = 1
                             Description = 
                             Graphic
                                 {
                                 Position = -1785 ; 600
                                 Dimension = 435
                                 Direction = Right
                                 }
                             }
                         Atomic
                             {
                             Name = Link
                             Ports = 1 ; 1
                             Path = PhaseI/Link.h
                             Description = Vector to scalar signal
                             Graphic
                                 {
                                 Position = -3990 ; -915
                                 Dimension = 630 ; 630
                                 Direction = Down
                                 Color = 15
                                 Icon = %vectors%vec2scalar.svg
                                 }
                             Parameters
                                 {
                                 link.capacity = Str; #{id}.egressPort#{portNumber}.link.capacity ; Signal Index
                                 link.delay = Str; #{id}.egressPort#{portNumber}.link.delay ; 
                                 }
                             }
                         Coupled
                             {
                             Type = Coordinator
                             Name = PriorityQueue
                             Ports = 2; 1
                             Description = Coupled DEVS model
                             Graphic
                                 {
                                 Position = -4080; -2895
                                 Dimension = 810; 990
                                 Direction = Down
                                 Color = 15
                                 Icon = %datanetworks%priorityqueue.jpg
                                 Window = 5000; 5000; 5000; 5000
                                 }
                             Parameters
                                 {
                                 }
                             System
                                 {
                                 Inport
                                     {
                                     Name = inPackets
                                     CoupledPort = 1
                                     Description = 
                                     Graphic
                                         {
                                         Position = -10710 ; -2625
                                         Dimension = 570
                                         Direction = Right
                                         }
                                     }
                                 Outport
                                     {
                                     Name = outPackets
                                     CoupledPort = 1
                                     Description = 
                                     Graphic
                                         {
                                         Position = -3990 ; -3750
                                         Dimension = 630
                                         Direction = Right
                                         }
                                     }
                                 Inport
                                     {
                                     Name = inDequeReq
                                     CoupledPort = 2
                                     Description = 
                                     Graphic
                                         {
                                         Position = -5880 ; -525
                                         Dimension = 525
                                         Direction = Right
                                         }
                                     }
                                 Atomic
                                     {
                                     Name = prioritySelect
                                     Ports = 1 ; 1
                                     Path = PhaseI/PacketPrioritySelect.h
                                     Description = Receives packets, and based on their flowId it selects which output port to send it through
                                     Graphic
                                         {
                                         Position = -8805 ; -2985
                                         Dimension = 675 ; 720
                                         Direction = Right
                                         Color = 15
                                         Icon = None
                                         }
                                     Parameters
                                         {
                                         }
                                     }
                                 Atomic
                                     {
                                     Name = VQueues
                                     Ports = 2 ; 2
                                     Path = PhaseI/Vector_NetworkQueueWithSizeOutput.h
                                     Description = In0 Incomming packets to queue\\nIn1 Incoming signal to request dequeue\\nOut0 Outgoing dequeued packets\\nOut1 Outgoing queue lenght information\\n\\nQueues incoming packets and enqueues them as required by a\\nexternal entity. FIFO Policy (FirstInFirstOut). Provides information\\nits internal state.
                                     Graphic
                                         {
                                         Position = -7200 ; -2805
                                         Dimension = 600 ; 675
                                         Direction = Right
                                         Color = 15
                                         Icon = %datanetworks%queue_vec.png
                                         }
                                     Parameters
                                         {
                                         MaxCapacity = Str; #{id}.queueCapacity ; Queue Capacity in Bytes. (Use -1 for INF capacity)
                                         ForcedPeriod = Str; -1 ; Force minimum period to transition. Use -1 for INF
                                         Instances = Str; #{id}.numberOfQueues ; 
                                         }
                                     }
                                 Atomic
                                     {
                                     Name = WRRScheduler
                                     Ports = 2 ; 1
                                     Path = PhaseI/WRRScheduler.h
                                     Description = Atomic DEVS model
                                     Graphic
                                         {
                                         Position = -4530 ; -2505
                                         Dimension = 675 ; 675
                                         Direction = Right
                                         Color = 15
                                         Icon = None
                                         }
                                     Parameters
                                         {
                                         numberOfQueues = Str; #{id}.numberOfQueues ; 
                                         weights = Str; #{id}.egressPort#{portNumber}.PriorityQueue.WRRScheduler.weights; vector<double> with the weights for each queue
                                         }
                                     }
                                 Atomic
                                     {
                                     Name = Vect2Scal
                                     Ports = 1 ; 1
                                     Path = vector/Vector2Scalar.h
                                     Description = Vector to scalar signal
                                     Graphic
                                         {
                                         Position = -5505 ; -4080
                                         Dimension = 675 ; 675
                                         Direction = Right
                                         Color = 15
                                         Icon = %vectors%vec2scalar.svg
                                         }
                                     Parameters
                                         {
                                         Index = Str; -1 ; Signal Index
                                         }
                                     }
                                 Line
                                     {
                                     Source = Cmp ;  1 ;  1 ; 0
                                     Sink = Cmp ;  2 ;  1 ; -1
                                     PointX = -8025 ; -8025 ; -7365
                                     PointY = -2625 ; -2625 ; -2625
                                     }
                                 Line
                                     {
                                     Source = Prt ;  1 ;  1 ; 0
                                     Sink = Cmp ;  1 ;  1 ; -1
                                     PointX = -9975 ; -9975 ; -8940
                                     PointY = -2625 ; -2625 ; -2625
                                     }
                                 Line
                                     {
                                     Source = Cmp ;  2 ;  2 ; 0
                                     Sink = Cmp ;  3 ;  1 ; -1
                                     PointX = -6450 ; -6450 ; -4665
                                     PointY = -2325 ; -2325 ; -2325
                                     }
                                 Line
                                     {
                                     Source = Prt ;  3 ;  1 ; 0
                                     Sink = Cmp ;  3 ;  2 ; -1
                                     PointX = -5250 ; -5025 ; -5025 ; -4665
                                     PointY = -525 ; -525 ; -2025 ; -2025
                                     }
                                 Line
                                     {
                                     Source = Cmp ;  3 ;  1 ; 0
                                     Sink = Cmp ;  2 ;  2 ; -1
                                     PointX = -3750 ; -3300 ; -3300 ; -7500 ; -7500 ; -7365
                                     PointY = -2175 ; -2175 ; -1275 ; -1275 ; -2325 ; -2325
                                     }
                                 Line
                                     {
                                     Source = Cmp ;  4 ;  1 ; 0
                                     Sink = Prt ;  2 ;  1 ; -1
                                     PointX = -4725 ; -4725 ; -4140
                                     PointY = -3750 ; -3750 ; -3750
                                     }
                                 Line
                                     {
                                     Source = Cmp ;  4 ;  1 ; -1
                                     Sink = Cmp ;  2 ;  1 ; 0
                                     PointX = -5640 ; -6075 ; -6075 ; -6450
                                     PointY = -3750 ; -3750 ; -2625 ; -2625
                                     }
                                 }
                             }
                                 Point
                                     {
                                     ConectedLines = 1 ; 4 ; 5
                                     ConectedExtrems = Org ; Org ; Org
                                     Position = -3675 ; 150
                                     }
                                 Line
                                     {
                                     Source = Cmp ;  1 ;  1 ; 0
                                     Sink = Pnt ;  1 ; -1 ; 0
                                     PointX = -3675 ; -3675 ; -3675
                                     PointY = -150 ; -150 ; 150
                                     }
                                 Line
                                     {
                                     Source = Prt ;  1 ;  1 ; 0
                                     Sink = Cmp ;  2 ;  1 ; -1
                                     PointX = -4200 ; -3900 ; -3900
                                     PointY = -3525 ; -3525 ; -2940
                                     }
                                 Line
                                     {
                                     Source = Cmp ;  2 ;  1 ; 0
                                     Sink = Cmp ;  1 ;  1 ; -1
                                     PointX = -3675 ; -3675 ; -3675
                                     PointY = -1875 ; -1875 ; -1065
                                     }
                                 Line
                                     {
                                     Source = Cmp ;  2 ;  2 ; -1
                                     Sink = Pnt ;  1 ; -1 ; 0
                                     PointX = -3450 ; -3450 ; -2775 ; -2775 ; -3675
                                     PointY = -2940 ; -3300 ; -3300 ; 150 ; 150
                                     }
                                 Line
                                     {
                                     Source = Pnt ;  1 ; -1 ; 0
                                     Sink = Prt ;  2 ;  1 ; -1
                                     PointX = -3675 ; -3675 ; -1965
                                     PointY = 150 ; 600 ; 600
                                     }
                         }
                     }
"
  end
  
  def buildLines(in_elements_quantity, out_elements_quantity)
    routing_position = 1 #Because is the first component after ports
    first_egress_port_position = routing_position
    first_output_port_position = in_elements_quantity # Because I have as many inputs_ports as in_elements

    lines = ''
    #from inports to Routing
    (1..in_elements_quantity).each do |i|
        lines += "Line
            {
            Source = Prt ;  #{i} ;  1 ; 0
            Sink = Cmp ; #{routing_position}  ;  #{i} ; -1
            PointX = -11325 ; -11325 ; -9840
            PointY = -2625 ; -2625 ; -2625
            }
        "
    end

    # from Routing to egressPorts
    (1..out_elements_quantity).each do |i|
        lines += "Line
            {
            Source = Cmp ;  #{routing_position} ;  #{i} ; 0
            Sink = Cmp ; #{first_egress_port_position + i}  ;  1 ; -1
            PointX = -11325 ; -11325 ; -9840
            PointY = -2625 ; -2625 ; -2625
            }
        "
    end

    # from egressPorts to outports
    (1..out_elements_quantity).each do |i|
        lines += "Line
            {
            Source = Cmp ;  #{first_egress_port_position + i} ;  1 ; 0
            Sink = Prt ; #{first_output_port_position + i}  ;  1 ; -1
            PointX = -11325 ; -11325 ; -9840
            PointY = -2625 ; -2625 ; -2625
            }
        "
    end    

    lines
  end
      
end