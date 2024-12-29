#include "menu.h"

#include <string.h>

void multi_select_combo_box(struct nk_context *ctx, const char **options, int **selections, int options_count, char *preview_text)
{
    const int preview_text_size = 128;

    preview_text[0] = '\0';
    for (int k = 0; k < options_count; k++)
    {
        if (*selections[k])
        {
            if (strlen(preview_text) > 0)
            {
                strncat(preview_text, ", ", preview_text_size - strlen(preview_text) - 1);
            }
            strncat(preview_text, options[k], preview_text_size - strlen(preview_text) - 1);
        }
    }
    if (strlen(preview_text) == 0)
    {
        strncpy(preview_text, "None", preview_text_size - 1);
        preview_text[preview_text_size - 1] = '\0';
    }

    if (nk_combo_begin_label(ctx, preview_text, nk_vec2(nk_widget_width(ctx), 300)))
    {
        nk_layout_row_dynamic(ctx, 20, 1);
        for (int i = 0; i < options_count; i++)
        {
            if (nk_checkbox_label(ctx, options[i], selections[i]))
            {
                preview_text[0] = '\0';
                for (int k = 0; k < options_count; k++)
                {
                    if (*selections[k])
                    {
                        if (strlen(preview_text) > 0)
                        {
                            strncat(preview_text, ", ", preview_text_size - strlen(preview_text) - 1);
                        }
                        strncat(preview_text, options[k], preview_text_size - strlen(preview_text) - 1);
                    }
                }
                if (strlen(preview_text) == 0)
                {
                    strncpy(preview_text, "None", preview_text_size - 1);
                    preview_text[preview_text_size - 1] = '\0';
                }
            }
        }
        nk_combo_end(ctx);
    }
}