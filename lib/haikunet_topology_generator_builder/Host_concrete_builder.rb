module HostConcreteBuilder    
    def build_output_representation
        "hosts.push @topology.add_host \"#{id}\", #{ips}, \"#{mac}\", #{queue_capacity}"
    end      
end