# iOS Device Bridge


## Installation

clone this repository

    $ hub clone seiji/idb

And then execute

    $ cd idb
    $ rake

print usage

    $ idb

## Usage

Connect your device with usb

### Print udid

    $ idb udid
    > qwertyuiopasdfghjkl1

### Print info

    > $ idb info

    > [INFO]
    ...

* print apps (User appplications)

> $ idb apps

> iBooks                  com.apple.iBooks

> Find My iPhone          com.apple.mobileme.fmip1

> Podcasts                com.apple.podcasts

> \-                      com.apple.Remote

* install app

> $ idb install /path/to/demo.ipa

> $ idb install /path/to/demo.app

* uninstall app

> $ idb unintall com.apple.iBooks

* list directory

> $ idb ls com.apple.iBooks 

> $ idb ls com.apple.iBooks Documents

* print syslog

> $ idb log

