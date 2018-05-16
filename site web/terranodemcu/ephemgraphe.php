<!DOCTYPE html> 
<head> 
<meta charset="utf-8" /> 
<title>Terrarium</title> <!-- titre -->
<link rel="icon" type="image/png" href="img/serpent.png" />
<script type="text/javascript" src="lib/jquery.js"></script> <!-- appel de la librairie jquery --> 
<script type="text/javascript" src="lib/dateheure.js"></script> <!-- appel de la fonction date et heure javascript --> 
<script src="lib/highcharts.js"></script> <!-- appel de la librairie highcharts --> 
<script src="lib/gray.js"></script> <!-- appel du thème highcharts --> 

<link rel="stylesheet" href="histo.css" />


<script type="text/javascript">
$(function () {
	$.getJSON('dataephem.php', function (data) {  
		
		Highcharts.chart('container', {
			chart: {
				type: 'area'
			},
			title: {
				text: "Historique de la répartition jour/nuit"
			},			
			xAxis: {
				categories: data[0].data,
				tickmarkPlacement: 'on',
				title: {
					enabled: false
				}
			},
			yAxis: {
				title: {
					text: 'Heures'
				},
				min: 0,
				max: 24,
				labels: {
					formatter: function () {
						return this.value;
					}
				}
			},
			tooltip: {
				split: true,
				valueSuffix: ' Heures'
			},
			plotOptions: {
				area: {
					stacking: 'normal',
					lineColor: '#666666',
					lineWidth: 1,
					marker: {
						lineWidth: 1,
						lineColor: '#666666'
					}
				}
			},
			series: [{
				name: 'jour',
				data: data[1].data
			}, {
				name: 'nuit',
				data: data[2].data,
			}]
		});
	});
});
</script>

</head>
<body>
	<header>    <!-- entête -->
	<div class="conteneur">       		 
		<div class="element" id="date"> <!-- contiendra la date -->		
			<script type="text/javascript">window.onload = date('date');</script>		
		</div>
		<div class="element" id="heure"> <!-- contiendra l'heure --> 		
			<script type="text/javascript">window.onload = heure('heure');</script>			
		</div> 		
	</div>
	</header>
	
	<section>
		<h1>éphéméride</h1>
		
		<div id="container"> <!-- contiendra le graphique -->		
			<div class="loader">Loading...</div>	
		</div> 
	</section>

	<footer>	<!-- pied de page -->		
		<div class="conteneur">       
			<div class="element" id="serpent">
				<a href="index.php" style="text-decoration:none"><span id="accueil">Accueil</span></a> <!--lien vers la page d'accueil-->
			</div> 			
			<div class="element" id="pc">
				<a href="admin/index.php" style="text-decoration:none"><span id="admin">Admin</span></a><!--lien vers la page admin-->
			</div>  
		</div>	
	</footer>

</body>
</html>
