red alert
=========

will alarm a red alert when deploying Haojing.

###set the ethernet first.

```
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 8, 222); // set the fixed IP of ethernet
IPAddress dns_server(172, 16, 0, 1); // dns in office
IPAddress gateway(192, 168, 8, 1);
IPAddress subnet(255, 255, 255, 0);
```

###and deploy server
```
char server[] = "{deploy address}";
```

###other settings
```postingInterval``` the frequency of checking (by ms)

