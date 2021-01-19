<?php
/*
 * Script:  First task of IPPcode19 interpreter
 * Name:    Martin Machacek
 * Login:   xmacha73
*/

/*
 * Generates XML to the users output (prints XML on screen)
 *
 * @param   $instructions    array of array with instructions and parameters
 */
function generate($instructions){
    $xml = new SimpleXMLElement('<?xml version="1.0" encoding="utf-8"?><program></program>');
    $xml->addAttribute('language', 'IPPcode19');

    for($i = 0; $i <= sizeof($instructions) - 1; $i++){
        $instruction = $xml->addChild('instruction');
        $instruction->addAttribute('order', $i+1);
        $instruction->addAttribute('opcode', $instructions[$i]['op_code']);
        for($j = 0; $j <= sizeof($instructions[$i]) - 2; $j++){
            if(array_key_exists("value", $instructions[$i][$j])){
                $arg = $instruction->addChild('arg'.($j+1), htmlspecialchars($instructions[$i][$j]['value']));
                $arg->addAttribute("type", $instructions[$i][$j]['type']);
            }
        }
    }

    $output = dom_import_simplexml($xml)->ownerDocument;
    $output->formatOutput = true;
    echo $output->saveXML();
}

/*
 * Based on given parameters, rearanges the array and renames the keys inside the array
 * Adds an opcode to each array for us to make sure its the correct instruction
 *
 * @param   $arrays     array of given instructions (array of arrays)
 * @param   $arr        array of one instruction and its parameters
 * @param   $type       general type of the instruction (symbol, var, type..)
 * @param   $op_code    instruction name (to distinguish parameters between instructions)
 */
function setup_instruction(&$arrays, $arr, $type, $op_code){
    if (!empty($arr)){
        $arr = array_values(array_filter($arr, 'strlen'));
        $arr["type"] = $type;

        //type (int, bool, string)
        $arr["full_operand"] = $arr[0];

        if ($arr["type"] == "type"){
            $arr["value"] = $arr[0];
        }
        elseif($arr["type"] == "symbol" || $arr["type"] == "var" || $arr["type"] == "literal"){
            if ($arr[1] == "GF" || $arr[1] == "LF" || $arr[1] == "TF"){
                $arr["type"] = "var";
                $arr["value"] = $arr[0];
            }
            elseif($arr[1] == "string" || $arr[1] == "int" || $arr[1] == "bool"){
                $arr["type"] = $arr[1];
                if (sizeof($arr) === 5){
                    $arr["value"] = "";
                }
                else{
                    $arr["value"] = $arr[3];
                }
            }
        }
        elseif($arr["type"] == "label"){
            $arr["value"] = $arr[0];
        }
        foreach ($arr as $key => $value) {
            if (is_int($key)) {
                unset($arr[$key]);
            }
    	}
    }
    $arrays["op_code"] = $op_code;
    
    array_push($arrays, $arr);
}

/*
 * Syntax check of command (one line)
 * Is supposed to be called in cycle for each line
 * Sorts out the command based on given instruction (first word)
 * Uses regex to check if the command has proper number and proper form of arguments (parameters)
 *
 * @param   $command    one line of the input file (one command)
 * @return  array ready to be inserted into setup of generated instruction
 */
function check_syntax($command){

    $ops_array = array();
    $three_ops = array("ADD","SUB","MUL","IDIV","LT","GT","EQ","AND","OR","STRI2INT","CONCAT","GETCHAR","SETCHAR","JUMPIFEQ","JUMPIFNEQ");
    $two_ops = array("MOVE","INT2CHAR","READ","STRLEN","TYPE","NOT");
    $one_op = array("DEFVAR","CALL","PUSHS","POPS","WRITE","LABEL","JUMP","EXIT","DPRINT");
    $no_op = array("CREATEFRAME","PUSHFRAME","POPFRAME","RETURN","BREAK");

    $type_regex = '/^(bool|int|string)$/';
    $label_regex = '/^([a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]+)$/';
    $var_regex = '/^(GF|LF|TF)(@)([a-zA-Z_\-$&%*!?]+[a-zA-Z0-9_\-$&%*!?]*)$/';
    $symb_regex ='/^(nil)(@)(nil)$|^(bool)(@)(true|false)$|^(int)(@)(\S*)$|^(string)(@)([\S]*)$|^(GF|LF|TF)(@)([a-zA-Z_\-$&%*!?]+[a-zA-Z0-9_\-$&%*!?]*)$/';

    $command[0] = strtoupper($command[0]);
    if (in_array($command[0], $three_ops)){
        if ((sizeof($command) -1) != 3){
            exit(23);
        }
        elseif ($command[0] === "JUMPIFEQ" || $command[0] === "JUMPIFNEQ"){
            if (preg_match($label_regex, $command[1], $key_reg) === 1){ //first argument <label>
                setup_instruction($ops_array, $key_reg, "label", $command[0]);
            }
            else{
                exit(23);
            }
        }
        else{
            if (preg_match($var_regex, $command[1], $key_reg) === 1){ //first argument <var>
                setup_instruction($ops_array, $key_reg, "var", $command[0]);
            }
            else{
                exit(23);
            }
        }
        for($i = 2; $i <= 3; $i++){
            if (preg_match($symb_regex, $command[$i], $key_reg) === 1){
                setup_instruction($ops_array, $key_reg, "symbol", $command[0]);
            }
            else{
                exit(23);
            }
        }
        return $ops_array;
    }

    elseif (in_array($command[0], $two_ops)){
        if ((sizeof($command) - 1) != 2){
            exit(23);
        }
        if(preg_match($var_regex, $command[1], $key_reg) === 1){
            setup_instruction($ops_array, $key_reg, "var", $command[0]);
        }
        else{
            exit(23);
        }
        if ($command[0] === "READ"){
            if(preg_match($type_regex, $command[2], $key_reg) === 1){
                setup_instruction($ops_array, $key_reg, "type", $command[0]);
            }
            else{
                exit(23);
            }
        }
        else{
            if(preg_match($symb_regex, $command[2], $key_reg) === 1){
                setup_instruction($ops_array, $key_reg, "symbol", $command[0]);
            }
            else{
                exit(23);
            }
        }
        return $ops_array;
    }
    elseif (in_array($command[0], $one_op)){
        if ((sizeof($command) - 1) != 1){
            exit(23);
        }
        if($command[0] == "DEFVAR" || $command[0] == "POPS"){
            if(preg_match($var_regex, $command[1], $key_reg) === 1){
                setup_instruction($ops_array, $key_reg, "var", $command[0]);
            }
            else{
                exit(23);
            }
        }
        if($command[0] == "CALL" || $command[0] == "LABEL" || $command[0] == "JUMP"){
            if(preg_match($label_regex, $command[1], $key_reg) === 1){
                setup_instruction($ops_array, $key_reg, "label", $command[0]);
            }
            else{
                exit(23);
            }
        }
        if($command[0] == "PUSHS" || $command[0] == "WRITE" || $command[0] == "EXIT" || $command[0] == "DPRINT"){
            if(preg_match($symb_regex, $command[1], $key_reg) === 1){
                setup_instruction($ops_array, $key_reg, "symbol", $command[0]);
            }
            else{
                exit(23);
            }
        }
        return $ops_array;
    }
    elseif (in_array($command[0], $no_op)){
        if ((sizeof($command) - 1) != 0){
            exit(23);
        }
        $key_reg = array();
        setup_instruction($ops_array, $key_reg, "label", $command[0]);
        return $ops_array;
    }
    else{
        exit(22);
    }
}

/*
 * Removes comments and spaces from given array
 *
 * @param   $array  given array to be manipulated
 * @return  clean array || false if the array would be empty
*/
function cleanup($array){
    $i = 0;
    $ret_array = array();

    foreach ($array as $t){
        if ($t == ""){
            continue;
        }
        else if ($t[0] == "#"){
            break;
        }
        else if(strpos($t, "#")){
            $t = explode("#", $t)[0];
            $ret_array[$i] = trim($t);
            break;
        }
        else{
            $ret_array[$i] = trim($t);
            $i++;
        }
    }
    if (sizeof($ret_array) > 0){
        return array_filter($ret_array);
    }
    return false;
}
/*
 * Checks arguments count
 * Checks if the input file exists and is accessible
 * Makes sure that file header exists, if so, cut out the comments
 *
 * @argv    array of input arguments
*/
function check_inputs($argv){
	if (sizeof($argv) > 2){
        exit(10);
	}
	if (($file_header = fgets(STDIN)) === false){
        exit(21);
	}
	if (strpos($file_header, "#")){
        $file_header = explode("#", $file_header)[0];
	}
	if (strtoupper(trim($file_header)) !== ".IPPCODE19"){
        exit(21);
	}
}
/*
 * Start of the program
 * Checks if --input argument exists
 * Retrieves one line at a time from a file input
 * Calls needed functions for each line (lexical, syntax + generating)
*/

if (sizeof($argv) == 2){
	if ($argv[1] == "--help"){
		print
            'IPP 2018/2019 PROJECT - FIRST TASK'
            ."\nAuthor:\tMartin Machacek"
            ."\nLogin:\txmacha73"
            ."\nScript:\tPHP"
            ."\n\n1) Loads input file written in IPPcode19"
            ."\n2) Runs a lexical & syntax analysis"
            ."\n3) Output is a XML representation of the IPPcode19 program"
                ."\n\nHow to run the script:"
                ."\n\tphp7.3 parse.php <parameters>"
                ."\n\n\tphp7.3\t\t= interpreter of this script"
                ."\n\tparse.php\t= name of this script file"
                ."\n\t<parameters>\t= --help"
                ."\n\t\t\t= <input_file>"
            ."\n\nExamples of starting this program:"
                ."\n\tphp7.3 parse.php --help\t\t\t (will print out this help)"
                ."\n\tphp7.3 parse.php < test.txt --help\t (will print out this help)"
                ."\n\tphp7.3 parse.php < test.txt\t\t (will take the test.txt file and work with it)"
            ."\n\n<input_file> needs to be written in IPPcode19, script won't work with any other format";
		exit(0);
	}
	else{
		exit(10);
	}
}
check_inputs($argv);
$array_of_instructions = array();
while (!feof(STDIN)){
    $line = fgets(STDIN);
    $line = preg_replace('/\t+/', ' ', $line);
    $words = explode(" ", trim($line));
    $clear_code = cleanup($words);
    if ($clear_code !== false) {
        $syntax_okay = check_syntax($clear_code);
        array_push($array_of_instructions, $syntax_okay);
    }
}
generate($array_of_instructions);
exit(0);

?>