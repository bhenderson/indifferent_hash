# -*- ruby -*-

require "rubygems"
require "hoe"
require 'rake/extensiontask'

Hoe.spec "indifferent_hash" do
  developer("Brian Henderson", "henderson.bj@gmail.com")
  self.readme_file = "README.rdoc"
  self.extra_dev_deps << ['rake-compiler', '>= 0']
  self.spec_extras = { :extensions => ["ext/indifferent_hash/extconf.rb"] }

  license "MIT" # this should match the license in the README

  Rake::ExtensionTask.new('indifferent_hash', spec) do |ext|
    ext.lib_dir = File.join('lib', 'indifferent_hash')
  end
end

Rake::Task[:test].prerequisites << :compile

# vim: syntax=ruby
