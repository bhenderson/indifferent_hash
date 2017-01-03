#include <ruby.h>

VALUE rb_cAwesomeHash;

// from error.c
static const char *
builtin_class_name(VALUE x)
{
	const char *etype;

	if (NIL_P(x)) {
		etype = "nil";
	}
	else if (FIXNUM_P(x)) {
		etype = "Fixnum";
	}
	else if (SYMBOL_P(x)) {
		etype = "Symbol";
	}
	else if (RB_TYPE_P(x, T_TRUE)) {
		etype = "true";
	}
	else if (RB_TYPE_P(x, T_FALSE)) {
		etype = "false";
	}
	else {
		etype = NULL;
	}
	return etype;
}

// from error.c
const char *
rb_builtin_class_name(VALUE x)
{
	const char *etype = builtin_class_name(x);

	if (!etype) {
		etype = rb_obj_classname(x);
	}
	return etype;
}

static void
convert_key(VALUE *argv)
{
	VALUE key;
	key = argv[0];

	if (SYMBOL_P(key)) argv[0] = rb_sym2str(key);
}

static VALUE
rb_awesome_hash_new(VALUE klass, VALUE hash);

static VALUE
convert_value(VALUE klass, VALUE obj);

static VALUE
convert_value(VALUE klass, VALUE obj)
{
	long i;
	VALUE collect;

	if (SPECIAL_CONST_P(obj)) return obj;
	switch (BUILTIN_TYPE(obj)) {
		case T_HASH:
			obj = rb_awesome_hash_new(klass, obj);
			break;
		case T_ARRAY:
			collect = rb_ary_new2(RARRAY_LEN(obj));
			for (i = 0; i < RARRAY_LEN(obj); i++) {
				rb_ary_push(collect, convert_value(klass, RARRAY_AREF(obj, i)));
			}
			obj = collect;
			break;
	}
	return obj;
}

static int
convert_key_i(VALUE key, VALUE value, VALUE arg)
{
	VALUE *args = (VALUE *)arg;
	VALUE klass = args[0];
	VALUE hash = args[1];

	convert_key(&key);
	rb_hash_aset(hash, key, convert_value(klass, value));
	return ST_CONTINUE;
}

static VALUE
rb_awesome_hash_new(VALUE klass, VALUE hash2)
{
	VALUE args[2];

	args[0] = klass;
	args[1] = rb_class_new_instance(0, 0, klass);
	rb_hash_foreach(hash2, convert_key_i, (VALUE)args);
	return args[1];
}

static VALUE
rb_awesome_hash_aset(VALUE hash, VALUE key, VALUE val);

// rb_hash_s_create reimplemented to support recursive klass creation.
static VALUE
rb_awesome_hash_s_create(int argc, VALUE *argv, VALUE klass)
{
	VALUE hash, tmp;
	int i;

	if (argc == 1) {
		tmp = rb_check_hash_type(argv[0]);
		if (!NIL_P(tmp)) {
			// hash = hash_alloc(klass);
			return rb_awesome_hash_new(klass, tmp);
		}

		tmp = rb_check_array_type(argv[0]);
		if (!NIL_P(tmp)) {
			long i;

			// hash = hash_alloc(klass);
			hash = rb_class_new_instance(0, 0, klass);
			for (i = 0; i < RARRAY_LEN(tmp); ++i) {
				VALUE e = RARRAY_AREF(tmp, i);
				VALUE v = rb_check_array_type(e);
				VALUE key, val = Qnil;

				if (NIL_P(v)) {
#if 0 /* refix in the next release */
					rb_raise(rb_eArgError, "wrong element type %s at %ld (expected array)",
						 rb_builtin_class_name(e), i);

#else
					rb_warn("wrong element type %s at %ld (expected array)",
						rb_builtin_class_name(e), i);
					rb_warn("ignoring wrong elements is deprecated, remove them explicitly");
					rb_warn("this causes ArgumentError in the next release");
					continue;
#endif
				}
				switch (RARRAY_LEN(v)) {
					default:
						rb_raise(rb_eArgError, "invalid number of elements (%ld for 1..2)",
							 RARRAY_LEN(v));
					case 2:
						val = RARRAY_AREF(v, 1);
					case 1:
						RARRAY_AREF(v, 0);
						some_hash_aset(hash, key, val);
				}
			}
			return hash;
		}
	}
	if (argc % 2 != 0) {
		rb_raise(rb_eArgError, "odd number of arguments for Hash");
	}

	hash = rb_class_new_instance(0, 0, klass);
	for (i=0; i<argc; i+=2) {
		rb_awesome_hash_aset(hash, argv[i], argv[i + 1]);
	}
	return hash;
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
	return rb_hash_aset(hash, key, convert_value(rb_obj_class(hash), val));
}

static VALUE
rb_awesome_hash_equal(VALUE self, VALUE hash)
{
	hash = rb_awesome_hash_new(rb_obj_class(self), hash);
	return rb_call_super(1, &hash);
}

/*
static VALUE
rb_awesome_hash_default(int argc, VALUE *argv, VALUE hash)
{
	rb_check_arity(argc, 0, 1);
	if (argc) convert_key(argv);
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
rb_awesome_hash_invert(VALUE self)
{
	VALUE hash = rb_newobj_of(rb_obj_class(self), 0);

	rb_hash_foreach(self, rb_awesome_hash_invert_i, hash);
	return hash;
}

static VALUE
rb_awesome_hash_update(VALUE self, VALUE hash)
{
	hash = rb_awesome_hash_new(rb_obj_class(self), hash);
	return rb_call_super(1, &hash);
}

static VALUE
rb_awesome_hash_replace(VALUE self, VALUE hash)
{
	hash = rb_awesome_hash_new(rb_obj_class(self), hash);
	return rb_call_super(1, &hash);
}

static VALUE
rb_awesome_hash_assoc(VALUE self, VALUE key)
{
	convert_key(&key);
	return rb_call_super(1, &key);
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

	rb_define_method(rb_cAwesomeHash,"==", rb_awesome_hash_equal, 1);
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
	rb_define_method(rb_cAwesomeHash,"replace", rb_awesome_hash_replace, 1);
	rb_define_method(rb_cAwesomeHash,"merge!", rb_awesome_hash_update, 1);
	// merge just works
	/* rb_define_method(rb_cAwesomeHash,"merge", rb_awesome_hash_merge, 1); */
	rb_define_method(rb_cAwesomeHash, "assoc", rb_awesome_hash_assoc, 1);

	rb_define_method(rb_cAwesomeHash,"include?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"member?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"has_key?", rb_awesome_hash_has_key, 1);
	rb_define_method(rb_cAwesomeHash,"key?", rb_awesome_hash_has_key, 1);

	rb_define_method(rb_cAwesomeHash, "dig", rb_awesome_hash_dig, -1);
}
