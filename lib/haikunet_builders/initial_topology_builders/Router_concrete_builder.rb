module RouterConcreteBuilder    
    def build_output_representation
        "routers.push @topology.add_router \"#{id}\", #{priority_weights}, #{buffer}"
    end      
end