# Contact angle measurement from raw image with liquid drop on a surface

This repository contains proof of concept code for **contact angle** measurement from raw image of liquid drop on a surface using **Computer Vision** techniques.

Insipred by https://github.com/wenting-zhao/ContactAngle.

# How to build

**NOTE**: C++ 17 compiler support required

## Ubuntu 18.04

### Prerequisites
- build-essential
- cmake
- libopencv-dev

```bash
$ git clone https://github.com/Blyschak/libcontactangle
$ cd libcontactangle
$ mkdir build
$ cmake ..
$ make
```

# High level overview of the processing pipeline

* load raw image
* convert it to gray scale
* apply blur to reduce noise for next step
* canny edge detection
* randomly pick many potential circles
* get best candidate that fits liquid drop surface
* get edge pixels outside the candidate circle
* get the surface line based on pixels from above step
* given a surface line and a circle get a contact angle
