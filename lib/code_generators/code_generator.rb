class CodeGenerator
    def generate_code(context, destiny, file_name, initial_topology)
        @identifiers = context['identifiers']
        @intents = context['intents']
        @initial_topology = initial_topology

        require_relative "#{destiny.downcase}_code_generator.rb"
        self.class.send(:include, Object.const_get("#{destiny.capitalize}CodeGenerator"))
        generate_output file_name
    end

end