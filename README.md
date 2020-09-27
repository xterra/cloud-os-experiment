# cloud-os-experiment

* [About](#about)
* [License](#license)
* [Tests](#tests)
   * [Host setup](#host-setup)


## About

This is an experimental project that turns to be a try of making a small prototype of an operating system in the cloud. The goals of the project are as follows:
[ ] Ability to run 2 applications in parallel with separate image and sound output
[ ] One-click application start
[ ] All applications are isolated from each other
[ ] Video streaming over the Internet
[ ] Remote control over the Internet
[ ] GPU-acceleration
[ ] Modern game launched from Proton

The experiment will be considered successful if all mains goals will be completed and as result it will be possible to play 2 modern games on 2 different TVs via the Internet on single computer with single GPU.

Extra goals:
[ ] Low latency: less than 50ms I/O delay
[ ] Adaptive bitrate
[ ] 4K support
[ ] 60+ fps support
[ ] Game controller support
[ ] Multiple window support
[ ] Audio support
[ ] Remote microphone support
[ ] Mobile support


## License

This project is published under GNU AGPLv3. Read more in [LICENSE](LICENSE).


## Tests

### Host setup

* Ubuntu 20.04.1 LTS
* Docker CE 19.03.13
* NVIDIA GeForce RTX2080 Super PCI-E 8192Mb
* Intel Core i7 - 8700K OEM
* 32GB DDR4 3200MHz
* 512 SSD Samsung 860 EVO
