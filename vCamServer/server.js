var requestify = require('requestify');
var fs = require('fs');
var FormData = require('form-data');
var exec = require('child_process').exec;  // running cmd

var vPlotterServer = '192.168.1.12';
var seconds = 10;

setInterval( function(){
  requestify.get('http://'+vPlotterServer+':8080/status.json').then(function(response) {
    // Get the response body (JSON parsed - JSON response or jQuery object in case of XML response)
    response.getBody();
    var status = JSON.parse(response.body);

    // console.log(status);

    if(status['printing']){
      console.log(vPlotterServer+' is Printing');
    } else {
      console.log(vPlotterServer+' is NOT Printing');

      //  Generate SVG
      //
      console.log('Running vCam to generate a new SVG file');
      var command = './vCam --width 480 --height 640 --space 5 --canny1 1040';
      var VCAMPIPE = exec(command, function (error, stdout, stderr) {

        //  Send SVG
        //
        var form = new FormData();
        form.append('rotate', 0);
        form.append('scale', 100);
        form.append('file', fs.createReadStream(__dirname+'/test.svg'));

        form.submit('http://'+vPlotterServer+':8080/addFile', function(err, res) {
          console.log(res);
          console.log(err);
        });
      });

    }
  });
}, seconds*1000);
