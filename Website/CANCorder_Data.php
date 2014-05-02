<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<title>CANcorder Log Viewer</title>
<style type="text/css">

body{
	margin:0;
	padding:0;
	line-height: 1.5em;
}

	b{font-size: 110%;}
	em{color: red;}


#topsection{
	background: #A00000;
	height: 90px; /*Height of top section*/
}

#topsection h1{
	margin: 0;
	padding-top: 30px;
	padding-left: 30px;
	color: #FFF;
	font-family: Arial, Helvetica, sans-serif;
}
#topbuffer{

	background-color:#FFFFFF;
	height: 20px;
}
#topbuffer2{
	background-color:#000000;
	height: 20px;
}
#contentwrapper{
	float: left;
	width: 100%;
}

#contentcolumn{
	margin-top: 20px;
	margin-left: 50px; /*Set left margin to LeftColumnWidth*/
	margin-right: 50px;
}

#footer{
	clear: left;
	width: 100%;
	background: black;
	color: #FFF;
	text-align: center;
	padding: 4px 0;
	font-family: Arial, Helvetica, sans-serif;
	font-size: 70%;
}

#footer a{
	color: #FFFF80;
}

.innertube{
	margin: 10px; /*Margins for inner DIV inside each column (to provide padding)*/
	margin-top: 0;
}


.tables
{
	float: left;
	width: 1750px;
	padding: 30px;
}

td
{
	padding-right:15px;
	padding-left:10px;
	width: 300;
}

</style>
<script src="jquery-2.1.0.min.js"></script>
<script type="text/javascript">
$(document).ready(function(){
      refreshTable();
    });

    function refreshTable(){
        $('#contentcolumn').load('reload.php', function(){
           setTimeout(refreshTable, 1500);
        });
    }

</script>


</head>
	<body>
		<div id="maincontainer">
			
			<div id="topsection"><div class="innertube"><h1>CANCorder Data Analyzer</h1></div></div>
			<div id="topbuffer"><body></body></div>
			<div id="topbuffer2"><body></body></div>
				<div id="contentwrapper">
					<div id="contentcolumn">
						
					</div>
				</div>
			<div id="footer">
				<b>If there are problems with this webpage, contact Sean -</b>
			</div>
		</div>
	</body>
</html>
