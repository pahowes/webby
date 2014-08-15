# webby

An embedded web server for C++ applications.

# Building

Webby is distributed only as a header. There is nothing to compile in this repository other than
tests which also illustrate how to configure and use webby.

## Dependencies

* [CMake](http://www.cmake.org) 2.8.12 or newer
* C compiler. The following have been tested:
  * [llvm](http://llvm.org) 3.4 or newer
  * [gcc](http://gcc.gnu.org) 4.8.3 or newer
  * [Visual Studio](http://www.visualstudio.com)

## Instructions

I highly recommend building outside of the source tree so that build products do not pollute the
repository. The simplest way to accomplish this is to create a sub-directory named "build" as
follows:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make test

# Examples

Add examples here.

# TODO List

These are the classes that are being designed and written:

1.  Server configuration
2.  Error logging
3.  Access logging
4.  Abstract request handler.
5.  Concrete file request handler.

# Contributing

1.  Fork it
2.  Create a feature branch (`git checkout -b new-feature`)
3.  Commit changes (`git commit -am "Added new feature xyz"`)
4.  Push the branch (`git push origin new-feature`)
5.  Create a new pull request.

# Maintainers

* Paul Howes (http://github.com/PaulHowes/)

# License

Webby copyright 2014 Paul Howes and is licensed under the [Apache License](LICENSE).
