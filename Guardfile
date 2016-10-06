# vim: set ft=ruby :
# A sample Guardfile
# More info at https://github.com/guard/guard#readme

## Uncomment and set this to only include directories you want to watch
# directories %w(app lib config test spec features) \
#  .select{|d| Dir.exists?(d) ? d : UI.warning("Directory #{d} does not exist")}

## Note: if you are using the `directories` clause above and you are not
## watching the project directory ('.'), then you will want to move
## the Guardfile to a watched dir and symlink it back, e.g.
#
#  $ mkdir config
#  $ mv Guardfile config/
#  $ ln -s config/Guardfile .
#
# and, you'll have to watch "config/Guardfile" instead of "Guardfile"

guard :bundler do
  watch('Gemfile')
end

guard :shell do
  watch(/^(src\/.*)|(test\/.*)$/) do
    `mkdir -p build`
    `cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..`
    `cd build && make && ./unit_tests`
  end
end
