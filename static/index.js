let interval;
let refreshSec = 2000;

const pageLoad = () => {
  let req = new XMLHttpRequest();

  let logs = document.getElementById("log");
  logs.innerHTML = "";

  req.onload = () => {
    let resp = JSON.parse(req.responseText);

    document.getElementById("uptime").innerText = resp.uptime;
    document.getElementById("cpu-temp").innerText = resp.temp;
    document.getElementById("load-avg").innerText = resp.loadavg;
    document.getElementById("cpu-loads").innerText = resp.loads.toString();

    let sec = resp.sec;
    refreshSec = (sec ? sec : sec + 1) * 1000;

    let table = document.getElementById("program-table");
    table.innerHTML =
      "<tr><th>Command</th><th>Process ID</th><th>Allowed CPU</th></tr>";

    for (const proc of resp.procs) {
      let row = table.insertRow(-1);

      for (let i = 0; i < proc.length; i++) {
        row.insertCell(i).innerText = proc[i];
      }
    }

    let logLen = Object.keys(resp.logs).length;
    let diff = logLen - resp.entry;
    let entryNumber = diff > 0 ? diff : 0;

    for (entryNumber; entryNumber < logLen; entryNumber++) {
      logs.innerText += resp.logs[entryNumber] + "\n";
    }

    clearInterval(interval);
    interval = setInterval(pageLoad, refreshSec);
  };

  req.open("GET", "/info");
  req.send();
};

window.onload = pageLoad;
