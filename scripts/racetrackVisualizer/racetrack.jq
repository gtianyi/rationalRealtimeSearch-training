$(document).ready(function() {
	
  var cellSize = 10;

  function drawCoordinates(x, y) {
    var pointSize = 3; // Change according to the size of the point.
    var ctx = document.getElementById("grid").getContext("2d");


    ctx.fillStyle = "#ff2626"; // Red color

    ctx.beginPath(); //Start path
    ctx.arc(x, y, pointSize, 0, Math.PI * 2, true); // Draw a point using the arc function of the canvas with a point structure.
    ctx.fill(); // Close the path and fill.
  }

  function drawMap(map, cellSize) {
    var cDist = (cellSize + cellSize/10) ;
    var cSize = cellSize;

    var lines = map.result.split('\n');

    width = parseInt(lines[0], 10);
    height = parseInt(lines[1], 10);

    var canvas = document.getElementById('grid');
    canvas.width = width * cDist ;
    canvas.height = height * cDist;
    /* canvas.style.width = width * cDist * canvasScale + 'px';
    canvas.style.height=  height * cDist * canvasScale +'px'; */

    var ctx = canvas.getContext('2d');

    for (var y = 0, j = 0; j < height; y += cDist, j++) {

      var cells = lines[j + 2];

      for (var x = 0, i = 0; i < width; x += cDist, i++) {

        if (cells[i] == '#') {
          ctx.beginPath();
          ctx.fillStyle = "rgb(0,0,0)";
          ctx.rect(x, y, cSize, cSize);
          ctx.fill();
        } else if (cells[i] == '*') {
          ctx.beginPath();
          ctx.strokeStyle = "green";
          ctx.lineWidth = 3;
          ctx.rect(x, y, cSize, cSize);
          ctx.stroke();
        } else if (cells[i] == '@') {
          ctx.beginPath();
          ctx.strokeStyle = "blue";
          ctx.lineWidth = 3;
          ctx.rect(x, y, cSize, cSize);
          ctx.stroke();
        }

      }
    }
    ctx.closePath();
  }

  function drawPath(path, cellSize) {
    var lines = path.result.trim().split('\n');

    var cDist = cellSize + cellSize/10 ;
    var cCenter = cellSize/2;

    var prev = [-1, -1];

    var canvas = document.getElementById('grid');

    var ctx = canvas.getContext('2d');

    for (var stateStr of lines) {
      console.log(stateStr);
      var state = stateStr.split(" ");

      var x = parseInt(state[0], 10) * cDist + cCenter;
      var y = parseInt(state[1], 10) * cDist + cCenter;
      drawCoordinates(x, y);

      if (prev[0] != -1) {
        ctx.beginPath();
        ctx.moveTo(prev[0], prev[1]);
        ctx.lineTo(x, y);
        ctx.stroke();
      }

      prev = [x, y];

    }

    ctx.closePath();
  }

  document.getElementById('mapFileInput').addEventListener('change', function selectedFileChanged() {
    if (this.files.length === 0) {
      console.log('No file selected.');
      return;
    }

    const reader = new FileReader();
    reader.onload = function fileReadCompleted() {
      // when the reader is done, the content is in reader.result.
      console.log(reader.result);
      drawMap(reader, cellSize);

    };
    reader.readAsText(this.files[0]);
  });

  document.getElementById('pathFileInput').addEventListener('change', function selectedFileChanged() {
    if (this.files.length === 0) {
      console.log('No file selected.');
      return;
    }

    const reader = new FileReader();
    reader.onload = function fileReadCompleted() {
      // when the reader is done, the content is in reader.result.
      console.log(reader.result);
      drawPath(reader, cellSize);
    };
    reader.readAsText(this.files[0]);
  });

  var link = document.createElement('a');
  link.innerHTML = 'download image';
  link.addEventListener('click', function(ev) {
    link.href = document.getElementById('grid').toDataURL();
    link.download = "racetack.png";
  }, false);
  document.body.appendChild(link);
});

