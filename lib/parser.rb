require_relative 'haikunet_objects/gramatic_objects/haikunet_identifier.rb'
require_relative 'haikunet_objects/network_objects/haikunet_intent.rb'
require_relative 'haikunet_objects/network_objects/haikunet_host.rb'
require_relative 'haikunet_objects/network_objects/haikunet_link.rb'
require_relative 'haikunet_objects/network_objects/haikunet_device.rb'
require_relative 'haikunet_objects/network_objects/haikunet_flow.rb'
require_relative 'haikunet_objects/network_objects/haikunet_action.rb'
require_relative 'haikunet_objects/network_objects/haikunet_condition.rb'
require_relative 'errors/syntactical_error.rb'
require_relative 'errors/semantical_error.rb'

class Parser
    attr_reader :context

    def initialize
        @lexeme_tokenize = nil
        @index_actual_token = 0
        @line_number = 0
        #Its not a tree as you can see, but at the end it will be in pre order form
        #Meaning, that having a tree structure and traversing it in a pre order way 
        #would be the same as just linearly go througth this array.
        @parse_tree = []
        
        #Creating code elements associated to the program to parse
        @context = {}
        @identifiers = []
        @intents = []
    end

    def parse(lexeme_tokenize)
        @lexeme_tokenize = lexeme_tokenize
        lookahead_token = get_lookahead
        while @index_actual_token < @lexeme_tokenize.size
            initial_symbol
            lookahead_token = get_lookahead
        end
        @context = { 'identifiers' => @identifiers, 'intents' => @intents }
        @parse_tree
    end

    def initial_symbol
        @parse_tree.push 'INITIAL_SYMBOL'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'IDENTIFIER'
            index_identifier = @index_actual_token
            match 'IDENTIFIER'
            match 'ASSIGN'

            identifier = HaikunetIdentifier.new @lexeme_tokenize[index_identifier].value
            @identifiers.push identifier
            identifier.value = network_elem_definition
        when 'INTENT'
            match 'INTENT'
            index_intent_identifier = @index_actual_token
            match 'IDENTIFIER'
            match 'SELECT'
            index_select_identifier = @index_actual_token
            match 'IDENTIFIER'

            action_and_condition = intent_production_with_action
            new_intent = HaikunetIntent.new @lexeme_tokenize[index_intent_identifier].value
            new_intent.select = get_identifier @lexeme_tokenize[index_select_identifier].value
            @intents.push new_intent

            return unless action_and_condition
            new_intent.action = action_and_condition['action']
            new_intent.condition = action_and_condition['condition']
        else
            raise_syntaxis_error "it was suppossed to found either an identifier or an intent, but instead was found #{lookahead_token.value}"
        end
    end

    def intent_production_with_action
        @parse_tree.push 'INTENT_PRODUCTION_WITH_ACTION'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'ACTION'
            match 'ACTION'
            index_action_identifier = @index_actual_token
            match 'IDENTIFIER'
            
            condition = intent_production_with_condition
            return {
                    'action' => @lexeme_tokenize[index_action_identifier].value,
                    'condition' => condition
                    }
        else
            @parse_tree.push 'LAMBDA'

            return {
                    'action' => nil,
                    'condition' => nil
                    }
        end  
    end

    def intent_production_with_condition
        @parse_tree.push 'INTENT_PRODUCTION_WITH_CONDITION'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'CONDITION'
            match 'CONDITION'
            index_identifier = @index_actual_token
            match 'IDENTIFIER'

            return @lexeme_tokenize[index_identifier].value
        else
            @parse_tree.push 'LAMBDA'

            return nil
        end
    end

    def network_elem_definition
        @parse_tree.push 'NETWORK_ELEM_DEFINITION'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'HOST'
            match 'HOST'
            match 'LEFT_PARENTHESIS'
            @actual_params = []
            params
            match 'RIGHT_PARENTHESIS'

            return HaikunetHost.new @actual_params
        when 'LINK'
            match 'LINK'
            match 'LEFT_PARENTHESIS'
            @actual_params = []
            params
            match 'RIGHT_PARENTHESIS'

            return HaikunetLink.new @actual_params
        when 'DEVICE'
            match 'DEVICE'
            match 'LEFT_PARENTHESIS'
            @actual_params = []
            params
            match 'RIGHT_PARENTHESIS'

            return HaikunetDevice.new @actual_params
        when 'ACTION'
            match 'ACTION'
            match 'LEFT_PARENTHESIS'
            @actual_params = []
            params
            match 'RIGHT_PARENTHESIS'

            return HaikunetAction.new @actual_params
        when 'FLOW'
            match 'FLOW'
            match 'LEFT_PARENTHESIS'
            @actual_params = []
            params
            match 'RIGHT_PARENTHESIS'

            return HaikunetFlow.new @actual_params
        when 'CONDITION'
            match 'CONDITION'
            match 'LEFT_PARENTHESIS'
            @actual_params = []
            params
            match 'RIGHT_PARENTHESIS'

            return HaikunetCondition.new @actual_params
        else
            raise_syntaxis_error "it was suppossed to found either a Host, or a Link, or a Device, bleh.. but instead #{lookahead_token.value} was found."
        end
    end

    def params
        @parse_tree.push 'PARAMS'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'IDENTIFIER'
            index_identifier = @index_actual_token
            match 'IDENTIFIER'
            match 'EQUAL_PARAMETERS'

            @actual_identifier = HaikunetIdentifier.new @lexeme_tokenize[index_identifier].value
            second_part_equal
        else
            @parse_tree.push 'LAMBDA'

            return @actual_params
        end
    end

    def second_part_equal
        @parse_tree.push 'SECOND_PART_EQUAL'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'DOUBLE_QUOTE'
            match 'DOUBLE_QUOTE'
            index_string = @index_actual_token
            match 'STRING'
            match 'DOUBLE_QUOTE'

            @actual_identifier.value = @lexeme_tokenize[index_string].value
            @actual_params.push @actual_identifier
            add_more_parameters
        when 'IDENTIFIER'
            index_identifier = @index_actual_token
            match 'IDENTIFIER'

            @actual_identifier.value = get_identifier @lexeme_tokenize[index_identifier].value
            @actual_params.push @actual_identifier
            add_more_parameters
        when 'LEFT_BRACE'
            match 'LEFT_BRACE'
            @elems_of_array = []
            elems_of_array
            match 'RIGHT_BRACE'

            @actual_identifier.value = @elems_of_array
            @actual_params.push @actual_identifier
            add_more_parameters
        else
            raise_syntaxis_error "it was suppossed to found either a \" or [ or and identifier, but instead #{lookahead_token.value} was found."
        end
    end

    def add_more_parameters
        @parse_tree.push 'ADD_MORE_PARAMETERS'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'COMMA'
            match 'COMMA'
            params
        else
            @parse_tree.push 'LAMBDA'
        end
    end

    def elems_of_array
        @parse_tree.push 'ELEMS_OF_ARRAY'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'IDENTIFIER' 
            index_identifier = @index_actual_token
            match 'IDENTIFIER'

            @elems_of_array.push get_identifier(@lexeme_tokenize[index_identifier].value)
            add_more_elements_to_array
        when 'DOUBLE_QUOTE'
            match 'DOUBLE_QUOTE'
            index_string = @index_actual_token
            match 'STRING'
            match 'DOUBLE_QUOTE'

            @elems_of_array.push @lexeme_tokenize[index_string].value
            add_more_elements_to_array
        else
            raise_syntaxis_error "it was suppossed to found an identifier inside the array, but instead #{lookahead_token.value} was found."
        end
    end

    def add_more_elements_to_array
        @parse_tree.push 'ADD_MORE_ELEMENTS_TO_ARRAY'
        lookahead_token = get_lookahead
        return unless lookahead_token
        case lookahead_token.keyword
        when 'COMMA' 
            match 'COMMA'
            elems_of_array
        else
            @parse_tree.push 'LAMBDA'
        end 
    end

    def get_identifier(identifier_name)
        identifier_in_array = @identifiers.select { |identifier| identifier.name == identifier_name }
        raise_semantic_error "Identifier #{identifier_name} was not defined previously." unless identifier_in_array.size == 1
        identifier_in_array.first
    end

    def get_lookahead
        @lexeme_tokenize[@index_actual_token]
    end

    def match(keyword)
       token = @lexeme_tokenize[@index_actual_token] 
       if token.keyword == keyword
            @parse_tree.push token
            @index_actual_token += 1
       else
            raise_syntaxis_error "it was suppossed to match token #{keyword}, but instead #{token.keyword} was found."
       end

       while @index_actual_token < @lexeme_tokenize.length && @lexeme_tokenize[@index_actual_token].keyword == 'END_OF_LINE'
            @line_number += 1 
            @index_actual_token += 1
       end
    end

    def raise_syntaxis_error(message)
        raise SyntacticalError, "A syntactical error was found on line:#{@line_number + 1}. The problem is that #{message}\n Please correct this error in order to run the program ;)."
    end

    def raise_semantic_error(message)
        raise SemanticalError, "A semantic error was found on line:#{@line_number + 1}. The problem is that #{message}\n Please correct this error in order to run the program ;)."
    end
end