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
    qwertyuiopasdfghjkl1

### Print info

    $ idb info
    [INFO]
    ...

### Print apps (User appplications)

    $ idb apps
    iBooks                  com.apple.iBooks
    Find My iPhone          com.apple.mobileme.fmip1
    Podcasts                com.apple.podcasts
    -                       com.apple.Remote

### Install app

    $ idb install /path/to/demo.ipa
    $ idb install /path/to/demo.app

### Uninstall app

    $ idb unintall com.apple.iBooks

### List directory

    $ idb ls com.apple.iBooks 
    $ idb ls com.apple.iBooks Documents

### Copy directory

    $ idb cp com.apple.iBooks 
    $ idb cp com.apple.iBooks Documents

### Up directory

    $ idb up com.apple.iBooks Documents

### Print syslog

    $ idb logcat

