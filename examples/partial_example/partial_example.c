struct line {
  char *field;
  size_t field_len;
  char *value;
  size_t value_len;
};

#define CURRENT_LINE (&header[nlines-1])
#define MAX_HEADER_LINES 2000

static struct line header[MAX_HEADER_LINES];
static int nlines = 0;
static int last_was_value = 0;

void
on_header_field (http_parser *_, const char *at, size_t len)
{
  if (last_was_value) {
    nlines++;

    if (nlines == MAX_HEADER_LINES) ;// error!
    
    CURRENT_LINE->value = NULL;
    CURRENT_LINE->value_len = 0;

    CURRENT_LINE->field_len = len;
    CURRENT_LINE->field = malloc(len+1);
    strncpy(CURRENT_LINE->field, at, len);

  } else {
    assert(CURRENT_LINE->value == NULL);
    assert(CURRENT_LINE->value_len == 0);

    CURRENT_LINE->field_len += len;
    CURRENT_LINE->field = realloc(CURRENT_LINE->field,
        CURRENT_LINE->field_len+1);
    strncat(CURRENT_LINE->field, at, len);
  }

  CURRENT_LINE->field[CURRENT_LINE->field_len] = '\0';
  last_was_value = 0;
}

void
on_header_value (http_parser *_, const char *at, size_t len)
{
  if (!last_was_value) {
    CURRENT_LINE->value_len = len;
    CURRENT_LINE->value = malloc(len+1);
    strncpy(CURRENT_LINE->value, at, len);

  } else {
    CURRENT_LINE->value_len += len;
    CURRENT_LINE->value = realloc(CURRENT_LINE->value,
        CURRENT_LINE->value_len+1);
    strncat(CURRENT_LINE->value, at, len);
  }

  CURRENT_LINE->value[CURRENT_LINE->value_len] = '\0';
  last_was_value = 1;
}