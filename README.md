GET / -> index.html

GET /style.css -> style.css

GET /index.js -> index.js

GET /info -> JSON

    {
        "uptime": string,
        "temp": number,
        "load-avg": ,
        "loads": ,
        "process": ,
        "logs":
    }

POST / -> index.html

    sys-cmd=s
    sys-cmd=r
    sys-cmd=c

    run-cmd=string
    kill-cmd=string

    sec-set=number
    util-set=number
    temp-set=number
    entry-set=number

    clear-logs=
