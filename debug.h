
#include <libdragon.h>

inline void debug_print(int x, int y, char* label, float value) {
    rdpq_text_printf(NULL, FONT_BUILTIN_DEBUG_MONO, x, y, "OBJTIME : %.2f", value);
  }
  
inline void debug_print_inputs(_SI_condat *inputs)
{
    printf(
        "Stick: %+04d,%+04d\n",
        inputs->x, inputs->y
    );
    printf(
        "D-U:%d D-D:%d D-L:%d D-R:%d C-U:%d C-D:%d C-L:%d C-R:%d\n",
        inputs->up, inputs->down,
        inputs->left, inputs->right,
        inputs->C_up, inputs->C_down,
        inputs->C_left, inputs->C_right
    );
    printf(
        "A:%d B:%d L:%d R:%d Z:%d Start:%d\n",
        inputs->A, inputs->B,
        inputs->L, inputs->R,
        inputs->Z, inputs->start
    );
}