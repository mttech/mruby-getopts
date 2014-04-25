Getopts
=======

This module provides getopts to parse command line arguments.

Usage
-----

To parse arguments in ARGV:

    hash = Getopts.getopts(optstring, longopts)

To parse arguments stored in any array:

    ary = []
    class << ary; include Getopts; end
    hash = ary.getopts(optstring, longopts)

Example
-------

    # ARGV = ["--verbose", "-c", "foo", "--add=bar"]
    
    hash = Getopts.getopts('c:d', "verbose", "add:", "hoge:piyo")
    
    # => {"verbose"=>"", "c"=>"foo", "add"=>"bar", "hoge"=>"piyo"}
