#include "form.h"
#include "ui.h"


FORM *form_create(WINDOW *window, FieldSet *set)
{
    int rows, cols;

    FIELD **fields = set->array;
    FORM *form = new_form(fields);

    scale_form(form, &rows, &cols);
    keypad(window, TRUE);

    set_form_win(form, window);
    set_form_sub(form, derwin(window, rows, cols, PADDING, PADDING));

    post_form(form);

    return form;
}

void form_default_actions(FORM *form, int input)
{
    switch (input) {
        case KEY_NPAGE:
            form_driver(form, REQ_NEXT_FIELD);
            form_driver(form, REQ_END_LINE);
            break;
        case KEY_PPAGE:
            form_driver(form, REQ_PREV_FIELD);
            form_driver(form, REQ_END_LINE);
            break;
        case KEY_LEFT:
            form_driver(form, REQ_PREV_CHAR);
            break;
        case KEY_RIGHT:
            form_driver(form, REQ_NEXT_FIELD);
            break;
        case KEY_BACKSPACE:
        case 127:
            form_driver(form, REQ_DEL_PREV);
            break;
        case KEY_DC:
            form_driver(form, REQ_DEL_CHAR);
            break;
        case KEY_HOME:
            form_driver(form, REQ_BEG_LINE);
            break;
        case KEY_END:
            form_driver(form, REQ_END_LINE);
            break;
        case KEY_UP:
            form_driver(form, REQ_PREV_LINE);
            break;
        case KEY_DOWN:
            form_driver(form, REQ_NEXT_LINE);
            break;
        default:
            form_driver(form, input);
            break;
    }
}
