require 'awesome_hash/awesome_hash'

class AwesomeHash
  VERSION = "1.0.0"

  def ==(other)
    super self.class[other]
  end
end