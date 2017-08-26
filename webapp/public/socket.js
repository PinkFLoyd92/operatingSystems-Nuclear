$.ajax({url: "/ip", success: function(ipAddress){
    console.info("The IP Adress is: ", ipAddress, ". The complete URL of the server is ", 'http://'+ipAddress+':9000');
    var socket = io('http://'+ipAddress+':9000');
//var socket = io('http://localhost:9000');
socket.on('ktotal', function (x){
    $( "#ktotal" ).text(x.resource.ktotal );
})
socket.on('kparcial', function (x){
    $( "#kparcial" ).text(x.resource.kparcial );
})
socket.on('deltak', function (x) {
    $( "#deltak" ).text(x.resource.deltak );
})

socket.on('exit', function (x) {
    alert("MURIO EL SISTEMA :( :( ")
})

socket.on('change_direction', function (x) {
    console.info(x.resource)
    $( "#switch-"+x.resource.id ).click()
})
socket.on('barValue', function (x) {
    console.info(x.resource)
    for(var i = 0; i < data.length; i++) {
        if (data[i].id == parseInt(x.resource.id)){
            data[i].cm = parseInt(x.resource.cm);
            // $("#barra"+data[i].id).val(data[i].cm)
            $("#barra"+data[i].id).get( 0 ).MaterialSlider.change(data[i].cm)
            $( "#bar"+data[i].id ).attr( "data-badge", x.resource.cm );
        }
    }
  svg.selectAll("*").remove()
  series = d3.stack()
             .keys(["cm"])
             .offset(d3.stackOffsetDiverging)
  (data);
svg = d3.select("svg"),
    margin = {top: 20, right: 30, bottom: 30, left: 60},
    width = +svg.attr("width"),
    height = +svg.attr("height");

    x = d3.scaleBand()
        .domain(data.map(function(d) { return d.name; }))
        .rangeRound([margin.left, width - margin.right])
        .padding(0.1);

var y = d3.scaleLinear()
    .domain([d3.min(series, stackMin), d3.max(series, stackMax)])
    .rangeRound([height - margin.bottom, margin.top]);

    z = d3.scaleOrdinal(d3.schemeCategory10);

    svg.append("g")
        .selectAll("g")
        .data(series)
        .enter().append("g")
        .attr("fill", function(d) { return z(d.key); })
        .selectAll("rect")
        .data(function(d) { return d; })
        .enter().append("rect")
        .attr("width", x.bandwidth)
        .attr("x", function(d) { return x(d.data.name); })
        .attr("y", function(d) { return y(d[1]); })
        .attr("height", function(d) { return y(d[0]) - y(d[1]); })

    svg.append("g")
        .attr("transform", "translate(0," + y(0) + ")")
        .call(d3.axisBottom(x));

    svg.append("g")
        .attr("transform", "translate(" + margin.left + ",0)")
        .call(d3.axisLeft(y));

});

    }});
