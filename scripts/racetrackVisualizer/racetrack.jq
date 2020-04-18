$(document).ready(function() {

  document.getElementById('fileInput').addEventListener('change', function selectedFileChanged() {
    if (this.files.length === 0) {
      console.log('No file selected.');
      return;
    }

    const reader = new FileReader();
    reader.onload = function fileReadCompleted() {
      // when the reader is done, the content is in reader.result.
      console.log(reader.result);

      var lines = reader.result.split('\n');

      width = parseInt(lines[0], 10);
      height = parseInt(lines[1], 10);

      var ctx = document.getElementById('grid').getContext('2d');
      
      

      for (var y = 0, j = 0; j < height; y += 11, j++) {

        var cells = lines[ j + 2 ];

        for (var x = 0, i = 0; i < width; x += 11, i++) {

          if (cells[i] == '#') {
          	ctx.beginPath();
          	ctx.fillStyle = "rgb(0,0,0)";
            ctx.rect(x, y, 10, 10);
            ctx.fill();
          }
          else if (cells[i] == '*') {
          	ctx.beginPath();
          	ctx.strokeStyle = "green";
            ctx.rect(x, y, 10, 10);
            ctx.stroke();
          }
          else if (cells[i] == '@') {
          	ctx.beginPath();
          	ctx.strokeStyle = "blue";
            ctx.rect(x, y, 10, 10);
            ctx.stroke();
          }

        }
      }      
      
      ctx.closePath();
    };
    reader.readAsText(this.files[0]);
  });
});

