<?php
$recursive_folders = false;
$int_only = false;
$parse_only = false;
$better_font = false;

$fail = 0;
$success = 0;

$dir_path = ".";
$parsephp = "parse.php";
$interpretpy = "interpret.py";

$paths_args = array("--directory", "--parse-script", "--int-script");
$true_false_args = array("--help", "--recursive", "--parse-only", "--int-only", "--better-font");
$testing_files = array();

$parsed_args = array();

/*
 * Parsing or given arguments
 * */
foreach($argv as $arg){
    if (strpos($arg, "=")){
        $divided_arg =  explode("=", $arg, 2);
        if (in_array($divided_arg[0], $paths_args)){
            array_push($parsed_args, $divided_arg);
        }
        else{
            exit(10);
        }
    }
    else{
        if ($arg == $argv[0]){
            continue;
        }
        else if (in_array($arg, $true_false_args)){
            array_push($parsed_args, $arg);
        }
        else{
            exit(10);
        }
    }
}
/*
 * Checks if there isnt --in-only and --parse-only at the same time
 * */
if (in_array("--int-only", $parsed_args) && in_array("--parse-only", $parsed_args)){
    exit(10);
}

/*
 * Checks if values after equal sign make sense, if so, assigns them
 * Sets boolean arguments to true/false depending on input
 * */
foreach($parsed_args as $arg){
    if (is_array($arg)){
        if ($arg[0] == "--directory"){
            if (file_exists($arg[1])){
                $dir_path = $arg[1];
            }
            else{
                exit(11);
            }
        }
        else if ($arg[0] == "--parse-script"){
            if (in_array("--int-only", $parsed_args)){
                exit(10);
            }
            if (file_exists($arg[1])){
                $parsephp = $arg[1];
            }
            else{
                exit(11);
            }
        }
        else if ($arg[0] == "--int-script"){
            if (in_array("--parse-only", $parsed_args)){
                exit(10);
            }
            if (file_exists($arg[1])){
                $interpretpy = $arg[1];
            }
            else{
                exit(11);
            }
        }
        else{
            exit(10);
        }
    }
    else{
        if ($arg == "--help"){
            if ($argc != 2){
                exit(10);
            }
            echo"How to run this script:
    php7.3 test.php > <output>.html <args>
        - where <output> is name of your output file
        - where <args> is field of optional arguments listed below:

    Arguments
            --help
                - explain:    prints this help. 
                              none of other arguments can be used with combination with --help argument

            --directory=<path>
                - explain:    sets a directory path with test files
                              default directory is the current folder
                - example:    --directory=tests/interpret-tests

            --parse-script=<path>
                - explain:    sets a path to parser script
                              default path to this script is current folder and default name is parse.php
                - example:    --parse-script=parse_script.php

            --int-script=<path>
                - explain:    sets a path to interpret script
                              default path to this script is current folder and default name is interpret.py
                - example:    --int-script=interpret_script.py

            --int-only
                - explain:    switch which tells the testing script to only test interpret script
                              if neither --int-only or --parse-only is chosen, test will run both

            --parse-only
                - explain:    switch which tells the testing script to only test parse script
                              if neither --int-only or --parse-only is chosen, test will run both

            --recursive
                - explain:    tells the test script to look for test files recursively
                              default behaviour is just searching test files inside current folder and not going deeper

            --better-font
                - explain:    HTML output will inclue better looking font
                              use this switch if you have your internet on\n";
            exit(0);
        }
        else if ($arg == "--recursive"){
            $recursive_folders = true;
        }
        else if ($arg == "--parse-only"){
            $parse_only = true;
        }
        else if ($arg == "--int-only"){
            $int_only = true;
        }
        else if ($arg == "--better-font"){
            $better_font = true;
        }
        else{
            exit(10);
        }
    }
}
/*
 * Searches for the deepest folder and with use of Recursive Iterator, goes through the file recursivly
 * */
function recursive_check($path){
    $tf = array();
    $s_dir = new RecursiveIteratorIterator(new RecursiveDirectoryIterator($path));
    foreach ($s_dir as $f){
        if ($f->isDir()){
            continue;
        }
        array_push($tf, $f->getPathname());
    }
    return $tf;
}
/*
 * Retrieves only .src files
 * */
function get_only_src_files($files){
    $src_files = array();
    foreach ($files as $f){
        if (preg_match('/.+(.src)$/', $f)){
            array_push($src_files, $f);
        }
    }
    return $src_files;
}
/*
 * test of interpret or parser depending on $what parameter
 * */
function test_interpret_or_parser($file, $expected_value, $out, $in, $what){
    $returned_values = array();
    global $parsephp;
    global $interpretpy;
    global $fail;
    global $success;

    if ($what == "interpret"){
        $temporary_file = preg_replace('/(.src)|(.xml)$/', ".tmp", $file);
        $generated_file = fopen($temporary_file, "w");
        fclose($generated_file);
        exec("python3.6 ".$interpretpy." --source=".$file. " < ".$in." > " .$temporary_file." 2> /dev/null", $test_return_int, $test_return_value);
    }
    else if ($what == "parser"){
        exec("php7.3 $parsephp < $file", $test_output, $test_return_value);
    }
	if ($test_return_value != 0){
        if ($expected_value == $test_return_value){
            array_push($returned_values, $file, $expected_value, $test_return_value, "", "OKAY");
           	if ($what == "interpret") unlink($temporary_file);
           	$success++;
        }
        else{
            array_push($returned_values, $file, $expected_value, $test_return_value, "", "FAILED");
            if ($what == "interpret") unlink($temporary_file);
            $fail++;
        }
    }
    else{
        if ($expected_value == $test_return_value){
            if ($what == "interpret"){
                shell_exec("diff $temporary_file $out");
            }
            else if ($what == "parser"){
                shell_exec('java -jar /pub/courses/ipp/jexamxml/jexamxml.jar $out $in /pub/courses/ipp/jexamxml/options');
            }
            $diff_ret = shell_exec('echo $?');
            if ($diff_ret == 0){
                array_push($returned_values, $file, $expected_value, $test_return_value, "Same", "OKAY");
                if ($what == "interpret") unlink($temporary_file);
                $success++;
            }
            else{
                array_push($returned_values, $file, $expected_value, $test_return_value, "Different", "FAILED");
                if ($what == "interpret") unlink($temporary_file);
                $fail++;
            }
        }
        else{
            array_push($returned_values, $file, $expected_value, $test_return_value, "", "FAILED");
            if ($what == "interpret") unlink($temporary_file);
            $fail++;
        }
    }
    return $returned_values;
}
/*
 * Tests of both interpret and parser at once
 * */
function test_both($file, $expected_value, $out, $in){
    $return_values = array();
    global $interpretpy;
    global $parsephp;
    global $fail;
    global $success;

    exec("php7.3 $parsephp < $file", $test_output, $test_return_value);

    if ($test_return_value == 0){
        $xml_input = preg_replace('/(.src)$/', ".xml", $file);
        $generated_file = fopen($xml_input, "w");
        foreach($test_output as $xml_instruction){
            fwrite($generated_file, $xml_instruction);
        }
        fclose($generated_file);
        $values = test_interpret_or_parser($xml_input, $expected_value, $out, $in, "interpret");
        unlink($xml_input);
        array_push($return_values, $values[0], $values[1], $values[2], $values[3], $values[4]);
    }
    else{
        if ($test_return_value == $expected_value){
            array_push($return_values, $file, $expected_value, $test_return_value, "", "OKAY");
            $success++;
        }
        else{
            array_push($return_values, $file, $expected_value, $test_return_value, "", "FAILED");
            $fail++;
        }
    }
    return $return_values;

}
/*
 * Checks if recursive search is set to true, if so, calls the function
 * If not, just searches inside current folder
 * */
if ($recursive_folders){
    $testing_files = recursive_check($dir_path);
}
else{
    $s_dir = glob($dir_path.'/*.*');
    foreach ($s_dir as $f){
        array_push($testing_files, $f);
    }
}
$source_files = get_only_src_files($testing_files);
$result = array();
/*
 *  Tests each .src file
 * */
foreach($source_files as $src_file){
    $in_f= preg_replace('/(.src)$/', ".in", $src_file);
    $out_f = preg_replace('/(.src)$/', ".out", $src_file);
    $expected_f = preg_replace('/(.src)$/', ".rc", $src_file);

    if (!in_array($in_f, $testing_files)){
        $generated_file = fopen($in_f, "w");

        fclose($generated_file);
    }

    if (!in_array($out_f, $testing_files)){
        $generated_file = fopen($out_f, "w");
        fclose($generated_file);
    }

    if (!in_array($expected_f, $testing_files)){
        $expected_value = 0;
        $generated_file = fopen($expected_f, "w");
        fwrite($generated_file, $expected_value);
        fclose($generated_file);
    }
    else{
        $read_file = fopen($expected_f, "r");
        $expected_value = fread($read_file, filesize($expected_f));
        fclose($read_file);
    }
    if ($int_only){
        $values = test_interpret_or_parser($src_file, $expected_value, $out_f , $in_f, "interpret");
        array_push($result, $values);
    }
    else if ($parse_only){
        $values = test_interpret_or_parser($src_file, $expected_value, $out_f , $in_f, "parser");
        array_push($result, $values);
    }
    else if(!$parse_only && !$int_only){
        $values = test_both($src_file, $expected_value, $out_f , $in_f);
        array_push($result, $values);
    }
}
if ($parse_only){
    $tested_script = $parsephp;
}
else if ($int_only){
    $tested_script = $interpretpy;
}
else{
    $tested_script = $interpretpy." and ".$parsephp;
}
/*----------------------------------------------------------------------
 *
 *  GENERATED HTML AND CSS
 *
 *---------------------------------------------------------------------- */
echo '
<!DOCTYPE HTML>
<html>
    <head>
	    <meta charset="UTF-8">
        <meta name="keywords" content="IPP, test.php, testing, script, report">
	    <meta name="description" content="IPP results">';
if ($better_font){
    echo '
        <link href="https://fonts.googleapis.com/css?family=Roboto:100,300,400,700" rel="stylesheet">
        ';
}
echo    '
        <meta name="author" content="Martin Machacek - xmacha73">
	    <title>IPP test.php results</title>
	    <style type="text/css">
        *{
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        body{
            min-width: 700px;
        ';
if ($better_font) echo '
            font-family: Roboto; ';
else echo'
            font-family: Monospace;';
echo '  }
        h3 {
            font-weight: initial;
        }
        header{
            background: #5E5E5E;
            width: 100%;
            height: auto;
            padding: 10px 0;
            color: white;
        }
        .header_content{
            display: flex;
            align-items: center;
            justify-content: space-between;
            font-size: 23px;
            max-width: 1240px;
            margin: 0 auto;
            padding: 0px 40px;
        }
        .header_content span{
            font-weight: 100;
        }
        #content{
            color: #5E5E5E;
        }
        #content .overview {
            display: flex;
            justify-content: space-between;
            padding: 40px;
            margin: 0 auto;
            max-width: 1240px;
        }
        #content .package{
            background: #EEEEEE;
        }
        #content .overview .result_numbers h3{
            display: inline-block;
            padding-left: 30px;
        }
        #content .overview h3 span{
            font-weight: bold;
        }
        #content .results {
            display: flex;
            flex-flow: column;
            padding: 30px 40px 60px 40px;
            margin: 0 auto;
            max-width: 1240px;
        }
        #content .results .row{
            display: flex;
            justify-content: space-between;
            margin-bottom: 12px;
        }
        #content .results .row:not(:first-child){
            background: white;
            box-shadow: 0px 5px 10px -2px #C7C7C7;
        }
        #content .results .row h3{
            padding: 15px 20px;
        }
        #content .results .row .field{
            display: flex;
            align-items: center
        }
        #content .results .row .field:first-child h3{
            text-align: left;
            max-width: 350px;
            word-break:  break-all;
        }
        #content .results .row .field:first-child {
            width: 100%;
        }
        #content .results .row:not(:first-child) .field:not(:first-child) {
            border-left: solid 1px #E5E5E5;
        }
        #content .results .row .field:not(:first-child) {
            text-align: center;
            justify-content: center;
            width: 40%;
        }
        #content .overview .no_files {
            width: 100%;
            text-align: center;
        }
        .check, .cross{
            border-radius: 100%;
            display: block;
            height: 30px;
            width: 30px;
            background: limegreen;
        }
        .cross{
            background: orangered;
        }
        .check:before, .check:after, .cross:before, .cross:after{
            display: block;
            background: white;
            width: 4px;
            content: "";
            position: relative;
        }
        .check:before{
            transform: rotate(40deg);
            height: 16px;
            left: 16px;
            top: 7px;
        }
        .check:after{
            transform: rotate(-50deg);
            height: 10px;
            left: 9px;
            top: -3px;
        }
        .cross:after{
            transform: rotate(-45deg);
            height: 20px;
            margin: 0 auto;
            left: 0px;
            top: -15px;
        }
        .cross:before{
            transform: rotate(45deg);
            height: 20px;
            margin: 0 auto;
            top: 5px;
        }
        .bold{
            font-weight: bold;
        }
        </style>
    </head>
    <body>
        <header>
            <div class="header_content">
                <h2>IPP <span>  testing script</span></h2>
                <span>'.$tested_script.'</span>
            </div>
        </header>
        <section id="content">
            <div class="overview"> ';
if ($success+$fail == 0){
    echo '      <h3 class="no_files"><span>No test files found in given directory</span></h3>
            </div>
        </section>
    </body>
</html>


';
    return 0;
}
else if ($success+$fail > 0){
      echo'
                <h3>TOTAL Number of tested files: <span>'.($success+$fail).'</span></h3>
                <div class="result_numbers">
                    <h3>PERCENTAGE: <span>'.round((($success*100)/($success+$fail)), 1).'%</span></h3>
                    <h3>OKAY: <span>'.$success.'</span></h3>
                    <h3>FAILED: <span>'.$fail.'</span></h3>
                </div>
            </div>
            <div class="package">
                <div class="results">
                    <div class="row">
                        <div class="field">
                            <h3>Path to tested file</h3>
                        </div>
                        <div class="field">
                            <h3>Expected value</h3>
                        </div>
                        <div class="field">
                            <h3>Actual value</h3>
                        </div>
                        <div class="field">
                            <h3>Output text</h3>
                        </div>
                        <div class="field">
                            <h3>Result</h3>
                        </div>
                    </div>'
      ;
}
foreach ($result as $one_result){
    if ($one_result[4] == "OKAY"){
        $one_result[4] = '<a class="check"></a>';
    }
    else{
        $one_result[4] = '<a class="cross"></a>';
    }
    echo '
                    <div class="row">
                        <div class="field">
                            <h3>'.$one_result[0].'</h3>
                        </div>
                        <div class="field">
                            <h3>'.$one_result[1].'</h3>
                        </div>
                        <div class="field">
                            <h3>'.$one_result[2].'</h3>
                        </div>
                        <div class="field">
                            <h3 class="bold">'.$one_result[3].'</h3>
                        </div>
                        <div class="field">
                            <h3>'.$one_result[4].'</h3>
                        </div>
                    </div>
    ';
}
echo '
                </div>
            </div>
        </section>
    </body>
</html>

';
?>