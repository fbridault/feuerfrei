#if !defined(INTERFACECOMMON_H)
#define INTERFACECOMMON_H

/* Types de flammes */
#define CANDLE 0
#define FIRMALAMPE 1
#define TORCH 2
#define CAMPFIRE 3
#define CANDLESSET 4
#define CANDLESTICK 5

/* Types de solveurs */
/* Attention la valeur est importante car elle va correspondre à */
/* l'index dans les boutons radios de SolverDialog */
/* Communs à rtflames et rtfluids */
#define GS_SOLVER 0
#define GCSSOR_SOLVER 1
#define HYBRID_SOLVER 2
#define LOD_HYBRID_SOLVER 3
#define SIMPLE_FIELD 4
#define FAKE_FIELD 5
#define LOD_FIELD 6
/* Spécifiques à rtfluids */
#define LOGRES_SOLVER 7
#define LOGRESAVG_SOLVER 8
#define LOGRESAVGTIME_SOLVER 9
#define GS_SOLVER2D 10

#define NB_MAXSOLVERS 10
#define NB_MAXFLAMMES 100

#define LIGHTING_STANDARD 0
#define LIGHTING_PHOTOMETRIC 1

#define DEPTH_PEELING_LAYERS_MAX 10

#endif
