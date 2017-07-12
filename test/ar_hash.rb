# Basic implementation of HashWithIndifferentAccess for benchmark comparison.
class ARHash < Hash
  def self.[](other) self.convert(other) end
  def self.convert(other)
    case other
    when self
      other
    when Hash
      new.update other
    when Array
      other.map{|o| convert(o)}
    else
      other
    end
  end

  alias_method :regular_writer, :[]= unless method_defined?(:regular_writer)
  alias_method :regular_update, :update unless method_defined?(:regular_update)

  def default(key = nil)
    if key.is_a?(Symbol) && include?(key = key.to_s)
      self[key]
    else
      super
    end
  end
  def []=(key, value)
    regular_writer(convert_key(key), convert_value(value))
  end
  def update(other_hash)
    if other_hash.is_a? self.class
      super(other_hash)
    else
      other_hash.to_hash.each_pair do |key, value|
        if block_given? && key?(key)
          value = yield(convert_key(key), self[key], value)
        end
        regular_writer(convert_key(key), convert_value(value))
      end
      self
    end
  end
  def convert_key(key)
    key.kind_of?(Symbol) ? key.to_s : key
  end

  def convert_value(value)
    self.class.convert(value)
  end
end
