require 'byebug'
require_relative 'token.rb'
require_relative 'errors/lexical_error.rb'

class Lexer
    #TODO: IF THIS IS A CONST, SHOULDN'T I DEFINE A KEYWORDS CLASS ??
    IDENTIFIER_REGEX = /[.A-Za-z0-9_-]+/ #This matchs alpha numeric symbols, allowing : . _ -. (But cannot end in : .)
    STRING_REGEX = /[:.A-Za-z0-9_-]+/

    def initialize
        @line_to_tokenize = ''
        @end_of_line_reached = false
        @current_index = 0
        @tokens = []
    end

    def tokenize_lexeme(program_lexeme)
        line_number = 0
        @program_lexeme_lines = program_lexeme.gsub(/\r\n?/, "\n")
        @program_lexeme_lines.each_line do |line|
            @line_to_tokenize = line
            @current_index = 0
            @end_of_line_reached = false
            while !@end_of_line_reached
                @tokens.push get_next_token line_number
            end
            line_number += 1
        end
        @tokens
    end

    def get_next_token(line_number)
        token = nil
        current_read_lexeme = ''
        while not token
            unless @current_index < @line_to_tokenize.length
                #This can happens when no end of line is found in the line. Probably meaning that is the 
                #last line of the program
                @end_of_line_reached = true
                return create_token_with_value_and_move_forward 'END_OF_LINE', '\n', 0
            end
            current_read_lexeme += @line_to_tokenize[@current_index]
            case current_read_lexeme
            when '='
                token = create_token_with_value_and_move_forward 'EQUAL_PARAMETERS', '=', 1
            when '('
                token = create_token_with_value_and_move_forward 'LEFT_PARENTHESIS', '(', 1
            when ')'
                token = create_token_with_value_and_move_forward 'RIGHT_PARENTHESIS', ')', 1
            when '"'
                token = create_token_with_value_and_move_forward 'DOUBLE_QUOTE', '"', 1
            when '['
                token = create_token_with_value_and_move_forward 'LEFT_BRACE', '[', 1
            when ']'
                token = create_token_with_value_and_move_forward 'RIGHT_BRACE', ']', 1
            when ','
                token = create_token_with_value_and_move_forward 'COMMA', ',', 1
            when ':'
                token = create_token_with_value_and_move_forward 'ASSIGN', ':=', 2  if looking_forward_is_keyword? ':=' 
            when IDENTIFIER_REGEX
                #We are in an identifier, host, link, flow, device, intent, select, action, condition case
                token = create_token_with_value_and_move_forward 'HOST', 'HOST', 4 if looking_forward_is_keyword? 'HOST'
                token = create_token_with_value_and_move_forward 'LINK', 'LINK', 4 if looking_forward_is_keyword? 'LINK'
                token = create_token_with_value_and_move_forward 'DEVICE', 'DEVICE', 6 if looking_forward_is_keyword? 'DEVICE'
                token = create_token_with_value_and_move_forward 'FLOW', 'FLOW', 4 if looking_forward_is_keyword? 'FLOW'
                token = create_token_with_value_and_move_forward 'INTENT', 'INTENT', 6 if looking_forward_is_keyword? 'INTENT'
                token = create_token_with_value_and_move_forward 'SELECT', 'SELECT', 6 if looking_forward_is_keyword? 'SELECT'
                token = create_token_with_value_and_move_forward 'ACTION', 'ACTION', 6 if looking_forward_is_keyword? 'ACTION'
                token = create_token_with_value_and_move_forward 'CONDITION', 'CONDITION', 9 if looking_forward_is_keyword? 'CONDITION'

                return token if token

                #If we are here, then it means that is an identifier. We should pick the longest 
                #identifier that we can, and keep going. 

                #If the previous token was a double quote, then we are in front of a string, 
                # otherwise is an identifier
                if !@tokens.last.nil? && @tokens.last.keyword == 'DOUBLE_QUOTE'
                    value = pick_longest_identifier STRING_REGEX
                    token = create_token_with_value_and_move_forward 'STRING', value, value.length
                else
                    value = pick_longest_identifier IDENTIFIER_REGEX
                    token = create_token_with_value_and_move_forward 'IDENTIFIER', value, value.length
                end
            when ' ' #When we have a space, we just keep going
                current_read_lexeme = ''
                @current_index += 1
            when /\t/ #When we have a tab, we just keep going
                current_read_lexeme = ''
                @current_index += 1
            when /\n/
                @end_of_line_reached = true
                token = create_token_with_value_and_move_forward 'END_OF_LINE', '\n', 0
            else
                raise_sintaxis_eror "there is no matching token for lexeme #{current_read_lexeme}\n", line_number
            end
        end
        token
    end

    def create_token_with_value_and_move_forward(keyword, value, move_forward)
        @current_index += move_forward
        Token.new keyword, value
    end

    def looking_forward_is_keyword?(keyword)
        #TODO: FIND A BETTER WAY TO DO THIS. :(
        keyword_regex = (keyword == ':=') ? keyword : /\b#{keyword}\b/i
        
        lexeme_found = @line_to_tokenize[@current_index,keyword.size].scan keyword_regex
        lexeme_found.size == 1
    end

    def pick_longest_identifier(regex)
        index_read_identifier = @current_index
        current_read_identifier = @line_to_tokenize[index_read_identifier]
        identifiers_found = current_read_identifier.scan regex
            while identifiers_found.size == 1 && identifiers_found.first == current_read_identifier && index_read_identifier + 1 < @line_to_tokenize.length 
                index_read_identifier += 1
                current_read_identifier += @line_to_tokenize[index_read_identifier]
                identifiers_found = current_read_identifier.scan regex
            end
        identifiers_found.first
    end

    def raise_sintaxis_eror(message, line_number)
        lexeme_with_error = @line_to_tokenize[[0,@current_index-5].max, @current_index+5]
        raise LexicalError, "A lexical error was found on line:#{line_number + 1}, on column #{@current_index} near #{lexeme_with_error}. The problem is that #{message}. Please correct this error in order to run the program ;)."
    end
end