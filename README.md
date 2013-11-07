Pebble watchface using SDK 2.0 + httpcapture
============================================

This is a modified version of SweWeek 2 to support both Pebble SDK 2.0 and the httpcapture screen capture hack (via httpebble-ios)

Original here https://github.com/chrobe/sweweek

The httpcapture can be used with this modified version of httpebble-ios https://github.com/epatel/httpebble-ios

I have also modified `http.h` and `http.c` to compile with Pebble SDK 2.0 (not yet functional tested)

At the moment one need to grab a `GContext` from some layer. I will look into making this automatically. See where `http_capture_set_gcontext()` is called in `sweweek.c`
