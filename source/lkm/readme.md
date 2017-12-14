installing 3rd party modules in newer versions
of ubunut:

disable secure boot

alternatively,
install mokutil and do :
sudo mokutil --disable-validation.

Note, this is not a problem on the beaglebone,
but with trying to build a simple module on
the other laptop, it gave an error.

