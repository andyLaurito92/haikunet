module OpenDayLightCodeGenerator
    def generate_output(file_name)
        code = ''
        @identifiers.each do |identifier|
            case identifier.value
            when HaikunetHost
                host_params = identifier.value.params
                mac = value_from 'mac', host_params
                ips = value_from 'ip', host_params
                vlan = value_from 'vlan', host_params
                code += "{
                    \"mac\" : \"#{mac}\", \n
                    \"vlan\" : \"#{vlan}\", \n
                    \"ipAddresses\" : #{ips}, \n
                    \"location\" : {
                        \"elementId\" : , \n
                        \"port\" : 
                    }
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
            end
        end

        @intents.each do |intent|
            flow_params = intent.select.value.params
            src = value_from 'src', flow_params
            dst = value_from 'dst', flow_params
            code += "{
                    \"type\" : \"HostToHostIntent\", \n
                    \"appId\" : \"#{intent.name}\", \n
                    \"one\" : \"#{src}\", \n
                    \"two\" : \"#{dst}\"\n
                }\n"
        end
        code
    end

    def value_from(value_name, array)
        array.select { |elem| elem.name == value_name }.first.value
    end

    def get_host_params(host_params)

    end
end