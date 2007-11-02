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
#define GCSSOR_SOLVER_SSE 2
#define HYBRID_SOLVER 3
#define LOD_HYBRID_SOLVER 4
#define SIMPLE_FIELD 5
#define FAKE_FIELD 6
#define LOD_FIELD 7
#define LOD_HYBRID_FIELD 8
/* Spécifiques à rtfluids */
#define LOGRES_SOLVER 9
#define LOGRESAVG_SOLVER 10
#define LOGRESAVGTIME_SOLVER 11
#define COMPRESAVG_SOLVER 12
#define GS_SOLVER2D 13
#define CGSSOR_SOLVER2D 14

#define NB_MAXLUMINARIES 20
#define NB_MAXSOLVERS 150
#define NB_MAXFLAMMES 150

#define LIGHTING_STANDARD 0
#define LIGHTING_MULTI 1
#define LIGHTING_PIXEL 2
#define LIGHTING_PHOTOMETRIC 3

#define DEPTH_PEELING_LAYERS_MAX 10

#define LOD_VALUES 5
#endif
