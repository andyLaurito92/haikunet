module LinkConcreteBuilder    
    def build_output_representation
        "@topology.add_link \"#{id}\", #{src_element.id}, #{src_port}, #{dst_element.id}, #{dst_port}, #{bandwith}"
    end      
end