#include <ruby.h>

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

static int
convert_key_i(VALUE key, VALUE value, VALUE hash)
{
	if (SYMBOL_P(key)) {
		rb_hash_delete_entry(hash, key);
		rb_hash_aset(hash, rb_sym2str(key), value);
	}
	return ST_CONTINUE;
}

static VALUE
rb_awesome_hash_s_create(int argc, VALUE *argv, VALUE klass)
{
	VALUE hash, ahash;
	hash = rb_call_super(argc, argv);
	ahash = rb_hash_dup(hash);
	rb_hash_foreach(hash, convert_key_i, ahash);
	return ahash;
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
	return rb_hash_aset(hash, key, val);
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

/* update */

/* static void */
/* rb_hash_modify(VALUE hash) */
/* { */
    /* rb_check_frozen(hash); */
    /* hash_tbl(hash); */
/* } */

/* static VALUE */
/* to_hash(VALUE hash) */
/* { */
    /* return rb_convert_type(hash, T_HASH, "Hash", "to_hash"); */
/* } */

/* static int */
/* rb_hash_update_block_i(VALUE key, VALUE value, VALUE hash) */
/* { */
    /* RHASH_UPDATE(hash, key, rb_hash_update_block_callback, value); */
    /* return ST_CONTINUE; */
/* } */

/* static VALUE */
/* rb_awesome_hash_update(VALUE hash1, VALUE hash2) */
/* { */
    /* rb_hash_modify(hash1); */
    /* hash2 = to_hash(hash2); */
    /* if (rb_block_given_p()) { */
	/* rb_hash_foreach(hash2, rb_hash_update_block_i, hash1); */
    /* } */
    /* else { */
	/* rb_hash_foreach(hash2, rb_hash_update_i, hash1); */
    /* } */
    /* return hash1; */
/* } */

/* update end */

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
	VALUE rb_cAwesomeHash = rb_define_class("AwesomeHash", rb_cHash);

	rb_define_singleton_method(rb_cAwesomeHash, "[]", rb_awesome_hash_s_create, -1);

	rb_define_method(rb_cAwesomeHash,"[]", rb_awesome_hash_aref, 1);
	rb_define_method(rb_cAwesomeHash,"[]=", rb_awesome_hash_aset, 2);
	rb_define_method(rb_cAwesomeHash,"store", rb_awesome_hash_aset, 2);

	rb_define_method(rb_cAwesomeHash,"values_at", rb_awesome_hash_values_at, -1);

	rb_define_method(rb_cAwesomeHash,"delete", rb_awesome_hash_delete_m, 1);
	rb_define_method(rb_cAwesomeHash,"invert", rb_awesome_hash_invert, 0);
	/* rb_define_method(rb_cAwesomeHash,"update", rb_awesome_hash_update, 1); */
	/* rb_define_method(rb_cAwesomeHash,"replace", rb_awesome_hash_replace, 1); */
	/* rb_define_method(rb_cAwesomeHash,"merge!", rb_awesome_hash_update, 1); */
	/* rb_define_method(rb_cAwesomeHash,"merge", rb_awesome_hash_merge, 1); */
	/* rb_define_method(rb_cAwesomeHash, "assoc", rb_awesome_hash_assoc, 1); */

	rb_define_method(rb_cAwesomeHash,"include?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"member?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"has_key?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"key?", rb_awesome_hash_has_key, 1);

	rb_define_method(rb_cAwesomeHash, "dig", rb_awesome_hash_dig, -1);
}
