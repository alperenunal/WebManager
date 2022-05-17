#ifndef HTTP_H
#define HTTP_H

#define CRLF "\r\n"

#define HTTP_OK "HTTP/1.1 200 OK" CRLF
#define HTTP_NF "HTTP/1.1 404 Not Found" CRLF

#define CT_HTML "Content-Type: text/html; charset=utf-8" CRLF
#define CT_CSS "Content-Type: text/css; charset=utf-8" CRLF
#define CT_JS "Content-Type: application/javascript" CRLF
#define CT_JSON "Content-Type: application/json" CRLF
#define CT_TXT "Content-Type: text/plain" CRLF

typedef enum { GET, POST } Method;

#endif
