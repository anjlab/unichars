#include <ruby.h>
#include <glib.h>

#ifndef RSTRING_LEN
#define RSTRING_LEN(string) RSTRING(string)->len
#endif

#ifndef RSTRING_PTR
#define RSTRING_PTR(string) RSTRING(string)->ptr
#endif

/*
 *  call-seq:
 *    utf8_size(string)
 *
 *  Returns the length of the string expressed in codepoints.
 *
 *    Glib.utf8_size('A ehm…, word.') #=> 13
 */
static VALUE utf8_size(VALUE self, VALUE string)
{
  VALUE result;

  Check_Type(string, T_STRING);
  result = ULONG2NUM(g_utf8_strlen(StringValuePtr(string), RSTRING_LEN(string)));

  return result;
}

/*
 *  call-seq:
 *    utf8_upcase(string)
 *
 *  Returns the string in capitals if they are are available for the supplied characters.
 *
 *    Glib.utf8_upcase('Sluß') #=> SLUSS
 */
static VALUE utf8_upcase(VALUE self, VALUE string)
{
  VALUE result;
  gchar *temp;

  Check_Type(string, T_STRING);
  temp = g_utf8_strup(StringValuePtr(string), RSTRING_LEN(string));
  result = rb_str_new2(temp);
  free(temp);

  return result;
}

/*
 *  call-seq:
 *    utf8_downcase(string)
 *
 *  Returns the string in lowercase characters if they are are available for the supplied characters.
 *
 *    Glib.utf8_downcase('ORGANISÉE') #=> organisée
 */
static VALUE utf8_downcase(VALUE self, VALUE string)
{
  VALUE result;
  gchar *temp;

  Check_Type(string, T_STRING);
  temp = g_utf8_strdown(StringValuePtr(string), RSTRING_LEN(string));
  result = rb_str_new2(temp);
  free(temp);

  return result;
}

/*
 *  call-seq:
 *    utf8_reverse(string)
 *
 *  Returns a string with the characters in reverse order.
 *
 *    Glib.utf8_reverse('Comment ça va?') #=> av aç tnemmoC
 */
static VALUE utf8_reverse(VALUE self, VALUE string)
{
  VALUE result;
  gchar *temp;

  Check_Type(string, T_STRING);
  temp = g_utf8_strreverse(StringValuePtr(string), RSTRING_LEN(string));
  result = rb_str_new2(temp);
  free(temp);

  return result;
}


/*
 *  call-seq:
 *    utf_normalize(string, form)
 *
 *  Returns the normalized form of the string. See http://www.unicode.org/reports/tr15/tr15-29.html for more
 *  information about normalization.
 *
 *  <i>form</i> can be one of the following: <tt>:c</tt>, <tt>:kc</tt>, <tt>:d</tt>, or <tt>:kd</tt>.
 *
 *    decomposed = [101, 769].pack('U*')
 *    composed = Glib.utf8_normalize(decomposed, :kc)
 *    composed.unpack('U*') #=> [233]
 */
static VALUE utf8_normalize(VALUE self, VALUE string, VALUE form)
{
  VALUE result;
  gchar *temp;
  GNormalizeMode mode;

  Check_Type(string, T_STRING);
  Check_Type(form, T_SYMBOL);

  if (ID2SYM(rb_intern("d")) == form) {
    mode = G_NORMALIZE_NFD;
  } else if (ID2SYM(rb_intern("c")) == form) {
    mode = G_NORMALIZE_NFC;
  } else if (ID2SYM(rb_intern("kd")) == form) {
    mode = G_NORMALIZE_NFKD;
  } else if (ID2SYM(rb_intern("kc")) == form) {
    mode = G_NORMALIZE_NFKC;
  } else {
    rb_raise(rb_eArgError, "%s is not a valid normalization form, options are: :d, :kd, :c, or :kc", RSTRING_PTR(rb_inspect(form)));
  }

  temp = g_utf8_normalize(StringValuePtr(string), RSTRING_LEN(string), mode);
  result = rb_str_new2(temp);
  free(temp);

  return result;
}

/*
 *  call-seq:
 *    utf8_titleize(string)
 *
 *  Returns a title case string.
 *
 *    Glib.utf8_titleize('привет всем') #=> Привет Всем
 */
static VALUE utf8_titleize(VALUE self, VALUE string)
{
  VALUE result;
  gunichar *unichars;
  gchar *temp;
  GError *error;

  glong len = RSTRING_LEN(string);
  Check_Type(string, T_STRING);
  unichars = g_utf8_to_ucs4(StringValuePtr(string), len, NULL, NULL, &error);

  gboolean is_first_in_word = TRUE;

  int i;
  for (i = 0; i < len; i++) {
    gunichar c = unichars[i];
    if (g_unichar_isalpha(c) && is_first_in_word == TRUE) {
      unichars[i] = g_unichar_totitle(c);
      is_first_in_word = FALSE;
    }

    if (g_unichar_isspace(c) || g_unichar_ispunct(c)) {
      is_first_in_word = TRUE;
    }
  }

  temp = g_ucs4_to_utf8(unichars, len, NULL, NULL, &error);

  result = rb_str_new(temp, len);
  free(temp);
  free(unichars);

  return result;
}


/* The Glib module holds methods which wrap Glib2 functions.
 */
void
Init_glib()
{
  VALUE mGlib;

  mGlib = rb_define_module("Glib");
  rb_define_module_function(mGlib, "utf8_size", utf8_size, 1);
  rb_define_module_function(mGlib, "utf8_upcase", utf8_upcase, 1);
  rb_define_module_function(mGlib, "utf8_downcase", utf8_downcase, 1);
  rb_define_module_function(mGlib, "utf8_reverse", utf8_reverse, 1);
  rb_define_module_function(mGlib, "utf8_normalize", utf8_normalize, 2);
  rb_define_module_function(mGlib, "utf8_titleize", utf8_titleize, 1);
}
