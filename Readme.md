# netgc - Gamecube network functions for Swiss-GC

netgc started with the idea to create a quick and dirty dropin FTP server for [swiss-gc](https://github.com/emukidid/swiss-gc), but got a immediate slowdown after a few needed features/functions were not part of the by swiss-gc used [libocg](https://github.com/devkitPro/libogc) and its network.h.
the first missing feature was a "host to IP address" function (now called `get_ipbyhost`), followed by the lag of a NTP requester (`get_tsfromntp`).
netgc aims to close a few gaps in a hopefully easy to use and stable way.

as the Gamecube is not a high performance device (anymore), the used way to code is simple and clean, without a lot of refactoring and code structure, which would use more RAM or slow the code down. the functions are by design not threadsafe (or not tested) and can cause unwanted behavior if they are called "at the same time".

let's start from here and see were we get :blush:.
