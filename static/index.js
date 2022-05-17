let refreshSec = 5000;

function pageLoad() {
  let req = new XMLHttpRequest();

  req.onload = () => {
    console.log(req.responseText);
    document.getElementById("uptime-info").innerText = req.responseText;

    // let resp = JSON.parse(req.responseText);
  };

  req.open("GET", "/number");
  req.send();
}

window.onload = pageLoad;
setInterval(pageLoad, refreshSec);
