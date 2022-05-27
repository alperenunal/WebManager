let interval;
let refreshSec = 2000;

const pageLoad = () => {
  let req = new XMLHttpRequest();

  req.onload = () => {
    let resp = req.responseText.split("\n");
    let logs = document.getElementById("log");
    logs.innerHTML = "";

    for (let i = 0; i < resp.length; i++) {
      const pair = resp[i].split("=");

      switch (pair[0]) {
        case "uptime":
          document.getElementById("uptime").innerText = pair[1];
          break;
        case "loadavg":
          document.getElementById("load-avg").innerText = pair[1];
          break;
        case "loads":
          document.getElementById("cpu-loads").innerText = pair[1];
          break;
        case "sec":
          let sec = parseInt(pair[1]);
          refreshSec = (sec ? sec : sec + 1) * 1000;
          break;
        case "proc":
          let table = document.getElementById("program-table");
          let row = table.insertRow(-1);

          let col = pair[1].split(" ");
          for (let j = 0; j < col.length; j++) {
            row.insertCell(j).innerText = col[j];
          }
          break;
        case "log":
          logs.innerText += pair[1] + "\n";
        default:
          break;
      }
    }

    clearInterval(interval);
    interval = setInterval(pageLoad, refreshSec);
  };

  req.open("GET", "/info");
  req.send();
};

window.onload = pageLoad;
