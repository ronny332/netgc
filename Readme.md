# netgc - Gamecube network functions for Swiss-GC

netgc started with the idea to create a quick and dirty dropin FTP server for [swiss-gc](https://github.com/emukidid/swiss-gc), but got a immediate slowdown after a few needed features/functions were not part of the by swiss-gc used [libocg](https://github.com/devkitPro/libogc) and its network.h.
the first missing feature was a "host to IP address" function (now called `getipbyhost`), followed by the lag of a NTP requester.
netgc aims to close a few gaps in a hopefully easy to use and stable way.

let's start from here and see were we get :blush:.
