#include <ruby.h>

VALUE rb_cAwesomeHash;

static void
convert_key(VALUE *argv)
{
	VALUE key;
	key = argv[0];

	if (SYMBOL_P(key)) {
		key = rb_sym2str(key);
		argv[0] = key;
	}
}

static VALUE
rb_awesome_hash_new(VALUE hash);

static VALUE
convert_value(VALUE obj);

static VALUE
convert_value(VALUE obj)
{
	long i;
	VALUE collect;
	if (!SPECIAL_CONST_P(obj)) {
	    switch (BUILTIN_TYPE(obj)) {
	      case T_HASH:
		obj = rb_awesome_hash_new(obj);
		break;
	      case T_ARRAY:
		collect = rb_ary_new2(RARRAY_LEN(obj));
		for (i = 0; i < RARRAY_LEN(obj); i++) {
			rb_ary_push(collect, convert_value(RARRAY_AREF(obj, i)));
		}
		obj = collect;
		break;
	    }
	}
	return obj;
}

static int
convert_key_i(VALUE key, VALUE value, VALUE hash)
{
	convert_key(&key);
	rb_hash_aset(hash, key, convert_value(value));
	return ST_CONTINUE;
}

static VALUE
rb_awesome_hash_new(VALUE hash2)
{
	VALUE hash1;
	hash1 =	rb_funcall(rb_cAwesomeHash, rb_intern("new"), 0);
	rb_hash_foreach(hash2, convert_key_i, hash1);
	return hash1;
}

static VALUE
rb_awesome_hash_s_create(int argc, VALUE *argv, VALUE klass)
{
	VALUE hash;
	hash = rb_call_super(argc, argv);
	return rb_awesome_hash_new(hash);
}

static VALUE
rb_awesome_hash_aref(VALUE hash, VALUE key)
{
	convert_key(&key);
	return rb_hash_aref(hash, key);
}

static VALUE
rb_awesome_hash_aset(VALUE hash, VALUE key, VALUE val)
{
	convert_key(&key);
	return rb_hash_aset(hash, key, convert_value(val));
}

/*
static VALUE
rb_awesome_hash_default(int argc, VALUE *argv, VALUE hash)
{
	rb_check_arity(argc, 0, 1);
	if (argc) {
		convert_key(argv);
	}
	return rb_call_super(argc, argv);
}
*/

static VALUE
rb_awesome_hash_fetch(int argc, VALUE *argv, VALUE hash)
{
	convert_key(argv);
	return rb_call_super(argc, argv);
}

static VALUE
rb_awesome_hash_values_at(int argc, VALUE *argv, VALUE hash)
{
	VALUE result = rb_ary_new2(argc);
	long i;

	for (i=0; i<argc; i++) {
		rb_ary_push(result, rb_awesome_hash_aref(hash, argv[i]));
	}
	return result;
}

static VALUE
rb_awesome_hash_delete_m(VALUE hash, VALUE key)
{
	convert_key(&key);
	return rb_call_super(1, &key);
}

static int
rb_awesome_hash_invert_i(VALUE key, VALUE value, VALUE hash)
{
	rb_awesome_hash_aset(hash, value, key);
	return ST_CONTINUE;
}

static VALUE
rb_awesome_hash_invert(VALUE hash)
{
    VALUE h = rb_hash_new();

    rb_hash_foreach(hash, rb_awesome_hash_invert_i, h);
    return h;
}

static VALUE
rb_awesome_hash_update(VALUE self, VALUE hash)
{
	hash = rb_awesome_hash_new(hash);
	return rb_call_super(1, &hash);
}

static VALUE
rb_awesome_hash_has_key(VALUE hash, VALUE key)
{
	convert_key(&key);
	return rb_call_super(1, &key);
}

static VALUE
rb_awesome_hash_dig(int argc, VALUE *argv, VALUE self)
{
	rb_check_arity(argc, 1, UNLIMITED_ARGUMENTS);
	convert_key(argv);
	return rb_call_super(argc, argv);
}

void Init_awesome_hash()
{
	rb_cAwesomeHash = rb_define_class("AwesomeHash", rb_cHash);

	rb_define_singleton_method(rb_cAwesomeHash, "[]", rb_awesome_hash_s_create, -1);

	rb_define_method(rb_cAwesomeHash,"[]", rb_awesome_hash_aref, 1);
	rb_define_method(rb_cAwesomeHash,"[]=", rb_awesome_hash_aset, 2);
	rb_define_method(rb_cAwesomeHash,"fetch", rb_awesome_hash_fetch, -1);
	rb_define_method(rb_cAwesomeHash,"store", rb_awesome_hash_aset, 2);
	// see tests
	/* rb_define_method(rb_cAwesomeHash,"default", rb_awesome_hash_default, -1); */

	rb_define_method(rb_cAwesomeHash,"values_at", rb_awesome_hash_values_at, -1);

	rb_define_method(rb_cAwesomeHash,"delete", rb_awesome_hash_delete_m, 1);
	rb_define_method(rb_cAwesomeHash,"invert", rb_awesome_hash_invert, 0);
	rb_define_method(rb_cAwesomeHash,"update", rb_awesome_hash_update, 1);
	/* rb_define_method(rb_cAwesomeHash,"replace", rb_awesome_hash_replace, 1); */
	rb_define_method(rb_cAwesomeHash,"merge!", rb_awesome_hash_update, 1);
	/* rb_define_method(rb_cAwesomeHash,"merge", rb_awesome_hash_merge, 1); */
	/* rb_define_method(rb_cAwesomeHash, "assoc", rb_awesome_hash_assoc, 1); */

	rb_define_method(rb_cAwesomeHash,"include?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"member?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"has_key?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"key?", rb_awesome_hash_has_key, 1);

	rb_define_method(rb_cAwesomeHash, "dig", rb_awesome_hash_dig, -1);
}
