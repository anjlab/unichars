$:.unshift(File.expand_path('../../ext/glib', __FILE__))
$:.unshift(File.expand_path('../../lib', __FILE__))

require 'rubygems' rescue LoadError
require 'test/spec'