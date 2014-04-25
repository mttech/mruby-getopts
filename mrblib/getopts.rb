#
# getopts for mruby
#
# Copyright (c) 2014 M&T Technology, Inc.
#
module Getopts
  def self.getopts(shortopt, *longopts)
    unless ARGV.methods.include? :getopts
      class << ARGV
        include Getopts
      end
    end
    ARGV.getopts(shortopt, *longopts)
  end

  def getopts(shortopt, *longopts)
    longoptions    = []
    default_values = {}

    longopts.each do |longopt|
      longopt              = longopt.to_s
      has_argument         = (longopt.include?(':') ? 1 : 0)
      name, default        = longopt.split(':', 2)
      default_values[name] = default if default

      longoptions << [name, has_argument, 0, 0]
    end

    opt = self.getopt_long(shortopt.to_s, longoptions)

    default_values.each do |k, v|
      opt[k] = v if not opt.key? k or opt[k].empty?
    end

    opt
  end
end
