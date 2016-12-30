require "minitest/autorun"
require "awesome_hash"

class TestAwesomeHash < MiniTest::Test
  def setup
    @ahash = AwesomeHash["foo" => "bar"]
  end

  def test_aref_with_string
    assert_equal "bar", @ahash["foo"]
  end

  def test_aref_with_symbol
    assert_equal "bar", @ahash[:foo]
  end

  def test_store_with_symbol
    @ahash[:bar] = "baz"
    assert_equal "baz", @ahash["bar"]
  end

  def test_class_create
    @ahash = AwesomeHash[:foo => "bar"]
    assert_equal "bar", @ahash["foo"]
  end

  def test_equal_equal
    exp, act = Hash[:foo => "bar"], @ahash
    msg = message(msg, E) { diff exp, act }
    assert act == exp, msg # assert_equal is defined as exp == act, but I'm testing act.==()
  end

  def test_values_at
    @ahash[:bar] = "baz"
    assert_equal ["bar", "baz"], @ahash.values_at(:foo, :bar)
  end

  def test_delete
    assert_equal "bar", @ahash.delete(:foo)
  end

  def test_invert
    @ahash[:bar] = :baz
    assert_equal Hash["baz" => "bar", "bar" => "foo"], @ahash.invert
  end

  def test_key_eh
    assert_operator @ahash, :include?, :foo
    assert_operator @ahash, :member?, :foo
    assert_operator @ahash, :has_key?, :foo
    assert_operator @ahash, :key?, :foo
  end

  def test_recursive_create
    @ahash = AwesomeHash[:foo => {:bar => :baz}]
    assert_kind_of AwesomeHash, @ahash[:foo]
  end

  def test_dig
    @ahash[:foo] = AwesomeHash["bar" => "baz"]
    assert_equal "baz", @ahash.dig(:foo, :bar)
  end

  def test_fetch
    assert_equal "bar",@ahash.fetch(:foo)
  end
end
