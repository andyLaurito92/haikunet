module HostConcreteBuilder
   def build_output_representation
      raise "A host should be either a generator or a receiving application. This host has 0 in_elements and 0 out_elements" if @in_elements.size == 0 && @out_elements.size == 0 
      
      pdm_output = "        Coupled
            {
            Type = Coordinator
            Name = #{id}
            Ports = #{@in_elements.size}; #{@out_elements.size}
            Description = Coupled DEVS model
            Graphic
                {
                Position = #{-13995 + 1600 * (my_number - 4)} ; -12375
                Dimension = 390; 1050
                Direction = Down
                Color = 15
                Icon = %datanetworks%server.jpg
                Window = 5000; 5000; 5000; 5000
                }
            Parameters
                {
                }
            System
                {#{componentsPdm}                                               
                }
            }
      "
      scilab_host_output = "\n"     
      scilab_host_output += "#{id}.egressPort.queueCapacity = #{queue_capacity};"
      {'scilab'=>scilab_host_output, 'pdm'=>pdm_output}
    end
    
  def componentsPdm
    components = ''
    # 1st we need the in/out ports
    components += portsPdm
    
    # 2nd we need the components
    components += generatorApplication if @out_elements.size > 0
    components += receivingApplication if @in_elements.size > 0
    # 3rd the lines
    components += linksPdm 
    
    components
  end  
  
  def portsPdm
    ports = ''
    # inports    
    (0..@in_elements.size-1).each do |i|
      ports +="
              Inport
              {
                Name = Inport#{i}
                CoupledPort = #{i+1}
                Description = 
                Graphic
                    {
                    Position = -9330 ; #{-3250 + (i*1000)}
                    Dimension = 510
                    Direction = Right
                    }
              }"         
    end
    
    # outports
    (0..@out_elements.size-1).each do |i|
      ports += "
                Outport
                {
                Name = Outport#{i}
                CoupledPort = #{i+1}
                Description = 
                Graphic
                    {
                    Position = #{-7000 + (2000 * i)} ; 3600
                    Dimension = 555
                    Direction = Right
                    }
                }"            
    end
    
    ports
  end
    
  def linksPdm
    links_pdm = ''
        
    # from generator (component 1) to router (component 2)
    if @out_elements.size > 0 # if there is a generator
      links_pdm += "
                              Line
                                  {
                                  Source = Cmp ;  1 ;  1 ; 0
                                  Sink = Cmp ;  2 ;  1 ; -1
                                  PointX = -6300 ; -6300 ; -6300
                                  PointY = -1725 ; -1725 ; -1140
                                  }"
    end
        
    (1..@out_elements.size).each do |i|
      # from router (component 2) to egressLink (components 3,4.. outElems+2)
      links_pdm += "                     
                              Line    
                                  {
                                  Source = Cmp ;  2 ;  #{i} ; 0
                                  Sink = Cmp ;  #{i+2} ;  1 ; -1
                                  PointX = -6300 ; -6300 ; -6300
                                  PointY = -1725 ; -1725 ; -1140
                                  }"        
      # from egressLink (components 3,4.. outElems+2) to outports 
      links_pdm += "
                              Line
                                  {
                                  Source = Cmp ;  #{i+2}  ;  1 ; 0
                                  Sink = Prt ;  #{i} ;  1 ; -1
                                  PointX = #{-7200 + (2000 * (i-1))} ; #{-7200 + (2000 * (i-1))} ; #{-7200 + (2000 * (i-1))}
                                  PointY = 900 ; 3525 ; 3525
                                  }"
    end
    
    # from inports to receiving app
    (0..@in_elements.size-1).each do |i|    
      links_pdm +="
        Line
        {
          Source = Prt ;  #{(@out_elements.size + 1) + i} ;  1 ; 0
          Sink = Cmp ;  #{(@out_elements.size * 3) + 1} ;  #{1 + i} ; -1
          PointX = -8700 ; #{-8325 + (i * 300)} ; #{-8325 + (i * 300)}
          PointY = #{-3250 + (i*1000)} ; #{-3250 + (i*1000)} ; -90
        }"      
    end      
    
    
    links_pdm
  end  
    
  def generatorApplication
    pdm_generator_application = "
    Atomic
                        {
                        Name = GeneratorApplication
                        Ports = 0 ; 1
                        Path = PhaseI/FelixServer.h
                        Description = Generates jobs. Distribution for the rate and jobSize are retrieved from the Flows assigned to this server
                        Graphic
                            {
                            Position = -7740 ; -4365
                            Dimension = 630 ; 645
                            Direction = Down
                            Color = 15
                            Icon = %datanetworks%generator.png
                            }
                        Parameters
                            {
                            }
                        }
    Atomic
                        {
                        Name = Routing
                        Ports = 1 ; #{@out_elements.size}
                        Path = PhaseI/Router.h
                        Description = InN: Incomming packets\\nInN: Outgoing packets \\n This model routes incoming packets to outports. The decision is based on the global table GlobalFlowBasedRoutingTable(flowId, routerName). If there are more than one possible outport, the model uses a routingPolicy (currently hardcoded to Round-Robin) to decide.
                        Graphic
                            {
                            Position = -7740 ; -2115
                            Dimension = 615 ; 1785
                            Direction = Down
                            Color = 15
                            Icon = %datanetworks%router.jpg
                            }
                        Parameters
                            {
                            }
                        }"
    (0..@out_elements.size-1).each do |i|
        pdm_generator_application += "                        
                        Coupled
                            {
                            Type = Coordinator
                            Name = egressPort#{i}
                            Ports = 1; 1
                            Description = Coupled DEVS model
                            Graphic
                                {
                                Position = #{-7700 + (2000 * i)}; -120
                                Dimension = 990; 840
                                Direction = Down
                                Color = 15
                                Icon = %datanetworks%egressport.jpg
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
                                        Position = -4725 ; -3525
                                        Dimension = 450
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
                                        Position = -930 ; 1575
                                        Dimension = 525
                                        Direction = Right
                                        }
                                    }
                                Atomic
                                    {
                                    Name = portQueue
                                    Ports = 2 ; 2
                                    Path = PhaseI/NetworkQueue.h
                                    Description = In0 Incomming packets to queue\\nIn1 Incoming signal to request dequeue\\nOut0 Outgoing dequeued packets\\nOut1 Outgoing queue lenght information\\n\\nQueues incoming packets and enqueues them as required by a\\nexternal entity. FIFO Policy (FirstInFirstOut). Provides information\\nits internal state.
                                    Graphic
                                        {
                                        Position = -3855 ; -2355
                                        Dimension = 675 ; 675
                                        Direction = Down
                                        Color = 15
                                        Icon = %datanetworks%queue.png
                                        }
                                    Parameters
                                        {
                                        MaxCapacity = Str; #{id}.egressPort.queueCapacity ; Queue Capacity in Bytes. (Use -1 for INF capacity)
                                        ForcedPeriod = Str; -1 ; Force minimum period to transition. Use -1 for INF
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
                                        Position = -3990 ; -540
                                        Dimension = 630 ; 630
                                        Direction = Down
                                        Color = 15
                                        Icon = %vectors%vec2scalar.svg
                                        }
                                    Parameters
                                        {
                                        link.capacity = Str; #{id}.egressPort#{i}.link.capacity ; Signal Index
                                        link.delay = Str; #{id}.egressPort#{i}.link.delay ; 
                                        }
                                    }
                                Point
                                    {
                                    ConectedLines = 3 ; 4 ; 5
                                    ConectedExtrems = Org ; Org ; Org
                                    Position = -3705 ; 765
                                    }
                                Line
                                    {
                                    Source = Prt ;  1 ;  1 ; 0
                                    Sink = Cmp ;  1 ;  1 ; -1
                                    PointX = -4125 ; -3675 ; -3675
                                    PointY = -3525 ; -3525 ; -2490
                                    }
                                Line
                                    {
                                    Source = Cmp ;  1 ;  1 ; 0
                                    Sink = Cmp ;  2 ;  1 ; -1
                                    PointX = -3675 ; -3675 ; -3675
                                    PointY = -1575 ; -1575 ; -690
                                    }
                                Line
                                    {
                                    Source = Cmp ;  2 ;  1 ; 0
                                    Sink = Pnt ;  1 ; -1 ; 0
                                    PointX = -3675 ; -3705 ; -3705
                                    PointY = 225 ; 225 ; 765
                                    }
                                Line
                                    {
                                    Source = Cmp ;  1 ;  2 ; -1
                                    Sink = Pnt ;  1 ; -1 ; 0
                                    PointX = -3375 ; -3375 ; -2775 ; -2775 ; -3705
                                    PointY = -2490 ; -2775 ; -2775 ; 765 ; 765
                                    }
                                Line
                                    {
                                    Source = Pnt ;  1 ; -1 ; 0
                                    Sink = Prt ;  2 ;  1 ; -1
                                    PointX = -3705 ; -3705 ; -1065
                                    PointY = 765 ; 1575 ; 1575
                                    }
                                }
                            }"   
    end 
    pdm_generator_application
  end  
    
    
  def receivingApplication
      # atomic model for receiving application 
      pdm_receiving_app = "                        
      Atomic
      {
        Name = ReceivingApplication
        Ports = #{@in_elements.size} ; 0
        Path = PhaseI/Packetsink.h
        Description = Generates jobs. Distribution for the rate and jobSize are retrieved from the Flows assigned to this server
        Graphic
            {
            Position = -8490 ; 45
            Dimension = 630 ; 675
            Direction = Down
            Color = 15
            Icon = %datanetworks%generator.png
            }
        Parameters
            {
            }
      }"      
      
      pdm_receiving_app
    end
      
end