module DebugCodeGenerator
    #For debugging we use DEVS 
    def generate_output(file_name)
        code = ''
        @identifiers.each do |identifier|
            case identifier.value
            when HaikunetHost
                host_params = identifier.value.params
                mac = value_from 'mac', host_params
                code += "{
                    \"id\" : \"#{identifier.name}\", \n
                    \"mac\" : \"#{mac}\"
                }\n"
            when HaikunetFlow
                flow_params = identifier.value.params
                src = value_from 'src', flow_params
                dst = value_from 'dst', flow_params
                code += "{
                    \"id\" : \"#{identifier.name}\", \n
                    \"src\" : \"#{src}\", \n
                    \"dst\" : \"#{dst}\", \n
                }\n"
            when HaikunetIntent
                code += "{
                    \"type\" : \"HostToHostIntent\", \n
                    \"appId\" : \"#{identifier.name}\", \n
                    \"one\" : \"#{src}\", \n
                    \"two\" : \"#{dst}\"\n
                }\n"
            end
        end
        code
    end

    def value_from(value_name, array)
        array.select { |elem| elem.name == value_name }.first.value
    end
end