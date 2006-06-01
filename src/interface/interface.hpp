#if !defined(INTERFACECOMMON_H)
#define INTERFACECOMMON_H

/* Types de flammes */
#define CANDLE 0
#define FIRMALAMPE 1
#define TORCH 2
#define CAMPFIRE 3

/* Types de solveurs */
#define GS_SOLVER 0
#define GCSSOR_SOLVER 1
#define HYBRID_SOLVER 2
#define LOD_HYBRID_SOLVER 3
#define LOGRES_SOLVER 4
#define LOGRESAVG_SOLVER 5
#define LOGRESAVGTIME_SOLVER 6

#define NB_MAXSOLVERS 10
#define NB_MAXFLAMMES 20

#define LIGHTING_STANDARD 0
#define LIGHTING_PHOTOMETRIC 1

#endif
