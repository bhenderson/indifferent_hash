require "minitest/autorun"
require "indifferent_hash"

class TestIndifferentHash < MiniTest::Test
  def setup
    @hash = IndifferentHash["foo" => "bar"]
  end

  def test_class_create_from_hash
    @hash = IndifferentHash[:foo => {:bar => :baz, :arr => [{:taz => 1}]}]
    assert_kind_of IndifferentHash, @hash["foo"]
    assert_kind_of IndifferentHash, @hash["foo"]["arr"][0]
  end

  def test_class_create_from_tuple
    @hash = IndifferentHash[:foo, :bar, :baz, :taz]
    assert_kind_of IndifferentHash, @hash
    assert_equal Hash["foo", :bar, "baz", :taz], @hash.to_h
  end

  def test_class_create_from_array
    @hash = IndifferentHash[ [[:a], [:b, 2]] ]
    assert_equal({"a" => nil, "b" => 2}, @hash)
  end

  def test_class_create_fails_with_array_of_non_tuples
    assert_output nil, /warning: wrong element/ do
      IndifferentHash[ [:foo, :bar] ]
    end
    assert_raises ArgumentError do
      IndifferentHash[ :foo ]
    end
  end

  def test_aref_with_string
    assert_equal "bar", @hash["foo"]
  end

  def test_aref_with_symbol
    assert_equal "bar", @hash[:foo]
  end

  def test_store_with_symbol
    @hash[:bar] = "baz"
    assert_equal "baz", @hash["bar"]
  end

  def test_equal_equal
    exp, act = Hash[:foo => "bar"], @hash
    msg = message(msg, E) { diff exp, act }
    assert act == exp, msg # assert_equal is defined as exp == act, but I'm testing act.==()
  end

  def test_values_at
    @hash["bar"] = "baz"
    assert_equal ["bar", "baz"], @hash.values_at(:foo, :bar)
  end

  def test_delete
    assert_equal "bar", @hash.delete(:foo)
    assert_equal [], @hash.keys
  end

  def test_invert
    @hash[:bar] = :baz
    @inverted = @hash.invert
    assert_equal Hash["baz" => "bar", "bar" => "foo"], @inverted
    assert_instance_of IndifferentHash, @inverted
  end

  def test_key_eh
    assert_operator @hash, :include?, :foo
    assert_operator @hash, :member?, :foo
    assert_operator @hash, :has_key?, :foo
    assert_operator @hash, :key?, :foo
  end

  def test_dig
    @hash[:foo] = IndifferentHash["bar" => "baz"]
    assert_equal "baz", @hash.dig(:foo, :bar)
  end

  def test_update
    @hash.update :bar => :baz
    assert_equal :baz, @hash["bar"]

    @hash.update(:foo => 3) { |*arg| arg }
    assert_equal ["foo", "bar", 3], @hash[:foo]
  end

  def test_store_recursive
    @hash[:bar] = {:baz => :taz}
    assert_kind_of IndifferentHash, @hash[:bar]
  end

  def test_default
    # The 2.x docs as well as ActiveSupport say that default(key) should
    # return like fetch(key, default) but in my testing, no ruby version ever
    # did this.
    @hash.default = 3
    assert_equal 3, @hash.default("foo")
  end

  def test_convert_subclass
    @klass = Class.new(IndifferentHash)
    @hash = @klass[:foo => {:bar => :baz}]
    assert_kind_of @klass, @hash["foo"]
  end

  def test_fetch
    assert_equal "bar", @hash.fetch(:foo)
    assert_equal "bar", @hash.fetch(:bar){|key| key}
  end

  def test_merge
    @newhash = @hash.merge("bar" => "baz")
    refute @hash.key?("bar")
    assert_kind_of IndifferentHash, @newhash
    assert_equal "baz", @newhash[:bar]
  end

  def test_replace
    @hash.replace(:bar => "baz")
    assert_equal ["bar"], @hash.keys
  end

  def test_to_h
    @hash[:foo] = {:bar => :baz}
    assert_kind_of Hash, @hash.to_h
    assert_kind_of IndifferentHash, @hash["foo"]
    assert_kind_of Hash, @hash.to_h["foo"]
  end

  def test_assoc
    assert_equal ["foo", "bar"], @hash.assoc(:foo)
  end
end
