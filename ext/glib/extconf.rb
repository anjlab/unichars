# encoding: utf-8

require 'mkmf'

name = "glib"

unless pkg_config('glib-2.0')
  abort "Glib requires glib-2.0 and pkg_config"
end

dir_config(name)

create_makefile(name)
