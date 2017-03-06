#PhaseITopologyPriorityQueues

=begin
http://asciiflow.com

+---------+                  +----------+
|         |                  |          |
|         |                  |          |
|  Host 1 |                  |          |
|         |                  |Host 2    |
+--------++                  +----+-----+
         |                        |
         |   +-----------+        |
         |   |           |        |
         +---+           +--------+
             |of:0000000000000001   |
             |           |
             +------+----+
                    |
                    |
                    |
                    |
            +-------+-----+
            |             |
            |             |
            |  Host 3     |
            |             |
            +-------------+
   

=end

module NetworkTopology
    def get_topology
        return @topology.topology_elements if @topology.topology_elements.size != 0

        routers = []
        hosts = []

        router = @topology.add_router "of:0000000000000001"
        routers.push router

        for i in 1..3  
          host = @topology.add_host "Host#{i}", "10.0.0.#{i}"
          hosts.push host     
        end

        #Links (starting from the flows that goes from top to bottom)    
        link1 = @topology.add_link "Link1", hosts[0], 0, routers[0], 0
        link2 = @topology.add_link "Link2", hosts[1], 0, routers[0], 1

        link3 = @topology.add_link "Link3", routers[0], 0, hosts[2], 0

        #Links (flows that goes from bottom to top)    
        link4 = @topology.add_link "Link4", hosts[2], 0, routers[0], 2

        link5 = @topology.add_link "Link5", routers[0], 1, hosts[0], 0
        link6 = @topology.add_link "Link6", routers[0], 2, hosts[1], 0

        @topology.topology_elements
    end  

    def get_path_between(source, destination)
        raise Exception, "Source must be either from class Router or class Host to ask for a path" unless [Host, Router].include? source.class
        raise Exception, "Destination must be either from class Router or class Host to ask for a path" unless [Host, Router].include? destination.class

        first_link = @topology.topology_elements.select { |elem| (elem.is_a? Link) && (elem.src_element == source) }.first
        second_link = @topology.topology_elements.select { |elem| (elem.is_a? Link) && (elem.dst_element == destination) }.first

        path = Path.new(source,destination)        
        path.add_link first_link
        path.add_link second_link
        path
    end
end