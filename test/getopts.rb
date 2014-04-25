optstring = "abc:d:012"
longopts = [
            ["add",     1, 0, 0],
            ["append",  0, 0, 0],
            ["delete",  1, 0, 0],
            ["verbose", 0, 0, 0],
            ["create",  1, 0, 99], # 99 == 'c'
            ["file",    1, 0, 0]
          ]

assert "Array#getopt_long" do
  # Array items must consist of Strings only
  assert_raise TypeError do
    ary = [0]
    class << ary; include Getopts; end
    ary.getopt_long(optstring, longopts)
  end

  # Array#getopt_long requires two arguments
  assert_raise ArgumentError do
    ary = %w(-a)
    class << ary; include Getopts; end
    ary.getopt_long(optstring)
  end

  # The second argument must be an array of arrays, each of which has four items
  assert_raise ArgumentError do
    ary = %w(-a)
    class << ary; include Getopts; end
    ary.getopt_long(optstring, [0])
  end
  assert_raise ArgumentError do
    ary = %w(-a)
    class << ary; include Getopts; end
    ary.getopt_long(optstring, [[0]])
  end

  ary = %w(--create=foo)
  class << ary; include Getopts; end
  opt = ary.getopt_long(optstring, longopts)
  assert_equal %w(c), opt.keys
  assert_equal ["foo"], opt.values

  ary = %w(-a -b -c 100 -d 200 -0 -1 -2)
  class << ary; include Getopts; end
  opt = ary.getopt_long(optstring, [])
  assert_equal %w(a b c d 0 1 2), opt.keys
  assert_equal ["", "", "100", "200", "", "", ""], opt.values

  # I had a hard time, thanks to optind global variable. :(
  ary = %w(--add=foo --append --delete=bar --verbose --create=baz --file=qux)
  class << ary; include Getopts; end
  opt = ary.getopt_long("", longopts)
  assert_equal %w(add append delete verbose c file), opt.keys
  assert_equal ["foo", "", "bar", "", "baz", "qux"], opt.values

  ary = %w(-a -b -c foo -d bar -0 -1 -2 --add=foo --append --delete=bar --verbose --create=baz --file=qux)
  class << ary; include Getopts; end
  opt = ary.getopt_long(optstring, longopts)
  assert_equal %w(a b c d 0 1 2 add append delete verbose file), opt.keys
  assert_equal ["", "", "baz", "bar", "", "", "", "foo", "", "bar", "", "qux"], opt.values
end

assert "Array#getopts" do
  ary = %w(--verbose -c foo --add=bar)
  class << ary; include Getopts; end
  opt = ary.getopts('c:d', "verbose", "add:", "hoge:piyo")
  assert_equal %w(verbose c add hoge), opt.keys
  assert_equal ["", "foo", "bar", "piyo"], opt.values
end
