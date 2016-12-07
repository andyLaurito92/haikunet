module LinkConcreteBuilder
    def build_output_representation
      scilab_link_definition = ''
      
      #Think a better way to do this. This is the only thing that I could image to find out which link is this
      egressPortnumber = src_element.out_elements.index dst_element
      scilab_link_definition += "\n"
      scilab_link_definition += "#{src_element.id}.egressPort#{egressPortnumber}.link.capacity = #{bandwith} ;\n"
      scilab_link_definition += "#{src_element.id}.egressPort#{egressPortnumber}.link.delay = #{delay}; \n"
    
      {'scilab'=>scilab_link_definition, 'pdm'=>''}
    end

    def create_pdm_line_between_src_and_dst(src_element_pdm_pos, dst_element_pdm_pos, link_element_pdm_pos)
    "Line
                       {
                       Source = Cmp ;  #{src_element_pdm_pos} ;   #{@src_port + 1} ; 0
                       Sink = Cmp ;  #{dst_element_pdm_pos} ;  #{@dst_port + 1} ; -1
                       PointX = -9675 ; -9675 ; -9675
                       PointY = -10350 ; -10350 ; -9990
                       }
                       "
  end

end