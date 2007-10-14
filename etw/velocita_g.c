#include "mytypes.h"

/* Velocita giocatore asse X */

WORD velocita_x[3][10][8]=
{
    { /* Velocita 0 */
        { /* Giocatore con velocita' 0 (0.750000 pixel)*/
            0,
            4,
            5,
            4,
            0,
            -4,
            -5,
            -4,
        },
        { /* Giocatore con velocita' 1 (0.812500 pixel)*/
            0,
            4,
            6,
            4,
            0,
            -4,
            -6,
            -4,
        },
        { /* Giocatore con velocita' 2 (0.875000 pixel)*/
            0,
            4,
            6,
            4,
            0,
            -4,
            -6,
            -4,
        },
        { /* Giocatore con velocita' 3 (1.000000 pixel)*/
            0,
            5,
            7,
            5,
            0,
            -5,
            -7,
            -5,
        },
        { /* Giocatore con velocita' 4 (1.062500 pixel)*/
            0,
            6,
            8,
            6,
            0,
            -6,
            -8,
            -6,
        },
        { /* Giocatore con velocita' 5 (1.125000 pixel)*/
            0,
            6,
            8,
            6,
            0,
            -6,
            -8,
            -6,
        },
        { /* Giocatore con velocita' 6 (1.250000 pixel)*/
            0,
            7,
            9,
            7,
            0,
            -7,
            -9,
            -7,
        },
        { /* Giocatore con velocita' 7 (1.312500 pixel)*/
            0,
            7,
            10,
            7,
            0,
            -7,
            -10,
            -7,
        },
        { /* Giocatore con velocita' 8 (1.375000 pixel)*/
            0,
            7,
            10,
            7,
            0,
            -7,
            -10,
            -7,
        },
        { /* Giocatore con velocita' 9 (1.500000 pixel)*/
            0,
            8,
            11,
            8,
            0,
            -8,
            -11,
            -8,
        },
    },
    { /* Velocita 1 */
        { /* Giocatore con velocita' 0 (1.375000 pixel)*/
            0,
            7,
            10,
            7,
            0,
            -7,
            -10,
            -7,
        },
        { /* Giocatore con velocita' 1 (1.437500 pixel)*/
            0,
            8,
            11,
            8,
            0,
            -8,
            -11,
            -8,
        },
        { /* Giocatore con velocita' 2 (1.500000 pixel)*/
            0,
            8,
            11,
            8,
            0,
            -8,
            -11,
            -8,
        },
        { /* Giocatore con velocita' 3 (1.625000 pixel)*/
            0,
            9,
            12,
            9,
            0,
            -9,
            -12,
            -9,
        },
        { /* Giocatore con velocita' 4 (1.687500 pixel)*/
            0,
            9,
            13,
            9,
            0,
            -9,
            -13,
            -9,
        },
        { /* Giocatore con velocita' 5 (1.750000 pixel)*/
            0,
            9,
            13,
            9,
            0,
            -9,
            -13,
            -9,
        },
        { /* Giocatore con velocita' 6 (1.875000 pixel)*/
            0,
            10,
            14,
            10,
            0,
            -10,
            -14,
            -10,
        },
        { /* Giocatore con velocita' 7 (1.937500 pixel)*/
            0,
            10,
            15,
            10,
            0,
            -10,
            -15,
            -10,
        },
        { /* Giocatore con velocita' 8 (2.000000 pixel)*/
            0,
            11,
            15,
            11,
            0,
            -11,
            -15,
            -11,
        },
        { /* Giocatore con velocita' 9 (2.125000 pixel)*/
            0,
            12,
            16,
            12,
            0,
            -12,
            -16,
            -12,
        },
    },
    { /* Velocita 2 */
        { /* Giocatore con velocita' 0 (2.000000 pixel)*/
            0,
            11,
            15,
            11,
            0,
            -11,
            -15,
            -11,
        },
        { /* Giocatore con velocita' 1 (2.062500 pixel)*/
            0,
            11,
            16,
            11,
            0,
            -11,
            -16,
            -11,
        },
        { /* Giocatore con velocita' 2 (2.125000 pixel)*/
            0,
            12,
            16,
            12,
            0,
            -12,
            -16,
            -12,
        },
        { /* Giocatore con velocita' 3 (2.250000 pixel)*/
            0,
            12,
            17,
            12,
            0,
            -12,
            -17,
            -12,
        },
        { /* Giocatore con velocita' 4 (2.312500 pixel)*/
            0,
            13,
            18,
            13,
            0,
            -13,
            -18,
            -13,
        },
        { /* Giocatore con velocita' 5 (2.375000 pixel)*/
            0,
            13,
            18,
            13,
            0,
            -13,
            -18,
            -13,
        },
        { /* Giocatore con velocita' 6 (2.500000 pixel)*/
            0,
            14,
            19,
            14,
            0,
            -14,
            -19,
            -14,
        },
        { /* Giocatore con velocita' 7 (2.562500 pixel)*/
            0,
            14,
            20,
            14,
            0,
            -14,
            -20,
            -14,
        },
        { /* Giocatore con velocita' 8 (2.625000 pixel)*/
            0,
            14,
            20,
            14,
            0,
            -14,
            -20,
            -14,
        },
        { /* Giocatore con velocita' 9 (2.750000 pixel)*/
            0,
            15,
            21,
            15,
            0,
            -15,
            -21,
            -15,
        },
    },
};

/* Velocita giocatore asse Y */

WORD velocita_y[3][10][8]=
{
    { /* Velocita 0 */
        { /* Giocatore con velocita' 0 (0.187500 pixel)*/
            -1,
            0,
            0,
            0,
            1,
            0,
            0,
            0,
        },
        { /* Giocatore con velocita' 1 (0.312500 pixel)*/
            -2,
            -1,
            0,
            1,
            2,
            1,
            0,
            -1,
        },
        { /* Giocatore con velocita' 2 (0.437500 pixel)*/
            -3,
            -2,
            0,
            2,
            3,
            2,
            0,
            -2,
        },
        { /* Giocatore con velocita' 3 (0.562500 pixel)*/
            -4,
            -2,
            0,
            2,
            4,
            2,
            0,
            -2,
        },
        { /* Giocatore con velocita' 4 (0.687500 pixel)*/
            -4,
            -3,
            0,
            3,
            4,
            3,
            0,
            -3,
        },
        { /* Giocatore con velocita' 5 (0.812500 pixel)*/
            -5,
            -4,
            0,
            4,
            5,
            4,
            0,
            -4,
        },
        { /* Giocatore con velocita' 6 (0.937500 pixel)*/
            -6,
            -4,
            0,
            4,
            6,
            4,
            0,
            -4,
        },
        { /* Giocatore con velocita' 7 (1.062500 pixel)*/
            -7,
            -5,
            0,
            5,
            7,
            5,
            0,
            -5,
        },
        { /* Giocatore con velocita' 8 (1.187500 pixel)*/
            -8,
            -6,
            0,
            6,
            8,
            6,
            0,
            -6,
        },
        { /* Giocatore con velocita' 9 (1.312500 pixel)*/
            -9,
            -6,
            0,
            6,
            9,
            6,
            0,
            -6,
        },
    },
    { /* Velocita 1 */
        { /* Giocatore con velocita' 0 (0.562500 pixel)*/
            -4,
            -2,
            0,
            2,
            4,
            2,
            0,
            -2,
        },
        { /* Giocatore con velocita' 1 (0.687500 pixel)*/
            -4,
            -3,
            0,
            3,
            4,
            3,
            0,
            -3,
        },
        { /* Giocatore con velocita' 2 (0.812500 pixel)*/
            -5,
            -4,
            0,
            4,
            5,
            4,
            0,
            -4,
        },
        { /* Giocatore con velocita' 3 (0.937500 pixel)*/
            -6,
            -4,
            0,
            4,
            6,
            4,
            0,
            -4,
        },
        { /* Giocatore con velocita' 4 (1.062500 pixel)*/
            -7,
            -5,
            0,
            5,
            7,
            5,
            0,
            -5,
        },
        { /* Giocatore con velocita' 5 (1.187500 pixel)*/
            -8,
            -6,
            0,
            6,
            8,
            6,
            0,
            -6,
        },
        { /* Giocatore con velocita' 6 (1.312500 pixel)*/
            -9,
            -6,
            0,
            6,
            9,
            6,
            0,
            -6,
        },
        { /* Giocatore con velocita' 7 (1.437500 pixel)*/
            -10,
            -7,
            0,
            7,
            10,
            7,
            0,
            -7,
        },
        { /* Giocatore con velocita' 8 (1.562500 pixel)*/
            -11,
            -8,
            0,
            8,
            11,
            8,
            0,
            -8,
        },
        { /* Giocatore con velocita' 9 (1.687500 pixel)*/
            -12,
            -8,
            0,
            8,
            12,
            8,
            0,
            -8,
        },
    },
    { /* Velocita 2 */
        { /* Giocatore con velocita' 0 (0.937500 pixel)*/
            -6,
            -4,
            0,
            4,
            6,
            4,
            0,
            -4,
        },
        { /* Giocatore con velocita' 1 (1.062500 pixel)*/
            -7,
            -5,
            0,
            5,
            7,
            5,
            0,
            -5,
        },
        { /* Giocatore con velocita' 2 (1.187500 pixel)*/
            -8,
            -6,
            0,
            6,
            8,
            6,
            0,
            -6,
        },
        { /* Giocatore con velocita' 3 (1.312500 pixel)*/
            -9,
            -6,
            0,
            6,
            9,
            6,
            0,
            -6,
        },
        { /* Giocatore con velocita' 4 (1.437500 pixel)*/
            -10,
            -7,
            0,
            7,
            10,
            7,
            0,
            -7,
        },
        { /* Giocatore con velocita' 5 (1.562500 pixel)*/
            -11,
            -8,
            0,
            8,
            11,
            8,
            0,
            -8,
        },
        { /* Giocatore con velocita' 6 (1.687500 pixel)*/
            -12,
            -8,
            0,
            8,
            12,
            8,
            0,
            -8,
        },
        { /* Giocatore con velocita' 7 (1.812500 pixel)*/
            -13,
            -9,
            0,
            9,
            13,
            9,
            0,
            -9,
        },
        { /* Giocatore con velocita' 8 (1.937500 pixel)*/
            -14,
            -9,
            0,
            9,
            14,
            9,
            0,
            -9,
        },
        { /* Giocatore con velocita' 9 (2.062500 pixel)*/
            -14,
            -10,
            0,
            10,
            14,
            10,
            0,
            -10,
        },
    },
};
