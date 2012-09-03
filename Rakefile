require 'rubygems'
require 'rake'

CC = 'gcc'
CFLAGS = '-Wall'
LDFLAGS = '-lz'
LIBS = ''
INCLUDES= "-Ilib/minizip"
task :default => 'idb'
desc 'Compile idb'
file 'idb' => ['idb.c', 'MobileDevice.h'] do |t|
  sh %Q["#{CC}" "#{CFLAGS}" "#{LDFLAGS}" -o "#{t.name}" \
"#{LIBS}" "#{INCLUDES}" \
-framework CoreFoundation \
-framework MobileDevice \
-F/System/Library/PrivateFrameworks \
"#{t.prerequisites.join('" "')}"]
end

desc 'Install idb on the system'
task :install => 'idb' do |t|
  sh %Q[/bin/cp -f "#{t.prerequisites.join('" "')}" /usr/local/bin/]
end

desc 'Clean'
task :clean do |t|
  sh 'rm -f idb'
end
